---
description: "Lua Scripting and Reflection Bindings"
---

- Module groups and page:
  `include/Pt/Lua/Api-Lua.h`, `doc/pages/pt-lua.md`
- Create and reset Lua runtime contexts, bind reflected types:
  `include/Pt/Lua/Context.h`, `include/Pt/Lua/Api-Runtime.h`
- Register Lua-compatible types and asynchronous reflected functions:
  `include/Pt/Lua/TypeManager.h`, `include/Pt/Lua/Api-Runtime.h`
- Load, advance, cancel and await Lua script execution:
  `include/Pt/Lua/Script.h`, `include/Pt/Lua/Api-Scripts.h`
- Read named values from Lua script results:
  `include/Pt/Lua/Result.h`, `include/Pt/Lua/Api-Scripts.h`
- Invoke reflected methods, properties and constructors from Lua:
  `include/Pt/Lua/Call.h`, `include/Pt/Lua/Api-Calls.h`
- Implement, start, cancel and retrieve results from asynchronous Lua calls:
  `include/Pt/Lua/AsyncCall.h`, `include/Pt/Lua/Api-Calls.h`
