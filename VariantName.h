#pragma once
#include "Diagnostic.h"
#include "Token.h"

char const *get_variant_name(Token::Kind);
char const *get_variant_name(Token::Keyword);
char const *get_variant_name(Token::Symbol);
char const *get_variant_name(Diagnostic::Severity);
