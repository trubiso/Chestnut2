#pragma once
#include "Parser.h"
#include <optional>

namespace Parser {

#define lazy(parser)                                                                               \
	Parser<typename std::decay_t<decltype(parser)>::value_type,                                    \
	       typename std::decay_t<decltype(parser)>::error_type,                                    \
	       typename std::decay_t<decltype(parser)>::input_type> {                                  \
		[=](auto &input) { return (parser)(input); }                                               \
	}

template <typename T, typename E, typename V, typename F>
auto transform(Parser<T, E, V> const &parser, F const &function)
    -> Parser<decltype(function(std::declval<T>())), E, V> {
	return [=](Stream<V> &input) -> Result<decltype(function(std::declval<T>())), E> {
		Result<T, E> result = parser(input);
		if (!bool(result))
			return std::get<E>(result);
		return function(std::get<T>(result));
	};
}

template <typename T, typename E, typename V, typename F>
    requires std::is_same_v<decltype(std::declval<F>()(std::declval<T const &>())),
                            std::optional<std::string>>
Parser<T, E, V> filter(Parser<T, E, V> const &parser, F const &function) {
	return [=](Stream<V> &input) {
		size_t original_index = input.index();
		Result<T, E> result = parser(input);
		if (!bool(result))
			return std::get<E>(result);
		std::optional<std::string> transformed = function(std::get<T>(result));
		if (!transformed.has_value())
			return std::get<T>(result);
		Error error{.span = make_span(input, original_index, input.index()),
		            .message = transformed.value()};
		input.set_index(original_index);
		return error;
	};
}

template <typename T, typename E, typename V>
Parser<std::vector<T>, E, V> many(Parser<T, E, V> const &parser) {
	return [=](Stream<V> &input) -> Result<std::vector<T>, E> {
		std::vector<T> elements{};
		while (true) {
			Result<T, E> element = parser(input);
			if (!bool(element))
				return elements;
			elements.push_back(std::get<T>(element));
		}
	};
}

// TODO: validate (adds warnings instead of errors)

} // namespace Parser