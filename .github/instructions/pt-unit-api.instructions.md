---
description: "Using the Pt::Unit API for test suites, assertions, and test registration."
---

# Pt::Unit API

`Pt::Unit` is the Pt Framework's unit testing module.

The public headers in `include/Pt/Unit` contain the API documentation.

For the implementation, see:
- `include/Pt/Unit/Assertion.h`
- `include/Pt/Unit/TestSuite.h`
- `include/Pt/Unit/RegisterTest.h`
- `src/Pt-Unit`

## Writing a Test Suite

- Derive the test class from `Pt::Unit::TestSuite`.
- Pass the fully qualified suite name to the `Pt::Unit::TestSuite` constructor, e.g. `"Pt::Gfx::PathTest"`.
- Register each test method in the constructor via `registerMethod()`.
- One test method should cover one public method or one observable behaviour.
- Use file-scope `Pt::Unit::RegisterTest<>` for self-registration.

### Required Headers

```cpp
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
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

### Example

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
        }
};

} // namespace Module

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Module::FooTest> register_FooTest;
```

## Running Pt::Unit Suites

- Run the test executable with no arguments to execute all suites.
- Pass `-t "<SuiteName>"` to run a single suite.
- Pt::Unit executes test methods in alphabetical order.