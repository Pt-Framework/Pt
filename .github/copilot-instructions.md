# Unit Test Conventions

## Framework

Unit tests use the `Pt::Unit` framework. The relevant headers are:

```cpp
#include <Pt/Unit/Assertion.h>    // PT_UNIT_ASSERT_* macros
#include <Pt/Unit/TestSuite.h>    // base class
#include <Pt/Unit/RegisterTest.h> // self-registration
```

## File Location

- Test files live in `src/<Module>/tests/`, e.g. `src/Pt-Gfx/tests/PathTest.cpp`.
- Each new test file must be added to two places:
  - `src/<Module>/tests/Jamfile` — in the `Main <target>` source list.
  - `src/Pt/Pt.vcxproj` and `src/Pt/Pt.vcxproj.filters` — as a `<ClCompile>` entry.

## Class Structure

- The test class inherits from `Pt::Unit::TestSuite`.
- It lives in the same namespace as the class under test (e.g. `Pt::Gfx`).
- The class name matches the class under test followed by `Test` (e.g. `PathTest`).
- Each test method is registered in the constructor via `registerMethod()`.
- The suite name passed to `Pt::Unit::TestSuite` is the fully qualified class name (e.g. `"Pt::Gfx::PathTest"`).
- At file scope, a `Pt::Unit::RegisterTest<>` instance self-registers the suite.

```cpp
namespace Pt {

namespace Gfx {

class PathTest : public Pt::Unit::TestSuite
{
    public:
        PathTest()
        : Pt::Unit::TestSuite("Pt::Gfx::PathTest")
        {
   registerMethod("MoveTo", *this, &PathTest::MoveTo);
   }

  void MoveTo()
        {
     // ...
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::PathTest> register_PathTest;
```

## Assertion Macros

| Macro | Usage |
|---|---|
| `PT_UNIT_ASSERT(cond)` | Fails if `cond` is false |
| `PT_UNIT_ASSERT_EQUAL(a, b)` | Fails if `a != b`, prints both values |
| `PT_UNIT_ASSERT_MSG(cond, msg)` | Fails with a custom message |
| `PT_UNIT_ASSERT_THROW(expr, ExType)` | Fails if `expr` does not throw `ExType` |
| `PT_UNIT_ASSERT_NOTHROW(expr)` | Fails if `expr` throws any exception |
| `PT_UNIT_FAIL(msg)` | Unconditionally fails with a message |

## Test Method Conventions

- One test method per public method or behaviour being verified.
- The method name matches the method under test (e.g. `MoveTo`, `LineTo`).
- Test the initial/empty state first, then the expected behaviour, then edge cases.

```cpp
void MoveTo()
{
    Path path;

    // initial state
    PT_UNIT_ASSERT(path.isEmpty());

    // expected behaviour
    path.moveTo(PointF(3.0, 7.0));
    PT_UNIT_ASSERT(!path.isEmpty());
    PT_UNIT_ASSERT_EQUAL(path.currentPosition().x(), 3.0);
    PT_UNIT_ASSERT_EQUAL(path.currentPosition().y(), 7.0);

    // calling again must update the state
    path.moveTo(PointF(5.0, 9.0));
    PT_UNIT_ASSERT_EQUAL(path.currentPosition().x(), 5.0);
    PT_UNIT_ASSERT_EQUAL(path.currentPosition().y(), 9.0);
}
```

## Jamfile Entry

Add the file to the `Main` source list in `src/<Module>/tests/Jamfile`:

```
Main Pt-Gfx-test : Pt-Gfx-test.cpp
 Argb32Test.cpp
       PathTest.cpp
            Yuv12Test.cpp
     ;
```
