/*
 * Copyright (C) 2012 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Xml/XmlResolver.h>
#include <cassert>

namespace Pt {

namespace Xml {

enum XmlParseState
{
    OnXmlBegin = 0,
    OnXmlDeclBegin,
    OnXmlDeclBeginQuest,
    OnXmlDeclBegin_x,
    OnXmlDeclBegin_m,
    OnXmlDeclBegin_l,
    OnXmlDecl,

    OnXmlDeclVersion_v,
    OnXmlDeclVersion_e,
    OnXmlDeclVersion_r,
    OnXmlDeclVersion_s,
    OnXmlDeclVersion_i,
    OnXmlDeclVersion_o,
    OnXmlDeclVersion_n,
    OnXmlDeclVersion_Eq,
    OnXmlDeclVersion_Quot,
    OnXmlDeclVersion_Apos,

    OnXmlDeclEncoding_e,
    OnXmlDeclEncoding_n1,
    OnXmlDeclEncoding_c,
    OnXmlDeclEncoding_o,
    OnXmlDeclEncoding_d,
    OnXmlDeclEncoding_i,
    OnXmlDeclEncoding_n2,
    OnXmlDeclEncoding_g,
    OnXmlDeclEncoding_Eq,
    OnXmlDeclEncoding_Quot,
    OnXmlDeclEncoding_Apos,

    OnXmlDeclStandalone_s,
    OnXmlDeclStandalone_t,
    OnXmlDeclStandalone_a1,
    OnXmlDeclStandalone_n1,
    OnXmlDeclStandalone_d,
    OnXmlDeclStandalone_a2,
    OnXmlDeclStandalone_l,
    OnXmlDeclStandalone_o,
    OnXmlDeclStandalone_n2,
    OnXmlDeclStandalone_e,
    OnXmlDeclStandalone_Eq,
    OnXmlDeclStandalone_Quot,
    OnXmlDeclStandalone_Apos,

    OnXmlDeclClose,
    OnXmlDeclCloseQuest,
    OnXmlDeclEnd = 64
};


bool isXmlBegin(unsigned xmlState)
{
    return xmlState != OnXmlDeclEnd;
}

const char PutbackParseXml[8] = {'<', '?', 'x', 'm','l', ' ', ' ', ' '};

bool parseXml(unsigned char& state, unsigned char c, const char*& pbBegin, const char*& pbEnd, XmlDeclaration& decl)
{
    switch(state)
    {
        case OnXmlBegin:
            if(c == '<')
            {
                state = OnXmlDeclBegin;
            }
            else
            {
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin:
            if(c == '?')
            {
                state = OnXmlDeclBeginQuest;
            }
            else
            {
                pbBegin = PutbackParseXml;
                pbEnd = PutbackParseXml + 1;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBeginQuest:
            if(c == 'x')
            {
                state = OnXmlDeclBegin_x;
            }
            else
            {
                pbBegin = PutbackParseXml;
                pbEnd = PutbackParseXml + 2;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin_x:
            if(c == 'm')
            {
                state = OnXmlDeclBegin_m;
            }
            else
            {
                pbBegin = PutbackParseXml;
                pbEnd = PutbackParseXml + 3;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin_m:
            if(c == 'l')
            {
                state = OnXmlDeclBegin_l;
            }
            else
            {
                pbBegin = PutbackParseXml;
                pbEnd = PutbackParseXml + 4;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin_l:
            if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                state = OnXmlDecl;
            }
            else
            {
                pbBegin = PutbackParseXml;
                pbEnd = PutbackParseXml + 5;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDecl:
            if(c == '?')
                state = OnXmlDeclCloseQuest;
            else if(c == 'v')
                state = OnXmlDeclVersion_v;
            else if(c == 'e')
                state = OnXmlDeclEncoding_e;
            else if(c == 's')
                state = OnXmlDeclStandalone_s;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
                state = OnXmlDecl;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_v:
            if(c == 'e')
                state = OnXmlDeclVersion_e;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_e:
            if(c == 'r')
                state = OnXmlDeclVersion_r;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_r:
            if(c == 's')
                state = OnXmlDeclVersion_s;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_s:
            if(c == 'i')
                state = OnXmlDeclVersion_i;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_i:
            if(c == 'o')
                state = OnXmlDeclVersion_o;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_o:
            if(c == 'n')
                state = OnXmlDeclVersion_n;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_n:
            if(c == '=')
                state = OnXmlDeclVersion_Eq;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                state = OnXmlDeclVersion_n;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_Eq:
            if(c == '"')
                state = OnXmlDeclVersion_Quot;
            else if(c == '\'')
                state = OnXmlDeclVersion_Apos;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                state = OnXmlDeclVersion_Eq;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclVersion_Quot:
            if(c == '"')
                state = OnXmlDecl;
            else
                decl.version() += c;

            break;

        case OnXmlDeclVersion_Apos:
            if(c == '\'')
                state = OnXmlDecl;
            else
                decl.version() += c;
            
            break;

        case OnXmlDeclEncoding_e:
            if(c == 'n')
                state = OnXmlDeclEncoding_n1;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_n1:
            if(c == 'c')
                state = OnXmlDeclEncoding_c;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_c:
            if(c == 'o')
                state = OnXmlDeclEncoding_o;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_o:
            if(c == 'd')
                state = OnXmlDeclEncoding_d;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_d:
            if(c == 'i')
                state = OnXmlDeclEncoding_i;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_i:
            if(c == 'n')
                state = OnXmlDeclEncoding_n2;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_n2:
            if(c == 'g')
                state = OnXmlDeclEncoding_g;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_g:
            if(c == '=')
                state = OnXmlDeclEncoding_Eq;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                state = OnXmlDeclEncoding_g;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_Eq:
            if(c == '"')
                state = OnXmlDeclEncoding_Quot;
            else if(c == '\'')
                state = OnXmlDeclEncoding_Apos;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                state = OnXmlDeclEncoding_Eq;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclEncoding_Quot:
            if(c == '"')
                state = OnXmlDecl;
            else
                decl.encoding() += c;

            break;

        case OnXmlDeclEncoding_Apos:
            if(c == '\'')
                state = OnXmlDecl;
            else
                decl.encoding() += c;

            break;

        case OnXmlDeclStandalone_s:
            if(c == 't')
                state = OnXmlDeclStandalone_t;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_t:
            if(c == 'a')
                state = OnXmlDeclStandalone_a1;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_a1:
            if(c == 'n')
                state = OnXmlDeclStandalone_n1;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_n1:
            if(c == 'd')
                state = OnXmlDeclStandalone_d;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_d:
            if(c == 'a')
                state = OnXmlDeclStandalone_a2;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_a2:
            if(c == 'l')
                state = OnXmlDeclStandalone_l;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_l:
            if(c == 'o')
                state = OnXmlDeclStandalone_o;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_o:
            if(c == 'n')
                state = OnXmlDeclStandalone_n2;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_n2:
            if(c == 'e')
                state = OnXmlDeclStandalone_e;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_e:
            if(c == '=')
                state = OnXmlDeclStandalone_Eq;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                state = OnXmlDeclStandalone_e;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_Eq:
            if(c == '"')
                state = OnXmlDeclStandalone_Quot;
            else if(c == '\'')
                state = OnXmlDeclStandalone_Apos;
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n' )
                state = OnXmlDeclStandalone_Eq;
            else
                state = OnXmlDeclEnd;

            break;

        case OnXmlDeclStandalone_Quot:
            if(c == '"')
                state = OnXmlDecl;
            else if(c == 'y')
                decl.setStandalone(true);
            else if(c == 'n')
                decl.setStandalone(false);

            break;

        case OnXmlDeclStandalone_Apos:
            if(c == '\'')
                state = OnXmlDecl;
            else if(c == 'y')
                decl.setStandalone(true);
            else if(c == 'n')
                decl.setStandalone(false);
            
            break;

        case OnXmlDeclCloseQuest:
            if(c == '>')
                state = OnXmlDeclClose;
            else
                state = OnXmlDeclEnd;
            
            break;

        case OnXmlDeclClose:
            state = OnXmlDeclEnd;
            break;

        case OnXmlDeclEnd:
            assert(false);
            break;

        default:
            break;
    }

    return state != OnXmlDeclEnd;
}


TextInputSource::TextInputSource()
: InputSource()
, _ios(0)
, _xmlState(OnXmlBegin)
, _pbBegin(0)
, _pbEnd(0)
{ 
}


TextInputSource::TextInputSource(std::basic_istream<Char>& is)
: InputSource()
, _ios(&is)
, _xmlState(OnXmlBegin)
, _pbBegin(0)
, _pbEnd(0)
{ 
}


void TextInputSource::reset(std::basic_istream<Char>& ios)
{
    init(0);

    _ios = &ios;

    _xmlDecl.clear();
    _id.clear();

    _xmlState = OnXmlBegin;
    _pbBegin = 0;
    _pbEnd = 0;
}


void TextInputSource::setId(const Pt::String& id)
{ 
    _id = id; 
}


bool TextInputSource::onParseXml(int_type c)
{ 
    char ch = static_cast<unsigned char>(c);
    if( ! parseXml(_xmlState, ch, _pbBegin, _pbEnd, _xmlDecl) )
    {
        return false;
    }

    return true;
}


std::streamsize TextInputSource::onImport()
{   
    if( ! _ios || ! _ios->rdbuf() )
    {
        return -1;
    }
    
    if( _ios->rdbuf()->in_avail() <= 0 )
    {
        bool r = onImportText();
        if( ! r)
            return -1;
    }

    if( isXmlBegin(_xmlState) )
    {
        std::basic_streambuf<Char>* sb = _ios->rdbuf();
        std::char_traits<Char>::int_type c = 0;
        std::streamsize avail = sb->in_avail();
        
        for( ; ; --avail)
        {            
            if(avail <= 0)
            {
                return true;
            }
            
            c = sb->sgetc();
            
            bool ok = onParseXml(c);
            if( ! ok)
                break;

            sb->sbumpc();
        }
    }

    if(_pbBegin < _pbEnd)
    {
        return *_pbBegin++;
    }

    init( _ios->rdbuf(), &_xmlDecl );
    return _ios->rdbuf()->in_avail();
}


InputSource::int_type TextInputSource::onGet()
{
    if( ! _ios || ! _ios->rdbuf() )
    {
        return std::char_traits<Char>::eof();
    }

    if( isXmlBegin(_xmlState) )
    {
        std::basic_streambuf<Char>* sb = _ios->rdbuf();
        std::char_traits<Char>::int_type c = 0;
        std::char_traits<Char>::int_type eofval = std::char_traits<Char>::eof();
        
        for( ; ; )
        {            
            c = sb->sgetc();
 
            if( std::char_traits<Char>::eq_int_type(c, eofval) )
            {
                break;    
            }
            
            bool ok = onParseXml(c);
            if( ! ok)
            {
                break;
            }

            sb->sbumpc();
        }
    }

    if(_pbBegin < _pbEnd)
    {
        return *_pbBegin++;
    }

    init( _ios->rdbuf(), &_xmlDecl );
    return _ios->rdbuf()->sbumpc();
}


const Pt::String& TextInputSource::onId() const
{ 
    return _id; 
}


bool TextInputSource::onImportText()
{
    if( ! _ios || ! _ios->rdbuf() || ! _ios->good() )
        return false;

    return true;
}


StringInputSource::StringInputSource(const String& str)
: TextInputSource()
, _ss(str)
{ 
    reset(_ss);
}


bool StringInputSource::onImportText()
{   
    // NOTE: on some systems stringbuf::in_avail never returns -1, 
    //       even if no more characters are available
    
    return _ss.rdbuf()->in_avail() > 0;
}


/*
    BOM must match the actual encoding:

    00 00 FE FF  UTF-32, big-endian
    FF FE 00 00  UTF-32, little-endian
    FE FF        UTF-16, big-endian
    FF FE        UTF-16, little-endian
    EF BB BF     UTF-8

    encoding attribute must be processed when:

    00 00 00 3C 32-bit characters encoded as ASCII values, big-endian
    3C 00 00 00 32-bit characters encoded as ASCII values, little-endian
    00 3C 00 3F 16-bit characters encoded as ASCII values, big-endian
    3C 00 3F 00 16-bit characters encoded as ASCII values, big-endian
    3C 3F 78 6D 8-bit characters encoded as ASCII values
    4C 6F A7 94 EBCDIC based encodings
*/
enum BomEncoding
{
    Bom8 = 1,
    BomUtf8 = 2,
    BomUtf16LE = 3,
    BomUtf16BE = 4,
};


