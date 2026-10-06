# Lua Embedding

Pt-Lua embeds a Lua interpreter in a C++ program and exposes Reflex types to scripts. A context binds a type catalog, a script runs source as a coroutine of that context, and native methods, properties, and constructors can be invoked from Lua. Asynchronous native work can finish later on an event loop.

That covers the direction from script to native code. The usual embedding cases also need the opposite direction, and a few script facilities that a host program expects to control. This document states those missing capabilities. It does not choose types, signatures, ownership mechanisms, or call sequences. An architecture document should derive those from the requirements below.

## Baseline

The following behavior exists and remains the starting point. New capabilities must not remove it.

A context opens one Lua state, loads the standard libraries, and installs globals for eligible Reflex types and for functions that return an asynchronous call. A type becomes a Lua class when it has a constructor, a method, or a property. Instances created by the script are owned by Lua. Methods use method call syntax, properties are fields, and a fixed set of operator names maps to metamethods. Scalar values convert in both directions when Lua calls native code. An object returned by native code is copy-constructed into Lua-owned storage. A type without a copy constructor cannot be returned by value.

A script loads a source string into one context. Only one script may use a context at a time. The script advances cooperatively: a yield pauses it, a native call is performed on a later step, and the script ends in success or error. Advance can run on the calling thread, on an event loop, or as a C++20 awaitable. Cancelling aborts a pending step and any active asynchronous call.

After success, the only host-facing read is a named global interpreted as an integer. A missing or non-numeric global becomes zero. Values returned by the chunk itself are discarded. There is no host operation to set a global, pass arguments, or call a Lua function. Free functions are bound only when they return an asynchronous call. Tables are not converted. A host object cannot be handed to a script as a reference the host still owns. Nested script loading is not resolved. Every context receives the full standard library set.

## Required capabilities

### Reading results

The host must be able to read values produced by a finished script without using the Lua C API.

The readable set is booleans, integers, floating-point numbers, strings, nil, bound objects, and tables. Nil is a distinct absence. It is not coerced to zero or to an empty string. A missing name and a value of the wrong kind are failures, not successful zeros.

Named globals remain a valid source. Values returned by the chunk are also a valid source. The host can ask for one value or for every value the script produced. Asking for one value may ignore the rest, but that choice is explicit. Silently dropping a requested value is not acceptable.

An object read by the host has a defined lifetime. Either the host receives an independent C++ value, or it receives a reference whose validity ends at a documented point: context reset, context destruction, script end, or an explicit release. A type that cannot be transferred under the chosen rule is an error. A reference must not outlive the Lua value it names, and it must not become a dangling pointer when the context is reset or closed.

### Supplying values

The host must be able to supply values to a script before it runs and between advances, without using the Lua C API.

The supplied set is the same as the readable set: booleans, integers, floating-point numbers, strings, nil, bound objects, and tables. The host can place a value in a named global. The host can also pass values as arguments to a chunk or to a Lua function it invokes.

A host-owned object can be made visible to the script while the host retains ownership. The script can call its methods and read or write its properties. Collecting the Lua value must not destroy the host object. Destroying the host object while the script can still reach it is either prevented or reported as a defined error. Transfer of ownership, if supported, is explicit and is not the default.

### Calling Lua from the host

Running a chunk and calling a Lua function are both required. The host can invoke a named function in the context, pass arguments, and read the returned values under the same conversion and error rules as a finished chunk. The call participates in the same advance and cancellation rules as a script. A Lua error becomes a host-visible failure, not an uncaught exception crossing the language boundary.

### Tables

Tables convert in both directions for the shapes embedding code normally uses: a sequence, a string-keyed map, and nesting of those shapes. Elements may be scalars, strings, nil, or bound objects. A cycle is an error. A limit on nesting depth, if any, is documented and enforced.

A table conversion is not the same capability as a Reflex class that wraps a container. Both may exist. A script must be able to pass and receive a table of the documented element kinds without a separate registered class for each element type.

### Native callables

Synchronous free functions are bindable on the same terms as methods. Functions and methods that return an asynchronous call remain bindable. A native callable may return no value, one value, or several values, using the same conversion rules as host-supplied values. An argument count or type mismatch is an error reported to the script and visible to the host as a script failure. A C++ exception thrown by native code becomes that same kind of failure and does not escape across the Lua boundary.

### Lua functions as callbacks

The host can receive a Lua function as a value and invoke it later, including when asynchronous native work completes. Invocation uses the same execution, conversion, and cancellation rules as calling Lua from the host. A callback is not callable after the context that created it is reset or destroyed. Storing a callback does not by itself keep that context alive. If the host needs the callback to extend the context lifetime, that extension is an explicit host action.

### Modules and source loading

The host can run source from a string and from a file path it allows. A script can load another script that the host has provided, by string, by allowed path, or by a name the host registered. Loading a name or path the host did not provide fails. A cyclic load fails.

Nested loading has to be reconciled with the rule that one context runs one top-level script. The architecture chooses whether a nested load shares that context or is rejected. The requirement is only that the choice is defined, and that a script cannot escape the host's loader to read arbitrary files.

### Standard library selection

The host can choose which standard libraries a context exposes before scripts run. A context for untrusted source can exclude the libraries that perform file, process, module, and debug access. The default for trusted source may remain the full set. A script cannot reintroduce an excluded library unless the host has allowed that library.

Library selection is the sandbox boundary required here. Operating-system isolation is not required.

### Errors

Compile failure, runtime failure, native failure, conversion failure, and cancellation are distinguishable by the host. Each carries a message. A runtime failure includes a Lua traceback when the interpreter can produce one. Success is not reported for a missing result, a mistyped result, or a cancelled script. Native exceptions and Lua errors stay inside the embedding boundary.

### Integer range

Integer conversion covers the integer range of the embedded interpreter, including values that do not fit in a 32-bit int or long. A value outside the range of the requested C++ type is a conversion failure.

## Constraints

Existing class binding, cooperative advance, event-loop advance, awaitable advance, and asynchronous native calls remain usable.

The host can perform the capabilities in this document without including the Lua C API headers. Direct use of the state may remain available for cases outside this document.

Reset still removes script-created state and keeps the bindings. After reset, references previously read from that state are invalid. Bindings are not collected as script state.

One context is not required to run two top-level scripts at the same time, except as far as nested loading needs a defined rule.

## Out of scope

This document does not require a second scripting language, a replacement for the Reflex catalog, a debugger interface, a package manager, or a change of Lua version. It does not require operating-system isolation. Bytecode loading is not required for the first architecture; if added later, it uses the same host-controlled loader as source loading.

Efficiency of context reset and cleanup of binding storage are implementation concerns. They are not separate embedding capabilities.

## Open points for the architecture

The architecture has to decide these points. This document only requires that each decision preserves the capabilities above.

- Whether an object result is copied out, referenced, or both, and which types support which form.
- Whether chunk returns, named globals, and host calls into Lua share one result representation.
- How nested loading interacts with one script per context.
- Whether table conversion is a built-in rule or a family of Reflex types.
- How long a host-owned object reference and a stored Lua callback remain valid, and what happens if the host destroys either side early.
- How the existing single integer global read relates to the new result rules, including whether the silent conversion to zero remains available.

## Priority

The first architecture should cover reading results, supplying values, and distinguishable errors. Those three close the gap that makes a script unable to exchange real values with the host.

Calling Lua, tables, synchronous free functions, and multiple values are the next slice. They are what ordinary scripts already assume.

Callbacks, host-controlled loading, standard library selection, and the full integer range follow. They matter for untrusted scripts and for larger script collections, and they depend on the conversion and lifetime rules chosen first.
