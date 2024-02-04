#pragma once
#include "Identifier.h"
#include "Parser.h"
#include "Statement.h"
#include "Type.h"

namespace AST {

struct Function {
	struct Signature {
		std::optional<Spanned<std::vector<Spanned<Identifier>>>> generics;
		Spanned<std::vector<Spanned<IdentifierWithType>>> arguments;
		Spanned<Type> return_type;
	};

	Spanned<Identifier> name;
	Spanned<Signature> signature;
	std::optional<Spanned<Scope>> body;
};

Parser::Parser<Function::Signature> function_signature();
Parser::Parser<Function> function();

} // namespace AST
