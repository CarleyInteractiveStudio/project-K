#include "codegen.hpp"
#include <iostream>
#include <stdexcept>

CodeGenerator::CodeGenerator(Arch arch) : targetArch(arch) {}

std::string CodeGenerator::newLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(labelCounter++);
}

std::string CodeGenerator::generate(ProgramNode* program) {
    asmOut.str("");
    asmOut.clear();

    if (targetArch == Arch::X86_64) {
        emitX86_64(program);
    } else {
        emitX86_64(program);
    }

    return asmOut.str();
}

void CodeGenerator::emitX86_64(ProgramNode* program) {
    // Bare-metal Multiboot 1 compliant boot header and entry code
    asmOut << ".code32\n";
    asmOut << ".global _start\n";
    asmOut << ".text\n\n";

    // Multiboot Header Magic constants
    asmOut << "# Multiboot 1 Header\n";
    asmOut << ".align 4\n";
    asmOut << "multiboot_header:\n";
    asmOut << "    .long 0x1BADB002\n";                     // Magic number
    asmOut << "    .long 0x00000003\n";                     // Flags: align modules + mem info
    asmOut << "    .long -(0x1BADB002 + 0x00000003)\n\n";   // Checksum

    asmOut << "_start:\n";
    asmOut << "    cli\n";
    asmOut << "    mov $0x90000, %esp\n"; // Setup stack pointer

    if (!program->topLevelStatements.empty()) {
        asmOut << "    call k_toplevel_init\n";
    }

    bool hasMain = false;
    for (const auto& fn : program->functions) {
        if (fn->name == "main") {
            hasMain = true;
            break;
        }
    }

    if (hasMain) {
        asmOut << "    call main\n";
    }

    asmOut << "1:\n";
    asmOut << "    hlt\n";
    asmOut << "    jmp 1b\n\n";

    // Generate user-defined functions
    for (const auto& fn : program->functions) {
        genFunction(fn.get());
    }

    // Top-level execution wrapper if top-level code exists
    if (!program->topLevelStatements.empty()) {
        localVars.clear();
        stackOffset = 0;

        asmOut << ".global k_toplevel_init\n";
        asmOut << "k_toplevel_init:\n";
        asmOut << "    push %ebp\n";
        asmOut << "    mov %esp, %ebp\n";
        asmOut << "    sub $256, %esp\n";

        for (const auto& stmt : program->topLevelStatements) {
            genStatement(stmt.get());
        }

        asmOut << "    mov %ebp, %esp\n";
        asmOut << "    pop %ebp\n";
        asmOut << "    ret\n\n";
    }
}

void CodeGenerator::genFunction(FunctionDecl* fn) {
    localVars.clear();
    stackOffset = 0;

    asmOut << ".global " << fn->name << "\n";
    asmOut << fn->name << ":\n";
    asmOut << "    push %ebp\n";
    asmOut << "    mov %esp, %ebp\n";
    asmOut << "    sub $256, %esp\n";

    // Standard cdecl 32-bit parameters from stack: [ebp + 8], [ebp + 12], etc.
    for (size_t i = 0; i < fn->params.size(); ++i) {
        int paramOffset = 8 + (i * 4);
        localVars[fn->params[i]] = paramOffset;
    }

    // Process function statements
    for (const auto& stmt : fn->body) {
        genStatement(stmt.get());
    }

    asmOut << "    mov %ebp, %esp\n";
    asmOut << "    pop %ebp\n";
    asmOut << "    ret\n\n";
}

