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
