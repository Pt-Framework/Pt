/*
 * Copyright (C) 2008-2013 by Marc Boris Duerner
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
#include <Pt/Types.h>
#include <map>
#include <vector>
#include <string>

namespace Pt {

namespace Xml {

/** @brief Serialization context for XML serialization.
*/
class PT_XML_API XmlSerializationContext : public SerializationContext
{
    public:
        class Fixup;

    public:
        //! @brief Default Constructor.
        XmlSerializationContext();

        //! @brief Destructor.
        ~XmlSerializationContext();

    protected:
        // inherit docs
        virtual void onClear();

        // inherit docs
        virtual bool onBeginSave(const void* p, const char* name);
        
        // inherit docs
        virtual void onFinishSave();
        
        // inherit docs
        virtual void onPrepareId(const void* p);
        
        // inherit docs
        virtual const char* onGetId(const void* p);
        
        // inherit docs
        virtual const char* onMakeId(const void* p);

    protected:
        // inherit docs
        virtual void onBeginLoad(void* obj, const std::type_info& fixupInfo,
                                 const char* name, const char* id);
        // inherit docs
        virtual void onFinishLoad();
        
        // inherit docs
        virtual void onRebindTarget(const char* id, void* obj);
        
        // inherit docs
        virtual void onRebindFixup(const char* id, void* obj, void* prev);
        
        // inherit docs
        virtual void onPrepareFixup(void* obj, const char* id, FixupInfo::FixupHandler, unsigned m);
        
        // inherit docs
        virtual void onFixup();

    private:
        XmlSerializationContext(const XmlSerializationContext& si)
        {}

        XmlSerializationContext& operator=(const XmlSerializationContext& si)
        { return *this; }

    private:
        std::map<const void*, std::size_t> _idmap;
        std::map<const void*, std::string> _refmap;
        std::vector<Fixup*> _targets;
        std::vector<Fixup*> _pointers; 
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_XmlSerializationContext_h
