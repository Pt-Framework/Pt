# Lua

Pt-Lua embeds a Lua interpreter in a C++ program and exposes Reflex
types to scripts. A context binds a type catalog. A script runs source
as a coroutine of that context. Native methods, properties, and
constructors can be invoked from Lua. Asynchronous native work can
finish later on an event loop.

The host must also be able to drive the script in the opposite
direction and to control a small set of script facilities. The model
below states the required capabilities. It does not choose concrete
types, signatures, or call sequences.

## Capabilities

The host can read values produced by a finished script and can supply
values to a script before it runs and between advances, without using
the Lua C API. The value set is booleans, integers, floating-point
numbers, strings, nil, bound objects, and tables. Nil is a distinct
absence. Named globals and values returned by the chunk are both valid
sources. The host can ask for one value or for every value the script
produced. Silently dropping a requested value is not acceptable.

An object exchanged with the host has a defined lifetime. Either the
host receives an independent C++ value, or it receives a reference
whose validity ends at a documented point. A reference must not outlive
the Lua value it names. A host-owned object can be made visible to the
script while the host retains ownership. Collecting the Lua value must
not destroy the host object. Destroying the host object while the
script can still reach it is either prevented or reported as a defined
error. Transfer of ownership, if supported, is explicit.

Tables convert in both directions for the shapes embedding code
normally uses: a sequence, a string-keyed map, and nesting of those
shapes. A cycle is an error. A table conversion is not the same
capability as a Reflex class that wraps a container.

The host can run a chunk and can call a named Lua function. Both
participate in the same advance and cancellation rules as a script. A
Lua error becomes a host-visible failure. Synchronous free functions
are bindable on the same terms as methods. A native callable may return
no value, one value, or several values. A C++ exception thrown by
native code becomes a script failure and does not escape across the Lua
boundary.

The host can receive a Lua function as a value and invoke it later. A
callback is not callable after the context that created it is reset or
destroyed. Storing a callback does not by itself keep that context
alive.

The host can run source from a string and from a file path it allows. A
script can load another script that the host has provided. Loading a
name or path the host did not provide fails. A cyclic load fails.
Nested loading has a defined relationship to the rule that one context
runs one top-level script.

The host can choose which standard libraries a context exposes before
scripts run. A script cannot reintroduce an excluded library unless the
host has allowed it. Library selection is the sandbox boundary.
Operating-system isolation is not required.

Compile failure, runtime failure, native failure, conversion failure,
and cancellation are distinguishable by the host. Each carries a
message. A runtime failure includes a Lua traceback when the
interpreter can produce one. Success is not reported for a missing
result, a mistyped result, or a cancelled script.

Integer conversion covers the integer range of the embedded
interpreter. A value outside the range of the requested C++ type is a
conversion failure.

## Constraints

Existing class binding, cooperative advance, event-loop advance,
awaitable advance, and asynchronous native calls remain usable.

The host can perform the capabilities without including the Lua C API
headers. Direct use of the state may remain available for cases outside
this model.

Reset still removes script-created state and keeps the bindings. After
reset, references previously read from that state are invalid. Bindings
are not collected as script state.

One context is not required to run two top-level scripts at the same
time, except as far as nested loading needs a defined rule.
