/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_XML_API_H
#define PT_XML_API_H

#include <Pt/Api.h>

#define PT_XML_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_XML_VERSION_MINOR PT_VERSION_MINOR
#define PT_XML_VERSION_REVISION PT_VERSION_REVISION
#define PT_XML_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_XML_API_EXPORT)
#    define PT_XML_API PT_EXPORT
#  else
#    define PT_XML_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Xml
    @brief Read and write XML ducuments.
*/
namespace Xml {

    class ByteorderMark;
    class Characters;
    class Comment;
    class DocType;
    class EndDocType;
    class DocTypeDefinition;
    class DocTypeValidator;
    class EndDocument;
    class EndElement;
    class Entity;
    class EntityReference;
    class InputSource;
    class Namespace;
    class Node;
    class Notation;
    class ProcessingInstruction;
    class QName;
    class StartDocument;
    class StartElement;
    class XmlDeclaration;
    class XmlDeserializer;
    class XmlError;
    class XmlFormatter;
    class XmlReader;
    class XmlResolver;
    class XmlSerializationContext;
    class XmlSerializer;
    class XmlWriter;

} // namespace Xml

} // namespace Pt

#endif
