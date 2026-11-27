---
proposal: jam-init.md
---

# Jam Initialization Distribution

## Purpose

Jam is maintained in the Pt repository. Pt versions `jam/` as its
vendored snapshot and keeps using that tree. The GitHub Release exists
so other projects can take only the entry scripts and install the
snapshot by download.

The entry scripts are small POSIX shell and Windows batch programs.
Download uses `curl` or `wget`. Extraction uses `tar`. There is no
separate configuration file and no extra bootstrap language.

## Installation root

`JAM_ROOT` identifies the root of one Jam installation. It is the
snapshot itself:

```text
<JAM_ROOT>/
  Jambase
  meta/
  lib/
  src/
  bin/
```

When `JAM_ROOT` is unset, it defaults to `jam` below the directory
containing the entry script. An external `JAM_ROOT` is any directory the
user sets. A valid snapshot there is a prepared installation. An absent
snapshot is installed from the distribution by `init`.

Pt versions `jam/` in this repository. A consuming project that
installs the snapshot from the distribution may version that tree, leave
it untracked, or keep it outside the project. Project extensions live
in the project, not under `JAM_ROOT`.

## Snapshot layout

`JAM_ROOT` is a complete, versioned Jam snapshot:

```text
<JAM_ROOT>/
  meta/
    format
    version
    contents
  Jambase
  lib/
    *.jam
  src/
    *.c
    *.h
    modules/
  bin/
    <host-id>/
      jam[.exe]
```

`meta/` identifies the snapshot layout, the snapshot release, and the
release-owned contents. Runtime scripts and sources in a snapshot
represent the same snapshot release. A host binary may be packaged with
the snapshot or built locally from its sources.

The engine version compiled into the host binary (`patchlevel.h`,
`jam -v`, `JAMVERSION`) is independent of the snapshot release. The
bootstrap never reads it.

## Snapshot metadata

`meta/` is a directory rather than a compound manifest file. This keeps
the scalar values directly readable by POSIX shell and Windows batch
without a JSON, YAML, or INI parser. Snapshot layout and snapshot
release are different identities, so they are different files.

`format` contains exactly one nonempty ASCII line terminated by LF with
one positive decimal integer and no other fields:

```text
1
```

That grammar is frozen. An entry script reads `format` before any other
part of the snapshot, including `version`, `Jambase`, `lib/`, `src/`, and
`bin/`. A snapshot whose format revision the script does not support is
rejected before it is used, not silently substituted or reinterpreted.

`version` contains exactly one nonempty ASCII line terminated by LF with
the dotted snapshot triple `major.minor.patch`. The major version is
positive; the minor and patch versions are nonnegative:

```text
4.0.0
```

That line is the identity of this snapshot. It matches the
entry-script pin, the release tag suffix, and the archive name.

`contents` is a line-oriented size inventory of the immutable snapshot
contents outside `meta/` and `bin/`. Each line contains a nonnegative
decimal byte count, two spaces, and a slash-separated relative path
without whitespace:

```text
12345  Jambase
```

The sizes are the byte lengths of those files as stored in the
snapshot. Text files in the snapshot use LF. It includes `Jambase`,
libraries, and sources. It excludes every binary below `bin/`,
whether that binary was packaged or built locally by `init`. An
initialization that downloads a snapshot checks the recorded file
sizes after extracting it. An initialization of an already present
snapshot whose version matches the pin checks `format` and `version`; a
normal build invocation likewise reads `format`, `version`, and the
selected binary path. After a download, a missing host binary is built
from `src/` under the same compiler rules as a local `init`.

A snapshot may omit a binary for a host. It still contains the sources
necessary to build that binary. A packaged binary and a locally built
binary use the same `bin/<host-id>/jam[.exe]` path. This makes the same
snapshot both a consumer installation and the input to a release worker
that creates a new host binary for later distribution.

Host IDs use the `OS-ARCH` form. The initial IDs are `windows-x64`,
`linux-x64`, and `macos-universal`. An entry script selects the host ID
for its platform; macOS always selects `macos-universal`. The selected
path is `bin/<host-id>/jam[.exe]`.

## Installation sources

Pt keeps a versioned snapshot in this repository. Other projects use the
distribution: they copy `jam.sh` and `jam.bat` and run `init`, which
downloads the snapshot when it is absent. They may then leave that tree
local, share it through `JAM_ROOT`, or check it in.

When the entry-script pin and `meta/version` name the same snapshot,
`init` and a normal invocation only build a missing host binary. They
do not download, replace, or repair the snapshot. That is the path Pt
uses, and the path a consumer uses after the snapshot is already present.

A prepared `JAM_ROOT` outside the project follows the same rule when the
pin and `meta/version` match. `init` downloads only when the snapshot is
absent or the pin does not match.

## Snapshot pin

The entry scripts embed the snapshot pin as the dotted triple
`major.minor.patch`. That pin is the snapshot version they require. It
matches `meta/version` and the names used in the distribution
asset URLs.

The release worker copies `meta/version` into both `jam.sh` and
`jam.bat` in one step. Users do not edit the pin. A newer snapshot is
taken by replacing both scripts with the pair from that release, then
running `init`.

`meta/version` is the only snapshot-version file in the tree. The
engine version in `patchlevel.h` is not derived from it.

## Initialization

The entry scripts own the `init` command and consume it before invoking
the Jam executable. `init` exits successfully after initialization and
does not run a project build. It ignores arguments that follow the
command. The scripts do not create or modify project files.

