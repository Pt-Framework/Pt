# Lua

## Documents

- [Embedding](lua_embedding.md)

## Open

- [results](lua_embedding.md#lua-open).
  Whether an object result is copied out, referenced, or both, and which types support which form.
- [representation](lua_embedding.md#lua-open).
  Whether chunk returns, named globals, and host calls into Lua share one result representation.
- [nested-load](lua_embedding.md#lua-open).
  How nested loading interacts with one script per context.
- [tables](lua_embedding.md#lua-open).
  Whether table conversion is a built-in rule or a family of Reflex types.
- [lifetime](lua_embedding.md#lua-open).
  How long a host-owned object reference and a stored Lua callback remain valid, and what happens if the host destroys either side early.
- [integer-global](lua_embedding.md#lua-open).
  How the existing single integer global read relates to the new result rules, including whether the silent conversion to zero remains available.

