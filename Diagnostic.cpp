#include "Diagnostic.h"
#include "OutFmt.h"
#include "VariantName.h"
#include <iostream>

#define TAB_WIDTH 4

void Diagnostic::Label::override_color(Diagnostic::Severity new_color) {
	color_override = new_color;
}

Diagnostic &Diagnostic::add_label(Span span, std::optional<std::string> label) {
	m_labels.push_back(Label(span, std::move(label)));
	return *this;
}

constexpr OutFmt::Color severity_color(Diagnostic::Severity severity) {
	using enum Diagnostic::Severity;
	switch (severity) {
	case Error:
		return OutFmt::Color::BrightRed;
	case Warning:
		return OutFmt::Color::BrightYellow;
	case Note:
		return OutFmt::Color::BrightCyan;
	}
}

// returns {line_number, line_index}
std::tuple<size_t, size_t> loc(std::string const &code, size_t idx, bool look_for_next = false) {
	size_t line_number = 1;
	size_t line_index = 0;
	for (size_t i = 0; i < std::min(idx, code.size()); ++i) {
		if (code.at(i) == '\n') {
			line_number++;
			line_index = i + 1; // skip the newline
		}
	}
	if (look_for_next) {
		size_t old_line_index = line_index;
		for (size_t i = idx + 1; i < code.size(); ++i) {
			if (code.at(i) == '\n') {
				line_index = i;
				break;
			}
		}
		if (old_line_index == line_index)
			line_index = code.size();
	}
	return {line_number, line_index};
}

inline constexpr size_t number_size(size_t x) { return std::to_string(x).size(); }

void print_loc_line(size_t loc_pad, std::optional<size_t> loc_current = {}) {
	OutFmt::color(OutFmt::Color::Gray);
	if (!loc_current.has_value()) {
		std::cout << std::string(loc_pad + 1, ' ');
	} else {
		auto size = number_size(loc_current.value());
		if (size <= loc_pad + 1)
			std::cout << std::string(loc_pad + 1 - size, ' ');
		std::cout << loc_current.value();
	}
	std::cout << " | ";
	OutFmt::color_reset();
}

void print_labels(std::vector<Diagnostic::Label> const &labels, std::string const &code,
                  Diagnostic::Severity base_severity) {
	size_t start_l = SIZE_MAX;
	size_t end_l = 0;
	for (Diagnostic::Label const &label : labels) {
		if (label.span.start < start_l)
			start_l = label.span.start;
		if (label.span.end > end_l)
			end_l = label.span.end;
	}

	auto [loc_start, start_index] = loc(code, start_l);
	auto [loc_end, end_index] = loc(code, end_l - 1, true);
	size_t loc_pad = number_size(loc_end);
	size_t loc_current = loc_start;

	std::cout << '\n';
	print_loc_line(loc_pad);
	std::cout << '\n';

	for (size_t i = start_index; i < end_index; ++i) {
		if (i == start_index || code.at(i) == '\n') {
			if (i != start_index)
				std::cout << '\n';
			if (code.at(i) == '\n')
				++loc_current;
			std::string loc_current_str = std::to_string(loc_current);
			print_loc_line(loc_pad, loc_current);
		}

		if (code.at(i) != '\n') {
			if (code.at(i) == '\t') {
				std::cout << std::string(TAB_WIDTH, ' ');
				continue;
			}

			OutFmt::color(OutFmt::Color::BrightWhite);
			for (Diagnostic::Label const &label : labels)
				if (i >= label.span.start && i < label.span.end)
					OutFmt::color(severity_color(label.color_override.value_or(base_severity)));
			std::cout << code.at(i);
			OutFmt::color_reset();
		}
	}

	std::cout << '\n';
	print_loc_line(loc_pad);
	std::cout << "\n\n";
}

void Diagnostic::print(std::string const &code) const {
	// TODO: maybe use bold/italics

	// 1. Print severity
	OutFmt::color(severity_color(m_severity));
	std::cout << get_variant_name(m_severity) << ":";
	OutFmt::color_reset();
	std::cout << " ";

	// 2. Print title
	OutFmt::color(OutFmt::Color::BrightWhite);
	std::cout << m_title << "\n";
	OutFmt::color_reset();

	// 3. Print subtitle (if it exists)
	if (m_subtitle.has_value()) {
		OutFmt::color(OutFmt::Color::White);
		std::cout << m_subtitle.value() << "\n";
		OutFmt::color_reset();
	}

	// 4. Labels
	if (!m_labels.empty()) {
		print_labels(m_labels, code, m_severity);
	}

	std::cout << std::endl;
}
