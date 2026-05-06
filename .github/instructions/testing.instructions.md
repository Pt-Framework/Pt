---
applyTo: "**/tests/**/*.{h,cpp}"
description: "Repository-specific unit test conventions."
---

# Unit Testing

- Use the `Pt::Unit` framework for all unit tests.
- For Pt::Unit API details, read `.github/instructions/pt-unit-api.instructions.md`.

## Repository Conventions

- Place test files in `src/<Module>/tests/`, e.g. `src/Pt-Gfx/tests/PathTest.cpp`.
- Tests live in the same namespace as the class or unit under test (e.g. `Pt::Gfx`).
- Class name = class or unit under test + `Test` (e.g. `PathTest`).
- Test method name matches the method under test when that is the clearest naming.
- Prefer test method names that describe the public behaviour or API entry point.
- Keep method names short and specific, e.g. `ConstructFromValue`, `CopyAssign`, `SetChannels`.
- Only use implementation terms such as `Constructor` when they are the clearest distinction.
- Avoid shared state between test methods. Each method should create its own objects.

## Build Integration

- Register test source files in `src/<Module>/tests/Jamfile`.
- Register test source files in `src/Pt/Pt.vcxproj` and `src/Pt/Pt.vcxproj.filters`.

## Running Tests

- NEVER use VS Code tasks for unit tests.
- ALWAYS check the exit code of the actual standalone test program to determine overall success.
- ALWAYS check the exit code after every unit test execution.
- NEVER rely on test output alone, but inspect it for details.
- ALWAYS run the test executable without any further processing.
- Run unit tests with no arguments to execute all suites.
- Pass `-t "<SuiteName>"` to run a single suite.
- Test methods execute in alphabetical order.
- Capture stdout from the test executable to see detailed results.
