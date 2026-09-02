---
description: "XML Parsing, Writing, and Serialization"
---

- Represent an XML byte-order mark and document encoding properties:
  `include/Pt/Xml/ByteorderMark.h`
- Inspect XML text, CDATA sections, and character-node chunks:
  `include/Pt/Xml/Characters.h`
- Inspect XML comment nodes:
  `include/Pt/Xml/Comment.h`
- Process XML document type declarations and DTD boundaries:
  `include/Pt/Xml/DocType.h`
- Access XML DTD definitions, declared entities, and notations:
  `include/Pt/Xml/DocTypeDefinition.h`
- Validate XML nodes against a document type definition:
  `include/Pt/Xml/DocTypeValidator.h`
- Detect the end of an XML document stream:
  `include/Pt/Xml/EndDocument.h`
- Inspect XML closing elements and namespace mappings:
  `include/Pt/Xml/EndElement.h`
- Access DTD entities and XML entity-reference nodes:
  `include/Pt/Xml/Entity.h`
- Provide text, string, or binary input to the XML reader:
  `include/Pt/Xml/InputSource.h`
- Work with XML namespaces and namespace mappings:
  `include/Pt/Xml/Namespace.h`
- Handle generic XML node types and node casting:
  `include/Pt/Xml/Node.h`
- Access DTD notation declarations:
  `include/Pt/Xml/Notation.h`
- Inspect XML processing-instruction nodes:
  `include/Pt/Xml/ProcessingInstruction.h`
- Parse, compare, and construct qualified XML names:
  `include/Pt/Xml/QName.h`
- Detect the start of an XML document stream:
  `include/Pt/Xml/StartDocument.h`
- Inspect XML opening elements and their attributes:
  `include/Pt/Xml/StartElement.h`
- Inspect XML declaration version, encoding, and standalone state:
  `include/Pt/Xml/XmlDeclaration.h`
- Deserialize objects and data from XML:
  `include/Pt/Xml/XmlDeserializer.h`
- Handle XML processing and syntax errors:
  `include/Pt/Xml/XmlError.h`
- Format XML data through the Pt serialization interfaces:
  `include/Pt/Xml/XmlFormatter.h`
- Stream XML nodes from an input source:
  `include/Pt/Xml/XmlReader.h`
- Resolve external XML entities, DTDs, and input encodings:
  `include/Pt/Xml/XmlResolver.h`
- Manage XML object-serialization identity and pointer fixups:
  `include/Pt/Xml/XmlSerializationContext.h`
- Serialize objects and data to XML:
  `include/Pt/Xml/XmlSerializer.h`
- Write XML documents, elements, attributes, and text streams:
  `include/Pt/Xml/XmlWriter.h`