#pragma once
#include "Token.h"
#include <iostream>
#include <vector>

void debug(Token const &token);

template <typename T> void debug(std::vector<T> const &vector, char const *separator = ", ") {
	for (size_t i = 0; i < vector.size(); ++i) {
		debug(vector.at(i));
		if (i != vector.size() - 1)
			std::cout << separator;
	}
}
