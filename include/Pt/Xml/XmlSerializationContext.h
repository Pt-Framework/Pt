/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#ifndef Pt_Xml_XmlSerializationContext_h
#define Pt_Xml_XmlSerializationContext_h

#include <Pt/Xml/Api.h>
#include <Pt/SerializationContext.h>
#include <map>
#include <string>

namespace Pt {

namespace Xml {

class XmlFormatter;

class PT_XML_API XmlSerializationContext : public SerializationContext
{
    public:
        XmlSerializationContext();

        //! @brief Destructor
        ~XmlSerializationContext();

        virtual void reset();

    public:
        virtual bool beginSave(const void* p, const std::string& name);

        virtual void finishSave();

        virtual void prepareId(const void* p);

        virtual const char* getId(const void* p);

        virtual const char* makeId(const void* p);

    private:
        std::map<const void*, unsigned> _idmap;
        std::map<const void*, std::string> _refmap;

    public:
        virtual void beginLoad(void* obj, const std::type_info& fixupInfo,
                               const std::string& name, const std::string& id);

        virtual void finishLoad();

        virtual void rebindTarget(const std::string& id, void* obj);

        virtual void rebindFixup(const std::string& id, void* obj, void* prev);

        virtual void prepareFixup(void* obj, const std::string& id, FixupInfo::FixupHandler, unsigned m);

        virtual void fixup();

    private:
        class Fixup
        {
            public:
                Fixup()
                : _instance(0)
                , _fixup(0)
                , _type(0)
                {}

                Fixup(void* fixme, FixupInfo::FixupHandler handler, const std::type_info* type, unsigned m = 0)
                : _instance(fixme)
                , _fixup(handler)
                , _type(type)
                , _m(m)
                {}

                ~Fixup()
                {}

                void* instance() const
                { return _instance; }

                void setInstance(void* obj)
                {
                    _instance = obj;
                }

                FixupInfo::FixupHandler fixup() const
                { return _fixup; }

                const std::type_info* type() const
                { return _type; }

                unsigned memberId() const
                { return _m; }

            private:
                void* _instance;
                FixupInfo::FixupHandler _fixup;
                const std::type_info* _type;
                unsigned _m;
        };

        std::map<std::string, Fixup> _targets;
        std::multimap<std::string, Fixup> _pointers;
};

} // namespace Xml

} // namespace Pt

#endif
