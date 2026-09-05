# Contributing to Platinum

Thank you for contributing to the Platinum C++ Framework.

## Before You Start

- Discuss substantial API, architecture, or cross-module changes in an issue
  before implementing them.
- Keep each pull request focused on one problem or feature.
- Do not include unrelated formatting changes, generated files, or build
  artifacts.
- Read the [requirements](https://pt-framework.net/htdocs/requirements.html)
  and [installation guide](https://pt-framework.net/htdocs/installing.html).

## Git Setup

Set your author identity before creating commits:

```sh
git config user.name "Your Name"
git config user.email "you@example.com"
```

To use separate GitHub credentials for repositories that share the same host,
enable path-aware credential lookup:

```sh
git config credential.github.com.useHttpPath true
```

Add `--local` to apply these settings to this repository, or `--global` to
apply them to all repositories for the current user. May require `ceredentials.helper` set to true.

## Repository Layout

- Public headers: `include/Pt/` and `include/Pt/<Module>/`
- Core sources: `src/Pt/`
- Module sources: `src/Pt-<Module>/`
- Tests: `src/Pt/tests/` or `src/Pt-<Module>/tests/`
- User documentation pages: `doc/pages/`
- Generated Doxygen HTML: `doc/website/htdocs/`

## Building

Pt uses its [Jam-based build system](https://pt-framework.net/htdocs/jam-intro.html).
For further commands and options, see the [Jam usage guide](https://pt-framework.net/htdocs/jam-usage.html).

On Windows, configure and build a debug configuration from the repository
root:

```bat
jam.bat configure -sCONFIG=debug --debug
jam.bat -q -j4
```

On Linux and macOS, use `jam.sh` with the same arguments:

```sh
./jam.sh configure -sCONFIG=debug --debug
./jam.sh -q -j4
```

Switch between existing configuration:

```sh
jam.bat switch debug
```

Run a full project build before opening a pull request.

## Tests

Add or update tests for every behavior change and bug fix. Pt uses the
[Pt::Unit testing framework](https://pt-framework.net/htdocs/Pt-Unit-Page.html).

- Keep tests in the namespace of the unit under test.
- Name test classes with a `Test` suffix.
- Avoid state shared between test methods.
- Register new test source files in the module test `Jamfile`.
- Run the generated `<Module>-test` executable directly and verify its exit
  code.

## Code Style

- Match the style of the surrounding code.
- Use C++14 by default; use C++20 only where necessary.
- Use 4 spaces for indentation and no tabs.
- Use Allman braces.
- Use `PascalCase` for classes and `camelCase` for functions.
- Prefix member variables with `_`.
- Keep lines at or below 100 columns.
- Keep comments, identifiers, log messages, and exception messages in English.
- Add the repository copyright header to new source and header files.
- Use the module API export macro for public, non-inline symbols.

## Public APIs and Documentation

Document public APIs in their public headers with Doxygen. The
[online reference](https://pt-framework.net/docs.html) provides the API and
concept documentation.

- Document public namespaces, classes, functions, and enums.
- Assign documented APIs to an appropriate Doxygen group.
- Add or update an API group header when introducing a new feature group.
- Add a page in `doc/pages/` when a feature needs user-facing guidance.

## Pull Requests

Include the following in every pull request:

- A concise description of the problem and solution.
- Tests covering changed behavior, or an explanation why tests are not needed.
- Documentation updates when public behavior or APIs change.
- The platforms and configurations used for verification.
- Any compatibility, performance, or migration implications.

By contributing, you agree that your contribution may be distributed under the
[repository license](LICENSE): LGPL-2.1-or-later with the Platinum linking
exception.
