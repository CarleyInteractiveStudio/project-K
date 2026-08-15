#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "ast.hpp"

enum class Arch {
    X86_64,
    X86_32,
    ARM64,
    RISCV64
};

class CodeGenerator {
public:
    explicit CodeGenerator(Arch arch = Arch::X86_64);
    std::string generate(ProgramNode* program);

private:
    Arch targetArch;
    std::ostringstream asmOut;
    int labelCounter = 0;

    std::unordered_map<std::string, int> localVars;
    int stackOffset = 0;

    std::string newLabel(const std::string& prefix = "L");

    void genProgram(ProgramNode* program);
    void genFunction(FunctionDecl* fn);
    void genStatement(StatementNode* stmt);
    void genExpression(ExpressionNode* expr);

    // Assembly primitives for x86_64 bare-metal
    void emitX86_64(ProgramNode* program);
};

#endif
