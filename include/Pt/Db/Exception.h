/*
 * Copyright (C) 2004-2006 Marc Boris Duerner
 * Copyright (C) 2005-2006 Aloysius Indrayanto
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

#ifndef Pt_Db_Exception_h
#define Pt_Db_Exception_h

#include <Pt/Db/Api.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <Pt/SourceInfo.h>


namespace Pt {

namespace Db {

    /** @brief This indicates that a resource could not be accessed.
        @ingroup Pt

        An exception of class AccessError is used to report failed access
        to a resource due to missing authorization, missing access rights
        or if a resource is in an otherwise inaccessible state. This class
        implements std::logic_error. Use the PT_SOURCEINFO macro to pass
        SourceInfo to Exception.

        TODO: rename AccessDenied
    */
    class PT_DB_API AccessError : public std::logic_error {
        public:
            //! @see Exception()
            AccessError(const std::string& what, const SourceInfo& si) throw();

            //! @brief Destructor.
            ~AccessError() throw();
    };

} // namespace Db

} // namespace Pt

#endif

