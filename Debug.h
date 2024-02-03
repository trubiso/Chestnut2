#pragma once
#include "Identifier.h"
#include "Token.h"
#include <iostream>
#include <vector>

void debug(Token const &token);
void debug(AST::Identifier const &identifier);

inline void debug(std::string const &string) { std::cout << string; }

template <typename T> void debug(std::vector<T> const &vector, char const *separator = ", ") {
	for (size_t i = 0; i < vector.size(); ++i) {
		debug(vector.at(i));
		if (i != vector.size() - 1)
			std::cout << separator;
	}
}
