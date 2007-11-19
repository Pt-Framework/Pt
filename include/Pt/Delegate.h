/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Dürner                          *
 *   Copyright (C) 2005 Stephan Beal                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Delegate_h
#define Pt_Delegate_h

#include <Pt/Void.h>
#include <Pt/Exception.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>
#include <stdexcept>


namespace Pt {

    /** @internal
    */
    class DelegateBase : public Connectable
    {
        public:
            DelegateBase()
            { }

            DelegateBase(const DelegateBase& rhs)
            { DelegateBase::operator=(rhs); }

            DelegateBase& operator=(const DelegateBase& other)
            {
                _target.close();

                if( !other._target )
                    return *this;

                const Slot& slot = other._target.slot();
                _target = Connection( *this, slot.clone()  );

                return *this;
            }

            virtual bool opened(const Connection& c)
            {
                const Connectable& sender = c.sender();

                if( &sender == this )
                {
                    _target.close();

                    bool accept = Connectable::opened(c);

                    if(accept)
                        _target = c;

                    return accept;
                }

                return Connectable::opened(c);
            }

            virtual void closed(const Connection& c)
            {
                Connectable::closed(c);
            }

        protected:
            Connection _target;
    };


#include <Pt/gen-delegate.h>

} // !namespace Pt

#endif