void CodeGenerator::genStatement(StatementNode* stmt) {
    if (auto assign = dynamic_cast<AssignStmt*>(stmt)) {
        genExpression(assign->value.get());
        if (localVars.find(assign->varName) == localVars.end()) {
            stackOffset += 4;
            localVars[assign->varName] = -stackOffset;
        }
        int offset = localVars[assign->varName];
        asmOut << "    mov %eax, " << offset << "(%ebp)\n";
    }
    else if (auto ret = dynamic_cast<ReturnStmt*>(stmt)) {
        if (ret->value) {
            genExpression(ret->value.get());
        }
        asmOut << "    mov %ebp, %esp\n";
        asmOut << "    pop %ebp\n";
        asmOut << "    ret\n";
    }
    else if (auto mw = dynamic_cast<MemWriteStmt*>(stmt)) {
        genExpression(mw->value.get());
        asmOut << "    push %eax\n";
        genExpression(mw->address.get());
        asmOut << "    pop %ebx\n"; // value
        asmOut << "    mov %ebx, (%eax)\n"; // write value to direct memory address in eax
    }
    else if (auto ob = dynamic_cast<OutbStmt*>(stmt)) {
        genExpression(ob->value.get());
        asmOut << "    push %eax\n";
        genExpression(ob->port.get());
        asmOut << "    mov %eax, %edx\n"; // port in edx
        asmOut << "    pop %eax\n";  // val in eax
        asmOut << "    out %al, %dx\n"; // x86 outb instruction
    }
    else if (auto ifs = dynamic_cast<IfStmt*>(stmt)) {
        std::string elseLabel = newLabel("L_else");
        std::string endLabel = newLabel("L_end");

        genExpression(ifs->condition.get());
        asmOut << "    cmp $0, %eax\n";
        asmOut << "    je " << elseLabel << "\n";

        for (const auto& s : ifs->thenBranch) {
            genStatement(s.get());
        }
        asmOut << "    jmp " << endLabel << "\n";
        asmOut << elseLabel << ":\n";
        asmOut << endLabel << ":\n";
    }
    else if (auto ws = dynamic_cast<WhileStmt*>(stmt)) {
        std::string startLabel = newLabel("L_while_start");
        std::string endLabel = newLabel("L_while_end");

        asmOut << startLabel << ":\n";
        genExpression(ws->condition.get());
        asmOut << "    cmp $0, %eax\n";
        asmOut << "    je " << endLabel << "\n";

        for (const auto& s : ws->body) {
            genStatement(s.get());
        }
        asmOut << "    jmp " << startLabel << "\n";
        asmOut << endLabel << ":\n";
    }
    else if (auto es = dynamic_cast<ExprStmt*>(stmt)) {
        genExpression(es->expr.get());
    }
}

void CodeGenerator::genExpression(ExpressionNode* expr) {
    if (auto num = dynamic_cast<NumberExpr*>(expr)) {
        asmOut << "    mov $" << num->value << ", %eax\n";
    }
    else if (auto var = dynamic_cast<VariableExpr*>(expr)) {
        if (localVars.find(var->name) != localVars.end()) {
            int offset = localVars[var->name];
            asmOut << "    mov " << offset << "(%ebp), %eax\n";
        } else {
            std::cerr << "Codegen error: undefined variable '" << var->name << "'\n";
            exit(1);
        }
    }
    else if (auto mr = dynamic_cast<MemReadExpr*>(expr)) {
        genExpression(mr->address.get());
        asmOut << "    mov (%eax), %eax\n";
    }
    else if (auto ib = dynamic_cast<InbExpr*>(expr)) {
        genExpression(ib->port.get());
        asmOut << "    mov %eax, %edx\n";
        asmOut << "    xor %eax, %eax\n";
        asmOut << "    in %dx, %al\n";
    }
    else if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
        genExpression(bin->right.get());
        asmOut << "    push %eax\n";
        genExpression(bin->left.get());
        asmOut << "    pop %ebx\n";

        if (bin->op == "+") {
            asmOut << "    add %ebx, %eax\n";
        } else if (bin->op == "-") {
            asmOut << "    sub %ebx, %eax\n";
        } else if (bin->op == "*") {
            asmOut << "    imul %ebx, %eax\n";
        } else if (bin->op == "/") {
            asmOut << "    xor %edx, %edx\n";
            asmOut << "    idiv %ebx\n";
        } else if (bin->op == "==") {
            asmOut << "    cmp %ebx, %eax\n";
            asmOut << "    sete %al\n";
            asmOut << "    movzbl %al, %eax\n";
        } else if (bin->op == ">") {
            asmOut << "    cmp %ebx, %eax\n";
            asmOut << "    setg %al\n";
            asmOut << "    movzbl %al, %eax\n";
        } else if (bin->op == "<") {
            asmOut << "    cmp %ebx, %eax\n";
            asmOut << "    setl %al\n";
            asmOut << "    movzbl %al, %eax\n";
        }
    }
    else if (auto call = dynamic_cast<CallExpr*>(expr)) {
        // Push arguments in reverse for cdecl 32-bit
        for (int i = (int)call->args.size() - 1; i >= 0; --i) {
            genExpression(call->args[i].get());
            asmOut << "    push %eax\n";
        }
        asmOut << "    call " << call->name << "\n";
        if (!call->args.empty()) {
            asmOut << "    add $" << (call->args.size() * 4) << ", %esp\n";
        }
    }
}
