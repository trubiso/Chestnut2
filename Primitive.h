#pragma once
#include "Parser.h"
#include <type_traits>

namespace Parser {

template <typename T, typename E = Error, typename V = Token>
inline Parser<T, E, V> constant(T const &value) {
	return [=](Stream<V> &) { return value; };
}

// F should return bool
template <typename T = Token, typename E = Error, typename V = Token, typename F>
Parser<T, E, V> satisfy(F const &check, std::string error_message) {
	return [=](Stream<V> &input) -> Result<T, E> {
		std::optional<T> value = input.peek();
		if (!value.has_value())
			return Error(make_span(input), "unexpected EOF");
		if (check(value.value())) {
			input.ignore();
			return value.value();
		} else {
			return Error(make_span(input), error_message);
		}
	};
}

} // namespace Parser
