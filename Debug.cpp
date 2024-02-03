#include "Debug.h"
#include "VariantName.h"

void debug(Token const &token) {
	std::cout << "[(" << get_variant_name(token.kind) << ") ";
	if (token.kind == Token::Kind::Symbol)
		std::cout << get_variant_name(std::get<Token::Symbol>(token.value));
	else
		std::cout << std::get<std::string>(token.value);
	std::cout << ']';
}

void debug(AST::Identifier const &identifier) {
	using enum AST::Identifier::Kind;
	switch (identifier.kind) {
	case Unqualified:
		std::cout << std::get<std::string>(identifier.value);
		break;
	case Qualified: {
		auto const &path = std::get<AST::Identifier::QualifiedPath>(identifier.value);
		if (path.absolute)
			std::cout << "::";
		debug(path.path, "::");
		break;
	}
	}
}

void debug(AST::Type const &type) {
	using enum AST::Type::Kind;
	std::cout << "[type ";
	switch (type.kind) {
	case Identified:
		debug(std::get<AST::Identifier>(type.value));
		break;
	case Inferred:
		std::cout << "inferred";
		break;
	}
	std::cout << "]";
}

void debug(AST::Expression const &expression) {
	using enum AST::Expression::Kind;
	switch (expression.kind) {
	case CharLiteral:
		std::cout << "char " << std::get<std::string>(expression.value);
		break;
	case StringLiteral:
		std::cout << "string " << std::get<std::string>(expression.value);
		break;
	case NumberLiteral:
		std::cout << "number " << std::get<std::string>(expression.value);
		break;
	case Identifier:
		debug(std::get<AST::Identifier>(expression.value));
		break;
	case BinaryOperation: {
		auto const &operation = std::get<AST::Expression::BinaryOperation>(expression.value);
		std::cout << "(";
		debug(*operation.lhs);
		std::cout << " binaryop";
		debug(operation.operator_);
		std::cout << " ";
		debug(*operation.rhs);
		std::cout << ")";
	} break;
	case UnaryOperation: {
		auto const &operation = std::get<AST::Expression::UnaryOperation>(expression.value);
		std::cout << "(unaryop";
		debug(operation.operator_);
		std::cout << " ";
		debug(*operation.value);
		std::cout << ")";
	} break;
	}
}

void debug(AST::Statement const &statement) {
	using enum AST::Statement::Kind;
	switch (statement.kind) {
	case Create: {
		auto const &create = std::get<AST::Statement::Create>(statement.value);
		std::cout << "CREATE [";
		debug(create.type);
		std::cout << " ";
		if (create.mutable_)
			std::cout << "(mutable) ";
		debug(create.identifier);
		if (create.value.has_value()) {
			std::cout << " = ";
			debug(create.value.value());
		}
		std::cout << "]";
		break;
	}
	}
}

void debug(Token::Symbol symbol) { std::cout << get_variant_name(symbol); }
