---
proposal: jam-packages.md
---

# Prebuilt Packages

A prebuilt bundle supplies compiled headers and libraries for one
platform. Its manifest is the pure-data file `prebuilts.jam` in the
bundle root. `prebuilt.jam` translates the `prebuilts.*` assignments
into the common package variables.

## Manifest {#manifest}

The file `prebuilts.jam` contains only assignments. It has no rules,
no include guard and no toolset flags. Unknown fields are ignored by
Jam.

Bundle-wide variables:

- `prebuilts.version` — format version (must be 1)
- `prebuilts.id` — same string as the directory / zip name
- `prebuilts.packages` — list of package names

Per-package variables use the form `prebuilts.<pkg>.<FIELD>`.
Omit empty lists. Values are names, tokens or paths relative to the
bundle root:

- `prebuilts.<pkg>.LIBS` — library names inside the bundle
- `prebuilts.<pkg>.DEPENDS` — other packages in this bundle
- `prebuilts.<pkg>.SYSLIBS` — OS linker names (not in the zip)
- `prebuilts.<pkg>.FRAMEWORKS` — macOS framework names
- `prebuilts.<pkg>.DEFINES` — preprocessor tokens
- `prebuilts.<pkg>.INCLUDES` — include paths relative to the bundle root
  (only when not `<pkg>/include`)
- `prebuilts.<pkg>.LIBDIR` — library paths relative to the bundle root
  (only when not `<pkg>/lib`)
- `prebuilts.<pkg>.LINKFLAGS` — extra linker flags
- `prebuilts.<pkg>.VERSION` — version string

Optional fields (ignored by Jam, available for other tools):

- `prebuilts.<pkg>.LICENSE` — SPDX identifier
- `prebuilts.<pkg>.URL` — upstream URL
- `prebuilts.<pkg>.SHA256` — checksum of key artifacts
- any other `prebuilts.<pkg>.*` key

Each package entry is the complete public usage of that package for
the linkage of this bundle: `LIBS`, `SYSLIBS`, `FRAMEWORKS`, `DEFINES`
and `LINKFLAGS`. The author copies public flags of dependencies onto
the consumed entry when a consumer of that entry needs them. A shared
bundle lists the public libraries. A static bundle lists the full
closure in link order.

`DEPENDS` is the dependency graph of the bundle. `Prebuilt-Use` walks
it only for include paths and library search directories. There is no
linkage field and no walk for `LINKLIBS`, `DEFINES` or `LINKFLAGS`.

Deployment uses the same graph. A consumer calls `Prebuilt-Depends` to
obtain the closure, then `Prebuilt-Bin` and `Prebuilt-Lib` to collect
the files to install. `Prebuilt-Use` does not select runtime files.

OpenSSL is one package `openssl`. Its entry is the complete public
usage (libraries `ssl` and `crypto`, extra include or lib paths when
the bundle does not use `openssl/include` and `openssl/lib`). `ssl/`
and `crypto/` may be directories of that package; they are not
packages of their own.

Example:

```jam
# prebuilts.jam
prebuilts.version  = 1 ;
prebuilts.id       = windows-x64-msvc-md ;
prebuilts.packages = zlib png jpeg freetype openssl ;

prebuilts.zlib.LIBS     = z ;
prebuilts.zlib.VERSION  = 1.3.1 ;
prebuilts.zlib.LICENSE  = Zlib ;

prebuilts.openssl.LIBS     = ssl crypto ;
prebuilts.openssl.SYSLIBS  = crypt32 ws2_32 advapi32 user32 ;
prebuilts.openssl.VERSION  = 3.3.1 ;
prebuilts.openssl.LICENSE  = OpenSSL ;
```

Directory layout:

```
prebuilt/<id>/
  prebuilts.jam
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
Prebuilt-Load dest
Prebuilt-Use pkg
```

`Prebuilt-Load dest` binds the bundle. It records the root, includes
`prebuilts.jam` from that directory, and checks that `prebuilts.version`
is 1. A missing file or an unsupported version is an `Exit`. The caller
invokes it once when a prebuilt folder is selected. `PREBUILT_ROOT` is
internal to `prebuilt.jam`.

`Prebuilt-Use pkg` returns the package name when the loaded bundle lists
the package, empty otherwise. On success it fills only still-empty common
flag variables (already set values, including `-s` overrides, are left
unchanged):

- `pkg.INCLUDES` from the include paths (default `<pkg>/include` plus
  any `prebuilts.<pkg>.INCLUDES`, walking `DEPENDS`)
- `pkg.DEFINES` from `prebuilts.<pkg>.DEFINES` (via `FDefines`).
  `DEPENDS` is not walked.
- `pkg.LINKLIBS` from `prebuilts.<pkg>.LIBS`, `SYSLIBS` and
  `FRAMEWORKS` (via `FLinkLibs`). `DEPENDS` is not walked. The entry
  is the complete public link line for this bundle.
- `pkg.LIBDIR` from the library paths (default `<pkg>/lib` plus any
  `prebuilts.<pkg>.LIBDIR`, walking `DEPENDS`)
- `pkg.LINKFLAGS` from `prebuilts.<pkg>.LINKFLAGS`. `DEPENDS` is not
  walked.
- leaves `pkg.TARGET` empty

The fill is done through the `Packages-Set*` accessors, but only when
the corresponding variable is still empty. The existing `Prebuilt-*`
primitive rules stay. They walk `prebuilts.<pkg>.DEPENDS` for include
paths and library search directories. They read the bundle bound by
`Prebuilt-Load`.
