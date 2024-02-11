#pragma once
#include <optional>
#include <vector>

template <typename T> class Stream {
public:
	Stream(std::vector<T> data) : m_data(data), m_index(0) {}

	constexpr inline std::optional<T> at(size_t index) const {
		if (index >= m_data.size()) return {};
		return m_data.at(index);
	}

	constexpr inline std::optional<T> peek() const { return at(m_index); }

	inline void ignore() { m_index++; }

	inline T const &last() const { return m_data.at(m_data.size() - 1); }

	inline size_t index() const { return m_index; }

	inline void set_index(size_t new_index) { m_index = new_index; }

private:
	std::vector<T> m_data;
	size_t m_index;
};
