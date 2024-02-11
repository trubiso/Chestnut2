#include "OutFmt.hpp"
#ifdef WINDOWS
#include <Windows.h>
#else
#include <cstdint>
#include <iostream>
#endif

namespace OutFmt {

#ifdef WINDOWS
void set_color_inner(WORD color) {
	HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h_console, color);
}

// clang-format off
void color(Color color) {
	using enum Color;
	switch (color) {
	case Black:         set_color_inner( 0); break;
	case Red:           set_color_inner( 1); break;
	case Green:         set_color_inner( 2); break;
	case Yellow:        set_color_inner( 3); break;
	case Blue:          set_color_inner( 4); break;
	case Magenta:       set_color_inner( 5); break;
	case Cyan:          set_color_inner( 6); break;
	case White:         set_color_inner( 7); break;
	case Gray:          set_color_inner( 8); break;
	case BrightRed:     set_color_inner(12); break;
	case BrightGreen:   set_color_inner(10); break;
	case BrightYellow:  set_color_inner(14); break;
	case BrightBlue:    set_color_inner( 9); break;
	case BrightMagenta: set_color_inner(13); break;
	case BrightCyan:    set_color_inner(11); break;
	case BrightWhite:   set_color_inner(15); break;
	}
}
// clang-format on
#else
void set_color_inner(uint8_t color) {
	std::cout << "\033[" << (int32_t)color << 'm';
}

// clang-format off
void color(Color color) {
	using enum Color;
	switch (color) {
	case Black:         set_color_inner(30); break;
	case Red:           set_color_inner(31); break;
	case Green:         set_color_inner(32); break;
	case Yellow:        set_color_inner(33); break;
	case Blue:          set_color_inner(34); break;
	case Magenta:       set_color_inner(35); break;
	case Cyan:          set_color_inner(36); break;
	case White:         set_color_inner(37); break;
	case Gray:          set_color_inner(90); break;
	case BrightRed:     set_color_inner(91); break;
	case BrightGreen:   set_color_inner(92); break;
	case BrightYellow:  set_color_inner(93); break;
	case BrightBlue:    set_color_inner(94); break;
	case BrightMagenta: set_color_inner(95); break;
	case BrightCyan:    set_color_inner(96); break;
	case BrightWhite:   set_color_inner(97); break;
	}
}
// clang-format on
#endif

void color_reset() { color(Color::White); }

} // namespace OutFmt
