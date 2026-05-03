---
applyTo: "**/tests/**/*.{h,cpp}"
description: "Unit testing"
---

# Unit Tests with Pt::Unit

- Use the `Pt::Unit` framework for all unit tests.
- `Pt::Unit` is a module of the Pt Framework itself.
  - The header files are in `include/Pt/Unit` and contain documentation.
  - The source files are in `src/Pt-Unit`

- NEVER use VS Code tasks for unit tests.
- ALWAYS check the exit code of the actual pure unit test program to determine overall test success.
- ALWAYS check the exit code after every unit test execution — no exceptions. 
- NEVER rely on test output alone, but look at test output for details.
- ALWAYS run the unit test program without any further processing.

## Writing a Unit Test

- Class inherits from `Pt::Unit::TestSuite`
- Lives in the same namespace as the class/unit under test (e.g. `Pt::Gfx`)
- Class name = class/unit under test + `Test` (e.g. `PathTest`)
- Suite name = fully qualified class name (e.g. `"Pt::Gfx::PathTest"`)
- The suite name is passed to the `Pt::Unit::TestSuite` constructor.
- Each test method registered via `registerMethod()` in constructor.
- One test method per public method or behaviour of the unit under test.
- Test method name matches the method under test
- Prefer names that describe the tested public behaviour or API entry point.
- Keep names short and specific, e.g. `ConstructFromValue`, `CopyAssign`, `SetChannels`.
- Only use implementation terms such as `Constructor` when they are the clearest distinction.
- Avoid shared state between methods — each creates its own objects
- File-scope `Pt::Unit::RegisterTest<>` for self-registration

- Place test files in `src/<Module>/tests/`, e.g. `src/Pt-Gfx/tests/PathTest.cpp`
- Register in `src/<Module>/tests/Jamfile` (source list)
- Register in `src/Pt/Pt.vcxproj` and `src/Pt/Pt.vcxproj.filters`

### Required Headers

```cpp
#include <Pt/Unit/Assertion.h>    // PT_UNIT_ASSERT_* macros
#include <Pt/Unit/TestSuite.h>    // base class
#include <Pt/Unit/RegisterTest.h> // self-registration
```

### Assertion Macros

| Macro | Usage |
|---|---|
| `PT_UNIT_ASSERT(cond)` | Fails if `cond` is false |
| `PT_UNIT_ASSERT_EQUAL(a, b)` | Integers or exact types only |
| `PT_UNIT_ASSERT_NEAR(a, b)` | Floating-point comparisons |
| `PT_UNIT_ASSERT_MSG(cond, msg)` | Custom failure message |
| `PT_UNIT_ASSERT_THROW(expr, ExType)` | Must throw `ExType` |
| `PT_UNIT_ASSERT_NOTHROW(expr)` | Must not throw |
| `PT_UNIT_FAIL(msg)` | Unconditional failure |

### Unit Test Example

```cpp
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Module {

class FooTest : public Pt::Unit::TestSuite
{
    public:
        FooTest()
        : Pt::Unit::TestSuite("Pt::Module::FooTest")
        {
            registerMethod("Bar", *this, &FooTest::Bar);
        }

        void Bar()
        {
            Foo foo;
            PT_UNIT_ASSERT( foo.isEmpty() );

            Bar bar(foo);
            PT_UNIT_ASSERT( bar.foo() == foo );

            // use other assertion macros accordingly
        }
};

} // namespace Module

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Module::FooTest> register_FooTest;
```

## Running Tests

- Run unit tests with no arguments to execute all suites.
- Pass `-t "<SuiteName>"` to run a single suite (`SuiteName` as passed to
  Pt::Unit::TestSuite constructor).
- Capture stdout from the test executable to see detailed results.
- Pt::Unit executes test methods in alphabetical order.
