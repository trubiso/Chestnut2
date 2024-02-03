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
