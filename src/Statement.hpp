#pragma once
#include "Expression.hpp"
#include "Identifier.hpp"
#include "Parser.hpp"
#include "Type.hpp"

namespace AST {

struct Statement {
	enum class Kind { Create, Set, BareExpression };

	struct Create {
		Spanned<Type> type;
		Spanned<Identifier> identifier;
		Spanned<bool> mutable_;
		std::optional<Spanned<Expression>> value;
	};

	// a @= b will desugar to a = a @ b (where @ is any binary operator)
	struct Set {
		Spanned<Expression> lhs;
		Spanned<Expression> rhs;
		Span operator_span;
	};

	struct BareExpression {
		Spanned<Expression> expression;
	};

	Kind kind;
	std::variant<Create, Set, BareExpression> value;
};

Parser::Parser<Statement> statement_create(bool semicolon);
Parser::Parser<Statement> statement_set(bool semicolon);
Parser::Parser<Statement> statement_bare_expression(bool semicolon);

Parser::Parser<Statement> statement(bool semicolon = true);

typedef std::vector<Statement> Scope;

Parser::Parser<Scope> scope();

}  // namespace AST
