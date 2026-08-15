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
        // Fallback x86_64 structure for multi-arch compatibility
        emitX86_64(program);
    }

    return asmOut.str();
}

void CodeGenerator::emitX86_64(ProgramNode* program) {
    asmOut << ".code64\n";
    asmOut << ".global _start\n";
    asmOut << ".text\n\n";

    // Standard bare-metal boot entry point
    asmOut << "_start:\n";
    asmOut << "    mov $0x90000, %rsp\n"; // setup initial stack pointer for bare-metal
    asmOut << "    call main\n";
    asmOut << "1:\n";
    asmOut << "    hlt\n";
    asmOut << "    jmp 1b\n\n";

    // Generate user-defined functions
    for (const auto& fn : program->functions) {
        genFunction(fn.get());
    }

    // Top-level execution wrapper if top-level code exists
    if (!program->topLevelStatements.empty()) {
        asmOut << "k_toplevel_init:\n";
        asmOut << "    push %rbp\n";
        asmOut << "    mov %rsp, %rbp\n";
        for (const auto& stmt : program->topLevelStatements) {
            genStatement(stmt.get());
        }
        asmOut << "    mov %rbp, %rsp\n";
        asmOut << "    pop %rbp\n";
        asmOut << "    ret\n\n";
    }
}

void CodeGenerator::genFunction(FunctionDecl* fn) {
    localVars.clear();
    stackOffset = 0;

    asmOut << ".global " << fn->name << "\n";
    asmOut << fn->name << ":\n";
    asmOut << "    push %rbp\n";
    asmOut << "    mov %rsp, %rbp\n";

    // Allocate parameters on stack
    // x86_64 System V ABI registers for params: rdi, rsi, rdx, rcx, r8, r9
    static const char* paramRegs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (size_t i = 0; i < fn->params.size(); ++i) {
        stackOffset += 8;
        localVars[fn->params[i]] = -stackOffset;
        if (i < 6) {
            asmOut << "    mov " << paramRegs[i] << ", " << -stackOffset << "(%rbp)\n";
        }
    }

    // Process function statements
    for (const auto& stmt : fn->body) {
        genStatement(stmt.get());
    }

    asmOut << "    mov %rbp, %rsp\n";
    asmOut << "    pop %rbp\n";
    asmOut << "    ret\n\n";
}

void CodeGenerator::genStatement(StatementNode* stmt) {
    if (auto assign = dynamic_cast<AssignStmt*>(stmt)) {
        genExpression(assign->value.get());
        if (localVars.find(assign->varName) == localVars.end()) {
            stackOffset += 8;
            localVars[assign->varName] = -stackOffset;
            asmOut << "    sub $8, %rsp\n";
        }
        int offset = localVars[assign->varName];
        asmOut << "    mov %rax, " << offset << "(%rbp)\n";
    }
    else if (auto ret = dynamic_cast<ReturnStmt*>(stmt)) {
        if (ret->value) {
            genExpression(ret->value.get());
        }
        asmOut << "    mov %rbp, %rsp\n";
        asmOut << "    pop %rbp\n";
        asmOut << "    ret\n";
    }
    else if (auto mw = dynamic_cast<MemWriteStmt*>(stmt)) {
        genExpression(mw->value.get());
        asmOut << "    push %rax\n";
        genExpression(mw->address.get());
        asmOut << "    pop %rbx\n"; // value
        asmOut << "    mov %rbx, (%rax)\n"; // write value to direct memory address in rax
    }
    else if (auto ob = dynamic_cast<OutbStmt*>(stmt)) {
        genExpression(ob->value.get());
        asmOut << "    push %rax\n";
        genExpression(ob->port.get());
        asmOut << "    mov %rax, %rdx\n"; // port in rdx
        asmOut << "    pop %rax\n";  // val in rax
        asmOut << "    out %al, %dx\n"; // x86 outb instruction
    }
    else if (auto ifs = dynamic_cast<IfStmt*>(stmt)) {
        std::string elseLabel = newLabel("L_else");
        std::string endLabel = newLabel("L_end");

        genExpression(ifs->condition.get());
        asmOut << "    cmp $0, %rax\n";
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
        asmOut << "    cmp $0, %rax\n";
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
        asmOut << "    mov $" << num->value << ", %rax\n";
    }
    else if (auto var = dynamic_cast<VariableExpr*>(expr)) {
        if (localVars.find(var->name) != localVars.end()) {
            int offset = localVars[var->name];
            asmOut << "    mov " << offset << "(%rbp), %rax\n";
        } else {
            std::cerr << "Codegen error: undefined variable '" << var->name << "'\n";
            exit(1);
        }
    }
    else if (auto mr = dynamic_cast<MemReadExpr*>(expr)) {
        genExpression(mr->address.get());
        asmOut << "    mov (%rax), %rax\n";
    }
    else if (auto ib = dynamic_cast<InbExpr*>(expr)) {
        genExpression(ib->port.get());
        asmOut << "    mov %rax, %rdx\n";
        asmOut << "    xor %rax, %rax\n";
        asmOut << "    in %dx, %al\n";
    }
    else if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
        genExpression(bin->right.get());
        asmOut << "    push %rax\n";
        genExpression(bin->left.get());
        asmOut << "    pop %rbx\n";

        if (bin->op == "+") {
            asmOut << "    add %rbx, %rax\n";
        } else if (bin->op == "-") {
            asmOut << "    sub %rbx, %rax\n";
        } else if (bin->op == "*") {
            asmOut << "    imul %rbx, %rax\n";
        } else if (bin->op == "/") {
            asmOut << "    xor %rdx, %rdx\n";
            asmOut << "    idiv %rbx\n";
        } else if (bin->op == "==") {
            asmOut << "    cmp %rbx, %rax\n";
            asmOut << "    sete %al\n";
            asmOut << "    movzbq %al, %rax\n";
        } else if (bin->op == ">") {
            asmOut << "    cmp %rbx, %rax\n";
            asmOut << "    setg %al\n";
            asmOut << "    movzbq %al, %rax\n";
        } else if (bin->op == "<") {
            asmOut << "    cmp %rbx, %rax\n";
            asmOut << "    setl %al\n";
            asmOut << "    movzbq %al, %rax\n";
        }
    }
    else if (auto call = dynamic_cast<CallExpr*>(expr)) {
        static const char* paramRegs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        for (size_t i = 0; i < call->args.size(); ++i) {
            genExpression(call->args[i].get());
            if (i < 6) {
                asmOut << "    mov %rax, " << paramRegs[i] << "\n";
            } else {
                asmOut << "    push %rax\n";
            }
        }
        asmOut << "    call " << call->name << "\n";
    }
}
