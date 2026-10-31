# Jam Packages

## Documents

- [Package Interface](jam-packages-interface.md)
- [Prebuilt](jam-packages-prebuilt.md)
- [Builtin Source](jam-packages-builtin.md)

## Open Tasks

- [variables](jam-packages-interface.md#variables): Introduce the common package variables (`pkg.INCLUDES`, `pkg.DEFINES`, `pkg.LINKLIBS`, `pkg.LIBDIR`, `pkg.LINKFLAGS`, `pkg.TARGET`).
- [modules](jam-packages-interface.md#modules): Split into `packages.jam` (interface only), `prebuilt.jam` and `builtin.jam`.
- [source-init](jam-packages-builtin.md#init): Add `Builtin-Use` / source package root registration and presence check.
- [source-target](jam-packages-builtin.md#target): Source packages set their own `pkg.TARGET` in their Jamconfigure.
- [configure-skip](jam-packages-interface.md#configure): In `Jamconfigure`, skip package-specific `*-Configure` when a source package is selected by configure option; otherwise call it (values already set or system probe).

## Priority

Interface and modules first, then builtin source support.
