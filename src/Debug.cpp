#include "Debug.hpp"

#include "VariantName.hpp"

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
		if (path.absolute) std::cout << "::";
		debug(path.path, "::");
		break;
	}
	case Resolved:
		std::cout << "@" << std::get<size_t>(identifier.value);
		break;
	}
}

void debug(AST::Type const &type) {
	using enum AST::Type::Kind;
	std::cout << "[type ";
	switch (type.kind) {
	case Identified:
		debug(std::get<AST::Identifier>(type.value));
		break;
	case BuiltIn: {
		auto built_in = std::get<AST::Type::BuiltIn>(type.value);
		std::cout << get_variant_name(built_in.kind);
		if (built_in.bit_width.has_value()) {
			std::cout << " with width " << built_in.bit_width.value();
		}
	} break;
	case Inferred:
		std::cout << "inferred";
		break;
	}
	std::cout << "]";
}

void debug(AST::IdentifierWithType const &pair) {
	debug(std::get<0>(pair));
	std::cout << ": ";
	debug(std::get<1>(pair));
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
		debug(std::get<Spanned<AST::Identifier>>(expression.value));
		break;
	case BinaryOperation: {
		auto const &operation = std::get<AST::Expression::BinaryOperation>(expression.value);
		std::cout << "(";
		debug(operation.lhs);
		std::cout << " binaryop";
		debug(operation.operator_);
		std::cout << " ";
		debug(operation.rhs);
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
	case Call: {
		auto const &call = std::get<AST::Expression::Call>(expression.value);
		std::cout << "(call (";
		debug(call.callee);
		if (call.generics.has_value()) {
			std::cout << ") with generics (";
			debug(call.generics);
		}
		std::cout << ") with args (";
		debug(call.arguments);
		std::cout << "))";
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
		if (create.mutable_.value) std::cout << "(mutable) ";
		debug(create.identifier);
		if (create.value.has_value()) {
			std::cout << " = ";
			debug(create.value.value());
		}
		std::cout << "]";
		break;
	}
	case Set: {
		auto const &set = std::get<AST::Statement::Set>(statement.value);
		std::cout << "SET [";
		debug(set.lhs);
		std::cout << " = ";
		debug(set.rhs);
		std::cout << "]";
		break;
	}
	case BareExpression: {
		auto const &bare_expression = std::get<AST::Statement::BareExpression>(statement.value);
		std::cout << "BARE EXPRESSION [";
		debug(bare_expression.expression);
		std::cout << "]";
		break;
	}
	}
}

void debug(AST::Function::Signature const &signature) {
	if (signature.generics.has_value()) {
		std::cout << "<";
		debug(signature.generics.value());
		std::cout << ">";
	}
	std::cout << "(";
	debug(signature.arguments);
	std::cout << ") -> ";
	debug(signature.return_type);
}

void debug(AST::Function const &function) {
	std::cout << "function ";
	debug(function.name);
	std::cout << " of signature ";
	debug(function.signature);
	std::cout << " with body ";
	if (!function.body.has_value()) {
		std::cout << "omitted\n";
	} else {
		std::cout << "{\n";
		debug(function.body.value().value, "\n");
		std::cout << "\n}\n";
	}
}

void debug(AST::Program const &program) { debug(program.functions, "\n"); }

void debug(Token::Symbol symbol) { std::cout << get_variant_name(symbol); }
