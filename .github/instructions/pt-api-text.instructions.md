---
description: "Text Processing"
---

- Unicode module overview and API boundaries:
  `include/Pt/Api-Unicode.h`
- Unicode string type based on std::basic_string<Pt::Char>:
  `include/Pt/String.h`
- UTF-8 byte sequence helpers:
  `include/Pt/Utf8.h`
- UTF-8 text codec:
  `include/Pt/Utf8Codec.h`
- UTF-16 text codec:
  `include/Pt/Utf16Codec.h`
- UTF-32 text codec:
  `include/Pt/Utf32Codec.h`
- Latin-1 text codec:
  `include/Pt/Latin1Codec.h`
- Abstract text codec used by text streams and locales:
  `include/Pt/TextCodec.h`
- Text input/output stream on top of stream buffers:
  `include/Pt/TextStream.h`
- Stream buffer backed by a Pt::String:
  `include/Pt/StringStream.h`
- Unicode regular expression matching and searching:
  `include/Pt/Regex.h`
- Base64 encoding and decoding:
  `include/Pt/Base64Codec.h`
- Localization support and facets:
  `include/Pt/Locale.h`
- Locale facet definitions for text processing:
  `include/Pt/Facets.h`
- String/number parsing and formatting (parseInt, formatInt, parseFloat, formatFloat, narrow):
  `include/Pt/Convert.h`
- Errors thrown by conversion utilities:
  `include/Pt/ConversionError.h`
