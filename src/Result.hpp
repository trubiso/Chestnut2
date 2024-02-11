#pragma once
#include <variant>

template <typename V, typename E> struct Result : public std::variant<V, E> {
	using std::variant<V, E>::variant;

	typedef V value_type;
	typedef E error_type;

	explicit constexpr operator bool() const { return std::holds_alternative<value_type>(*this); }
};
