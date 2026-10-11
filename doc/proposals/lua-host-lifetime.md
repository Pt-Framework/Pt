---
proposal: lua-host.md
---

# Lua Host Lifetime

## Baseline

Reset still removes script-created state and keeps the bindings. After
reset, references previously read from that state are invalid. Bindings
are not collected as script state.

## Lifetime {#lifetime}

An object exchanged with the host has a defined lifetime. Either the
host receives an independent C++ value, or it receives a reference
whose validity ends at a documented point: context reset, context
destruction, script end, or an explicit release. A type that cannot be
transferred under the chosen rule is an error. A reference must not
outlive the Lua value it names, and it must not become a dangling
pointer when the context is reset or closed.

A host-owned object can be made visible to the script while the host
retains ownership. Collecting the Lua value must not destroy the host
object. Destroying the host object while the script can still reach it
is either prevented or reported as a defined error. Transfer of
ownership, if supported, is explicit and is not the default.

A stored Lua callback has the same lifetime question. A callback is not
callable after the context that created it is reset or destroyed.
Storing a callback does not by itself keep that context alive. If the
host needs the callback to extend the context lifetime, that extension
is an explicit host action.

The open point is how long a host-owned object reference and a stored
Lua callback remain valid, and what happens if the host destroys either
side early.