enum BomParseState
{
    OnBomBegin = 0,
            
    OnBomUtf8_0,
    OnBomUtf8_1,
    OnBomUtf8_2,

    OnBomEnd = 64,
};


bool isBomBegin(unsigned char state)
{
    return state != OnBomEnd;
}


BinaryInputSource::BinaryInputSource()
: InputSource()
, _resolver(0)
, _is(0)
, _utf8Codec(1)
, _tbuf(&_utf8Codec)
, _bomState(OnBomBegin)
, _bomEncoding(Bom8)
, _xmlState(OnXmlBegin)
, _pbBegin(0)
, _pbEnd(0)
{
}


BinaryInputSource::BinaryInputSource(std::istream& is)
: InputSource()
, _resolver(0)
, _is(&is)
, _utf8Codec(1)
, _tbuf(&is, &_utf8Codec)
, _bomState(OnBomBegin)
, _bomEncoding(Bom8)
, _xmlState(OnXmlBegin)
, _pbBegin(0)
, _pbEnd(0)
{ 
}


BinaryInputSource::BinaryInputSource(XmlResolver& resolver)
: InputSource()
, _resolver(&resolver)
, _is(0)
, _utf8Codec(1)
, _tbuf(&_utf8Codec)
, _bomState(OnBomBegin)
, _bomEncoding(Bom8)
, _xmlState(OnXmlBegin)
, _pbBegin(0)
, _pbEnd(0)
{
}


