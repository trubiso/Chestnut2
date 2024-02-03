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
