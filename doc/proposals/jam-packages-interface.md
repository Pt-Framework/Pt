---
proposal: jam-packages.md
---

# Package Interface

Source packages and prebuilt packages share one variable interface.
A package name `pkg` exposes these variables:

- `pkg.INCLUDES`
- `pkg.DEFINES`
- `pkg.LINKLIBS`
- `pkg.LIBDIR`
- `pkg.LINKFLAGS`
- `pkg.TARGET`

Prebuilt and system packages fill the flag variables. Source packages
fill `pkg.TARGET` (and usually `pkg.INCLUDES`). Consumers read only
these variables. Package-specific `*-Configure` rules stay ignorant of
whether the values came from a user, a prebuilt bundle, a source tree,
or a system probe.

A non-empty `pkg.TARGET` means the package is builtin (source-built).
Prebuilt and system leave `TARGET` empty.

## Baseline

`CheckDependencyLinkC++` and `CheckDependencyC++` apply `FLibPath` to
`LIBDIR` and store the result in `*_LINKFLAGS`. `SetDependencyPath`
exists in `configure.jam` and has no callers. `Jamconfigure` persists
`INCLUDES`, `C++FLAGS`, `LINKLIBS` and `LINKFLAGS`; it does not persist
`LIBDIR`. `src/Pt/Jamfile`, `src/Pt-Gfx/Jamfile` and `src/Pt-Ssl/Jamfile`
apply `LinkLibs` and `LinkFlags` only.

## Variables {#variables}

The common variables are dotted: `$(pkg.INCLUDES)`, `$(pkg.DEFINES)`,
`$(pkg.LINKLIBS)`, `$(pkg.LIBDIR)`, `$(pkg.LINKFLAGS)`, `$(pkg.TARGET)`.

`pkg.TARGET` is the Jam target name of a source-built library. When it
is set, linking uses that target. `pkg.LINKLIBS`, `pkg.LIBDIR` and
`pkg.LINKFLAGS` stay unused for that consumer.

`pkg.INCLUDES` is raw include directories. `pkg.DEFINES` is formatted
compiler flags (`-DFOO=1` / `/DFOO=1`), the result of `FDefines`.
The manifest stores raw tokens (`FOO=1`); `Prebuilt-Use` converts them
with `FDefines` when filling `pkg.DEFINES`. `pkg.LINKLIBS` is
link-library flags. `pkg.LIBDIR` is raw library search directories.
`pkg.LINKFLAGS` is extra linker flags. Empty lists are omitted.

A consumer applies the flags with the ordinary Jam rules:

```
SubDirHdrs [ Packages-Includes pkg ] ;
SubDirCcFlags [ Packages-Defines pkg ] ;
LinkLibs  tgt : [ Packages-LinkLibs pkg ] ;
LinkFlags tgt : [ FLibPath [ Packages-LibDir pkg ] ]
                [ Packages-LinkFlags pkg ] ;
```

C targets use `SubDirCcFlags`. C++ targets use `SubDirC++Flags`.

Consumers, user overrides and Configure rules use only the dotted form.
The former five-tuple names (`ZLIB_*`, `PNG_*`, `JPEG_*`, `FREETYPE_*`,
`SSL_*`) are not produced and are not read.

`packages.jam` provides accessors so callers do not spell the dotted
names by hand:

```
Packages-Includes pkg
Packages-Defines pkg
Packages-LinkLibs pkg
Packages-LibDir pkg
Packages-LinkFlags pkg
Packages-Target pkg

Packages-SetIncludes pkg : values *
Packages-SetDefines pkg : values *
Packages-SetLinkLibs pkg : values *
Packages-SetLibDir pkg : values *
Packages-SetLinkFlags pkg : values *
Packages-SetTarget pkg : value
```

Getters return the current list (empty if unset). Setters replace the
list. Direct assignment to the dotted variables remains valid; the
accessors are the preferred form.

`packages.jam` also provides:

```
Packages-Echo pkg
Packages-Configure pkg
```

`Packages-Echo` always prints the six variables via `EchoOption`.
`Packages-Configure` persists them via `Configure`. Both operate only
on the common names.

## Modules {#modules}

Three modules own the mechanism:

- `packages.jam` defines the common variable interface, its accessors,
  `Packages-Echo` and `Packages-Configure`. It knows nothing about
  prebuilt bundles, source package roots, configure options, or
  resolution order.
- `prebuilt.jam` maps a prebuilt bundle onto the common variables.
- `builtin.jam` registers source-package roots and selects them.

`packages.jam` is included once. The other two modules are loaded on
demand by `Jamconfigure`.

## Configure {#configure}

Resolution order and Configure dispatch live in `Jamconfigure`. It is
the only place that knows about user settings, configure options,
prebuilt and builtin.

