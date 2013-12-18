/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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

#ifndef Pt_XmlRpc_Formatter_h
#define Pt_XmlRpc_Formatter_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Composer.h>
#include <Pt/Formatter.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>
#include <iostream>

namespace Pt {

namespace XmlRpc {

/** @internal @brief Formatter to read and write XML-RPC messages.
*/
class PT_XMLRPC_API Formatter : public Pt::Formatter
                              , private NonCopyable
{
    public:
        Formatter(std::basic_ostream<Char>& os);

        ~Formatter();

        void attach(Xml::XmlReader& reader);

        void attach(std::basic_ostream<Char>& os);

        /** @internal @brief onParse() onParseSome() should be implemented 
            instead of this method.
        */
        bool advance(const Pt::Xml::Node& node);

    protected:
        void onAddString(const char* name, const char* type,
                         const Pt::Char* value, const char* id);

        void onAddBool(const char* name, bool value, 
                       const char* id);

        void onAddChar(const char* name, const Pt::Char& value,
                       const char* id);

        void onAddInt8(const char* name, Pt::int8_t value,
                       const char* id);
        
        void onAddInt16(const char* name, Pt::int16_t value,
                        const char* id);
        
        void onAddInt32(const char* name, Pt::int32_t value,
                        const char* id);
        
        void onAddInt64(const char* name, Pt::int64_t value,
                        const char* id);

        void onAddUInt8(const char* name, Pt::uint8_t value, const char* id);
        
        void onAddUInt16(const char* name, Pt::uint16_t value,  const char* id);
        
        void onAddUInt32(const char* name, Pt::uint32_t value, const char* id);
        
        void onAddUInt64(const char* name, Pt::uint64_t value, const char* id);

        void onAddFloat(const char* name, float value, 
                        const char* id);

        void onAddDouble(const char* name, double value, 
                         const char* id);

        void onAddLongDouble(const char* name, long double value, 
                             const char* id);

        void onAddBinary(const char* name, const char* type,
                         const char* value, std::size_t length, const char* id);

        void onAddReference(const char* name, const char* id);

        void onBeginSequence(const char* name, const char* type,
                             const char* id);

        virtual void onBeginElement();

        virtual void onFinishElement();

        void onFinishSequence();

        void onBeginStruct(const char* name, const char* type,
                           const char* id);

        void onBeginMember(const char* name);

        void onFinishMember();

        void onFinishStruct();

    protected:
        void onBeginParse(Composer& composer);

        bool onParseSome();

        void onParse();

    private:
        enum State
        {
            OnParam,
            OnValueBegin,
            OnValueEnd,
            OnBoolBegin,
            OnIntBegin,
            OnDoubleBegin,
            OnStringBegin,
            OnScalar,
            OnScalarEnd,
            OnStructBegin,
            OnMemberBegin,
            OnNameBegin,
            OnName,
            OnNameEnd,
            OnStructEnd,
            OnArrayBegin,
            OnDataBegin,
            OnDataEnd,
            OnArrayEnd
        };

        Xml::XmlReader* _reader;
        State _state;
        Composer* _composer;

        std::basic_ostream<Char>* _os;
        Pt::String _str;

        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_Formatter_h
