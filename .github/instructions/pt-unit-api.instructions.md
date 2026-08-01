---
description: "Unit Testing"
---

# API Reference

- Module overview, namespace docs:
  `include/Pt/Unit/Api.h`
- Assert condition, assert equal, assert near, assert throw, fail:
  `include/Pt/Unit/Assertion.h`
- Derive test suite, register methods, setUp, tearDown, run:
  `include/Pt/Unit/TestSuite.h`
- Auto-register test at program start:
  `include/Pt/Unit/RegisterTest.h`
- Report test results, observe success/failure/error:
  `include/Pt/Unit/Reporter.h`
- Test runner, run single test, run all, attach reporter, TestMain.h usage, CLI arguments, custom main:
  `include/Pt/Unit/Application.h`
- Provides `main()` for the test executable (include in exactly one `.cpp` per test binary):
  `include/Pt/Unit/TestMain.h`
