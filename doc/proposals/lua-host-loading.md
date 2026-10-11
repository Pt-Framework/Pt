---
proposal: lua-host.md
---

# Lua Host Loading

## Baseline

A script loads a source string into one context. Only one script may
use a context at a time. Nested script loading is not resolved. Every
context receives the full standard library set.

## Modules and source loading {#nested-load}

The host can run source from a string and from a file path it allows. A
script can load another script that the host has provided, by string,
by allowed path, or by a name the host registered. Loading a name or
path the host did not provide fails. A cyclic load fails.

Nested loading has to be reconciled with the rule that one context runs
one top-level script. The architecture chooses whether a nested load
shares that context or is rejected. The requirement is only that the
choice is defined, and that a script cannot escape the host's loader to
read arbitrary files.

## Standard library selection

The host can choose which standard libraries a context exposes before
scripts run. A context for untrusted source can exclude the libraries
that perform file, process, module, and debug access. The default for
trusted source may remain the full set. A script cannot reintroduce an
excluded library unless the host has allowed that library.

Library selection is the sandbox boundary required here.
Operating-system isolation is not required.

## Errors

Compile failure, runtime failure, native failure, conversion failure,
and cancellation are distinguishable by the host. Each carries a
message. A runtime failure includes a Lua traceback when the
interpreter can produce one. Success is not reported for a missing
result, a mistyped result, or a cancelled script. Native exceptions and
Lua errors stay inside the embedding boundary.
