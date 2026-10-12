# Lua Host

## Documents

- [Values](lua-host-values.md)
- [Lifetime](lua-host-lifetime.md)
- [Loading](lua-host-loading.md)
- [Interop](lua-host-interop.md)
- [Concept](../concepts/lua.md)

## Open

- [representation](lua-embedding-interop.md#representation).
  Whether chunk returns, named globals, and host calls into Lua share
  one result representation.

- [lifetime](lua-host-lifetime.md#lifetime).
  How long a host-owned object reference and a stored Lua callback
  remain valid, and what happens if the host destroys either side
  early.

- [results](lua-host-values.md#results).
  Whether an object result is copied out, referenced, or both, and
  which types support which form.

- [tables](lua-host-values.md#tables).
  Whether table conversion is a built-in rule or a family of Reflex
  types.

- [integer-global](lua-host-values.md#integer-global).
  How the existing single integer global read relates to the new
  result rules, including whether the silent conversion to zero
  remains available.

- [nested-load](lua-host-loading.md#nested-load).
  How nested loading interacts with one script per context.

## Priority

The first architecture should define the values and distinguishable errors,
then decide the result representation shared by chunk returns and host calls.
It should cover reading results and supplying values. Those choices close the
gap that makes a script unable to exchange real values with the host.

Calling Lua, tables, synchronous free functions, and multiple values
are the next slice. They are what ordinary scripts already assume.

Callbacks, host-controlled loading, standard library selection, and the
full integer range follow. They matter for untrusted scripts and for
larger script collections, and they depend on the conversion and
lifetime rules chosen first.
