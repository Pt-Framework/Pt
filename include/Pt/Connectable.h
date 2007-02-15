/***************************************************************************
 *   Copyright (C) 2004-2006 by Dr. Marc Boris Drner                           *
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
#ifndef Pt_Connectable_h
#define Pt_Connectable_h

#include <Pt/Api.h>
#include <Pt/Connection.h>
#include <list>


namespace Pt {

    class PT_API Connectable {
        public:
            Connectable();

            virtual ~Connectable();

            //! @brief Registers a Connection with the Connectable.
            virtual void opened(const Connection& c);

            //! @brief Unregisters a Connection from the Connectable.
            virtual void closed(const Connection& c);

            const std::list<Connection>& connections() const
            { return _connections; }

            std::list<Connection>& connections()
            { return _connections; }

        protected:
            Connectable(const Connectable& c);

            Connectable& operator=(const Connectable& rhs);

            void clear();

        protected:
            mutable std::list<Connection> _connections;
    };

} // !namespace Pt


#endif
