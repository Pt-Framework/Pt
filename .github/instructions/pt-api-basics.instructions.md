---
description: "Basic Types"
---

- Command-line argument parsing:
  `include/Pt/Arg.h`
- Key/value application settings persistence:
  `include/Pt/Settings.h`
- Type-safe event object base class:
  `include/Pt/Event.h`
- Type-erased value container for any copyable type:
  `include/Pt/Any.h`
- Placeholder/void marker type for template specializations:
  `include/Pt/Void.h`
- Non-copyable mixin base class:
  `include/Pt/NonCopyable.h`
- Singleton pattern helper:
  `include/Pt/Singleton.h`
- Pt-IntTypes group overview (fixed-size integer typedefs):
  `include/Pt/Api-IntTypes.h`
- Fixed-size integer typedefs (int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t):
  `include/Pt/Types.h`
  `include/Pt/Api-Types.h`
- Pt-Byteorder group overview (host and endian conversion):
  `include/Pt/Api-Byteorder.h`
- Byte-order conversion (swab, hostToLe, leToHost, hostToBe, beToHost):
  `include/Pt/Byteorder.h`
- Pt-DateTime subgroup overview (durations and points in calendar time):
  `include/Pt/Api-DateTime.h`
- Calendar date representation, comparison and ISO string conversion:
  `include/Pt/Date.h`
- Time-of-day representation, comparison and ISO string conversion:
  `include/Pt/Time.h`
- Combined date and time, arithmetic and ISO string conversion:
  `include/Pt/DateTime.h`
- Duration between dates and times, arithmetic:
  `include/Pt/Timespan.h`
- Pt-Atomics group overview (lock-free atomic operations):
  `include/Pt/Api-Atomics.h`
- Low-level atomic integer operations and memory fences:
  `include/Pt/Atomicity.h`
