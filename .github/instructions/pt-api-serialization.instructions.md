---
description: "Serialization"
---

- Serialization module overview and two-phase compose/decompose design:
  `include/Pt/Api-Serialization.h`
- High-level serialization of types to a formatter:
  `include/Pt/Serializer.h`
- High-level deserialization of types from a formatter:
  `include/Pt/Deserializer.h`
- Object-graph node tree for intermediate representation:
  `include/Pt/SerializationInfo.h`
- Context with reference IDs, surrogates and memory management:
  `include/Pt/SerializationContext.h`
- Errors thrown during serialization or deserialization:
  `include/Pt/SerializationError.h`
- Type-specific serialization override (surrogate):
  `include/Pt/SerializationSurrogate.h`
- Pointer fixup information for shared references:
  `include/Pt/FixupInfo.h`
- High-performance custom compose interface:
  `include/Pt/Composer.h`
- High-performance custom decompose interface:
  `include/Pt/Decomposer.h`
- Pluggable output format front-end (text/binary):
  `include/Pt/Formatter.h`
