/*
 * Copyright (C) 2004-2006 Marc Boris Duerner
 * Copyright (C) 2005 Aloysius Indrayanto
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

#ifndef PT_SYSTEM_IOERROR_H
#define PT_SYSTEM_IOERROR_H

#include <Pt/System/Api.h>
#include <Pt/SourceInfo.h>
#include <ios>
#include <stdexcept>

namespace Pt {

namespace System {

    class PT_SYSTEM_API IOError : public std::ios::failure
    {
        public:
            IOError(const std::string& what, const SourceInfo& si);

            IOError(const char* what);

            ~IOError() throw()
            {}
    };

    class PT_SYSTEM_API IOTimeout : public IOError
    {
        public:
            IOTimeout();

            ~IOTimeout() throw()
            {}
    };

    class PT_SYSTEM_API AccessFailed : public IOError
    {
        public:
            AccessFailed(const std::string& resource, const SourceInfo& si);

            ~AccessFailed() throw()
            {}

            const std::string& resource() const
            { return _resource; }

        private:
            std::string _resource;
    };

    class PT_SYSTEM_API PermissionDenied : public AccessFailed
    {
        public:
            PermissionDenied(const std::string& resource, const SourceInfo& si);

            ~PermissionDenied() throw()
            {}
    };

    class PT_SYSTEM_API DeviceNotFound : public AccessFailed
    {
        public:
            DeviceNotFound(const std::string& device, const SourceInfo& si);

            ~DeviceNotFound() throw()
            {}
    };


    class PT_SYSTEM_API FileNotFound : public AccessFailed
    {
        public:
            FileNotFound(const std::string& path, const SourceInfo& si);

            ~FileNotFound() throw()
            {}
    };

    /** @brief A directory could not be found at a given path
    */
    class PT_SYSTEM_API DirectoryNotFound : public AccessFailed
    {
        public:
            /** @brief Construct from path and source info

                Constructs the exception from the path where the directory
                could not be found and the location in the source code where
                he exception was thrown.
            */
            DirectoryNotFound(const std::string& path, const SourceInfo& si);

            //! @brief Destructor
            ~DirectoryNotFound() throw()
            {}
    };

    class PT_SYSTEM_API IOPending : public IOError
    {
        public:
            IOPending(const std::string& what, const SourceInfo& si);

            IOPending(const char* what);

            ~IOPending() throw()
            {}
    };

} // namespace System

} // namespace Pt

#endif