BinaryInputSource::BinaryInputSource(XmlResolver& resolver, std::istream& is)
: InputSource()
, _resolver(&resolver)
, _is(&is)
, _utf8Codec(1)
, _tbuf(&is, &_utf8Codec)
, _bomState(OnBomBegin)
, _bomEncoding(Bom8)
, _xmlState(OnXmlBegin)
, _pbBegin(0)
, _pbEnd(0)
{ 
}


void BinaryInputSource::reset(std::istream& is)
{ 
    init(0);

    _resolver = 0;
    _is = &is;
    _tbuf.attach(is); 
    _tbuf.setCodec(&_utf8Codec);

    _xmlDecl.clear();
    _id.clear();

    _mbState = MBState();
    _bomState = OnBomBegin;
    _bomEncoding = Bom8;

    _xmlState = OnXmlBegin;
    _pbBegin = 0;
    _pbEnd = 0;
}


void BinaryInputSource::setId(const Pt::String& id)
{ 
    _id = id; 
}


std::streamsize BinaryInputSource::onImport()
{
    if( ! _is || ! _is->rdbuf() )
    {
        return -1;
    }
    
    if( _is->rdbuf()->in_avail() <= 0 )
    {
        bool r = onImportData();
        if( ! r)
        {
            return -1;
        }
    }

    if( isBomBegin(_bomState) )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::streamsize avail = sb->in_avail();
        
        for( ; ; --avail)
        {            
            if(avail <= 0)
            {
                return true;
            }

            c = sb->sgetc();
            char ch = std::char_traits<char>::to_char_type(c);

            if( ! onParseBom(ch) )
            {
                break;
            }

            sb->sbumpc();
        }
    }

    if( isXmlBegin(_xmlState) )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::streamsize avail = sb->in_avail();
        
        for( ; ; --avail)
        {            
            if(avail <= 0)
            {
                return true;
            }
            
            c = sb->sgetc();
            
            bool ok = onParseXml(c);
            if( ! ok)
            {
                onDeclaration();
                break;
            }

            sb->sbumpc();
        }
    }

    if(_pbBegin < _pbEnd)
    {
        return *_pbBegin++;
    }

    _tbuf.import();
    init(&_tbuf, &_xmlDecl);
    return _tbuf.in_avail();
}


