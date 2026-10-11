# Lua Host

## Documents

- [Values](lua-host-values.md)
- [Lifetime](lua-host-lifetime.md)
- [Loading](lua-host-loading.md)
- [Concept](../concepts/lua.md)

## Open

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

- [lifetime](lua-host-lifetime.md#lifetime).
  How long a host-owned object reference and a stored Lua callback
  remain valid, and what happens if the host destroys either side
  early.

- [nested-load](lua-host-loading.md#nested-load).
  How nested loading interacts with one script per context.
