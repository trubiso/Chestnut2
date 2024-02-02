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

template <typename T, typename E, typename V>
inline Parser<std::vector<T>, E, V> at_least(Parser<T, E, V> const &parser, size_t minimum,
                                             std::string error_message) {
	return filter(many(parser), [=](std::vector<T> const &element) {
		if (element.size() >= minimum)
			return std::nullopt;
		return (std::optional<std::string>)error_message;
	});
}

template <typename Ta, typename Ea, typename Tb, typename Eb, typename V>
inline Parser<std::vector<Ta>, Ea, V> separated(Parser<Ta, Ea, V> const &parser,
                                                Parser<Tb, Eb, V> const &separator) {
	return separated_no_trailing(parser, separator) << optional(separator);
}

template <typename Ta, typename Ea, typename Tb, typename Eb, typename V>
Parser<std::vector<Ta>, Ea, V> separated_no_trailing(Parser<Ta, Ea, V> const &parser,
                                                     Parser<Tb, Eb, V> const &separator) {
	return transform(parser & many(separator >> parser),
	                 [](std::tuple<Ta, std::vector<Ta>> const &data) {
		                 std::vector<Ta> new_data = std::get<1>(data);
		                 new_data.insert(new_data.begin(), std::get<0>(data));
		                 return new_data;
	                 });
}

// if A == B, return A, otherwise std::variant<A, B>
template <typename A, typename B>
using or_t = std::conditional_t<std::is_same_v<A, B>, A, std::variant<A, B>>;

// a & b, extract element N from the resulting tuple
template <typename Ta, typename Tb, typename Ea, typename Eb, typename V, size_t N>
Parser<std::conditional_t<N == 0, Ta, Tb>, or_t<Ea, Eb>, V> join_get(Parser<Ta, Ea, V> const &a,
                                                                     Parser<Tb, Eb, V> const &b) {
	return transform(a & b, [](std::tuple<T1, T2> const &value) { return std::get<N>(value); });
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
	return [=](Stream<V> &input) -> Result<or_t<Ta, Tb>, or_t<Ea, Eb>> {
		Result<Ta, Ea> result_a = a(input);
		if (bool(result_a))
			return std::get<Ta>(result_a);
		Result<Tb, Eb> result_b = b(input);
		if (bool(result_b))
			return std::get<Tb>(result_b);
		return Error{.span = make_span(input),
		             .message = std::get<Ea>(result_a) | std::get<Eb>(result_b)};
	}
}

// run A then B, any failure will abort the entire operation
template <typename Ta, typename Tb, typename Ea, typename Eb, typename V>
Parser<std::tuple<Ta, Tb>, or_t<Ea, Eb>, V> operator&(Parser<Ta, Ea, V> const &a,
                                                      Parser<Tb, Eb, V> const &b) {
	return [=](Stream<V> &input) -> Result<std::tuple<Ta, Tb>, or_t<Ea, Eb>> {
		size_t original_index = input.index();
		Result<Ta, Ea> result_a = a(input);
		if (!bool(result_a))
			return std::get<Ea>(result_a);
		Result<Tb, Eb> result_b = b(input);
		if (!bool(result_b)) {
			input.set_index(original_index);
			return std::get<Eb>(result_b);
		}
		return {std::get<Ta>(result_a), std::get<Tb>(result_b)};
	}
}

// TODO: validate (adds warnings instead of errors)

} // namespace Parser