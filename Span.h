#pragma once

#include <cstdint>
#include <string>

struct Span {
	size_t start;
	size_t end;

	Span(size_t start, size_t end) : start(start), end(end) {}
	Span(size_t end) : start(0), end(end) {}

	inline constexpr std::string value(std::string const &source) const {
		return source.substr(start, end - start);
	}
};
