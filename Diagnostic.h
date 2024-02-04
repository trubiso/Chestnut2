#pragma once
#include "Span.h"
#include <optional>
#include <string>
#include <vector>

class Diagnostic {
public:
	enum class Severity {
		Error,   // (red)
		Warning, // (yellow)
		Note,    // (blue)
	};

	struct Label {
		Span span;
		std::optional<std::string> label = {};
		std::optional<Severity> color_override = {};

		Label(Span span, std::optional<std::string> label = {}) : span(span), label(label) {}
		void override_color(Severity new_color);
	};

	Diagnostic(Severity severity, std::string title, std::optional<std::string> subtitle = {})
	    : m_severity(severity), m_title(title), m_subtitle(subtitle) {}

	Diagnostic &add_label(Span span, std::optional<std::string> label = {});

	constexpr inline Severity severity() const { return m_severity; }

	void print(std::string const &code) const;

private:
	Severity m_severity;
	std::string m_title;
	std::optional<std::string> m_subtitle;
	std::vector<Diagnostic::Label> m_labels;
};
