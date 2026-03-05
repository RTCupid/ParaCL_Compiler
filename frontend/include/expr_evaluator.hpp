#ifndef FRONTEND_INCLUDE_EXPR_EVALUATOR_HPP
#define FRONTEND_INCLUDE_EXPR_EVALUATOR_HPP

#include "simulator.hpp"

namespace language {

class Expression_evaluator final : public ASTVisitor {
  private:
    Simulator &simulator_;
    number_t result_{0};

  public:
    Expression_evaluator(Simulator &simulator) : simulator_{simulator} {};

    number_t get_result() const noexcept;

    void visit(Number &node) override;

    void visit(Variable &node) override;

    void visit(Binary_operator &node) override;

    void visit(Unary_operator &node) override;

    void visit(Assignment_expr &node) override;

    void visit(Input &node) override;

    void visit(Program &node) override;
    void visit(Block_stmt &node) override;
    void visit(Empty_stmt &node) override;
    void visit(Assignment_stmt &node) override;
    void visit(If_stmt &node) override;
    void visit(While_stmt &node) override;
    void visit(Print_stmt &node) override;

    void visit(Func &node) override;
    void visit(Call &node) override;
};

} // namespace language

#endif // FRONTEND_INCLUDE_EXPR_EVALUATOR_HPP
