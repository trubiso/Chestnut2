#pragma once
#include <optional>

#include "Box.hpp"
#include "Parser.hpp"
#include "Primitive.hpp"

namespace Parser {

#define lazy(parser)                                                              \
	::Parser::Parser<typename std::decay_t<decltype(parser)>::value_type,         \
	                 typename std::decay_t<decltype(parser)>::error_type,         \
	                 typename std::decay_t<decltype(parser)>::inner_input_type> { \
		[=](auto &input, auto &errors) { return (parser)(input, errors); }        \
	}

// if A == B, return A, otherwise std::variant<A, B>
template <typename A, typename B>
using or_t = std::conditional_t<std::is_same_v<A, B>, A, std::variant<A, B>>;

template <typename T, typename E, typename V, typename F>
auto transform(Parser<T, E, V> const &parser, F const &function)
    -> Parser<decltype(function(std::declval<T>())), E, V> {
	return [=](Stream<V> &input,
	           std::vector<Error> &errors) -> Result<decltype(function(std::declval<T>())), E> {
		Result<T, E> result = parser(input, errors);
		if (!bool(result)) return std::get<E>(result);
		return function(std::get<T>(result));
	};
}

template <typename T, typename E, typename V, typename F>
    requires std::is_same_v<decltype(std::declval<F>()(std::declval<T const &>())),
                            std::optional<std::string>>
Parser<T, or_t<E, Error>, V> filter(Parser<T, E, V> const &parser, F const &function) {
	return [=](Stream<V> &input, std::vector<Error> &errors) -> Result<T, or_t<E, Error>> {
		size_t original_index = input.index();
		Result<T, E> result = parser(input, errors);
		if (!bool(result)) return std::get<E>(result);
		std::optional<std::string> transformed = function(std::get<T>(result));
		if (!transformed.has_value()) return std::get<T>(result);
		Error error(make_span(input, original_index, input.index()), transformed.value());
		input.set_index(original_index);
		return error;
	};
}

template <typename T, typename E, typename V>
Parser<std::vector<T>, E, V> many(Parser<T, E, V> const &parser) {
	return [=](Stream<V> &input, std::vector<Error> &errors) -> Result<std::vector<T>, E> {
		std::vector<T> elements{};
		while (true) {
			Result<T, E> element = parser(input, errors);
			if (!bool(element)) return elements;
			elements.push_back(std::get<T>(element));
		}
	};
}

template <typename T, typename E, typename V>
inline Parser<std::vector<T>, or_t<E, Error>, V> at_least(Parser<T, E, V> const &parser,
                                                          size_t minimum,
                                                          std::string error_message) {
	if (minimum == 0) return many(parser);
	return filter(many(parser), [=](std::vector<T> const &element) -> std::optional<std::string> {
		if (element.size() >= minimum) return {};
		return error_message;
	});
}

template <typename Ta, typename Ea, typename Tb, typename Eb, typename V>
Parser<std::vector<Ta>, Ea, V> separated_no_trailing_at_least(Parser<Ta, Ea, V> const &parser,
                                                              Parser<Tb, Eb, V> const &separator,
                                                              size_t minimum,
                                                              std::string error_message) {
	return transform(parser & at_least(separator >> parser, minimum, error_message),
	                 [](std::tuple<Ta, std::vector<Ta>> const &data) {
		                 std::vector<Ta> new_data = std::get<1>(data);
		                 new_data.insert(new_data.begin(), std::get<0>(data));
		                 return new_data;
	                 });
}

template <typename Ta, typename Ea, typename Tb, typename Eb, typename V>
Parser<std::vector<Ta>, Ea, V> separated_no_trailing(Parser<Ta, Ea, V> const &parser,
                                                     Parser<Tb, Eb, V> const &separator) {
	return separated_no_trailing_at_least(parser, separator, 0, "this error shouldn't occur");
}

template <typename Ta, typename Ea, typename Tb, typename Eb, typename V>
inline Parser<std::vector<Ta>, Ea, V> separated(Parser<Ta, Ea, V> const &parser,
                                                Parser<Tb, Eb, V> const &separator, size_t minimum,
                                                std::string error_message) {
	return separated_no_trailing_at_least(parser, separator, minimum, error_message)
	       << optional(separator);
}

template <typename Ta, typename Ea, typename Tb, typename Eb, typename V>
inline Parser<std::vector<Ta>, Ea, V> separated(Parser<Ta, Ea, V> const &parser,
                                                Parser<Tb, Eb, V> const &separator) {
	return separated_no_trailing(parser, separator) << optional(separator);
}

// a & b, extract element N from the resulting tuple
template <typename Ta, typename Tb, typename Ea, typename Eb, typename V, size_t N>
Parser<std::conditional_t<N == 0, Ta, Tb>, or_t<Ea, Eb>, V> join_get(Parser<Ta, Ea, V> const &a,
                                                                     Parser<Tb, Eb, V> const &b) {
	return transform(a & b, [](std::tuple<Ta, Tb> const &value) { return std::get<N>(value); });
}

// for convenience, << and >> are shorthands for join_get<N = 0> and join_get<N = 1> respectively.

template <typename Ta, typename Tb, typename Ea, typename Eb, typename V>
inline Parser<Ta, or_t<Ea, Eb>, V> operator<<(Parser<Ta, Ea, V> const &a,
                                              Parser<Tb, Eb, V> const &b) {
	return join_get<Ta, Tb, Ea, Eb, V, 0>(a, b);
}

template <typename Ta, typename Tb, typename Ea, typename Eb, typename V>
inline Parser<Tb, or_t<Ea, Eb>, V> operator>>(Parser<Ta, Ea, V> const &a,
                                              Parser<Tb, Eb, V> const &b) {
	return join_get<Ta, Tb, Ea, Eb, V, 1>(a, b);
}

// run A, if it fails, run B, if both fail, return combined error, if one succeeds, return its
// return value.
template <typename Ta, typename Tb, typename Ea, typename Eb, typename V>
Parser<or_t<Ta, Tb>, or_t<Ea, Eb>, V> operator|(Parser<Ta, Ea, V> const &a,
                                                Parser<Tb, Eb, V> const &b) {
	return [=](Stream<V> &input, std::vector<Error> &errors) -> Result<or_t<Ta, Tb>, or_t<Ea, Eb>> {
		Result<Ta, Ea> result_a = a(input, errors);
		if (bool(result_a)) return std::get<Ta>(result_a);
		Result<Tb, Eb> result_b = b(input, errors);
		if (bool(result_b)) return std::get<Tb>(result_b);
		return std::get<Ea>(result_a) | std::get<Eb>(result_b);
	};
}

// run A then B, any failure will abort the entire operation
template <typename Ta, typename Tb, typename Ea, typename Eb, typename V>
Parser<std::tuple<Ta, Tb>, or_t<Ea, Eb>, V> operator&(Parser<Ta, Ea, V> const &a,
                                                      Parser<Tb, Eb, V> const &b) {
	return [=](Stream<V> &input,
	           std::vector<Error> &errors) -> Result<std::tuple<Ta, Tb>, or_t<Ea, Eb>> {
		size_t original_index = input.index();
		Result<Ta, Ea> result_a = a(input, errors);
		if (!bool(result_a)) return std::get<Ea>(result_a);
		Result<Tb, Eb> result_b = b(input, errors);
		if (!bool(result_b)) {
			input.set_index(original_index);
			return std::get<Eb>(result_b);
		}
		return std::tuple<Ta, Tb>{std::get<Ta>(result_a), std::get<Tb>(result_b)};
	};
}

// if the parser fails, it is added to accumulated errors
template <typename T, typename E, typename V> Parser<T, E, V> must(Parser<T, E, V> const &parser) {
	return [=](Stream<V> &input, std::vector<Error> &errors) {
		Result<T, E> result = parser(input, errors);
		if (!bool(result)) {
			errors.push_back((Error)std::get<E>(result));
		}
		return result;
	};
}

// TODO: validate (adds warnings instead of errors)

template <typename T, typename E, typename V>
Parser<Spanned<T>, E, V> spanned(Parser<T, E, V> const &parser) {
	return [=](Stream<V> &input, std::vector<Error> &errors) -> Result<Spanned<T>, E> {
		size_t start = input.index();
		Result<T, E> result = parser(input, errors);
		if (!bool(result)) return std::get<E>(result);
		size_t end = input.index();
		return Spanned<T>{.value = std::get<T>(result), .span = Span(start, end)};
	};
}

template <typename T, typename E, typename V>
Parser<Box<T>, E, V> boxed(Parser<T, E, V> const &parser) {
	return transform(parser, [](auto x) -> Box<T> { return x; });
}

template <typename T, typename E, typename V>
Parser<std::optional<T>, E, V> as_optional(Parser<T, E, V> const &parser) {
	return transform(parser, [](auto x) -> std::optional<decltype(x)> { return x; });
}

template <typename T, typename E, typename V>
Parser<std::optional<T>, E, V> optional(Parser<T, E, V> const &parser) {
	return as_optional(parser) | constant<std::optional<T>, E, V>(std::nullopt);
}

}  // namespace Parser
