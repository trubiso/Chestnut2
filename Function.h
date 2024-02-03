#pragma once
#include "Identifier.h"
#include "Parser.h"
#include "Statement.h"
#include "Type.h"

namespace AST {

struct Function {
	Identifier name;
	struct Signature {
		std::optional<std::vector<Identifier>> generics;
		std::vector<std::tuple<Identifier, Type>> arguments;
		Type return_type;
	} signature;
	std::optional<Scope> body;
};

Parser::Parser<Function::Signature> function_signature();
Parser::Parser<Function> function();

} // namespace AST
