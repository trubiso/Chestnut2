#include <algorithm>
#include <map>
#include <unordered_map>

#include "Box.hpp"
#include "Diagnostic.hpp"
#include "Program.hpp"
#include "Type.hpp"

namespace Resolver {

typedef size_t ID;

struct Context {
	std::map<std::string, ID> name_to_id{};
	std::map<std::string, std::vector<ID>> name_to_id_funcs{};
	std::unordered_map<ID, std::string> id_to_name{};
	std::unordered_map<ID, Span> id_to_span{};
	AST::Program const &program;
};

Spanned<AST::Identifier> resolve(Spanned<AST::Identifier> const &, Context &);
Spanned<AST::Type> resolve(Spanned<AST::Type> const &, Context &);

template <typename T> Spanned<T> resolve(Spanned<T> const &x, Context &ctx) {
	return Spanned<T>{.value = resolve(x.value, ctx), .span = x.span};
}

template <typename T> std::vector<T> resolve(std::vector<T> const &x, Context &ctx) {
	std::vector<T> new_vector{};
	for (T const &e : x) {
		new_vector.push_back(resolve(e, ctx));
	}
	return new_vector;
}

template <typename T> std::optional<T> resolve(std::optional<T> const &x, Context &ctx) {
	if (x.has_value())
		return resolve(x.value(), ctx);
	else
		return {};
}

template <typename T> Box<T> resolve(Box<T> const &x, Context &ctx) { return resolve(*x, ctx); }

Spanned<AST::Identifier> resolve_add(Spanned<AST::Identifier> const &, Context &,
                                     bool func_id = false);
AST::IdentifierWithType resolve_add(AST::IdentifierWithType const &, Context &);
inline Spanned<AST::IdentifierWithType> resolve_add(Spanned<AST::IdentifierWithType> const &iwt,
                                                    Context &ctx) {
	return Spanned<AST::IdentifierWithType>{.value = resolve_add(iwt.value, ctx), .span = iwt.span};
};

AST::IdentifierWithType resolve(AST::IdentifierWithType const &, Context &);
AST::Expression resolve(AST::Expression const &, Context &);
AST::Statement resolve(AST::Statement const &, Context &);
AST::Function::Signature resolve(AST::Function::Signature const &, Context &);
AST::Function resolve(AST::Function const &, Context &);

void register_(std::vector<Spanned<AST::Function>> &, Context &);
[[nodiscard]] std::vector<Diagnostic> resolve(AST::Program &);

}  // namespace Resolver