For each package the steps are independent `if`s. Each step runs only
while `WITH_pkg` is still empty. The first success sets `WITH_pkg`.

1. Builtin override: `--with-inline-xxx` is given and `Builtin-Use`
   succeeds. This overrides every other source.
2. Prebuilt or system/custom: when `--with-prebuilt` is selected,
   `Jamconfigure` calls `Prebuilt-Load dest` once after fetch.
   `Prebuilt-Use` fills only still-empty flag variables when the
   bundle contains the package (side effect only). Already set values
   (including `-s` overrides) are left unchanged. Then the
   package-specific `*-Configure` is called with the resulting current
   values (prebuilt paths, `-s` hints, or empty for a system probe).
   Custom `-s` settings are hints for the system probe, not a separate
   source.
3. Builtin fallback: `Builtin-Use` succeeds when nothing earlier did.

`Prebuilt-Use` returns the package name when the bundle contains it,
empty otherwise. Its return value is not assigned to `WITH_pkg`. It
only fills still-empty flag variables.

`Builtin-Use pkg : root` includes the source package's `Jamconfigure`.
That file calls `Packages-SetIncludes` and `Packages-SetTarget` only
after its checks succeed. A non-empty `pkg.TARGET` is the success
signal. `Builtin-Use` returns the package name when `pkg.TARGET` is
set, empty otherwise. A configure error in the package `Jamconfigure`
calls `Exit` and aborts. A missing root returns empty.

A `*-Configure` rule receives the current flag values as hints. It does
not know whether they came from a prebuilt bundle, a user override, or
an empty system probe.

It probes with `CheckBuildC++`. `FIncludes` of `INCLUDES` and `FLibPath`
of `LIBDIR` appear only on the probe line, together with `LINKLIBS` and
`LINKFLAGS`. On success it writes the five flag variables unmerged
through the `Packages-Set*` accessors (`LIBDIR` still raw, `LINKFLAGS`
still extra flags) and returns the package name. On failure it returns
empty and leaves the variables unchanged. It does not set `pkg.TARGET`.

`SetDependencyPath`, `DEPS_INCLUDES`, `DEPS_LIBDIR`,
`CheckDependencyCompileC++`, `CheckDependencyLinkC++` and
`CheckDependencyC++` are not used.

`pkg.TARGET` is set only by a builtin package `Jamconfigure`. Consumers
detect a source package by a non-empty target. There is no separate
`WITH_BUILTIN_xxx` flag.

`Packages-Echo` always runs so the configure output shows the option
and the variable values. `Packages-Configure` runs only when `WITH_pkg`
is set. It persists the six variables that are already set; it does not
probe and it does not decide success. An unfound package does not
persist values for later build steps.

When builtin is selected the package-specific `*-Configure` is not
called. The source package `Jamconfigure` has already set
`pkg.INCLUDES` and `pkg.TARGET`.

### zlib example

```
Load zlib.jam ;

EchoOption --with-inline-zlib : "Using inline zlib"
    : $([ HasOption --with-inline-zlib ]:E="no") ;

# 1. Builtin per Option (Override)
if ! $(WITH_ZLIB) && --with-inline-zlib in $(ARGV)
{
    WITH_ZLIB = [ Builtin-Use zlib : src/zlib ] ;
}

# 2. Prebuilt or System/Custom
if ! $(WITH_ZLIB)
{
    Prebuilt-Use zlib ;

    WITH_ZLIB = [ ZLib-Configure
                    [ Packages-Includes zlib ]
                  : [ Packages-Defines zlib ]
                  : [ Packages-LinkLibs zlib ]
                  : [ Packages-LibDir zlib ]
                  : [ Packages-LinkFlags zlib ] ] ;
}

# 3. Builtin fallback
if ! $(WITH_ZLIB)
{
    WITH_ZLIB = [ Builtin-Use zlib : src/zlib ] ;
}

Packages-Echo zlib ;
EchoOption WITH_ZLIB : "ZLib found" : $(WITH_ZLIB:E="no") ;

if $(WITH_ZLIB)
{
    Packages-Configure zlib ;
}
```

The same pattern applies to png, jpeg and FreeType.

### SSL backends

`Jamconfigure` selects the Pt-Ssl backend. The package is `openssl`.
Resolution uses `Prebuilt-Use`, `OpenSsl-Configure` and
`Builtin-Use openssl : src/openssl`. Builtin runs only for
`--with-openssl`; there is no builtin fallback. Success sets
`PT_SSL_IMPL` to `openssl` and `WITH_SSL`. `--with-mbedtls` is the
other backend override. When neither backend is found, `PT_SSL_IMPL`
is `nossl`. `Pt-Ssl` reads the accessors of `openssl` or `mbedtls`.
