#include "Resolver.hpp"

#include <algorithm>
#include <optional>

#include "Diagnostic.hpp"
#include "Expression.hpp"
#include "Function.hpp"
#include "Identifier.hpp"
#include "Statement.hpp"
#include "Type.hpp"

namespace Resolver {

ID counter = 0;
std::vector<Diagnostic> diagnostics{};

// TODO: unify identifier to string API with debug API
std::string identifier_str(AST::Identifier const &identifier) {
	std::string identifier_str;
	switch (identifier.kind) {
	case AST::Identifier::Kind::Unqualified:
		identifier_str = std::get<std::string>(identifier.value);
		break;
	case AST::Identifier::Kind::Qualified: {
		identifier_str = "";
		auto path = std::get<AST::Identifier::QualifiedPath>(identifier.value);
		if (path.absolute) identifier_str += "::";
		for (size_t i = 0; i < path.path.size(); ++i) {
			identifier_str += path.path.at(i);
			if (i < path.path.size() - 1) identifier_str += "::";
		}
	} break;
	case AST::Identifier::Kind::Resolved:
		throw "unknown name is resolved identifier (should never happen)";
	}
	return identifier_str;
}

void unknown_name(Spanned<AST::Identifier> const &identifier, Context const &ctx) {
	std::string id = identifier_str(identifier.value);
	Diagnostic diagnostic(
	    Diagnostic::Severity::Error, "unknown symbol",
	    "identifier " + id + " was referenced, but it doesn't exist in the current scope");
	diagnostic.add_label(identifier.span);
	diagnostics.push_back(diagnostic);
}

Spanned<AST::Identifier> resolve_add(Spanned<AST::Identifier> const &identifier, Context &ctx) {
	if (identifier.value.kind == AST::Identifier::Kind::Resolved) return identifier;
	// TODO: support qualified identifier
	if (identifier.value.kind == AST::Identifier::Kind::Qualified) return identifier;
	// keep in mind, this function shadows earlier declarations of identifier
	std::string name = std::get<std::string>(identifier.value.value);
	ID new_id = counter++;
	ctx.name_to_id.insert_or_assign(name, new_id);
	ctx.id_to_name.insert_or_assign(new_id, name);
	ctx.id_to_span.insert_or_assign(new_id, identifier.span);
	return Spanned<AST::Identifier>{
	    .value = AST::Identifier{.kind = AST::Identifier::Kind::Resolved, .value = new_id},
	    .span = identifier.span};
}

AST::IdentifierWithType resolve_add(AST::IdentifierWithType const &identifier_with_type,
                                    Context &ctx) {
	return AST::IdentifierWithType{resolve_add(std::get<0>(identifier_with_type), ctx),
	                               resolve(std::get<1>(identifier_with_type), ctx)};
}

Spanned<AST::Identifier> resolve(Spanned<AST::Identifier> const &identifier, Context &ctx) {
	using enum AST::Identifier::Kind;
	switch (identifier.value.kind) {
	case Unqualified: {
		std::string name = std::get<std::string>(identifier.value.value);
		auto it = ctx.name_to_id.find(name);
		if (it == ctx.name_to_id.end()) {
			unknown_name(identifier, ctx);
			return identifier;
		}
		return Spanned<AST::Identifier>{.value =
		                                    AST::Identifier{
		                                        .kind = Resolved,
		                                        .value = it->second,
		                                    },
		                                .span = identifier.span};
	}
	case Qualified:
		// TODO: implement qualified ids. for now they're probably all unknown tho
		unknown_name(identifier, ctx);
		return identifier;
		break;
	case Resolved:
		return identifier;  // why would we resolve an already-resolved identifier
	}
	throw "";
}

Spanned<AST::Type> resolve(Spanned<AST::Type> const &type, Context &ctx) {
	using enum AST::Type::Kind;
	switch (type.value.kind) {
	case Identified:
		return Spanned<AST::Type>{
		    .value =
		        AST::Type{.kind = Identified,
		                  .value = resolve(
		                               Spanned<AST::Identifier>{
		                                   .value = std::get<AST::Identifier>(type.value.value),
		                                   .span = type.span},
		                               ctx)
		                               .value},
		    .span = type.span};
	case BuiltIn:
		return type;  // no need to resolve built in types
	case Inferred:
		return type;  // no need to resolve inferred types
	}
	throw "";
}

AST::IdentifierWithType resolve(AST::IdentifierWithType const &identifier_with_type, Context &ctx) {
	return AST::IdentifierWithType{
	    resolve(std::get<0>(identifier_with_type), ctx),
	    resolve(std::get<1>(identifier_with_type), ctx),
	};
}

AST::Expression resolve(AST::Expression const &expression, Context &ctx) {
	using enum AST::Expression::Kind;
	switch (expression.kind) {
	case CharLiteral:
	case StringLiteral:
	case NumberLiteral:
		return expression;  // literals don't need any resolution
	case Identifier:
		// identifier must already exist
		return AST::Expression{
		    .kind = Identifier,
		    .value = resolve(std::get<Spanned<AST::Identifier>>(expression.value), ctx)};
	case BinaryOperation: {
		auto binary_operation = std::get<AST::Expression::BinaryOperation>(expression.value);
		return AST::Expression{
		    .kind = BinaryOperation,
		    .value = AST::Expression::BinaryOperation{.lhs = resolve(binary_operation.lhs, ctx),
		                                              .rhs = resolve(binary_operation.rhs, ctx),
		                                              .operator_ = binary_operation.operator_}};
	}
	case UnaryOperation: {
		auto unary_operation = std::get<AST::Expression::UnaryOperation>(expression.value);
		return AST::Expression{.kind = UnaryOperation,
		                       .value = AST::Expression::UnaryOperation{
		                           .value = resolve(unary_operation.value, ctx),
		                           .operator_ = unary_operation.operator_,
		                       }};
	}
	case Call: {
		// everything must already exist in a call
		auto call = std::get<AST::Expression::Call>(expression.value);
		return AST::Expression{.kind = Call,
		                       .value = AST::Expression::Call{
		                           .callee = resolve(call.callee, ctx),
		                           .generics = resolve(call.generics, ctx),
		                           .arguments = resolve(call.arguments, ctx),
		                       }};
	}
	}
	throw "";
}

AST::Statement resolve(AST::Statement const &statement, Context &ctx) {
	switch (statement.kind) {
	case AST::Statement::Kind::Create: {
		auto create = std::get<AST::Statement::Create>(statement.value);
		// resolve value beforehand so the same identifier isn't used in the actual assignment
		create.value = resolve(create.value, ctx);
		create.type = resolve(create.type, ctx);
		create.identifier = resolve_add(create.identifier, ctx);
		return AST::Statement{.kind = statement.kind, .value = create};
	}
	case AST::Statement::Kind::Set: {
		auto set = std::get<AST::Statement::Set>(statement.value);
		set.lhs = resolve(set.lhs, ctx);
		set.rhs = resolve(set.rhs, ctx);
		return AST::Statement{.kind = statement.kind, .value = set};
	}
	case AST::Statement::Kind::BareExpression: {
		auto bare_expression = std::get<AST::Statement::BareExpression>(statement.value);
		bare_expression.expression = resolve(bare_expression.expression, ctx);
		return AST::Statement{.kind = statement.kind, .value = bare_expression};
	}
	}
	throw "";
}

AST::Function::Signature resolve(AST::Function::Signature const &signature, Context &ctx) {
	// when we resolve a function signature, we must add all the function signature's identifiers to
	// be able to resolve the function itself, as this resolution function is only called when
	// resolving functions and ctx is copied for that purpose.
	// we must first add generics, then arguments (whose types will be resolved including generics),
	// then the return type (same parenthesis as before).

	std::vector<Spanned<AST::Identifier>> transformed_generics{};
	if (signature.generics.has_value())
		std::transform(
		    signature.generics.value().value.cbegin(), signature.generics.value().value.cend(),
		    transformed_generics.begin(),
		    [&](Spanned<AST::Identifier> const &value) { return resolve_add(value, ctx); });

	std::vector<Spanned<AST::IdentifierWithType>> transformed_arguments{};
	std::transform(
	    signature.arguments.value.cbegin(), signature.arguments.value.cend(),
	    transformed_arguments.begin(),
	    [&](Spanned<AST::IdentifierWithType> const &value) { return resolve_add(value, ctx); });

	auto return_type = resolve(signature.return_type, ctx);

	return AST::Function::Signature{
	    .generics =
	        signature.generics.has_value()
	            ? (decltype(AST::Function::Signature::generics))
	                  Spanned<decltype(transformed_generics)>{.value = transformed_generics,
	                                                          .span =
	                                                              signature.generics.value().span}
	            : (decltype(AST::Function::Signature::generics))std::nullopt,
	    .arguments = Spanned<decltype(transformed_arguments)>{.value = transformed_arguments,
	                                                          .span = signature.arguments.span},
	    .return_type = return_type,
	};
}

AST::Function resolve(AST::Function const &function, Context &ctx) {
	Context inner_ctx = ctx;
	auto signature = resolve(function.signature, inner_ctx);
	auto body = resolve(function.body, inner_ctx);
	return AST::Function{
	    .name = function.name,
	    .signature = signature,
	    .body = body,
	};
}

void register_(std::vector<Spanned<AST::Function>> &functions, Context &ctx) {
	for (auto &function : functions) function.value.name = resolve_add(function.value.name, ctx);
}

std::vector<Diagnostic> resolve(AST::Program &program) {
	Context context{.program = program};
	register_(program.functions, context);
	program.functions = resolve(program.functions, context);
	return diagnostics;
}

}  // namespace Resolver
