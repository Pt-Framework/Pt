---
description: "JSON Reading, Writing, and Documents"
---

- Load, modify, navigate, and save an in-memory JSON document:
  `include/Pt/Json/Document.h`
- Read a JSON stream into a Document:
  `include/Pt/Json/DocumentReader.h`
- Stream JSON tokens from a character input, configure parsing limits, iterate nodes:
  `include/Pt/Json/JsonReader.h`
- Write JSON objects, arrays, members, and scalar values to a character output:
  `include/Pt/Json/JsonWriter.h`
- Serialize and deserialize Pt values through the JSON formatter:
  `include/Pt/Json/JsonFormatter.h`
- Handle JSON processing and syntax errors with source line information:
  `include/Pt/Json/JsonError.h`
- Inspect generic JSON token types and safely cast specialized nodes:
  `include/Pt/Json/Node.h`
- Process boolean, integer, floating-point, string, and null JSON value tokens:
  `include/Pt/Json/Boolean.h`
- Process integer JSON value tokens:
  `include/Pt/Json/Integer.h`
- Process floating-point JSON value tokens:
  `include/Pt/Json/Float.h`
- Process string JSON value tokens:
  `include/Pt/Json/String.h`
- Process null JSON value tokens:
  `include/Pt/Json/Null.h`
- Process JSON object member and object-boundary tokens:
  `include/Pt/Json/Member.h`
- Process object start tokens:
  `include/Pt/Json/StartObject.h`
- Process object end tokens:
  `include/Pt/Json/EndObject.h`
- Process JSON array-boundary tokens:
  `include/Pt/Json/StartArray.h`
- Process array end tokens:
  `include/Pt/Json/EndArray.h`
- Detect the terminal token after reading a JSON document:
  `include/Pt/Json/EndDocument.h`