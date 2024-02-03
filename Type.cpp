#include "Type.h"
#include "Combinator.h"
#include "Primitive.h"

namespace AST {

Parser::Parser<Type> type_identified() {
	using namespace Parser;
	return transform(identifier_qualified(), [](Identifier const &identifier) {
		return Type{.kind = Type::Kind::Identified, .value = identifier};
	});
}

Parser::Parser<Type> type_inferred() {
	using namespace Parser;
	return filter(identifier_qualified(),
	              [](Identifier const &identifier) -> std::optional<std::string> {
		              if (identifier.kind != Identifier::Kind::Unqualified ||
		                  std::get<std::string>(identifier.value) != "_")
			              return "expected discarded type";
		              return {};
	              }) >>
	       constant(Type{.kind = Type::Kind::Inferred, .value = std::monostate{}});
}

Parser::Parser<Type> type() { return type_inferred() | type_identified(); }

} // namespace AST
