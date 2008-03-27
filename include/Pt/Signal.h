/***************************************************************************
 *   Copyright (C) 2004-2006 by Dr. Marc Boris Duerner                     *
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

#ifndef Pt_Signal_h
#define Pt_Signal_h

#include <Pt/Void.h>
#include <Pt/Function.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Connectable.h>

#include <list>
#include <algorithm>

namespace Pt {

    /** @internal
    */
    class PT_API SignalBase : public Connectable
    {
        public:
            struct PT_API Sentry
            {
                Sentry(const SignalBase* signal);

                ~Sentry();

                void detach();

                bool operator!() const
                { return _signal == 0; }

                const SignalBase* _signal;
            };

            SignalBase();

            ~SignalBase();

            SignalBase& operator=(const SignalBase& other);

            virtual bool opened(const Connection& c);

            virtual void closed(const Connection& c);

        private:
            mutable Sentry* _sentry;
            mutable bool _sending;
            mutable bool _dirty;
    };


#include <Pt/Signal.tpp>

} // !namespace Pt

#endif
