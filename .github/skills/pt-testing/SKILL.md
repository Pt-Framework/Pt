---
name: pt-testing
description: Writing and structuring unit tests with the Pt::Unit framework. Use this when asked to create tests, add test methods, or debug test failures.
---

# Unit Testing with Pt::Unit

## Framework

Use the `Pt::Unit` framework for all unit tests.

### Required Headers

```cpp
#include <Pt/Unit/Assertion.h>    // PT_UNIT_ASSERT_* macros
#include <Pt/Unit/TestSuite.h>    // base class
#include <Pt/Unit/RegisterTest.h> // self-registration
```

## Test File Location

- Place test files in `src/<Module>/tests/`, e.g. `src/Pt-Gfx/tests/PathTest.cpp`
- Register in `src/<Module>/tests/Jamfile` (source list)
- Register in `src/Pt/Pt.vcxproj` and `src/Pt/Pt.vcxproj.filters`

## Test Structure

- Class inherits from `Pt::Unit::TestSuite`
- Lives in the same namespace as the class under test (e.g. `Pt::Gfx`)
- Class name = class under test + `Test` (e.g. `PathTest`)
- Suite name = fully qualified class name (e.g. `"Pt::Gfx::PathTest"`)
- Each test method registered via `registerMethod()` in constructor
- File-scope `Pt::Unit::RegisterTest<>` for self-registration

## Assertion Macros

| Macro | Usage |
|---|---|
| `PT_UNIT_ASSERT(cond)` | Fails if `cond` is false |
| `PT_UNIT_ASSERT_EQUAL(a, b)` | Integers or exact types only |
| `PT_UNIT_ASSERT_NEAR(a, b)` | Floating-point comparisons |
| `PT_UNIT_ASSERT_MSG(cond, msg)` | Custom failure message |
| `PT_UNIT_ASSERT_THROW(expr, ExType)` | Must throw `ExType` |
| `PT_UNIT_ASSERT_NOTHROW(expr)` | Must not throw |
| `PT_UNIT_FAIL(msg)` | Unconditional failure |

## Conventions

- One test method per public method or behaviour
- Method name matches the method under test
- No shared state between methods — each creates its own objects

## Template

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
            // test code here
        }
};

} // namespace Module

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Module::FooTest> register_FooTest;
```

## Running Tests

- Executables are in `build/debug/`
- Run with no arguments to execute all suites
- Pass `-t "<SuiteName>"` to run a single suite in the constructor.
- The suite name should be the fully qualified class name (e.g. `"Pt::Gfx::PathTest"`).
- Capture stdout from the test executable to see detailed results
