# Jam Packages

## Documents

- [Package Interface](jam-packages-interface.md)
- [Prebuilt](jam-packages-prebuilt.md)
- [Builtin Source](jam-packages-builtin.md)

## Open Tasks

- [source-deps](jam-packages-builtin.md#dependencies): Builtin packages consume already-resolved dependencies through the accessors. Consumers apply `FLibPath(LIBDIR)` and `LINKFLAGS`. Dependency packages are resolved first.
- [configure](jam-packages-interface.md#configure): In `Jamconfigure`, resolve each package with independent steps: builtin override (`--with-inline-xxx`), then prebuilt or system/custom via `*-Configure`, then builtin fallback. OpenSSL is one package used as the Pt-Ssl backend (`--with-openssl` override, prebuilt/system, no builtin fallback, else `nossl`). `*-Configure` uses `CheckBuildC++` and writes unmerged variables. Remove `SetDependencyPath` and the three `CheckDependency*` rules (call sites: zlib, png, jpeg, freetype, openssl, qt). `Packages-Configure` only when found.

## Priority

Builtin Use rules first, then Jamconfigure / probes.