InputSource::int_type BinaryInputSource::onGet()
{
    if( ! _is || ! _is->rdbuf() )
    {
        return std::char_traits<Char>::eof();
    }

    if( isBomBegin(_bomState) )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();
        
        for( ; ; )
        {           
            c = sb->sgetc();
 
            if( std::char_traits<char>::eq_int_type(c, eofval) )
            {
                break;    
            }

            char ch = std::char_traits<char>::to_char_type(c);
            
            if( ! onParseBom(ch) )
            {
                break;
            }

            sb->sbumpc();
        }
    }

    if( isXmlBegin(_xmlState) )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();
        
        for( ; ; )
        {            
            c = sb->sgetc();
 
            if( std::char_traits<char>::eq_int_type(c, eofval) )
            {
                break;    
            }
            
            bool ok = onParseXml(c);
            if( ! ok)
            {
                onDeclaration();
                break;
            }

            sb->sbumpc();
        }
    }
    
    if(_pbBegin < _pbEnd)
    {
        return *_pbBegin++;
    }

    init(&_tbuf, &_xmlDecl);
    return _tbuf.sbumpc();
}


const Pt::String& BinaryInputSource::onId() const
{ 
    return _id; 
}


bool BinaryInputSource::onImportData()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
        return false;

    return true;
}


