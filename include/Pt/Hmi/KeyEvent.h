/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
#ifndef Pt_Hmi_KeyEvent_h
#define Pt_Hmi_KeyEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Key.h>
#include <Pt/Event.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API KeyEvent : public Pt::BasicEvent<KeyEvent>
{
    public:
	      enum Action
	      {
		      Press,
		      Release
	      };

    public:	
        KeyEvent()
        : _vid(0)
        , _visual(0)
        , _action(Release)
	      {
	      }

	      explicit KeyEvent(Visual& v)
        : _vid( v.vid() )
        , _visual(&v)
        , _action(Release)
	      {
	      }

        Pt::uint64_t vid() const
        {
            return _vid;
        }

        Visual* visual() const
        {
            return _visual;
        }
        
        void setVisual(Visual* v)
        {
            _visual = v;
            _vid = v ? v->vid() : 0;
        }
        
        const Key& key() const
        {
            return _key;
        }

        const Pt::Char& unicode() const
        {
            return _unicode;
        }

        bool isPress() const
        {
            return _action == Press;
        }

        void setPress(const Key& key, const Pt::Char& ch)
        {
            _action = Press;
            _key = key;
            _unicode = ch;
        }

        bool isRelease() const
        {
            return _action == Release;
        }

        void setRelease(const Key& key, const Pt::Char& ch)
        {
            _action = Release;
            _key = key;
            _unicode = ch;
        }

    private:
        Pt::uint64_t _vid;
        Visual*      _visual;
        Action       _action;
        Key          _key;
        Pt::Char     _unicode;        
};

} // namespace

} // namespace

#endif