`init` reads `format` first, then compares the pin to `meta/version`.

When the snapshot is present, the version matches the pin, and the format
revision is supported, `init` builds the selected host binary from
`src/` if that binary is missing. It does not contact the network.
It does not replace any other file in the snapshot.

When the snapshot is absent, `init` installs the snapshot named by the pin as
specified in Distribution initialization.

When the snapshot is present and valid but the version does not match the
pin, `init` downloads that snapshot and atomically replaces `JAM_ROOT`.
The previous tree, including its host binaries, is gone. If the new
snapshot has no binary for the current host, `init` builds one from the
new `src/`.

When the snapshot is present but invalid, `init` fails. It does not download
or overwrite the invalid tree. Removing `JAM_ROOT` and running `init`
installs a new snapshot.

A normal invocation never downloads. It requires a snapshot whose
version matches the pin and whose format revision is supported. If the
selected host binary is missing, it may build it from `src/` under the
same compiler rules as `init`. If no compiler is available, it fails with an
error that identifies the missing host binary and compiler requirement. If
the snapshot is absent, invalid, or names a different snapshot version than the
pin, it fails with an instruction to run `init`.

The bootstrap scripts report validation, compiler, download, extraction,
and filesystem failures explicitly. They do not silently substitute a
different installation root, host binary, snapshot version, or remote.

On a shared `JAM_ROOT`, the pin of the scripts that invoke `init`
selects the snapshot installed there.

## Host binary publication {#host-binaries}

A published snapshot contains a precompiled host binary for each
initial host ID: `windows-x64`, `linux-x64`, and `macos-universal`. They
occupy the same `bin/<host-id>/` paths as a locally built binary.

`init` downloads that one snapshot when `JAM_ROOT` is absent or the pin
does not match. After extraction the current host binary is already
present, so a compiler is not required for the initial hosts.

`init` does not download a binary into an existing snapshot whose
version already matches the pin. A missing binary there is built from
`src/`. A project that versions the snapshot may omit `bin/` from
version control; `init` then builds the missing binary from sources
without a network.

Sibling per-host binary archives are not part of the bootstrap. The
snapshot layout already allows a snapshot to omit a host binary; the
published distribution does not omit the three initial hosts.

## Distribution artifacts {#distribution-artifacts}

`.github/workflows/jam-build.yml` builds the host binaries and
publishes a GitHub Release of the Jam snapshot. It is separate from the
Pt CI workflow. Jam lives in this repository; there is no separate Jam
project remote.

Three jobs run in parallel, one per initial host. Each job checks out
the tree, runs `init` on that host, and uploads `bin/<host-id>/`
as an artifact. A fourth job depends on all three, checks out the tree,
downloads the artifacts, places the three host binaries into
`bin/`, and publishes the release. That job is the only one that
writes the GitHub Release. The workflow does not run on every push to
`main`.
The release tag is `jam-<major>.<minor>.<patch>`, next to framework
tags (`v*`) and prebuilt-dependency tags (`prebuilt-*`). The default
`JAM_DIST_ROOT` is the HTTPS download base of this repository:

```text
https://github.com/Pt-Framework/Pt/releases/download
```

The release publishes these assets. The archive and checksum names do
not contain the version, so the same file name exists on every Jam
release:

```text
jam.sh
jam.bat
jam.tar.gz
jam.tar.gz.sha256
```

The archive has a single top-level `jam/` directory whose layout
matches Snapshot layout, including the three initial host binaries.
After extraction, `init` checks the `contents` size inventory and that
`meta/version` matches the pin.

The entry scripts in the release already contain that pin. The bootstrap
fetches only the archive:

```text
<JAM_DIST_ROOT>/jam-<major>.<minor>.<patch>/jam.tar.gz
```

The stable asset name is what makes a latest download URL possible:

```text
https://github.com/Pt-Framework/Pt/releases/latest/download/jam.tar.gz
```

That URL follows whichever release GitHub marks latest. `init` does
not use it. A pinned install always requests the tag named by the pin.
Framework tags and prebuilt tags share the repository latest pointer
and do not publish `jam.tar.gz`.

The SHA-256 sidecar is a release asset for manual verification. It is
one line of hexadecimal digest, two spaces, and `jam.tar.gz`,
terminated by LF.

## Distribution initialization {#distribution-initialization}

`JAM_DIST_ROOT` identifies the HTTP(S) base URL for snapshot assets.
When it is unset, the entry scripts use their embedded default. Setting
it selects a mirror that serves the same
`jam-<version>/jam.tar.gz` layout.

`init` downloads the complete pinned package when the snapshot is absent
or the pin does not match `meta/version`, stages it in a temporary
directory next to `JAM_ROOT`, and installs it atomically as `JAM_ROOT`.

POSIX `jam.sh` uses `curl` when it is present, otherwise `wget`.
Windows `jam.bat` uses `curl.exe`. Extraction uses `tar` on every host.
If a required tool is missing, the script fails with an error that names
the tool. The downloader's proxy environment applies. The default remote
is a public GitHub Release served over HTTPS.

Download writes into a temporary directory. The archive is extracted,
then checked for a supported format revision, a `meta/version` that
matches the pin, and the `contents` inventory. Only a fully checked tree
is installed as `JAM_ROOT`. An interrupted or failed operation leaves the
previous snapshot in place when one existed, and does not leave a new
snapshot that appears valid.
