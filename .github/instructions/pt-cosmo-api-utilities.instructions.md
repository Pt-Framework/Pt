---
description: "Cosmo Utilities"
---

- Adapt standard containers for type-safe Cosmo enumeration:
  `include/Pt/Cosmo/Enumerable.h`
- Implement enumerable collections and enumerators for range-based iteration:
  `include/Pt/Cosmo/IEnumerable.h`
- Manage exclusive object ownership with a configurable deleter and move semantics:
  `include/Pt/Cosmo/AutoPtr.h`
- Create and reset an auto-release pool for attached resources:
  `include/Pt/Cosmo/AutoReleasePool.h`
- Implement auto-release pools and resources released when their pool resets:
  `include/Pt/Cosmo/IAutoReleasePool.h`
- Store, append, assign, and clear managed byte buffers:
  `include/Pt/Cosmo/Buffer.h`