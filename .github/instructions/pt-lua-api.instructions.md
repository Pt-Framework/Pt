---
description: "Lua Scripting and Reflection Bindings"
---

- Create and reset Lua runtime contexts, bind reflected types:
  `include/Pt/Lua/Context.h`
- Load, advance, cancel and await Lua script execution:
  `include/Pt/Lua/Script.h`
- Read named values from Lua script results:
  `include/Pt/Lua/Result.h`
- Invoke reflected methods, properties and constructors from Lua:
  `include/Pt/Lua/Call.h`
- Implement, start, cancel and retrieve results from asynchronous Lua calls:
  `include/Pt/Lua/AsyncCall.h`
- Register Lua-compatible types and asynchronous reflected functions:
  `include/Pt/Lua/TypeManager.h`