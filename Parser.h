#pragma once
#include <functional>
#include <string>

#include "Result.h"
#include "Stream.h"
#include "Token.h"

// thanks to https://github.com/Qqwy/cpp-parser_combinators

namespace Parser {

struct Error {
	Span span;
	std::string message;

	Error(Span span, std::string const &message) : span(span), message(message) {}
};

// TODO: make this more rigorous (add "expected" and remove if already present)
inline Error operator|(Error const &l, Error const &r) {
	return Error(l.span, l.message + ", " + r.message);
}

// Remember!!! A parser returning an error will leave the stream index unmodified, whereas a parser
// returning a result usually modifies the stream index.

template <typename T, typename E = Error, typename V = Token>
struct Parser : public std::function<Result<T, E>(Stream<V> &)> {
	using std::function<Result<T, E>(Stream<V> &)>::function;
	typedef T value_type;
	typedef E error_type;
	typedef Stream<V> input_type;
	typedef V inner_input_type;
};

template <typename V> Span make_span(Stream<V> const &input, size_t start_index, size_t end_index) {
	std::optional<V> start = input.at(start_index);
	std::optional<V> end = input.at(end_index);
	return Span(start.has_value() ? start.value().span.start : input.last().span.start,
	            end.has_value() ? end.value().span.end : input.last().span.end);
}

template <typename V> inline Span make_span(Stream<V> const &input) {
	return make_span(input, input.index(), input.index());
}

} // namespace Parser
