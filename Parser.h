#pragma once
#include <functional>
#include <string>

#include "Result.h"
#include "Stream.h"
#include "Token.h"

namespace Parser {

struct Error {
	Span span;
	std::string message;
};

// thanks to https://github.com/Qqwy/cpp-parser_combinators
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
	return Span{.start = start.has_value() ? start.value().span.start : input.last().span.start,
	            .end = end.has_value() ? end.value().span.end : input.last().span.end};
}

} // namespace Parser
