/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include <Pt/Json/Document.h>
#include <Pt/Json/JsonReader.h>
#include <Pt/Json/StartObject.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/EndObject.h>
#include <Pt/Json/StartArray.h>
#include <Pt/Json/EndArray.h>
#include <Pt/Json/String.h>
#include <Pt/Json/Float.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/Boolean.h>
#include <Pt/Json/Null.h>

#include <stack>

namespace Pt {

namespace Json {

Document::Document()
{
}


void Document::clear()
{
    _root.clear();
}


bool Document::isEmpty() const
{
    return _root.isVoid();
}


void Document::load(std::basic_istream<Pt::Char>& is)
{
    clear();
    
    DocumentReader reader(is, *this);
    reader.read();
}


void Document::save(std::basic_ostream<Pt::Char>& os) const
{
}

//
// DocumentReader
//

DocumentReader::DocumentReader()
: _parse(&DocumentReader::onRoot)
, _current(0)
, _doc(0)
{
    reset();
}


DocumentReader::DocumentReader(std::basic_istream<Pt::Char>& is, Document& doc)
: _parse(&DocumentReader::onRoot)
, _current( doc.root() )
, _doc(&doc)
{
    reset(is, doc);
}


void DocumentReader::reset()
{
    _parse = &DocumentReader::onRoot;
    _parseStack.push(_parse);
            
    _reader.reset();

    _doc = 0;
    _current = Document::Element();
}


void DocumentReader::reset(std::basic_istream<Pt::Char>& is, Document& doc)
{
    _parse = &DocumentReader::onRoot;
    _parseStack.push(_parse);
            
    _reader.reset(is);

    doc.clear();

    _doc = &doc;
    _current = doc.root();
}


Document& DocumentReader::read()
{
    if( ! _doc )
        throw JsonError("invalid document"); 

    InputIterator it = _reader.current();
    for( ; it != _reader.end(); ++it)
    {
        (this->*_parse)(*it);
    }

    return *_doc;
}


Document* DocumentReader::advance()
{
    if( ! _doc )
        throw JsonError("invalid document"); 

    Node* node = _reader.advance();
    while(node)
    {
      (this->*_parse)(*node);
      
      if(node->type() == Node::EndDocument)
          return _doc;

      node = _reader.advance();
    }

    return 0;
}


void DocumentReader::onRoot(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartArray:
        {
            //_current->setTypeName("array");

            _parse = &DocumentReader::onArray;
            _parseStack.push(_parse);
            break;
        }

        case Node::StartObject:
        {
            //_current->setTypeName("object");

            _parse = &DocumentReader::onObject;
            _parseStack.push(_parse);
            break;
        }
        
        case Node::String:
        {
            const String& s = toString(node);
            //_current->setString( s.value() );
            _current.setValue( s.value() );
            _current = _current->parent();
            break;
        }

        case Node::Float:
        {
            const Float& f = toFloat(node);
            //_current->setDouble( f.value() );
            _current.setValue( f.value() );
            _current = _current->parent();
            break;
        }

        case Node::Integer:
        { 
            const Integer& i = toInteger(node);
            //_current->setInt64( i.value() );
            _current.setValue( i.value() );
            _current = _current->parent();
            break;
        }

        case Node::Boolean:
        {
            const Boolean& b = toBoolean(node);
            //_current->setBool( b.value() );
            _current.setValue( b.value() );
            _current = _current->parent();
            break;
        }

        case Node::Null:
        {
            //const Null& n = toNull(node);
            //_current->setVoid();
            _current->setNull();
            _current = _current->parent();
            break;
        }

        default:
            break;
    }
}


void DocumentReader::onArray(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartArray:
        {
            //_current = &_current->addElement();
            //_current->setTypeName("array");

            _current = _current.addElement();

            _parse = &DocumentReader::onArray;
            _parseStack.push(_parse);
            break;
        }

        case Node::EndArray:
        {
            _current = _current->parent();

            _parseStack.pop();
            _parse = _parseStack.top();
            break;
        }

        case Node::StartObject:
        {
            //_current = &_current->addElement();
            //_current->setTypeName("object");

            _current = _current.addElement();

            _parse = &DocumentReader::onObject;
            _parseStack.push(_parse);
            break;
        }

        case Node::String:
        {
            //_current = &_current->addElement();
            _current = _current.addElement();

            const String& s = toString(node);
            //_current->setString( s.value() );
            _current->setValue( s.value() );

            _current = _current->parent();
            break;
        }

        case Node::Float:
        {
            //_current = &_current->addElement();
            _current = _current.addElement();

            const Float& f = toFloat(node);
            //_current->setDouble( f.value() );
            _current->setValue( f.value() );
            _current = _current->parent();
            break;
        }

        case Node::Integer:
        {
            //_current = &_current->addElement();
            _current = _current.addElement();

            const Integer& i = toInteger(node);
            //_current->setInt64( i.value() );
            _current->setValue( i.value() );
            _current = _current->parent();
            break;
        }

        case Node::Boolean:
        {
            //_current = &_current->addElement();
            _current = _current.addElement();

            const Boolean& b = toBoolean(node);
            //_current->setBool( b.value() );
            _current->setValue( b.value() );
            _current = _current->parent();
            break;
        }

        case Node::Null:
        {
            //_current = &_current->addElement();
            _current = _current.addElement();

            //const Null& n = toNull(node);
            //_current->setVoid();
            _current->setNull();
            _current = _current->parent();
            break;
        }

        default:
            break;
    }
}


void DocumentReader::onObject(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartArray:
        {
            //_current->setTypeName("array");

            _parse = &DocumentReader::onArray;
            _parseStack.push(_parse);
            break;
        }

        case Node::StartObject:
        {
            //_current->setTypeName("object");
            
            _parse = &DocumentReader::onObject;
            _parseStack.push(_parse);
            break;
        }

        case Node::EndObject:
        {
            _current = _current->parent();

            _parseStack.pop();
            _parse = _parseStack.top();
            break;
        }

        case Node::Member:
        {
            const Member& m = toMember(node);
            //_current = &_current->addMember( m.name().narrow() );
            _current = _current->addMember( m.name().narrow() );
            break;
        }

        case Node::String:
        {
            const String& s = toString(node);
            //_current->setString( s.value() );
            _current->setValue( s.value() );
            _current = _current->parent();
            break;
        }

        case Node::Float:
        {           
            const Float& f = toFloat(node);
            //_current->setDouble( f.value() );
            _current->setValue( f.value() );
            _current = _current->parent();
            break;
        }

        case Node::Integer:
        {
            const Integer& i = toInteger(node);
            //_current->setInt64( i.value() );
            _current->setValue( i.value() );
            _current = _current->parent();
            break;
        }

        case Node::Boolean:
        {
            const Boolean& b = toBoolean(node);
            //_current->setBool( b.value() );
            _current->setValue( b.value() );
            _current = _current->parent();
            break;
        }

        case Node::Null:
        {
            const Null& n = toNull(node);
            //_current->setVoid();
            _current->setNull();
            _current = _current->parent();
            break;
        }

        default:
            break;
    }
}

} // namespace

} // namespace
