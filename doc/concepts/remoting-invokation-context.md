# Remoting Invocation Context {#remoting-invocation-context}

This document is the design for a request-scoped invocation context in
Pt-Remoting. It is a framework concept, not an application sketch. The
goal is that a registered service procedure can learn which principal
invoked it without taking the caller as a serialized RPC argument and
without inspecting a transport type such as `HttpResponder`.

The work is staged. Stages 1 to 3 solve the original problem. Stages 4
and 5 add fault codes and a filter pipeline. Existing
`registerProcedure` signatures stay valid throughout.

This chapter covers:

- [Purpose](#ric-purpose)
- [Principles](#ric-principles)
- [Current state](#ric-current)
- [Object model](#ric-model)
- [Principal](#ric-principal)
- [Invocation](#ric-invocation)
- [Registration](#ric-registration)
- [Transport binding](#ric-transport)
- [Faults](#ric-faults)
- [Filter pipeline](#ric-filters)
- [Stages](#ric-stages)
- [Module layout](#ric-layout)
- [Out of scope](#ric-scope)

## Purpose {#ric-purpose}

Pt-Remoting dispatches a named procedure with serialized arguments.
The procedure signature is the RPC contract. The identity of the
caller is not part of that contract. It arrives on the transport:
HTTP Basic credentials, a Bearer token, a session cookie, a TLS
client certificate, or a handshake on a long-lived connection.

Today a service procedure does not see that identity. `BasicProcedure`
calls the registered callable with the deserialized arguments only.
`ServiceProcedure` holds a `Responder*` but does not expose it.
`Http::Authorizer` grants or denies access and then discards the
`Credential`. The only supported hook that sees a `Responder` is the
`registerActiveProcedure` factory, which forces every caller-aware
method to become an asynchronous procedure and to `dynamic_cast` the
responder to a protocol type.

The first useful result is a `Principal` and an `Invocation` stored on
`Remoting::Responder`, filled by the transport before the procedure
runs, and readable from both synchronous and asynchronous procedures.
A later filter pipeline can require authentication or a role without
changing procedure signatures.

## Principles {#ric-principles}

The procedure knows the caller through the invocation, never through
the transport. The transport knows credentials, never the business
method.

One in-flight request is one `Responder`. The invocation lives on that
responder. That is what survives `ActiveProcedure`, worker threads,
and the `EventLoop`. Thread-local storage is not an invocation
context.

`ServiceDefinition` stays protocol-neutral. HTTP, JSON-RPC, XML-RPC,
SOAP, MCP, and stdio fill the same `Invocation` in different ways. A
procedure that reads `inv.principal()` runs unchanged on all of them.

Identity is not an RPC parameter. A token may be an explicit argument
when the public API specifies it. The default path is implicit: the
transport resolves a `Principal`, the procedure consumes it.

`SerializationContext` stays serializer state. Application payload on
the call (tenant, locale, loaded user record) goes on `Invocation`,
not on the composer context.

Existing registrations remain valid. A method that does not need the
caller keeps its current signature. A method that does need the caller
opts in with a leading `const Invocation&` that is not serialized.

No authentication protocol is built into Remoting. Basic, Bearer,
cookie, and certificate checks stay in `Http::Authorizer` and later
connection handshakes. Remoting only stores the result.

## Current state {#ric-current}

`ServiceDefinition::registerProcedure` wraps a `Callable` in
`BasicProcedureDef`. `getProcedure` creates a `BasicProcedure` with
the `Responder`. `onEndCall` invokes the callable with the composed
arguments and returns a decomposer for the result.

`registerActiveProcedure` registers a factory `A*(Responder&)`. The
factory is the only public place that currently receives the
responder. `JsonRpc::HttpResponder` and `XmlRpc::HttpResponder` expose
`request()` and `reply()`, so an application can read
`Authorization` or `Cookie` there after a `dynamic_cast`.

`Http::Authorizer` is attached to a servlet. `beginAuthorize` returns
granted or an asynchronous `Authorization`. `BasicAuthorizer` parses
HTTP Basic credentials into `Http::Credential` and calls
`onAuthorizeCredentials`. The user name is not stored on the remoting
responder.

`Remoting::Fault` is a `std::runtime_error` with a message and no
code. Protocol responders map every fault the same way.

These points stay. The design adds storage and accessors on types that
already exist, then optional registration and filter APIs.

## Object model {#ric-model}

```
Transport (HTTP Authorizer, WS handshake, stdio)
        |
        | Principal
        v
Remoting::Responder ---- Invocation ---- Principal
        |                     |
        |                     +-- procedure name
        |                     +-- user data (Any)
        v
ServiceProcedure  (BasicProcedure / ActiveProcedure)
        |
        | invocation()  or  leading const Invocation&
        v
Registered C++ callable
        ^
        |
ServiceDefinition filters (optional)
```

`Responder` already owns the `SerializationContext` and the active
`ServiceProcedure`. It also owns the `Invocation`. Protocol responders
set the principal when the request is accepted. `setProcedure(name)`
records the procedure name on the invocation. Filters run after that,
before `beginCall` / `call`.

## Principal {#ric-principal}

`Pt::Remoting::Principal` is a small, copyable value in the remoting
core. It does not include a password or a raw token. Those stay in the
authorizer and are dropped after the principal is built.

```cpp
namespace Pt {

namespace Remoting {

class PT_REMOTING_API Principal
{
    public:
        static Principal anonymous();

        static Principal authenticated(const std::string& name);

        bool isAuthenticated() const;

        const std::string& name() const;

        bool hasRole(const std::string& role) const;

        void addRole(const std::string& role);

        void setAttribute(const std::string& key, const std::string& value);

        const std::string* attribute(const std::string& key) const;
};

} // namespace Remoting

} // namespace Pt
```

The default principal is anonymous. Authenticated principals have a
name. Roles cover coarse authorization. Attributes cover transport or
application facts that are not roles (session id, tenant id, token
issuer). A loaded domain user object is not a principal field; the
application stores that in `Invocation::userData()`.

Remoting does not implement OAuth, JWT, or a claims graph. A Bearer
authorizer may put selected claims into attributes. The core type stays
a name, roles, and a string map.

## Invocation {#ric-invocation}

`Pt::Remoting::Invocation` is the per-request facade. It is not a
serializable RPC type.

```cpp
class PT_REMOTING_API Invocation
{
    public:
        const std::string& procedureName() const;

        const Principal& principal() const;

        void setPrincipal(const Principal& principal);

        void setUserData(const Any& data);

        const Any& userData() const;

        bool isCancelled() const;
};
```

`Responder` exposes it:

```cpp
class Responder : private NonCopyable
{
    public:
        Invocation& invocation();
        const Invocation& invocation() const;
        // existing API unchanged
};
```

`ServiceProcedure` exposes the responder it already holds:

```cpp
class ServiceProcedure
{
    public:
        Responder& responder()
        { return *_responder; }

        const Responder& responder() const
        { return *_responder; }

        Invocation& invocation()
        { return _responder->invocation(); }

        const Invocation& invocation() const
        { return _responder->invocation(); }
};
```

`ActiveProcedure` therefore sees the caller with `this->invocation()`
and no longer needs a `dynamic_cast` to read HTTP headers for identity.
Protocol-specific work (writing an extra response header) can still
cast the responder; identity must not require that cast.

`cancel()` on the responder marks the invocation cancelled so a late
`setReady()` can refuse to send a result.

## Registration {#ric-registration}

`registerProcedure` gains an overload path that treats a leading
`Invocation` reference as an injected argument. It is not composed
from the wire and must be the first parameter. A `Invocation` in any
other position is a compile-time error.

```cpp
class OrderService : public Pt::Remoting::ServiceDefinition
{
    public:
        OrderService()
        {
            registerProcedure("add", *this, &OrderService::add);
            registerProcedure("getOrders", *this, &OrderService::getOrders);
        }

        int add(int a, int b)
        { return a + b; }

        std::vector<Order> getOrders(const Invocation& inv, int customerId)
        {
            const Principal& user = inv.principal();
            if ( ! user.isAuthenticated() )
                throw Fault("unauthorized");

            return loadOrders(user.name(), customerId);
        }
};
```

Detection is a type trait on the first parameter of the callable.
`BasicProcedure` builds composers only for the remaining argument
types. `callWith` prepends `responder().invocation()`. The same trait
applies to `ActiveProcedure::onInvoke`: either
`onInvoke(EventLoop&, const Invocation&, const As&...)` or the current
`onInvoke(EventLoop&, const As&...)`.

The factory for `registerActiveProcedure` already receives `Responder&`.
It does not need a second overload. New code should read the principal
from `responder.invocation()` inside the constructed procedure, not in
the factory, so filters can still change the principal before
`onInvoke`.

## Transport binding {#ric-transport}

The remoting core never reads HTTP headers. Protocol responders copy a
principal onto the invocation after the transport has accepted the
request.

### HTTP authorizer

`Authorizer::onBeginAuthorize` gains an output `Principal`:

```cpp
virtual Authorization* onBeginAuthorize(const Request& req,
                                        Reply& reply,
                                        Principal& principal,
                                        bool& granted) = 0;
```

`BasicAuthorizer` sets `Principal::authenticated(cred.user())` when
credentials are accepted. A derived authorizer may add roles and
attributes. When no authorizer is attached, the principal stays
anonymous and the request is still dispatched.

The JSON-RPC and XML-RPC HTTP responders assign that principal in
`onBeginRequest` (or immediately after `endAuthorization`):

```cpp
invocation().setPrincipal(principal);
```

Optional request facts that procedures may want (request id, locale)
can be stored as attributes or as `userData`. They are not implied by
this design.

### Other transports

MCP over stdio and other non-HTTP responders leave the principal
anonymous unless they implement their own handshake. The same
`ServiceDefinition` remains usable.

A later WebSocket or framed connection may hold a `Session` on the
connection. Each RPC message creates a new `Invocation` and copies or
shares the session principal. Session lifetime is the connection.
Invocation lifetime is the call. Those types stay distinct.

## Faults {#ric-faults}

`Fault` needs a machine-readable code so HTTP and JSON-RPC can map
authentication and authorization failures without parsing the message
string.

```cpp
class PT_REMOTING_API Fault : public std::runtime_error
{
    public:
        explicit Fault(const std::string& msg);
        Fault(int code, const std::string& msg);

        int code() const;
};
```

Reserved application codes (exact values to be chosen when implemented):

- unauthorized: the principal is missing or not authenticated
- forbidden: the principal is authenticated but lacks a role or right

JSON-RPC maps them to JSON-RPC error objects. HTTP responders may set
401 or 403 when the fault occurs before a successful procedure result
is produced, without changing the rule that a completed RPC fault is
still a 200 body for XML-RPC. The mapping lives in the protocol
responder, not in `Fault`.

Existing `Fault(const char*)` constructors remain. Code defaults to a
generic application-error value.

## Filter pipeline {#ric-filters}

Filters are optional and run on the `ServiceDefinition` for every call.
They are the place for cross-cutting authentication, coarse
authorization, audit, and deadlines.

```cpp
class ProcedureFilter
{
    public:
        virtual ~ProcedureFilter()
        {}

        virtual void onInvoke(Invocation& inv) = 0;

        virtual void onCompleted(Invocation& inv, const std::exception* error) = 0;
};

class ServiceDefinition : private NonCopyable
{
    public:
        void addFilter(ProcedureFilter& filter);

        void requireAuth(const std::string& procedureName);

        void requireRole(const std::string& procedureName, const std::string& role);
};
```

`requireAuth` and `requireRole` are declaration helpers that attach a
small built-in filter to one procedure name. They do not change the
callable.

Order inside `Responder` after the procedure has been selected:

1. `invocation` receives the procedure name
2. each filter `onInvoke` (may throw `Fault`)
3. `beginCall` / `call`
4. each filter `onCompleted`, with a `Fault` or other exception pointer
   on failure, null on success

`onCompleted` must not swallow the error. It may log or write audit
state. The protocol responder still formats the fault.

A procedure that needs row-level rights still checks them itself. The
pipeline only closes the coarse door.

```cpp
registerProcedure("getOrders", *this, &OrderService::getOrders);
requireAuth("getOrders");

registerProcedure("deleteUser", *this, &AdminService::deleteUser);
requireRole("deleteUser", "admin");
```

Filters are not an HTTP middleware stack and not a replacement for
`Authorizer`. The authorizer authenticates. The filter authorizes and
observes.

## Stages {#ric-stages}

Each stage has a stop condition. The next stage does not start until
that condition is met.

| Stage | Ships in | Stop condition |
|---|---|---|
| 1 | Remoting | `Principal` and `Invocation` on `Responder`; `ServiceProcedure::invocation()` |
| 2 | Http, JsonRpc, XmlRpc | Authorizer writes a principal; HTTP responders copy it onto the invocation |
| 3 | Remoting | `registerProcedure` injects a leading `const Invocation&` |
| 4 | Remoting plus protocol responders | `Fault` has a code; unauthorized and forbidden map per protocol |
| 5 | Remoting | `ProcedureFilter`, `requireAuth`, `requireRole` |

### Stage 1 - Invocation on the responder

Add `Principal` and `Invocation` under `include/Pt/Remoting`.
`Responder` owns an `Invocation`. `ServiceProcedure` returns the
responder and the invocation. Default principal is anonymous.
`procedureName` is empty until `setProcedure` runs.

No registration or HTTP change. Tests construct a responder, set a
principal, create a `BasicProcedure`, and read the same principal from
`invocation()`.

### Stage 2 - HTTP fills the principal

Extend `Authorizer` so a successful check produces a `Principal`.
`BasicAuthorizer` and `BasicUserListAuthorizer` set an authenticated
principal from `Credential::user()`. JsonRpc and XmlRpc HTTP
responders assign it before parsing the body. SOAP and MCP HTTP
responders follow the same assignment when those paths are touched.

Stop condition: a unit test serves a procedure over HTTP with Basic
auth, and an `ActiveProcedure` reads `invocation().principal().name()`
without casting to `HttpResponder`.

### Stage 3 - Injected Invocation argument

Type trait and `BasicProcedure` / `ActiveProcedure` support for a
leading `const Invocation&` or `Invocation&`. Composers skip that
parameter. The wire arity stays the RPC arity.

Stop condition: `registerProcedure("getOrders", obj, &T::getOrders)`
with `getOrders(const Invocation&, int)` round-trips over JSON-RPC and
sees the principal set in Stage 2. `add(int, int)` still registers and
calls as today.

### Stage 4 - Fault codes

`Fault` stores an integer code. Document the unauthorized and
forbidden values. Protocol responders map them. Existing tests that
construct `Fault` with only a message keep compiling.

### Stage 5 - Filters

`addFilter`, `requireAuth`, `requireRole`. Built-in filters throw
`Fault` with the Stage 4 codes. A test registers a method without a
leading `Invocation&`, calls `requireAuth`, and receives a fault for
an anonymous principal.

## Module layout {#ric-layout}

```
include/Pt/Remoting/Principal.h
include/Pt/Remoting/Invocation.h
include/Pt/Remoting/ProcedureFilter.h
include/Pt/Remoting/Responder.h          Invocation member
include/Pt/Remoting/ServiceProcedure.h   responder() / invocation()
include/Pt/Remoting/ServiceDefinition.h  filters, requireAuth/Role
include/Pt/Remoting/BasicProcedure.h     optional Invocation argument
include/Pt/Remoting/ActiveProcedure.h    optional Invocation argument
include/Pt/Remoting/Fault.h              optional code
include/Pt/Http/Authorizer.h             Principal out-parameter
src/Pt-Remoting/
src/Pt-Http/
src/Pt-JsonRpc/                          copy principal onto invocation
src/Pt-XmlRpc/
doc/concepts/remoting-invokation-context.md
doc/pages/                                module page when the API ships
```

No new library. Types live in `Pt-Remoting`. HTTP assignment lives in
the protocol modules that already depend on Remoting and Http.

## Out of scope {#ric-scope}

This design does not add OAuth, OpenID Connect, JWT parsing, or a
built-in session store. Those are authorizer plugins or application
services.

It does not put a `Responder&` or `Http::Request&` into every
procedure signature. `Invocation` is the stable facade.

It does not use thread-local `currentPrincipal()`. That conflicts with
`ActiveProcedure` work posted to an `EventLoop`.

It does not store identity on `SerializationContext`.

It does not replace servlet-level `Authorizer` with remoting filters.
Unauthenticated HTTP can still be rejected before a remoting responder
is created.

Connection-scoped sessions, per-procedure rate limits, distributed
tracing, and multi-tenant resolvers may use `Invocation::userData()` or
a custom `ProcedureFilter`. They are not part of the first API.
