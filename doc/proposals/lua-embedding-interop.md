---
proposal: lua-embedding.md
---

# Lua Embedding Interop

## Baseline

A context opens one Lua state, loads the standard libraries, and
installs globals for eligible Reflex types and for functions that
return an asynchronous call. A type becomes a Lua class when it has a
constructor, a method, or a property. Instances created by the script
are owned by Lua. Methods use method call syntax, properties are
fields, and a fixed set of operator names maps to metamethods.

Free functions are bound only when they return an asynchronous call.
There is no host operation to call a Lua function.

The script advances cooperatively: a yield pauses it, a native call is
performed on a later step, and the script ends in success or error.
Advance can run on the calling thread, on an event loop, or as a C++20
awaitable. Cancelling aborts a pending step and any active asynchronous
call.

## Calling Lua from the host {#representation}

Running a chunk and calling a Lua function are both required. The host
can invoke a named function in the context, pass arguments, and read
the returned values under the same conversion and error rules as a
finished chunk. The call participates in the same advance and
cancellation rules as a script. A Lua error becomes a host-visible
failure, not an uncaught exception crossing the language boundary.

Whether chunk returns, named globals, and host calls into Lua share one
result representation is open.

## Native callables

Synchronous free functions are bindable on the same terms as methods.
Functions and methods that return an asynchronous call remain bindable.
A native callable may return no value, one value, or several values,
using the same conversion rules as host-supplied values. An argument
count or type mismatch is an error reported to the script and visible
to the host as a script failure. A C++ exception thrown by native code
becomes that same kind of failure and does not escape across the Lua
boundary.

## Lua functions as callbacks

The host can receive a Lua function as a value and invoke it later,
including when asynchronous native work completes. Invocation uses the
same execution, conversion, and cancellation rules as calling Lua from
the host. A callback is not callable after the context that created it
is reset or destroyed. Storing a callback does not by itself keep that
context alive. If the host needs the callback to extend the context
lifetime, that extension is an explicit host action.

## Constraints

Existing class binding, cooperative advance, event-loop advance,
awaitable advance, and asynchronous native calls remain usable.

The host can perform the capabilities without including the Lua C API
headers. Direct use of the state may remain available for cases outside
this document.
