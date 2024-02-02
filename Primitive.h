#pragma once
#include "Parser.h"

namespace Parser {

template <typename T, typename E = Error, typename V = Token>
inline Parser<T, E, V> constant(T const &value) {
	return [=](Stream<V> &) { return value; };
}

template <typename T = Token, typename E = Error, typename V = Token>
Parser<T, E, V> satisfy(std::function<bool(V const &)> check, std::string error_message) {
	return [=](Stream<V> &input) -> Result<T, E> {
		std::optional<T> value = input.peek();
		if (!value.has_value())
			return Error{.span = make_span(input), .message = "unexpected EOF"};
		if (check(value.value())) {
			input.ignore();
			return value.value();
		} else {
			return Error{.span = make_span(input), .message = error_message};
		}
	};
}

template <typename T, typename E, typename V>
Parser<std::optional<T>, E, V> optional(Parser<T, E, V> const &parser) {
	return transform(parser, [](auto x) -> std::optional<decltype(x)> { return x; }) |
	       constant<std::optional<T>, E, V>(std::nullopt);
}

} // namespace Parser
