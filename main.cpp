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

void print_error(Parser::Error const &error, std::string const &code) {
	std::cout << "Error: " << error.span.start << ":" << error.span.end << " -> parser error: expected ";
	for (size_t i = 0; i < error.expected.size(); ++i) {
		std::cout << error.expected.at(i);
		if (error.expected.size() >= 2 && i < error.expected.size() - 2)
			std::cout << ", ";
		else if (error.expected.size() >= 1 && i < error.expected.size() - 1)
			std::cout << " or ";
	}
	std::cout << "; found '" << error.span.value(code) << "'" << std::endl;
}

int main(void) {
	std::string code = read_file("source");
	Lexer tokenizer(&code);
	std::vector<Token> tokens = tokenizer.collect_all();

	debug(tokens, "\n");
	std::cout << "\n" << std::endl;

	Stream<Token> token_stream{tokens};
	std::vector<Parser::Error> accumulated_errors{};

	auto parse_result = AST::program()(token_stream, accumulated_errors);
	if (bool(parse_result)) {
		debug(std::get<AST::Program>(parse_result), "\n\n");
	} else {
		auto error = std::get<Parser::Error>(parse_result);
		accumulated_errors.push_back(error);
	}

	if (accumulated_errors.size() > 0) {
		std::cout << "\n\n--- Errors ---\n";
		for (auto const &error : accumulated_errors) {
			print_error(error, code);
		}
	}

	return 0;
}