---
proposal: jam-rpath.md
---

# Jam Runtime Search Path

## Purpose

Binaries and shared libraries built by Jam must locate their dependent shared libraries at runtime without LD_LIBRARY_PATH (Unix) or PATH changes (Windows). Both a flat layout and a separated bin/lib layout must work. The user controls placement through MakeLocate and MakeInstall. Build-tree and install-tree layouts may be the same or different.

## Runtime search path {#runtime-search-path}

On ELF platforms the link step adds a DT_RUNPATH (not the legacy DT_RPATH) containing two entries:

- $ORIGIN
- $ORIGIN/../lib

The flags are:

```
-Wl,-rpath,\$ORIGIN -Wl,-rpath,\$ORIGIN/../lib -Wl,--enable-new-dtags
```

If the two paths would be identical strings they are deduplicated. The existing -rpath-link handling for link-time transitive dependencies is kept.

On macOS the link step adds:

```
-Wl,-rpath,@executable_path -Wl,-rpath,@executable_path/../lib
```

Shared libraries are built with -install_name @rpath/<libname>.dylib.

On Windows no runtime search path is embedded. When a shared library is installed, its DLL is also installed into the directory given by MakeInstall for the consuming executable (typically the bindir). The import library goes to the directory given by MakeInstall for the library itself.

A variable PT_ENABLE_RPATH (default yes) disables embedding of the runtime search path. This supports system packaging where the dynamic linker search path is sufficient.

Placement remains under user control via MakeLocate and MakeInstall. The defaults in Jamconfigure are unchanged:

- PT_OUTPUT_TARGET -> build/<Config>/
- PT_INSTALL_BINDIR -> deploy/<Config>/bin
- PT_INSTALL_LIBDIR -> deploy/<Config>/lib

## Baseline

The toolset files (gcc.jam, clang.jam, icc-linux.jam) set only RPATH_LINKFLAGS (-Wl,-rpath-link) and collect RPATHLIBS for link-time use. No runtime search path is embedded. On macOS an optional INSTALL_PATH may set -install_name to an absolute path. On Windows DLLs and import libraries are installed according to the MakeInstall destination of the target; DLLs are not automatically copied next to executables.
