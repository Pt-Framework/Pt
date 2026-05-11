---
description: "Repository-specific unit test conventions."
---

# Writing Test

- Use the `Pt::Unit` framework for all unit tests.
- Tests live in the same namespace as the class or unit under test.
- Name the test class after the class or unit under test, with a `Test` suffix (e.g. `MyClassTest`).
- Test method name matches the method under test when that is the clearest naming.
- Prefer test method names that describe the public behaviour or API entry point.
- Keep method names short and specific, e.g. `ConstructFromValue`, `CopyAssign`, `SetChannels`.
- Only use implementation terms such as `Constructor` when they are the clearest distinction.
- Avoid shared state between test methods. Each method should create its own objects.
- Suite name is always fully qualified with namespace, e.g. `"<Project>::<ClassName>Test"` 
  or `"<Project>::<Module>::<ClassName>Test"`.

# Building Tests

- The test executable is a standard `Main` target named `<Module>-test`, linked
  against `Pt-Unit` plus all module dependencies.
- Register test source files in `src/<Module>/tests/Jamfile`.

# Running Tests

- NEVER use VS Code tasks for unit tests.
- ALWAYS check the exit code of the actual standalone test program to determine overall success.
- ALWAYS check the exit code after every unit test execution.
- NEVER rely on test output alone, but inspect it for details.
- ALWAYS run the test executable directly, without piping or further processing.
- Run unit tests with no arguments to execute all suites.
- Pass `-t "<SuiteName>"` to run a single suite, where `<SuiteName>` is the string
  passed to the `Pt::Unit::TestSuite` constructor (e.g., `"AverageTest"`).
- Test methods execute in the order their names are registered via `registerMethod`.
- Capture stdout from the test executable to see detailed results.
