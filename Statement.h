#pragma once
#include "Expression.h"
#include "Identifier.h"
#include "Parser.h"
#include "Type.h"

namespace AST {

struct Statement {
	enum class Kind { Create };

	struct Create {
		Type type;
		Identifier identifier;
		bool mutable_;
		std::optional<Expression> value;
	};

	Kind kind;
	std::variant<Create> value;
};

Parser::Parser<Statement> statement_create();

Parser::Parser<Statement> statement();

} // namespace AST
