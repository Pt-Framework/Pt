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

The six variables are the complete public usage of the package, the
same result a consumer would get from pkgconf. Consumers read only
these variables. Package-specific `*-Configure` rules stay ignorant of
whether the values came from a user, a prebuilt bundle, a source tree,
or a system probe.

Prebuilt and system packages fill the flag variables and leave
`TARGET` empty. A source package fills `TARGET` with its Jam library
target and usually `INCLUDES`. When it depends on other packages it
copies their public usage onto its own variables.

A non-empty `pkg.TARGET` means the package is builtin (source-built).
Prebuilt and system leave `TARGET` empty.

## Variables {#variables}

The common variables are dotted: `$(pkg.INCLUDES)`, `$(pkg.DEFINES)`,
`$(pkg.LINKLIBS)`, `$(pkg.LIBDIR)`, `$(pkg.LINKFLAGS)`, `$(pkg.TARGET)`.

`pkg.TARGET` is the Jam target name of a source-built library. Empty
when the package is prebuilt or system. Consumers call `LinkLibraries`
with that name when it is set. They also apply `LINKLIBS`, `FLibPath`
of `LIBDIR`, and `LINKFLAGS`.

`pkg.INCLUDES` is raw include directories. `pkg.DEFINES` is formatted
compiler flags (`-DFOO=1` / `/DFOO=1`), the result of `FDefines`.
The manifest stores raw tokens (`FOO=1`); `Prebuilt-Use` converts them
with `FDefines` when filling `pkg.DEFINES`. `pkg.LINKLIBS` is
link-library flags. `pkg.LIBDIR` is raw library search directories.
`pkg.LINKFLAGS` is extra linker flags. Empty lists are omitted:
`SubDirHdrs` and `SubDirCcFlags` run only when the package supplied
includes or defines.

A consumer applies the variables with the ordinary Jam rules:

```
if [ Package-Includes pkg ]
{
    SubDirHdrs [ Package-Includes pkg ] ;
}

if [ Package-Defines pkg ]
{
    SubDirCcFlags [ Package-Defines pkg ] ;
}

if [ Package-Target pkg ]
{
    LinkLibraries tgt : [ Package-Target pkg ] ;
}

LinkLibs  tgt : [ Package-LinkLibs pkg ] ;
LinkFlags tgt : [ FLibPath [ Package-LibDir pkg ] ]
                [ Package-LinkFlags pkg ] ;
```

C targets use `SubDirCcFlags`. C++ targets use `SubDirC++Flags`.

Consumers, user overrides and Configure rules use only the dotted form.
The former five-tuple names (`ZLIB_*`, `PNG_*`, `JPEG_*`, `FREETYPE_*`,
`SSL_*`) are not produced and are not read.

`package.jam` provides accessors so callers do not spell the dotted
names by hand:

```
Package-Includes pkg
Package-Defines pkg
Package-LinkLibs pkg
Package-LibDir pkg
Package-LinkFlags pkg
Package-Target pkg

Package-SetIncludes pkg : values *
Package-SetDefines pkg : values *
Package-SetLinkLibs pkg : values *
Package-SetLibDir pkg : values *
Package-SetLinkFlags pkg : values *
Package-SetTarget pkg : value
```

Getters return the current list (empty if unset). Setters replace the
list. Direct assignment to the dotted variables remains valid; the
accessors are the preferred form.

`package.jam` also provides:

```
Package-Echo pkg
Package-Configure pkg
```

`Package-Echo` always prints the six variables via `EchoOption`.
`Package-Configure` persists them via `Configure`. Both operate only
on the common names.

## Modules {#modules}

Three modules own the mechanism:

- `package.jam` defines the common variable interface, its accessors,
  `Package-Echo` and `Package-Configure`. It knows nothing about
  prebuilt bundles, source package roots, configure options, or
  resolution order.
- `prebuilt.jam` maps a prebuilt bundle onto the common variables.
- `builtin.jam` selects source packages from a root directory.

`package.jam` is included once. The other two modules are loaded on
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
   `Jamconfigure` calls `Prebuilt-Load root` once after fetch.
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
That file publishes the six variables only after its checks succeed.
A non-empty `pkg.TARGET` is the success signal. `Builtin-Use` returns
the package name when `pkg.TARGET` is set, empty otherwise. A configure
error in the package `Jamconfigure` calls `Exit` and aborts. A missing
root returns empty.

A `*-Configure` rule receives the current flag values as hints. It does
not know whether they came from a prebuilt bundle, a user override, or
an empty system probe.

It probes with `CheckBuildC++`. `FIncludes` of `INCLUDES` and `FLibPath`
of `LIBDIR` appear only on the probe line, together with `LINKLIBS` and
`LINKFLAGS`. On success it writes the five flag variables unmerged
through the `Package-Set*` accessors (`LIBDIR` still raw, `LINKFLAGS`
still extra flags) and returns the package name. On failure it returns
empty and leaves the variables unchanged. It does not set `pkg.TARGET`.

`SetDependencyPath`, `DEPS_INCLUDES`, `DEPS_LIBDIR`,
`CheckDependencyCompileC++`, `CheckDependencyLinkC++` and
`CheckDependencyC++` are not used.

`pkg.TARGET` is set only by a builtin package `Jamconfigure`. Consumers
detect a source package by a non-empty target. There is no separate
`WITH_BUILTIN_xxx` flag.

`Package-Echo` always runs so the configure output shows the option
and the variable values. `Package-Configure` runs only when `WITH_pkg`
is set. It persists the six variables that are already set; it does not
probe and it does not decide success. An unfound package does not
persist values for later build steps.

When builtin is selected the package-specific `*-Configure` is not
called. The source package `Jamconfigure` has already published the
six variables.

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
                    [ Package-Includes zlib ]
                  : [ Package-Defines zlib ]
                  : [ Package-LinkLibs zlib ]
                  : [ Package-LibDir zlib ]
                  : [ Package-LinkFlags zlib ] ] ;
}

# 3. Builtin fallback
if ! $(WITH_ZLIB)
{
    WITH_ZLIB = [ Builtin-Use zlib : src/zlib ] ;
}

Package-Echo zlib ;
EchoOption WITH_ZLIB : "ZLib found" : $(WITH_ZLIB:E="no") ;

if $(WITH_ZLIB)
{
    Package-Configure zlib ;
}
```

The same pattern applies to png, jpeg and FreeType.

### SSL backends

`Jamconfigure` selects the Pt-Ssl backend from packages `openssl` and
`mbedtls`. Builtin only with `--with-inline-openssl` or
`--with-inline-mbedtls`. `--with-openssl` and `--with-mbedtls` name the
prebuilt or system backend.

Independent `if`s, each `if ! $(WITH_SSL)`. Explicit flags run first:

1. `--with-inline-openssl`: `Builtin-Use openssl`
2. `--with-openssl`: `Prebuilt-Use openssl` and `OpenSsl-Configure`
3. `--with-inline-mbedtls`: `Builtin-Use mbedtls`
4. `--with-mbedtls`: `Prebuilt-Use mbedtls` and `MbedTls-Configure`
5. Default openssl: `Prebuilt-Use openssl` and `OpenSsl-Configure`
6. Default mbedtls: `Prebuilt-Use mbedtls` and `MbedTls-Configure`
7. `PT_SSL_IMPL = nossl`

Success sets `WITH_SSL` and `PT_SSL_IMPL` to `openssl` or `mbedtls`.
`Pt-Ssl` reads the accessors of that package. The bcrypt probe for
builtin mbedtls lives in `src/mbedtls/Jamconfigure`.
