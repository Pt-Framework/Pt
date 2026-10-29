---
proposal: jam-init.md
---

# Jam Initialization Distribution

## Purpose

Jam is maintained as an independent project while remaining a vendored build
system for its consumers. The first implementation keeps a complete Jam
snapshot in the Pt repository. Its entry scripts prepare and start that local
snapshot without a distribution server.

The entry scripts are intentionally small shell and batch programs. Their
bootstrap model must remain practical to implement in POSIX shell, Windows
batch, and, where useful, PowerShell. The first implementation does not
require a separate configuration file, an offline-mode protocol, a download
protocol, or archive extraction.

## Installation root

`JAM_ROOT` identifies the root of one Jam installation. It contains both the
release-managed core and the user-owned local extensions:

```text
<JAM_ROOT>/
  core/
  local/
```

When `JAM_ROOT` is unset, it defaults to `jam` below the directory containing
the entry script. In the first implementation, an external `JAM_ROOT` must
already contain a prepared snapshot supplied by build-server infrastructure.

The Pt repository versions `jam/core/` as its vendored snapshot. Projects
that later initialize `jam/core/` from a distribution may ignore it, while
`jam/local/` remains visible and may be versioned with the project's build
configuration.

## Snapshot layout

`core/` is a complete, versioned Jam snapshot:

```text
<JAM_ROOT>/core/
  meta/
    version
    contents
  Jambase
  lib/
    *.jam
    vswhere.exe
  src/
    *.c
    *.h
    modules/
  bin/
    <host-id>/
      jam[.exe]
```

`meta/` identifies the Jam release, the snapshot format revision, and the
release-owned contents. Runtime scripts and sources in a snapshot represent
the same Jam release. A host binary may be packaged with the snapshot or
built locally from its sources.

## Snapshot metadata

`meta/` is a directory rather than a compound manifest file. This keeps
the scalar values directly readable by POSIX shell, Windows batch, and
PowerShell without a JSON, YAML, or INI parser.

`version` contains exactly one nonempty ASCII line terminated by LF with four
positive decimal fields separated by one ASCII space:

```text
4 0 0 1
```

The first three fields are the pinned Jam major, minor, and patch version. The
fourth field is the positive snapshot format revision. An entry script checks
that it supports the format revision before relying on any other part of the
`core/` layout, including `Jambase`, `lib/`, `src/`, and `bin/`. A `core/` with
an unsupported format revision is rejected before it is used, not silently
substituted or reinterpreted. Entry scripts do not compare Jam versions.

`contents` is a line-oriented size inventory of the immutable `core/`
contents outside `meta/` and `bin/`. Each line contains a nonnegative decimal
byte count, two spaces, and a slash-separated relative path without
whitespace:

```text
12345  Jambase
```

It includes `Jambase`, libraries, runtime assets such as `lib/vswhere.exe`,
and sources. It excludes every binary below `bin/`, whether that binary was
packaged or built locally by `--init`. `--init` validates `version` and the
recorded file sizes before accepting a snapshot. A normal build invocation
reads only `version` and the selected binary path; it does not rescan the
complete snapshot. The inventory detects missing and truncated immutable
files, but does not authenticate their contents. Distribution integrity
validation verifies the complete package before installation.

A snapshot may omit a binary for a host. It still contains the sources
necessary to build that binary. A packaged binary and a locally built binary
use the same `bin/<host-id>/jam[.exe]` path. This makes the same snapshot both
a consumer installation and the input to a release worker that creates a new
host binary for later distribution.

Host IDs use the `OS-ARCH` form. The initial IDs are `windows-x64`,
`linux-x64`, `macos-x64`, `macos-arm64`, and `macos-universal`. An entry
script first selects the native host ID. On macOS, it then falls back to
`macos-universal` when no native binary exists. The selected path is
`bin/<host-id>/jam[.exe]`.

## Phase 1: vendored snapshot

The first implementation restructures the existing vendored Jam files into
`jam/core/`. `Jambase` resides at `core/Jambase`, Jam libraries reside below
`core/lib/`, including `vswhere.exe`, C sources reside below `core/src/`, and
host binaries reside below `core/bin/<host-id>/`. The existing Windows binary
is a packaged `windows-x64` binary. `jam/local/` is created only when users
add local extensions.

The Pt checkout is the source of the snapshot. No download source, archive
format, or distribution environment variable exists in this phase.

## Phase 1: local initialization

The entry scripts own `--init` and remove it before invoking the Jam
executable. `--init` validates the vendored `core/` snapshot and builds a
binary for the native host when no selected host binary exists. It exits
successfully after initialization and does not run a project build.

A normal invocation requires a complete installed `core/`. If `core/` is
present but no native or macOS-universal host binary is available, the entry
script may build the native binary from `core/src/`. If no compiler is
available, it fails with an error that identifies the missing host binary and
compiler requirement. If `core/` is absent, it fails with an instruction to
run `--init` or provide a prepared `JAM_ROOT`.

The bootstrap scripts must report validation, compiler, and filesystem
failures explicitly. They do not silently substitute a different installation
root or host binary.

`--update` is not a public command in this phase. Without a distribution
source, an update operation would either do nothing or misleadingly suggest
that it could obtain a newer snapshot.

The entry scripts own generated binaries below `core/bin/`; they do not
otherwise replace the vendored `core/` snapshot. They never create, remove,
replace, or otherwise modify `local/`.

## Phase 2: distribution artifacts

A release worker initializes the vendored source snapshot on each supported
host and publishes its generated binary together with the corresponding
runtime, sources, and metadata. This phase defines archive names and
integrity validation for each complete distribution package. It does not
change the local runtime model.

## Phase 3: distribution initialization

After published snapshots exist, `JAM_DIST_ROOT` identifies the HTTP(S) base
URL for pinned Jam distribution snapshots. The entry scripts use their
embedded default when it is unset; setting it selects a release mirror.

`--init` downloads and verifies the complete pinned package when `core/` is
absent, stages it in a temporary directory, and installs it atomically. If
the snapshot lacks a binary for the current host, it builds one from
`core/src/`.

`--update` downloads and verifies the complete pinned package, stages it, and
atomically replaces `core/`. It does not select a different Jam version. If
the replacement lacks the current host binary, it builds that binary from the
replacement sources. It never changes `local/`.

An interrupted or failed initialization or update must not leave a partial
`core/` that appears valid. Download, validation, extraction, compiler, and
filesystem failures are reported explicitly. The scripts do not silently
substitute a different Jam version, remote, or installation root.

## Baseline

The current entry scripts use a vendored `jam/` directory in the project.
`jam.bat` starts `jam/jam.exe` and builds it with MSVC when it is absent.
`jam.sh` starts `jam/jam` and builds it with `cc` when it is absent. The
current layout has no versioned snapshot, no explicit initialization command,
and no separate ownership boundary for user extensions.
