#include "Debug.h"
#include "Tokenizer.h"
#include "Identifier.h"
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

	auto parse_result = AST::identifier()(token_stream);
	if (bool(parse_result)) {
		debug(std::get<AST::Identifier>(parse_result));
	} else {
		std::cout << "Error" << std::endl;
	}

	return 0;
}