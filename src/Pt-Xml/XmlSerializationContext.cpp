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

#include <Pt/Xml/XmlSerializationContext.h>
#include <Pt/Xml/XmlFormatter.h>
#include <Pt/Convert.h>
#include <algorithm>
#include <memory>

namespace {

template<class Iter, class T, class Pred> 
inline Iter lowerBound(Iter first, Iter last, const T& value, Pred pred)
{	
    typedef std::iterator_traits<Iter> TraitsType;

    typename TraitsType::difference_type count = std::distance(first, last);
	
    for( ; count > 0; )
	{	
		typename TraitsType::difference_type count2 = count / 2;
		
        Iter mid = first;
		std::advance(mid, count2);

		if( pred(*mid, value) ) // upper half
		{	
		    first = ++mid;
		    count -= count2 + 1;
		}
		else // lower half
        {
			count = count2;
        }
	}
	
    return first;
}

} // namespace

namespace Pt {

namespace Xml {

class XmlSerializationContext::Fixup
{
    public:
        Fixup(const std::string& id, void* fixme, FixupInfo::FixupHandler handler, const std::type_info* type, unsigned m = 0)
        : _instance(fixme)
        , _fixup(handler)
        , _type(type)
        , _id(id)
        , _m(m)
        {}

        void* instance() const
        { return _instance; }

        const std::string& id() const
        { return _id; }

        void setId(const std::string& id)
        { _id = id; }

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
        std::string _id;
        unsigned _m;
};


bool lessFixupId(XmlSerializationContext::Fixup* f, const std::string& id)
{
    return f->id() < id;
}


XmlSerializationContext::XmlSerializationContext()
{
    this->enableReferencing(true);
}


XmlSerializationContext::~XmlSerializationContext()
{
    clear();
}


void XmlSerializationContext::onClear()
{
    _refmap.clear();
    _idmap.clear();

    std::vector<Fixup*>::iterator it;
    for(it = _targets.begin(); it != _targets.end(); ++it)
    {
        delete *it;
    }

    _targets.clear();

    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        delete *it;
    }
    
    _pointers.clear();
}


bool XmlSerializationContext::onBeginSave(const void* p, const char* n)
{
    if( _idmap.find(p) == _idmap.end() )
    {
        std::size_t id = _idmap.size();
        //std::cerr << "BEGIN SAVE " << p << " " << n << std::endl;
        _idmap[p] = id;
        return true;
    }

    return false;
}


void XmlSerializationContext::onFinishSave()
{
}


void XmlSerializationContext::onPrepareId(const void* p)
{
    if(p)
    {
        //std::cerr << "PREP ID " << p  << std::endl;
        _refmap[p] = std::string();
    }
}


const char* XmlSerializationContext::onGetId(const void* p)
{
    //std::cerr << "GET ID " << p << std::endl;
    if(p == 0)
        return "null";

    if( _refmap.find(p) == _refmap.end() )
        throw SerializationError("invalid reference");

    if( _refmap[p].empty() )
    {
        if( _idmap.find(p) == _idmap.end() )
            throw SerializationError("stray reference");

        _refmap[p].clear();
        formatInt( std::back_inserter(_refmap[p]), _idmap[p] );
    }

    return _refmap[p].c_str();
}


const char* XmlSerializationContext::onMakeId(const void* p)
{
    //std::cerr << "MAKE ID " << p << std::endl;

    // already saved
    if( _idmap.find(p) == _idmap.end() )
        return 0;

    // not referenced, return empty id
    if( _refmap.find(p) == _refmap.end() )
    {
        _idmap.erase(p);
        return "";
    }

    // referenced, return id
    _refmap[p].clear();
    formatInt( std::back_inserter(_refmap[p]), _idmap[p] );

    _idmap.erase(p);
    return _refmap[p].c_str();
}


void XmlSerializationContext::onBeginLoad(void* obj, const std::type_info& fixupInfo,
                                          const char* name, const char* id)
{
    if( ! id || id[0] == '\0' )
        return;

    std::vector<Fixup*>::iterator it;
    it = lowerBound(_targets.begin(), _targets.end(), id, lessFixupId);

    //std::cerr << "beginLoad: "  << obj << " " << fixupInfo.name() << " id: " << id << std::endl;
    
    //if( it != _targets.end() && (*it)->id() == id)
    //    throw SerializationError("object loaded twice");

    std::auto_ptr<Fixup> ap( new Fixup(id, obj, 0, &fixupInfo) );
    _targets.insert(it, ap.get());
    ap.release();
}


void XmlSerializationContext::onFinishLoad()
{
}


void XmlSerializationContext::onRebindTarget(const char* id, void* obj)
{
    //std::cerr << "rebindTarget: " << id << " to " << obj << std::endl;
    
    std::vector<Fixup*>::iterator it;
    it = lowerBound(_targets.begin(), _targets.end(), id, lessFixupId);

    if( it != _targets.end() && (*it)->id() == id )
    {
        if(obj)
            (*it)->setInstance(obj);
        else
            _targets.erase(it);
    }
}


void XmlSerializationContext::onRebindFixup(const char* id, void* obj, void* from)
{
    //std::cerr << "rebindFixup " << id << " from " << from << " to " << obj << std::endl;
    
    std::vector<Fixup*>::iterator it;
    it = lowerBound(_pointers.begin(), _pointers.end(), id, lessFixupId);

    for( ; id == (*it)->id(); ++it)
    {
        if( (*it)->instance() == from )
        {
            if(obj)
                (*it)->setInstance(obj);
            else
                _pointers.erase(it);
            
            break;
        }
    }
}


void XmlSerializationContext::onPrepareFixup(void* obj, const char* id, FixupInfo::FixupHandler fh, unsigned m)
{
    //std::cerr << "prepareFixup: " << obj << " id " << id << std::endl;
    
    std::vector<Fixup*>::iterator it;
    it = lowerBound(_pointers.begin(), _pointers.end(), id, lessFixupId);

    std::auto_ptr<Fixup> ap( new Fixup(id, obj, fh, 0, m) );
    _pointers.insert(it, ap.get());
    ap.release();
}


void XmlSerializationContext::onFixup()
{
    std::vector<Fixup*>::iterator it;
    
    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        Fixup* fixup = *it;
        
        void* fixme = fixup->instance();
        const std::string& id = fixup->id();
        unsigned m = fixup->memberId();

        if( id == "null" )
        {
            const std::type_info* targetType = &( typeid(void*) );
            fixup->fixup()(fixme, 0, *targetType, m);
        }
        else
        {
            std::vector<Fixup*>::iterator target;
            target = lowerBound(_targets.begin(), _targets.end(), id, lessFixupId);

            if( target == _targets.end() || (*target)->id() != id)
            {
                throw SerializationError("reference target not found");
            }
            
            void* targetInstance = (*target)->instance();
            const std::type_info* targetType = (*target)->type();

            //std::cerr << "FIXING: " << fixme << " to " << target  << " by id " << id << std::endl;
            fixup->fixup()(fixme, targetInstance, *targetType, m);
        }
    }

    for(it = _targets.begin(); it != _targets.end(); ++it)
    {
        delete *it;
    }

    _targets.clear();

    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        delete *it;
    }
    
    _pointers.clear();
}

} // namespace Xml

} // namespace Pt
