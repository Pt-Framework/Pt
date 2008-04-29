/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Duerner                            *
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
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

#ifndef PT_SYSTEM_IOERROR_H
#define PT_SYSTEM_IOERROR_H

#include <Pt/System/Api.h>
#include <Pt/SourceInfo.h>
#include <ios>
#include <stdexcept>

namespace Pt {

namespace System {

    class PT_SYSTEM_API IOError : public std::ios::failure {
        public:
            IOError(const std::string& what, const SourceInfo& si);

            ~IOError() throw();
    };

    class PT_SYSTEM_API OpenFailed : public IOError {
        public:
            OpenFailed(const std::string& what, const SourceInfo& si);

            ~OpenFailed() throw();
    };

    class PT_SYSTEM_API CloseFailed : public IOError {
        public:
            CloseFailed(const std::string& what, const SourceInfo& si);

            ~CloseFailed() throw();
    };

} // namespace System

} // namespace Pt

#endif
