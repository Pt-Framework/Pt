---
proposal: jam-packages.md
---

# Prebuilt Packages

A prebuilt bundle supplies compiled headers and libraries for one
platform. Its manifest is the pure-data file `prebuilts.jam` in the
bundle root. `prebuilt.jam` translates the `prebuilts.*` assignments
into the common package variables.

## Baseline

`Prebuilt-Use` sets `PREBUILT_ROOT` and includes `jam/packages.jam`.
The rules `Prebuilt-Includes`, `Prebuilt-LinkLibs`, `Prebuilt-LibPath`
and `Prebuilt-LinkFlags` return the values that `Jamconfigure` currently
assigns with `?=` into `ZLIB_INCLUDES` and the other five variables
before it calls `ZLib-Configure`. The same pattern is used for png,
jpeg, FreeType and OpenSSL.

The manifest uses `PREBUILT_SCHEMA`, `PREBUILT_ID`, `PREBUILT_PACKAGES`
and dotted names of the form `PREBUILT_PACKAGE_LIBS.$(pkg)`. It lives
under `jam/` and has an include guard. Schema must be 1.

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

`DEPENDS` is the dependency graph of the bundle. `Prebuilt-Use` walks
it only to collect include paths. Link libraries, system libraries,
frameworks and link flags are taken as written. A shared bundle lists
the direct libraries. A static bundle lists the full closure in link
order. There is no linkage field and no automatic walk for `LINKLIBS`.

Deployment uses the same graph. A consumer calls `Prebuilt-Depends` to
obtain the closure, then `Prebuilt-Bin` and `Prebuilt-Lib` to collect
the files to install. `Prebuilt-Use` does not select runtime files.

Components are ordinary packages. OpenSSL appears as the two packages
`ssl` and `crypto` (or equivalent names). Each has its own directory
and its own `prebuilts.<pkg>.*` variables.

Example:

```jam
# prebuilts.jam
prebuilts.version  = 1 ;
prebuilts.id       = windows-x64-msvc-md ;
prebuilts.packages = zlib png jpeg freetype ssl crypto ;

prebuilts.zlib.LIBS     = z ;
prebuilts.zlib.VERSION  = 1.3.1 ;
prebuilts.zlib.LICENSE  = Zlib ;

prebuilts.ssl.LIBS      = ssl ;
prebuilts.ssl.DEPENDS   = crypto ;
prebuilts.ssl.SYSLIBS   = crypt32 ws2_32 advapi32 user32 ;
prebuilts.ssl.VERSION   = 3.3.1 ;
prebuilts.ssl.LICENSE   = OpenSSL ;

prebuilts.crypto.LIBS     = crypto ;
prebuilts.crypto.VERSION  = 3.3.1 ;
prebuilts.crypto.LICENSE  = OpenSSL ;
```

Directory layout:

```
prebuilt/<id>/
  prebuilts.jam
  zlib/
    include/
    lib/
    bin/          # optional
  ssl/
    include/
    lib/
    bin/
  crypto/
    ...
```

## Use {#use}

`prebuilt.jam` provides:

```
Prebuilt-Use pkg
```

It returns the package name when the loaded bundle lists the package,
empty otherwise. On success it fills only still-empty common flag
variables (already set values, including `-s` overrides, are left
unchanged):

- `pkg.INCLUDES` from the include paths (default `<pkg>/include` plus
  any `prebuilts.<pkg>.INCLUDES`, walking `DEPENDS`)
- `pkg.DEFINES` from `prebuilts.<pkg>.DEFINES` (via `FDefines`)
- `pkg.LINKLIBS` from `prebuilts.<pkg>.LIBS`, `SYSLIBS` and
  `FRAMEWORKS` (via `FLinkLibs`). `DEPENDS` is not walked for link
  libraries. The bundle author lists the complete link line for the
  linkage of this bundle.
- `pkg.LIBDIR` from the library paths (default `<pkg>/lib` plus any
  `prebuilts.<pkg>.LIBDIR`)
- `pkg.LINKFLAGS` from `prebuilts.<pkg>.LINKFLAGS`
- leaves `pkg.TARGET` empty

The fill is done through the `Packages-Set*` accessors, but only when
the corresponding variable is still empty. The existing `Prebuilt-*`
primitive rules stay. They walk `prebuilts.<pkg>.DEPENDS` for include
paths only.

`Prebuilt-Use` checks that `prebuilts.version` is 1.
