---
proposal: jam-packages.md
---

# Prebuilt Packages

A prebuilt bundle supplies compiled headers and libraries for one
platform. Its manifest is the pure-data file `prebuilt.jam` in the
bundle root. `prebuilt.jam` translates the `prebuilt.*` assignments
into the common package variables.

## Manifest {#manifest}

The file `prebuilt.jam` contains only assignments. It has no rules,
no include guard and no toolset flags. Unknown fields are ignored by
Jam.

Bundle-wide variables:

- `prebuilt.VERSION` — format version (must be 1)
- `prebuilt.ID` — same string as the directory / zip name
- `prebuilt.PACKAGES` — list of package names

Per-package variables use the form `prebuilt.<pkg>.<FIELD>`.
Omit empty lists. Values are names, tokens or paths relative to the
bundle root:

- `prebuilt.<pkg>.LIBS` — library names inside the bundle
- `prebuilt.<pkg>.DEPENDS` — other packages in this bundle
- `prebuilt.<pkg>.SYSLIBS` — OS linker names (not in the zip)
- `prebuilt.<pkg>.FRAMEWORKS` — macOS framework names
- `prebuilt.<pkg>.DEFINES` — preprocessor tokens
- `prebuilt.<pkg>.INCLUDES` — include paths relative to the bundle root
  (only when not `<pkg>/include`)
- `prebuilt.<pkg>.LIBDIR` — library paths relative to the bundle root
  (only when not `<pkg>/lib`)
- `prebuilt.<pkg>.LINKFLAGS` — extra linker flags
- `prebuilt.<pkg>.VERSION` — version string

Optional fields (ignored by Jam, available for other tools):

- `prebuilt.<pkg>.LICENSE` — SPDX identifier
- `prebuilt.<pkg>.URL` — upstream URL
- `prebuilt.<pkg>.SHA256` — checksum of key artifacts
- any other `prebuilt.<pkg>.*` key

Each package entry is the complete public usage of that package for
the linkage of this bundle: `LIBS`, `SYSLIBS`, `FRAMEWORKS`, `DEFINES`
and `LINKFLAGS`. The author copies public flags of dependencies onto
the consumed entry when a consumer of that entry needs them. A shared
bundle lists the public libraries. A static bundle lists the full
closure in link order.

`DEPENDS` is the dependency graph of the bundle. `Prebuilt-Enable` walks
it only for include paths and library search directories. There is no
linkage field and no walk for `LINKLIBS`, `DEFINES` or `LINKFLAGS`.

Deployment uses the same graph. A consumer calls `Prebuilt-Depends` to
obtain the closure, then `Prebuilt-Bin` and `Prebuilt-Lib` to collect
the files to install. `Prebuilt-Enable` does not select runtime files.

OpenSSL is one package `openssl`. Its entry is the complete public
usage (libraries `ssl` and `crypto`, extra include or lib paths when
the bundle does not use `openssl/include` and `openssl/lib`). `ssl/`
and `crypto/` may be directories of that package; they are not
packages of their own.

Example:

```jam
# prebuilt.jam
prebuilt.VERSION  = 1 ;
prebuilt.ID       = windows-x64-msvc-md ;
prebuilt.PACKAGES = zlib png jpeg freetype openssl ;

prebuilt.zlib.LIBS     = z ;
prebuilt.zlib.VERSION  = 1.3.1 ;
prebuilt.zlib.LICENSE  = Zlib ;

prebuilt.openssl.LIBS     = ssl crypto ;
prebuilt.openssl.SYSLIBS  = crypt32 ws2_32 advapi32 user32 ;
prebuilt.openssl.VERSION  = 3.3.1 ;
prebuilt.openssl.LICENSE  = OpenSSL ;
```

Directory layout:

```
prebuilt/<id>/
  prebuilt.jam
  zlib/
    include/
    lib/
    bin/          # optional
  openssl/
    include/
    lib/
    bin/
```

## Use {#use}

`prebuilt.jam` provides:

```
Prebuilt-Import root
Prebuilt-Enable pkg
```

`Prebuilt-Import root` binds the bundle. It sets `prebuilt.ROOT`, includes
`prebuilt.jam` from that directory, and checks that `prebuilt.VERSION`
is 1. A missing file or an unsupported version is an `Exit`. The caller
invokes it once when a prebuilt folder is selected. `prebuilt.ROOT` is
internal to `prebuilt.jam` and is not a manifest key.

`Prebuilt-Enable pkg` returns the package name when the loaded bundle lists
the package, empty otherwise. On success it fills only still-empty common
flag variables (already set values, including `-s` overrides, are left
unchanged):

- `pkg.INCLUDES` from the include paths (default `<pkg>/include` plus
  any `prebuilt.<pkg>.INCLUDES`, walking `DEPENDS`)
- `pkg.DEFINES` from `prebuilt.<pkg>.DEFINES` (via `FDefines`).
  `DEPENDS` is not walked.
- `pkg.LINKLIBS` from `prebuilt.<pkg>.LIBS`, `SYSLIBS` and
  `FRAMEWORKS` (via `FLinkLibs`). `DEPENDS` is not walked. The entry
  is the complete public link line for this bundle.
- `pkg.LIBDIR` from the library paths (default `<pkg>/lib` plus any
  `prebuilt.<pkg>.LIBDIR`, walking `DEPENDS`)
- `pkg.LINKFLAGS` from `prebuilt.<pkg>.LINKFLAGS`. `DEPENDS` is not
  walked.
- leaves `pkg.TARGET` empty

`Prebuilt-Enable` prints nothing. Filling hints is not a find. The
following `*-Configure` decides the find.

The fill is done through the `Package-Set*` accessors, but only when
the corresponding variable is still empty. The existing `Prebuilt-*`
primitive rules stay. They walk `prebuilt.<pkg>.DEPENDS` for include
paths and library search directories. They read the bundle bound by
`Prebuilt-Import`.
