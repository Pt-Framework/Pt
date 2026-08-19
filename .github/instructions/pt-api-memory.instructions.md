---
description: "Memory Management"
---

- Allocator module overview and API boundaries:
  `include/Pt/Api-Allocator.h`
- Abstract allocator interface:
  `include/Pt/Allocator.h`
- Page-based allocator for consecutive allocations freed as a block:
  `include/Pt/PageAllocator.h`
- Pool-based allocator for many small objects of similar size:
  `include/Pt/PoolAllocator.h`
- Smart pointer wrapper for reference-counted objects:
  `include/Pt/SmartPtr.h`
- Reference-counted mixin base class:
  `include/Pt/RefCounted.h`
