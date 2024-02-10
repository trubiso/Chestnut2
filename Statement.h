#pragma once
#include "Expression.h"
#include "Identifier.h"
#include "Parser.h"
#include "Type.h"

namespace AST {

struct Statement {
	enum class Kind { Create, Set };

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

	Kind kind;
	std::variant<Create, Set> value;
};

Parser::Parser<Statement> statement_create(bool semicolon);
Parser::Parser<Statement> statement_set(bool semicolon);

Parser::Parser<Statement> statement(bool semicolon = true);

typedef std::vector<Statement> Scope;

Parser::Parser<Scope> scope();

} // namespace AST
