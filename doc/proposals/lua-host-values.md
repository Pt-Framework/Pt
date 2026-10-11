---
proposal: lua-host.md
---

# Lua Host Values

## Baseline

After success, the only host-facing read is a named global interpreted
as an integer. A missing or non-numeric global becomes zero. Values
returned by the chunk itself are discarded. There is no host operation
to set a global, pass arguments, or call a Lua function. Tables are not
converted. A host object cannot be handed to a script as a reference
the host still owns.

Scalar values convert in both directions when Lua calls native code. An
object returned by native code is copy-constructed into Lua-owned
storage. A type without a copy constructor cannot be returned by value.

## Reading results {#results}

The host must be able to read values produced by a finished script
without using the Lua C API.

The readable set is booleans, integers, floating-point numbers,
strings, nil, bound objects, and tables. Nil is a distinct absence. It
is not coerced to zero or to an empty string. A missing name and a
value of the wrong kind are failures, not successful zeros.

Named globals remain a valid source. Values returned by the chunk are
also a valid source. The host can ask for one value or for every value
the script produced. Asking for one value may ignore the rest, but that
choice is explicit. Silently dropping a requested value is not
acceptable.

An object read by the host has a defined lifetime. Either the host
receives an independent C++ value, or it receives a reference whose
validity ends at a documented point: context reset, context
destruction, script end, or an explicit release. A type that cannot be
transferred under the chosen rule is an error. A reference must not
outlive the Lua value it names, and it must not become a dangling
pointer when the context is reset or closed.

## Supplying values

The host must be able to supply values to a script before it runs and
between advances, without using the Lua C API.

The supplied set is the same as the readable set: booleans, integers,
floating-point numbers, strings, nil, bound objects, and tables. The
host can place a value in a named global. The host can also pass values
as arguments to a chunk or to a Lua function it invokes.

A host-owned object can be made visible to the script while the host
retains ownership. The script can call its methods and read or write
its properties. Collecting the Lua value must not destroy the host
object. Destroying the host object while the script can still reach it
is either prevented or reported as a defined error. Transfer of
ownership, if supported, is explicit and is not the default.

## Tables {#tables}

Tables convert in both directions for the shapes embedding code
normally uses: a sequence, a string-keyed map, and nesting of those
shapes. Elements may be scalars, strings, nil, or bound objects. A
cycle is an error. A limit on nesting depth, if any, is documented and
enforced.

A table conversion is not the same capability as a Reflex class that
wraps a container. Both may exist. A script must be able to pass and
receive a table of the documented element kinds without a separate
registered class for each element type.

## Integer range {#integer-global}

Integer conversion covers the integer range of the embedded
interpreter, including values that do not fit in a 32-bit int or long.
A value outside the range of the requested C++ type is a conversion
failure.

The existing single integer global read must be related to the new
result rules, including whether the silent conversion to zero remains
available.
