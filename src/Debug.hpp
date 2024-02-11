#pragma once
#include <iostream>
#include <vector>

#include "Expression.hpp"
#include "Function.hpp"
#include "Identifier.hpp"
#include "Program.hpp"
#include "Statement.hpp"
#include "Token.hpp"
#include "Type.hpp"

// TODO: for debugging statements, allow some kind of pre-\t

void debug(Token const &);
void debug(AST::Identifier const &);
void debug(AST::Type const &);
void debug(AST::IdentifierWithType const &);
void debug(AST::Expression const &);
void debug(AST::Statement const &);
void debug(AST::Function::Signature const &);
void debug(AST::Function const &);
void debug(AST::Program const &);

void debug(Token::Symbol);

inline void debug(std::string const &string) { std::cout << string; }

template <typename T> void debug(std::optional<T> const &value) {
	if (value.has_value()) debug(value.value());
}

template <typename T> inline void debug(Spanned<T> const &value) { debug(value.value); }

template <typename T> void debug(std::vector<T> const &vector, char const *separator = ", ") {
	for (size_t i = 0; i < vector.size(); ++i) {
		debug(vector.at(i));
		if (i != vector.size() - 1) std::cout << separator;
	}
}
