#include "Debug.h"
#include "Tokenizer.h"
#include "Type.h"
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

int main(void) {
	std::string code = read_file("source");
	Tokenizer tokenizer(&code);
	std::vector<Token> tokens = tokenizer.collect_all();

	debug(tokens, "\n");
	std::cout << std::endl;

	Stream<Token> token_stream{tokens};

	auto parse_result = AST::type()(token_stream);
	if (bool(parse_result)) {
		debug(std::get<AST::Type>(parse_result));
	} else {
		auto error = std::get<Parser::Error>(parse_result);
		std::cout << "Error: " << error.span.start << ":" << error.span.end << " -> "
		          << error.message << std::endl;
	}

	return 0;
}