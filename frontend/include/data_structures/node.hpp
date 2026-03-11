#ifndef FRONTEND_INCLUDE_NODE_HPP
#define FRONTEND_INCLUDE_NODE_HPP

#include "config.hpp"
#include <optional>
#include <vector>

namespace language {

class Node;
class Program;
class Statement;
class Expression;
class Assignment_expr;
class Block_stmt;
class Empty_stmt;
class If_stmt;
class While_stmt;
class Print_stmt;
class Number;
class Variable;
class Input;
class Func;
class Call;
class Return_stmt;
class Expr_stmt;
class Binary_operator;
class Unary_operator;

class ASTVisitor {
  public:
    virtual ~ASTVisitor() = default;

    virtual void visit(Program &node) = 0;
    virtual void visit(Block_stmt &node) = 0;
    virtual void visit(Empty_stmt &node) = 0;
    virtual void visit(Assignment_expr &node) = 0;
    virtual void visit(Input &node) = 0;
    virtual void visit(If_stmt &node) = 0;
    virtual void visit(While_stmt &node) = 0;
    virtual void visit(Print_stmt &node) = 0;
    virtual void visit(Binary_operator &node) = 0;
    virtual void visit(Unary_operator &node) = 0;
    virtual void visit(Func &node) = 0;
    virtual void visit(Call &node) = 0;
    virtual void visit(Return_stmt &node) = 0;
    virtual void visit(Expr_stmt &node) = 0;
    virtual void visit(Number &node) = 0;
    virtual void visit(Variable &node) = 0;
};

class Node {
  public:
    virtual ~Node() = default;
    virtual void accept(ASTVisitor &visitor) = 0;
};

enum class Binary_operators {
    Eq,
    Neq,
    Less,
    LessEq,
    Greater,
    GreaterEq,
    Add,
    Sub,
    Mul,
    Div,
    RemDiv,
    And,
    Xor,
    Or,
    LogOr,
    LogAnd,
};

enum class Unary_operators { Neg, Plus, Not };

class Statement : public Node {};
class Expression : public Node {};

using Statement_ptr = Statement *;
using StmtList = std::vector<Statement_ptr>;
using Expression_ptr = Expression *;
using Variable_ptr = Variable *;

class Program final : public Node {
  private:
    StmtList stmts_;

  public:
    explicit Program(StmtList stmts) : stmts_(std::move(stmts)) {}

