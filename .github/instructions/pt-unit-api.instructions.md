---
description: "Unit Testing"
---

- Concepts, writing suites, fixtures, running tests, reporting:
  `include/Pt/Unit/Api.h`
- Assert condition, assert equal, assert near, assert throw, fail:
  `include/Pt/Unit/Assertion.h`
- Derive test suite, register methods, setUp, tearDown, run:
  `include/Pt/Unit/TestSuite.h`
- Auto-register test at program start:
  `include/Pt/Unit/RegisterTest.h`
- Report test results, observe success/failure/error:
  `include/Pt/Unit/Reporter.h`
- Test runner, find test, run all, attach reporter:
  `include/Pt/Unit/Application.h`