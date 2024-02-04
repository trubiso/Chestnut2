#pragma once
#include "Box.h"
#include "Identifier.h"
#include "Parser.h"
#include "Token.h"

namespace AST {

struct Expression {
	enum class Kind {
		CharLiteral,
		StringLiteral,
		NumberLiteral,
		Identifier,
		BinaryOperation,
		UnaryOperation,
	};

	struct BinaryOperation {
		Box<Spanned<Expression>> lhs;
		Box<Spanned<Expression>> rhs;
		Spanned<Token::Symbol> operator_;
	};

	struct UnaryOperation {
		Box<Spanned<Expression>> value;
		Spanned<Token::Symbol> operator_;
	};

	Kind kind;
	std::variant<std::string, Identifier, BinaryOperation, UnaryOperation> value;
};

Parser::Parser<Expression> expression_char_literal();
Parser::Parser<Expression> expression_string_literal();
Parser::Parser<Expression> expression_number_literal();
Parser::Parser<Expression> expression_identifier();
Parser::Parser<Expression> expression_atom();

Parser::Parser<Expression> expression();

} // namespace AST
