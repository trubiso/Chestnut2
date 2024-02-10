#include "Debug.h"
#include "Lexer.h"
#include "Program.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string read_file(char const *path) noexcept {
	std::string content;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		file.open(path);
		std::stringstream str;
		str << file.rdbuf();
		file.close();
		content = str.str();
	} catch (std::ifstream::failure const &err) {
		// TODO: proper logging
		std::cout << "[ERROR] Couldn't read file " << path << "\n" << err.what() << std::endl;
	}

	return content;
}

Diagnostic error_cast(Parser::Error const &error, std::string const &code) {
	std::string error_title = "expected ";
	std::vector<std::string> expected(error.expected.begin(), error.expected.end());
	for (size_t i = 0; i < expected.size(); ++i) {
		error_title += expected[i];
		if (expected.size() >= 2 && i < expected.size() - 2)
			error_title += ", ";
		else if (expected.size() >= 1 && i < expected.size() - 1)
			error_title += " or ";
	}
	Diagnostic diag(Diagnostic::Severity::Error, error_title, "program couldn't be parsed");
	diag.add_label(error.span);
	return diag;
}

int main(void) {
	std::string code = read_file("source");

	// Lexing
	Lexer lexer(&code);
	std::vector<Diagnostic> diagnostics{};
	std::vector<Token> tokens = lexer.collect_all();

	for (auto const &diagnostic : lexer.diagnostics()) {
		diagnostics.push_back(diagnostic);
	}

	// Parsing
	Stream<Token> token_stream{tokens};
	std::vector<Parser::Error> accumulated_errors{};

	auto parse_result = AST::program()(token_stream, accumulated_errors);
	if (!bool(parse_result)) {
		auto error = std::get<Parser::Error>(parse_result);
		accumulated_errors.push_back(error);
	} else {
		debug(std::get<AST::Program>(parse_result));
	}

	if (accumulated_errors.size() > 0) {
		for (auto const &error : accumulated_errors) {
			diagnostics.push_back(error_cast(error, code));
		}
	}

	// TODO: Checker (checks the case of variables, checks function overloads)
	// TODO: Macros (compile-time functions that turn AST into AST? or special syntax?)
	// TODO: Imports

	// ---- AST lowering
	// TODO: Expression lowering
	// TODO: Resolver (turns named identifiers and qualified identifiers into numbers)
	// TODO: Type inference, coercion and typechecking

	// ---- Analysis
	// TODO: reference checking (no refs to temporary values, etc)
	// TODO: RAII (insert defer drops based on ownership)
	// TODO: Moving/copying
	// TODO: if, switch, while, return, etc. analysis
	// TODO: defer
	// TODO: check if mutability of variables is being violated

	// ---- Codegen
	// TODO: codegen to LLVM IR
	// TODO: inlining

	for (auto const &diagnostic : diagnostics) {
		diagnostic.print(code);
	}

	return 0;
}