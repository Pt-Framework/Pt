---
applyTo: "jam/**"
description: "Jam build tool internals — C implementation in jam/src/ and Jam script layer in jam/*.jam. Use when extending the jam executable with new builtin rules, modifying jam's C source, editing the Jambase boot sequence, or working with configure/build/toolset .jam files."
---

# Jam Internals

The Pt project ships a customized jam executable based on Boost.Jam (b2)
v2011.12. The implementation spans two layers: a C core (`jam/src/`) and a
Jam-script layer (`jam/` and `jam/Jambase`). Extensions should follow existing
patterns and be marked with `/* Pt extension */` comments in C code.

## Directory Layout

```
jam/
├── Jambase              # Boot script: loads base.jam, runs configure or build
├── base.jam             # Core rules reused from original Jambase (Cc, C++, Main, Library…)
├── build.jam            # Build-phase rules (Cc, C++, File, SharedLibrary…)
├── class.jam            # OOP support (new, INSTANCE)
├── configure.jam        # Configure-phase: detect tools, write Jamrules
├── shell.jam            # Shell/script rules
├── unittest.jam         # RunUnitTest rule and PtUnitTestAction
├── pt.jam               # ConfigurePt — Pt-specific library detection
├── vc*.jam / gcc.jam …  # Toolset-specific compiler flags and rules
├── jam.exe              # Pre-built Windows executable
└── src/                 # C source for the jam executable
    ├── jam.c            # Entry point, option parsing, main()
    ├── jam.h            # Platform detection, OS macros, global config
    ├── builtins.c/.h    # Built-in rules (ECHO, DEPENDS, GLOB… + Pt extensions)
    ├── compile.c/.h     # Statement evaluation, evaluate_rule()
    ├── function.c/.h    # Rule body compilation and execution
    ├── rules.c/.h       # RULE, TARGET, ACTION data structures
    ├── make.c/.h        # Dependency graph analysis (make0)
    ├── make1.c          # Command execution scheduling (make1)
    ├── execnt.c         # Windows command execution
    ├── execunix.c       # Unix command execution
    ├── execcmd.c/.h     # Shared exec interface and ExecCmdCallback
    ├── lists.c/.h       # LIST / LOL (list-of-lists) data types
    ├── object.c/.h      # Immutable interned string (OBJECT)
    ├── variable.c/.h    # Variable get/set/swap
    ├── modules.c/.h     # Module system (module_t, native_rules)
    ├── native.c/.h      # declare_native_rule() for module-scoped rules
    ├── constants.c/.h   # Pre-interned OBJECT constants
    ├── frames.c/.h      # Call frame (FRAME) for rule invocation
    ├── scan.c/.h        # Lexer / scanner
    ├── jamgram.c/.y     # Yacc grammar → parse tree
    ├── strings.c/.h     # Dynamic string buffer (string)
    ├── hash.c/.h        # Hash table
    ├── mem.c/.h         # Memory allocation (+ Pt PT_MCHECK extension)
    ├── Makefile.win32   # nmake build for Windows
    ├── build.bat/.sh    # Bootstrap build scripts
    └── modules/         # Native rule implementations for Boost.Build modules
        ├── set.c        #   set.difference
        ├── path.c       #   path operations
        ├── regex.c      #   regex operations
        └── …
```

## Two Layers: C Builtins vs Jam Rules

Jam rules are defined in one of two ways:

| Layer | Location | Mechanism | Example |
|-------|----------|-----------|---------|
| **C builtin** | `jam/src/builtins.c` | `bind_builtin()` in `load_builtins()` | `ECHO`, `GLOB`, `EXEC`, `WriteFile` |
| **Jam script** | `jam/*.jam`, `jam/Jambase` | `rule RuleName { … }` | `Cc`, `C++`, `Main`, `SharedLibrary`, `RunUnitTest` |

C builtins are available globally. Jam-script rules are loaded via
`Load <file>.jam ;` from the `Jambase` or from other `.jam` files.

Actions (shell commands) are always defined in Jam script, even when the
associated rule logic is in C:
```jam
actions Cc { $(CC) -c -o $(<) $(CCFLAGS) $(>) }
```

### Pt-Specific Extensions (C)

The following builtins were added by the Pt project:

| Builtin | Purpose |
|---------|---------|
| `EXEC` | Run a command synchronously, return exit code + stdout lines |
| `WriteFile` | Append a line to a file (used heavily in configure) |

These are marked with `/* Pt extension */` comments.

### Pt-Specific Extension (Jam Script)

The `Jambase` has a Pt-specific modification: when no `-f` flag is given, it
looks for a `Jambase` file in the current directory before falling back to the
compiled-in default (`jam/src/jam.c` around line 504).

## How to Add a New C Builtin Rule

### 1. Declare in `builtins.h`

```c
/* Pt extension: */
LIST *builtin_myrule(FRAME *frame, int flags);
```

### 2. Register in `load_builtins()` in `builtins.c`

```c
/* Pt extension */
{
    char const * args[] = { "arg1", ":", "arg2", "?", 0 };
    bind_builtin( "MYRULE", builtin_myrule, 0, args );
}
```

Argument spec tokens:
- `"name"` — parameter name (documentation only, not enforced)
- `"*"` — zero or more values
- `"+"` — one or more values
- `"?"` — zero or one value
- `":"` — separator between argument groups
- `0` — null terminator

