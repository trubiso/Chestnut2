#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Diagnostic.hpp"
#include "Token.hpp"

class Lexer {
public:
	Lexer(std::string const *source) : m_source(source), m_index(0) {}

	std::optional<std::tuple<Token, std::optional<Token>>> next();
	std::vector<Token> collect_all();

	inline std::vector<Diagnostic> const &diagnostics() const { return m_diagnostics; }

private:
	constexpr inline std::optional<std::reference_wrapper<char const>> current() const {
		if (!is_index_valid()) return {};
		return m_source->at(m_index);
	}

	inline bool advance(size_t amount = 1) {
		m_index += amount;
		return is_index_valid();
	}

	constexpr inline bool is_index_valid() const { return m_index < m_source->size(); }

	constexpr static inline bool is_whitespace(char x) {
		return x == ' ' || x == '\t' || x == '\r';
	}
	static inline bool is_id_start(char x) { return std::isalpha(x) || x == '_'; }
	static inline bool is_id_nonstart(char x) { return std::isalnum(x) || x == '_'; }

	void consume_whitespace();
	void consume_identifier();
	void consume_number_literal();
	void consume_wrapped_literal(char wrap, std::string name);
	Token::Symbol consume_symbol();

	std::string const *m_source;
	size_t m_index;
	std::vector<Diagnostic> m_diagnostics;
};
