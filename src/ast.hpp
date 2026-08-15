#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class ExpressionNode : public ASTNode {};
class StatementNode : public ASTNode {};

using ExprPtr = std::unique_ptr<ExpressionNode>;
using StmtPtr = std::unique_ptr<StatementNode>;

class NumberExpr : public ExpressionNode {
public:
    uint64_t value;
    explicit NumberExpr(uint64_t val) : value(val) {}
};

class StringExpr : public ExpressionNode {
public:
    std::string value;
    explicit StringExpr(std::string val) : value(std::move(val)) {}
};

class VariableExpr : public ExpressionNode {
public:
    std::string name;
    explicit VariableExpr(std::string name) : name(std::move(name)) {}
};

class BinaryExpr : public ExpressionNode {
public:
    std::string op;
    ExprPtr left;
    ExprPtr right;
    BinaryExpr(std::string op, ExprPtr left, ExprPtr right)
        : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
};

class CallExpr : public ExpressionNode {
public:
    std::string name;
    std::vector<ExprPtr> args;
    CallExpr(std::string name, std::vector<ExprPtr> args)
        : name(std::move(name)), args(std::move(args)) {}
};

class MemReadExpr : public ExpressionNode {
public:
    ExprPtr address;
    explicit MemReadExpr(ExprPtr addr) : address(std::move(addr)) {}
};

class InbExpr : public ExpressionNode {
public:
    ExprPtr port;
    explicit InbExpr(ExprPtr port) : port(std::move(port)) {}
};

class ImportStmt : public StatementNode {
public:
    std::string filepath;
    explicit ImportStmt(std::string path) : filepath(std::move(path)) {}
};

class MemWriteStmt : public StatementNode {
public:
    ExprPtr address;
    ExprPtr value;
    MemWriteStmt(ExprPtr addr, ExprPtr val)
        : address(std::move(addr)), value(std::move(val)) {}
};

class OutbStmt : public StatementNode {
public:
    ExprPtr port;
    ExprPtr value;
    OutbStmt(ExprPtr port, ExprPtr val)
        : port(std::move(port)), value(std::move(val)) {}
};

class AssignStmt : public StatementNode {
public:
    std::string varName;
    ExprPtr value;
    AssignStmt(std::string name, ExprPtr val)
        : varName(std::move(name)), value(std::move(val)) {}
};

class ExprStmt : public StatementNode {
public:
    ExprPtr expr;
    explicit ExprStmt(ExprPtr expr) : expr(std::move(expr)) {}
};

class ReturnStmt : public StatementNode {
public:
    ExprPtr value;
    explicit ReturnStmt(ExprPtr val) : value(std::move(val)) {}
};

class IfStmt : public StatementNode {
public:
    ExprPtr condition;
    std::vector<StmtPtr> thenBranch;
    IfStmt(ExprPtr cond, std::vector<StmtPtr> thenBranch)
        : condition(std::move(cond)), thenBranch(std::move(thenBranch)) {}
};

class WhileStmt : public StatementNode {
public:
    ExprPtr condition;
    std::vector<StmtPtr> body;
    WhileStmt(ExprPtr cond, std::vector<StmtPtr> body)
        : condition(std::move(cond)), body(std::move(body)) {}
};

class FunctionDecl : public StatementNode {
public:
    std::string name;
    std::vector<std::string> params;
    std::vector<StmtPtr> body;
    FunctionDecl(std::string name, std::vector<std::string> params, std::vector<StmtPtr> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ImportStmt>> imports;
    std::vector<std::unique_ptr<FunctionDecl>> functions;
    std::vector<StmtPtr> topLevelStatements;
};

#endif