Use `duplicate_rule("Alias", bind_builtin(…))` to register case-insensitive
aliases (e.g. `Echo` / `ECHO`).

### 3. Implement the function in `builtins.c`

```c
/* Pt extension: MYRULE
 */
LIST *builtin_myrule(FRAME *frame, int flags)
{
    LIST * arg1 = lol_get(frame->args, 0);  /* 1st group (before :) */
    LIST * arg2 = lol_get(frame->args, 1);  /* 2nd group (after :) */

    if (list_empty(arg1))
        return L0;

    char const * str = object_str(list_front(arg1));
    /* … */
    return list_new(object_new("result"));
}
```

### 4. Add to `Makefile.win32`

If you created a new `.c` file, add it to the `SOURCES` variable in
`jam/src/Makefile.win32`. For a new builtin in the existing `builtins.c`, no
build file changes are needed.

## Key C API Patterns

### OBJECT — Immutable Interned String
```c
OBJECT * o = object_new("hello");   // create (or intern)
char const * s = object_str(o);     // get C string
OBJECT * copy = object_copy(o);     // reference copy
object_free(o);                     // release
```

### LIST — Linked List of OBJECTs
```c
LIST * l = L0;                                  // empty list (NULL)
l = list_new(object_new("first"));              // single-element list
l = list_push_back(l, object_new("second"));    // append
int n = list_length(l);
int empty = list_empty(l);                      // l == L0
OBJECT * first = list_front(l);                 // first element

// Iteration
LISTITER iter = list_begin(l);
LISTITER end  = list_end(l);
for (; iter != end; iter = list_next(iter)) {
    char const * s = object_str(list_item(iter));
}

list_free(l);   // free list and its objects
```

**Ownership**: `list_new()` and `list_push_back()` take ownership of the
OBJECT passed to them. Use `object_copy()` if you need to keep a reference.

### LOL — List of Lists (Argument Groups)
```c
LIST * first_group  = lol_get(frame->args, 0);  // before first ':'
LIST * second_group = lol_get(frame->args, 1);  // after first ':'
// up to LOL_MAX (19) groups
```

### FRAME — Rule Invocation Context
```c
struct frame {
    FRAME    * prev;          // caller frame
    LOL        args[1];       // argument groups
    module_t * module;        // current module
    OBJECT   * file;          // source file name
    int        line;          // source line
    char const * rulename;    // name of the executing rule
};
```

### Variables
```c
OBJECT * name = object_new("MYVAR");
LIST * val = var_get(root_module(), name);   // read
var_set(root_module(), name, list_new(object_new("value")), VAR_SET);
object_free(name);
```

### string — Dynamic Buffer
```c
string buf[1];
string_new(buf);
string_append(buf, "hello ");
string_append(buf, "world");
// buf->value is the C string, buf->size is the length
string_free(buf);
```

### Return Values

- `L0` — empty result (no value / false in boolean context)
- `list_new(object_new("value"))` — single-value result
- Build up multi-value results with `list_push_back()`

## How to Add or Modify Jam Script Rules

### Rule Definition
```jam
rule MyRule
{
    local _target = $(1) ;    # first argument (same as $(<))
    local _source = $(2) ;    # second argument (same as $(>))
    # …
}
```

### Action Definition
Actions define the actual shell commands:
```jam
actions MyAction
{
    $(TOOL) -o $(<) $(>) $(FLAGS)
}
```
- `$(<)` — targets, `$(>)` — sources
- Actions are bound to targets and executed by `make1`

### Calling C Builtins from Jam
C builtins are called like any rule. Return values are captured with `[ ]`:
```jam
local result = [ EXEC "echo hello" ] ;
local exitcode = $(result[1]) ;
local output = $(result[2-]) ;
```

### Loading Order (Jambase)
1. `Jambase` loads `base.jam` via `Load base.jam ;`
2. If `configure` or `switch` is in `ARGV`, loads `configure.jam`
3. `configure.jam` detects platform tools, writes `Jamrules`
4. `Jamconfigure` (project-specific) is included during configure
5. During build: loads `build.jam`, then project `Jamfile`
6. Toolset files (`vc17.jam`, `gcc.jam`, …) are loaded by `ConfigureToolset`

### Key Jam Script Files

| File | Phase | Purpose |
|------|-------|---------|
| `Jambase` | Boot | Entry point, dispatches configure vs build |
| `base.jam` | Both | Core rules from original Jambase |
| `build.jam` | Build | Cc, C++, Main, Library, SharedLibrary, File… |
| `configure.jam` | Configure | Tool detection, ConfigureWrite, ConfigureSave |
| `class.jam` | Both | `new` keyword, OOP support |
| `unittest.jam` | Build | `RunUnitTest`, `PtUnitTestAction` |
| `pt.jam` | Configure | `ConfigurePt` — detect Pt includes/libs |
| `vc*.jam` / `gcc.jam` | Configure | Toolset-specific compiler setup |

## Building the Jam Executable

- **Windows**: `cd jam\src && nmake /f Makefile.win32` (requires MSVC `cl` in PATH)
- **Unix**: `cd jam/src && ./build.sh <toolset>` (gcc, clang, etc.)
- Pre-built binaries: `jam/jam.exe` and `jam/jam-x86.exe`
- New `.c` files must be added to `SOURCES` in `Makefile.win32`
- Debug memory: compile with `/DPT_MCHECK` for allocation tracking
