# Jam Initialization

## Documents

- [Distribution](jam-init-distribution.md)
- [Runtime](jam-init-runtime.md)

## Open Tasks

- [Vendored snapshot layout](jam-init-distribution.md#phase-1-vendored-snapshot).
  Restructure the vendored Jam files into the `core/` and `local/` layout.

- [Local initialization](jam-init-distribution.md#phase-1-local-initialization).
  Make `--init` validate the vendored core and build a missing host binary.

- [Runtime invocation](jam-init-runtime.md#runtime-invocation).
  Start Jam from the installed core runtime instead of a vendored `jam/`
  directory in the project.

- [Local modules](jam-init-runtime.md#local-modules).
  Make `Load` search the update-safe local extension directory after the core
  runtime directory.

- [Documentation and migration](jam-init-runtime.md#documentation-and-migration).
  Document initialization and migrate the current project-root bootstrap
  layout.

- [Distribution initialization](jam-init-distribution.md#phase-3-distribution-initialization).
  Publish snapshots and add `JAM_DIST_ROOT` and `--update` after the local
  snapshot model is established.

## Priority

The vendored snapshot layout and local initialization come before runtime
invocation. Local modules and migration follow. Distribution initialization is
last.
