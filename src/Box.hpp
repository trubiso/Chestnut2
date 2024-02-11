#pragma once
#include <memory>

// thanks to https://www.foonathan.net/2022/05/recursive-variant-box/

template <typename T> class Box {
public:
	// Automatic construction from a `T`, not a `T*`.
	Box(T &&obj) : underlying(new T(std::move(obj))) {}
	Box(T const &obj) : underlying(new T(obj)) {}

	// Copy constructor copies `T`.
	Box(Box const &other) : Box(*other.underlying) {}
	Box &operator=(Box const &other) {
		*underlying = *other.underlying;
		return *this;
	}

	// unique_ptr destroys `T` for us.
	~Box() = default;

	// Access propagates constness.
	T &operator*() { return *underlying; }
	T const &operator*() const { return *underlying; }

	T *operator->() { return underlying.get(); }
	T const *operator->() const { return underlying.get(); }

private:
	std::unique_ptr<T> underlying;
};
