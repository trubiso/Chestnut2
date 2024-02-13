#pragma once
#include "Box.hpp"
#include "Identifier.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "Type.hpp"

namespace AST {

struct Expression {
	enum class Kind {
		CharLiteral,
		StringLiteral,
		NumberLiteral,
		Identifier,
		BinaryOperation,
		UnaryOperation,
		Call,
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

	struct Call {
		Box<Spanned<Expression>> callee;
		std::optional<Spanned<std::vector<Spanned<Type>>>> generics;
		Spanned<std::vector<Box<Spanned<Expression>>>> arguments;
	};

	Kind kind;
	std::variant<std::string, Identifier, BinaryOperation, UnaryOperation, Call> value;
};

Parser::Parser<Expression> expression_char_literal();
Parser::Parser<Expression> expression_string_literal();
Parser::Parser<Expression> expression_number_literal();
Parser::Parser<Expression> expression_identifier();
Parser::Parser<Expression> expression_atom();

Parser::Parser<Expression> expression();

}  // namespace AST
