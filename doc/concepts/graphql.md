# GraphQL Module {#graphql}

This document is the design for a GraphQL module in Platinum. It is a
framework concept, not an application sketch. The module exposes C++
objects through a GraphQL-shaped contract while the objects themselves
stay in the C++ runtime. It does not generate C++ from a schema file
and it does not depend on Pt-Remoting.

The work is staged. Stage 0 changes existing modules so the later
GraphQL types have something solid to bind to. Stages 1 to 4 are the
module itself. Stages 5 and 6 are optional extensions.

This chapter covers:

- [Purpose](#graphql-purpose)
- [Principles](#graphql-principles)
- [Existing patterns](#graphql-patterns)
- [Object model](#graphql-model)
- [Stages](#graphql-stages)
- [Stage 0 - Preparatory work](#graphql-stage0)
- [Stage 1 - Schema graph and SDL](#graphql-stage1)
- [Stage 2 - Structured query and field execution](#graphql-stage2)
- [Stage 3 - Nested fields, lists, and arguments](#graphql-stage3)
- [Stage 4 - HTTP service, errors, and introspection](#graphql-stage4)
- [Stage 5 - Mutations](#graphql-stage5)
- [Stage 6 - Query text and subscriptions](#graphql-stage6)
- [Module layout](#graphql-layout)
- [Out of scope](#graphql-scope)

## Purpose {#graphql-purpose}

GraphQL lets a client name the fields it wants and receive a document
in that shape. In Pt the source of those fields is a live C++ object
graph, described by Pt-Reflex and reached through `Pt::Any`. The
public contract is a separate in-memory schema. That split is the same
split SOAP already makes between `ServiceDeclaration` and the C++
operations that implement it.

The first useful result is not a full GraphQL server for Apollo or
Relay. It is a schema that can print SDL, an executor that walks a
structured selection tree, and an HTTP endpoint that accepts that tree
as JSON. A text parser for the GraphQL language comes later, when the
execution model is already stable.

## Principles {#graphql-principles}

Data stays in C++. The schema does not own application objects. It
holds names, GraphQL types, nullability, arguments, and a binding to a
Reflex property, a Reflex method, or a caller-supplied resolve
function.

Reflex is not the API. Lua may see every reflected member. GraphQL
sees only members the application puts on the schema. A new C++ field
that is not declared on the schema is invisible to old clients. That
is the compatibility story, not a filter that drops unknown JSON keys
after a full serialize.

Execution is per field. The engine does not load a complete aggregate
and then discard members. A parent field yields an `Any`. Child fields
read that `Any`. When the `Any` is a reference (`Any::isRef()`), the
child walks the same instance. When the `Any` holds a copy, the child
walks the copy. Getters that return `T&` or `T*` already produce a
reference `Any` through `ReturnTraits`.

Remoting is optional. Root fields may be free functions, object
methods, or later a remoting procedure. The executor talks to a
resolve callback and to Reflex. It does not require
`Pt::Remoting::ServiceDefinition`.

No code generator. The schema is built in C++ at process start, the
same way SOAP builds operations and MCP builds tool catalogs.

## Existing patterns {#graphql-patterns}

### SOAP code-first WSDL

`Pt::Soap::ServiceDeclaration` is the closest ancestor. It owns a
graph of `Type`, `Parameter`, and `Operation`. Simple types are
process-wide. Complex types are application-owned and must outlive the
declaration. Parameters carry occurrence. `toWsdl()` writes the
external contract from that graph. The C++ implementation lives
elsewhere.

GraphQL repeats that shape: schema types and fields in memory, SDL as
the export, resolve functions as the implementation. SOAP occurrence
becomes GraphQL list and non-null wrappers. SOAP operations become
root fields on `Query` and later `Mutation`.

### MCP type graph

`Pt::Mcp::Type` already has object, array, enum, nullable, and named
properties with a required flag, plus `toSchema()`. GraphQL needs a
richer type system (interfaces, unions, input versus output, field
arguments), so it does not reuse the MCP classes. It reuses the
ownership rule: the schema does not own the C++ types it names.

### Reflex and Any

`Pt::Reflex::Type` lists properties and methods. `PropertyInfo::get`
returns `Pt::Any`. `MethodInfo::call` returns `Pt::Any` and takes an
`ArgumentList`. `Any` stores either a copy or a pointer. Pointer
construction is a reference: the pointed-to object must outlive the
`Any`. `type()` on a reference `Any` is the pointed-to type, not
`T*`.

That is enough to walk an object graph without copying every node, but
only if property getters can return references and if collections can
be iterated without turning them into an opaque value. Stage 0 adds
those Reflex capabilities. GraphQL then binds to them.

### HTTP and JSON

`Pt::Http::Service` and `Pt::Json` already move documents. GraphQL
over HTTP is a POST of JSON, not JSON-RPC and not SOAP. Stage 4 adds a
GraphQL `HttpService`. It does not extend `JsonRpc::HttpService`.

## Object model {#graphql-model}

```
Application objects
        |
        | Reflex Type / Property / Method
        | Any (copy or reference)
        v
Graphql::Schema ---- Field ---- Type (Named, List, NonNull)
        |              |
        |              +-- Argument
        |              +-- Resolve (property, method, or function)
        v
SDL text, introspection, HTTP JSON
```

`Schema` owns named types and the root `Query` type. Each `Field` has
a GraphQL name that may differ from the Reflex member name. Each field
has exactly one resolve binding. Unbound fields are a declaration
error at schema freeze, not at request time.

A request is a selection tree: field name, optional alias, argument
map, child selections. Stage 2 accepts that tree as JSON. Stage 6 may
parse the same tree from GraphQL text. The executor does not care how
the tree was built.

The result is a JSON object with `data` and, when needed, `errors`.
A failed child field becomes `null` at that path and an error entry.
It does not abort siblings unless the schema marks the field non-null
and the null bubbles to a non-null parent.

## Stages {#graphql-stages}

Each stage has a stop condition. The next stage does not start until
that condition is met. Stage 0 ships in the modules it touches. Stages
1 to 4 ship as `Pt-Graphql`. Stages 5 and 6 stay out of the first
release of that module if time is short.

| Stage | Ships in | Stop condition |
|---|---|---|
| 0 | Reflex, tests | Read-only and reference properties; collection walk; member metadata |
| 1 | Graphql | Schema graph prints SDL |
| 2 | Graphql | JSON selection tree resolves scalars on one object |
| 3 | Graphql | Nested objects, lists, field arguments |
| 4 | Graphql | HTTP endpoint, partial errors, introspection |
| 5 | Graphql | Mutations |
| 6 | Graphql | Query text parser; later subscriptions |

## Stage 0 - Preparatory work {#graphql-stage0}

GraphQL is not added yet. The gaps are in Reflex. Filling them helps
Lua and any other dynamic facade as well. That is why they land first
and stay in `include/Pt/Reflex` and `src/Pt`.

### Read-only properties

`Type::registerProperty` and `Property<C, T>` require a getter and a
setter. Query fields are read-only. Add a read-only property class and
a `registerProperty` overload that takes only a getter. `PropertyInfo`
gains `isWritable()` so a facade can refuse `set` without catching an
exception. `set` on a read-only property throws a clear error.

Keep the existing read-write overloads. Do not make setters optional
with a null function pointer; a separate type states the contract.

### Reference-returning properties

`Property<C, T>` calls a getter that returns `T` by value and wraps it
with `ReturnTraits<T>::make`. Nested GraphQL fields then project a
copy that was already built in full. `ReturnTraits<R&>` already stores
`Any(&r)`. Add property templates whose getters return `T&`,
`const T&`, or `T*`, and register overloads for those signatures.

The documented contract: a getter that returns a reference or pointer
yields an `Any` with `isRef() == true`. The instance remains owned by
the application. The executor must not `clear()` that `Any` and then
use the pointer. A getter that returns `T` by value yields a copy;
that is valid for small scalars and for values the application wants
to detach.

`PropertyInfo::get` already returns `Any`. No change to that
signature. Tests must cover:

- `int` / `Pt::String` by value
- nested object by `T&` and by `T*`
- null `T*` becomes an empty or null `Any` (decide one rule and test it)

Null pointers need an explicit rule before GraphQL non-null wrapping
can mean anything. Prefer: a null `T*` produces an empty `Any`.
`any_cast` on empty stays forbidden; callers test `empty()` first.

### Collection walk

A GraphQL list field is not one value. The executor needs the element
count and an `Any` per element. Reflex has `GenericType` as a cache of
specializations, not as a sequence protocol.

Add a small protocol on `Type` or as a sibling interface the type can
implement:

- `isSequence()` (or a `SequenceType` base)
- `size(const void* instance)`
- `at(void* instance, std::size_t index)` returning `Any`

Register `std::vector<T>` (and later other containers) through that
protocol, with elements as references into the vector when `T` is an
object type. Do not invent a second container library. The first
implementation may support only `std::vector` and C arrays.

Without this, Stage 3 would serialize a whole vector through
`SerializationInfo` and throw the extra elements away.

### Member metadata

GraphQL fields carry description, deprecation, and default argument
values. Reflex members currently have only a name and a type. Add
optional metadata on `PropertyInfo` and `MethodInfo`:

- description string
- deprecated flag and optional reason string

Do not put GraphQL-only names (`nonNull`, `listOf`) on Reflex. Those
belong on the GraphQL field. Description and deprecation are useful to
Lua and to documentation dumps as well.

Method parameter names are missing. `MethodInfo` has types and arity,
not names. GraphQL arguments are named. Add optional parameter names
on `MethodInfo`, filled at registration when the caller supplies them.
Unnamed parameters can still be invoked by position. Schema binding in
Stage 3 maps GraphQL argument names to those parameter names or to
positions.

### What Stage 0 does not do

Do not extract a shared "schema kernel" from SOAP, MCP, and GraphQL.
Three declaration graphs with the same ownership rule are better than
one abstraction that fits none of the wire formats.

Do not add a GraphQL parser, an SDL writer, or HTTP routes.

Do not change `Any`. Reference storage already exists.

Do not require serialization operators on every reflected type.
Projection reads properties, it does not flatten the object through
`operator<<=`.

### Stage 0 stop condition

A unit test registers a type with a scalar property, a read-only
nested object property that returns `T&`, a nullable `T*` property, a
`std::vector` of objects, and a method with named parameters. A test
harness walks those members through Reflex only, using `Any` handles,
without copying the nested object or the vector elements.

## Stage 1 - Schema graph and SDL {#graphql-stage1}

Add `Pt::Graphql` with types that mirror SOAP's declaration graph for
this wire format:

- `Schema`
- `Type` (scalar, object, enum; list and non-null as wrappers)
- `Field`
- `Argument`
- `SdlWriter`

Built-in scalars: `Int`, `Float`, `String`, `Boolean`, `ID`. Object
types expose fields. Fields name a result type and optional arguments.
The schema is mutable while it is built and immutable after `freeze()`
(name to be chosen). Freeze checks: named types unique, field types
resolved, no cycles in non-null wrappers, every field has a type.

Bindings to Reflex may be attached in this stage but are not executed.
SDL export must work from the type graph alone, like `toWsdl()`.

Stop condition: a test builds a `Query` with `user(id: ID!): User` and
`User { id, name }`, then `SdlWriter` emits the corresponding SDL.

## Stage 2 - Structured query and field execution {#graphql-stage2}

Define a selection tree in C++ (`Selection`, `FieldSelection`) and a
JSON reader that fills it. The JSON form is an implementation detail
of this stage, not a public client standard. A possible shape:

```json
{
  "user": {
    "args": { "id": "1" },
    "select": {
      "id": true,
      "name": true
    }
  }
}
```

The executor starts at the `Query` type. For each selected field it
calls the field's resolve binding with the parent `Any`, the argument
map, and the child selections. A Reflex property binding calls
`PropertyInfo::get`. A function binding calls the function. The result
`Any` is written as JSON when the field type is a scalar.

Unknown selected fields are errors. Extra properties on the C++ object
are not written. That is the projection.

Stop condition: given a live C++ user object and a selection of two
scalar fields, the executor writes a JSON `data` object with only
those fields.

## Stage 3 - Nested fields, lists, and arguments {#graphql-stage3}

Child selections run on the `Any` returned by the parent. If that
`Any` is a reference, child property gets use `Any::get()` as the
instance pointer and the Reflex type of the field. If it is empty, the
JSON value is `null` (unless the field is non-null, which is an
error).

List fields use the Stage 0 sequence protocol. Each element becomes a
parent `Any` for the child selection. Scalar lists write JSON arrays
of scalars.

Field arguments are validated against the schema (name, type,
required) and passed to the resolve binding. A method binding fills an
`ArgumentList` from named arguments. A property binding rejects
arguments unless Stage 5 introduces argument-bearing properties, which
it should not; arguments belong on methods or on custom resolve
functions.

Stop condition: a query that selects `user { name posts { title } }`
walks a user and a vector of posts through references and writes only
the requested members.

## Stage 4 - HTTP service, errors, and introspection {#graphql-stage4}

`Graphql::HttpService` is an `Http::Service`. POST body is JSON. The
first accepted body is the structured selection tree from Stage 2. A
GET or POST that asks for the schema returns SDL (`content-type` text,
or a JSON wrapper). No remoting base class.

Errors follow the GraphQL result shape: `errors[]` with `message` and
`path`. A resolver exception becomes one entry. Sibling fields still
run. Non-null violation nulls the nearest nullable ancestor and
records the path.

Introspection is data on the frozen schema, not a second type system.
`__schema` and `__type` can be implemented as ordinary object types
bound to the schema object. That reuses the executor.

Stop condition: an HTTP client can fetch SDL, post a structured query,
and receive `data` plus `errors` with paths.

## Stage 5 - Mutations {#graphql-stage5}

`Mutation` is a second root type on the schema. Resolve bindings may
call Reflex setters or methods that mutate. Execution of mutation root
fields is sequential. Query execution may stay unordered among sibling
fields.

Input object types are added here. They are not the same objects as
output types even when they share a C++ class. SOAP already separates
input and output parameters on an operation; GraphQL does the same at
type level.

## Stage 6 - Query text and subscriptions {#graphql-stage6}

A recursive-descent parser turns GraphQL query text into the Stage 2
selection tree. Variables and fragments land in this stage. After the
parser exists, the HTTP body may use the common `{ "query": "...",
"variables": { } }` envelope. SDL export then matches what standard
clients expect to send.

Subscriptions wait until the query path is production-quality. Pt
already has an event loop and WebSocket. The subscription model is an
event source that runs the same executor per payload, not a new schema
language.

## Module layout {#graphql-layout}

```
include/Pt/Graphql/          public headers
include/Pt/Graphql/Api.h
src/Pt-Graphql/              implementation
src/Pt-Graphql/tests/        unit tests
doc/pages/pt-graphql.md      module page (when the API exists)
.github/instructions/pt-graphql-api.instructions.md
```

Jam and the Visual Studio project files gain a `Pt-Graphql` target
when Stage 1 starts. Stage 0 only touches Reflex sources and tests
that already exist under `src/Pt`.

Dependencies of `Pt-Graphql`: Core (`Any`), Reflex, Json, Http. Not
Remoting, not SOAP, not MCP, not Lua.

## Out of scope {#graphql-scope}

The first release does not include DataLoader-style batching, persisted
queries as a protocol, federation, Relay connection types as a built-in,
file uploads, or a query cost limiter. Those are application or later
framework work. They must not leak into Stage 0 Reflex APIs.

A full serialize-then-filter path through `SerializationInfo` is not an
implementation of this module. It may exist as a temporary test helper.
It is not the execution model.
