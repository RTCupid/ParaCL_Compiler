#ifndef FRONTEND_INCLUDE_SIMULATOR_HPP
#define FRONTEND_INCLUDE_SIMULATOR_HPP

#include "node.hpp"
#include <string>
#include <unordered_map>

namespace language {

class Simulator final : public ASTVisitor {

    using nametable_t = std::unordered_map<std::string, number_t>;
    nametable_t nametable_;

  public:
    nametable_t &get_nametable() noexcept { return nametable_; }

    void visit(Program &node) override;
    void visit(Block_stmt &node) override;

    void visit(Empty_stmt &node) override;

    void visit(Assignment_stmt &node) override;

    void visit(Input &node) override;

    void visit(If_stmt &node) override;

    void visit(While_stmt &node) override;

    void visit(Print_stmt &node) override;

    void visit(Assignment_expr &node) override;
    void visit(Binary_operator &node) override;
    void visit(Unary_operator &node) override;
    void visit(Number &node) override;
    void visit(Variable &node) override;

    void visit(Func &node) override;
    void visit(Call &node) override;
    void visit(Return_stmt &node) override;
    void visit(Expr_stmt &node) override;

  private:
    number_t evaluate_expression(Expression &expression);
};

} // namespace language

#endif // FRONTEND_INCLUDE_SIMULATOR_HPP
