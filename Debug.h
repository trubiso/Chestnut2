#pragma once
#include "Expression.h"
#include "Function.h"
#include "Identifier.h"
#include "Statement.h"
#include "Token.h"
#include "Type.h"
#include <iostream>
#include <vector>

void debug(Token const &);
void debug(AST::Identifier const &);
void debug(AST::Type const &);
void debug(std::tuple<AST::Identifier, AST::Type> const &);
void debug(AST::Expression const &);
void debug(AST::Statement const &);
void debug(AST::Function::Signature const &);
void debug(AST::Function const &);

void debug(Token::Symbol);

inline void debug(std::string const &string) { std::cout << string; }

template <typename T> void debug(std::optional<T> const &value) {
	if (value.has_value())
		debug(value.value());
}

template <typename T> void debug(std::vector<T> const &vector, char const *separator = ", ") {
	for (size_t i = 0; i < vector.size(); ++i) {
		debug(vector.at(i));
		if (i != vector.size() - 1)
			std::cout << separator;
	}
}