bool BinaryInputSource::onParseBom(unsigned char c)
{    
    switch(_bomState)
    {
        case OnBomBegin:
            if(c == 0xef)
                _bomState = OnBomUtf8_0;
            else
                _bomState = OnBomEnd;

            break;

        case OnBomUtf8_0:
            if(c == 0xbb)
                _bomState = OnBomUtf8_1;
            else
                _bomState = OnBomEnd;
                    
            break;

        case OnBomUtf8_1:
            if(c == 0xbf)
            {
                _bomState = OnBomUtf8_2;
                _bomEncoding = BomUtf8;
                break;
            }

            _bomState = OnBomEnd;
            break;

        case OnBomUtf8_2:
            _bomState = OnBomEnd;
            break;

        case OnBomEnd:
            assert(false);
            break;

        default:
            break;
    }

    return _bomState != OnBomEnd;
}


bool BinaryInputSource::onParseXml(int c)
{ 
    char ch = 0;
    
    if(_bomEncoding == Bom8) // ASCII compatible
    {
        ch = std::char_traits<char>::to_char_type(c);
    }
    else if(_bomEncoding == BomUtf16BE) // ASCII subset in UTF-16-BE
    {
        if(_mbState.n == 0)
        {
            _mbState.n = 1;
            return true;
        }

        _mbState.n = 0;
        ch = std::char_traits<char>::to_char_type(c);
    }
    else if(_bomEncoding == BomUtf16LE)  // ASCII subset in UTF-16-LE
    {
        if(_mbState.n == 0)
        {
            _mbState.n = 1;
            ch = std::char_traits<char>::to_char_type(c);
            return true;
        }

        _mbState.n = 0;
        ch = _mbState.value.mbytes[0];
    }
            
    if( ! parseXml(_xmlState, ch, _pbBegin, _pbEnd, _xmlDecl) )
    {
        return false;
    }

    return true;
}


void BinaryInputSource::onDeclaration()
{
    if(_bomEncoding == BomUtf8)
    {
        if( _xmlDecl.encoding().empty() || _xmlDecl.encoding() == "UTF-8" )
            return;

        throw SyntaxError("invalid encoding", 0);
    }
    else if(_bomEncoding == Bom8)
    {
        if( _xmlDecl.encoding().empty() || _xmlDecl.encoding() == "UTF-8" )
            return;
    }

    const char* encoding = _xmlDecl.encoding().c_str();
    TextCodec<Char, char>* codec = 0;
    if(_resolver)
        codec = _resolver->resolveEncoding(encoding);

    if( ! codec)
        throw SyntaxError("invalid encoding", 0);
    
    _tbuf.setCodec(codec);
}

} // namespace Xml

} // namespace Pt