    const StmtList &get_stmts() const noexcept { return stmts_; }
    StmtList &get_stmts() noexcept { return stmts_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Empty_stmt final : public Statement {
  public:
    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Block_stmt final : public Statement {
  private:
    StmtList stmts_;

  public:
    explicit Block_stmt(StmtList stmts) : stmts_(std::move(stmts)) {}

    const StmtList &get_stmts() const noexcept { return stmts_; }
    StmtList &get_stmts() noexcept { return stmts_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Assignment_expr final : public Expression {
  private:
    Variable_ptr variable_;
    Expression_ptr value_;

  public:
    Assignment_expr(Variable_ptr variable, Expression_ptr value)
        : variable_(variable), value_(value) {}

    const Variable_ptr get_variable() const noexcept { return variable_; }
    Expression &get_value() noexcept { return *value_; }
    const Expression &get_value() const noexcept { return *value_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class While_stmt final : public Statement {
  private:
    Expression_ptr condition_;
    Statement_ptr body_;

  public:
    While_stmt(Expression_ptr condition, Statement_ptr body)
        : condition_(condition), body_(body) {}

    Expression &get_condition() noexcept { return *condition_; }
    Statement &get_body() noexcept { return *body_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class If_stmt final : public Statement {
  private:
    Expression_ptr condition_;
    Statement_ptr then_branch_;
    Statement_ptr else_branch_;

  public:
    If_stmt(Expression_ptr condition, Statement_ptr then_branch,
            Statement_ptr else_branch = nullptr)
        : condition_(condition), then_branch_(then_branch),
          else_branch_(else_branch) {}

    Expression &get_condition() noexcept { return *condition_; }
    Statement &then_branch() noexcept { return *then_branch_; }
    Statement &else_branch() noexcept { return *else_branch_; }
    bool contains_else_branch() const noexcept { return else_branch_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Input final : public Expression {
  public:
    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Print_stmt final : public Statement {
  private:
    Expression_ptr value_;

  public:
    explicit Print_stmt(Expression_ptr value) : value_(std::move(value)) {}

    Expression &get_value() noexcept { return *value_; }
    const Expression &get_value() const noexcept { return *value_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Func final : public Expression {
  public:
    using ParamList = std::vector<name_t_sv>;

  private:
    std::optional<name_t_sv> func_name_;
    ParamList params_;
    Statement_ptr body_;

  public:
    Func(std::optional<name_t_sv> func_name, ParamList params,
         Statement_ptr body)
        : func_name_(func_name), params_(std::move(params)),
          body_(std::move(body)) {}

    bool has_name() const noexcept { return func_name_.has_value(); }

    std::optional<name_t_sv> get_func_name() const noexcept {
        return func_name_;
    }

    const ParamList &get_params() const noexcept { return params_; }

    Statement &get_body() noexcept { return *body_; }
    const Statement &get_body() const noexcept { return *body_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Call final : public Expression {
  public:
    using ArgExprList = std::vector<Expression_ptr>;

  private:
    Expression_ptr target_;
    ArgExprList args_;

  public:
    Call(Expression_ptr target, ArgExprList args)
        : target_(std::move(target)), args_(std::move(args)) {}

    Expression &get_target() noexcept { return *target_; }
    const Expression &get_target() const noexcept { return *target_; }

    const ArgExprList &get_args() const noexcept { return args_; }
    ArgExprList &get_args() noexcept { return args_; }

    void accept(ASTVisitor &v) override { v.visit(*this); }
};

class Return_stmt final : public Statement {
  private:
    Expression_ptr value_;

  public:
    explicit Return_stmt(Expression_ptr value = nullptr) : value_(value) {}

    bool has_value() const noexcept { return value_ != nullptr; }

    Expression &get_value() noexcept { return *value_; }
    const Expression &get_value() const noexcept { return *value_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Expr_stmt final : public Statement {
  private:
    Expression_ptr expr_;

  public:
    explicit Expr_stmt(Expression_ptr expr) : expr_(expr) {}

    Expression &get_expr() noexcept { return *expr_; }
    const Expression &get_expr() const noexcept { return *expr_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Binary_operator final : public Expression {
  private:
    Binary_operators op_;
    Expression_ptr left_;
    Expression_ptr right_;

  public:
    Binary_operator(Binary_operators op, Expression_ptr left,
                    Expression_ptr right)
        : op_(op), left_(left), right_(right) {}

    Binary_operators get_operator() const noexcept { return op_; }
    Expression &get_left() noexcept { return *left_; }
    const Expression &get_left() const noexcept { return *left_; }
    Expression &get_right() noexcept { return *right_; }
    const Expression &get_right() const noexcept { return *right_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Unary_operator final : public Expression {
  private:
    Unary_operators op_;
    Expression_ptr operand_;

  public:
    Unary_operator(Unary_operators op, Expression_ptr operand)
        : op_(op), operand_(operand) {}

    Unary_operators get_operator() const noexcept { return op_; }
    Expression &get_operand() noexcept { return *operand_; }
    const Expression &get_operand() const noexcept { return *operand_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Number final : public Expression {
  private:
    number_t number_;

  public:
    explicit Number(number_t number) : number_(number) {}
    number_t &get_value() noexcept { return number_; }
    const number_t &get_value() const noexcept { return number_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

class Variable final : public Expression {
  private:
    name_t_sv var_name_;

  public:
    explicit Variable(name_t_sv var_name) : var_name_(var_name) {}

    name_t_sv get_name() const noexcept { return var_name_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};

} // namespace language

#endif // FRONTEND_INCLUDE_NODE_HPP
