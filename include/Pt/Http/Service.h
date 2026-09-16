/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Service_h
#define Pt_Http_Service_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Responder.h>
#include <Pt/Types.h>
#include <Pt/Allocator.h>
#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

namespace Http {

class Request;
class IOStream;

/** @brief HTTP service.
*/
class PT_HTTP_API Service : private NonCopyable
{
    friend class ServerImpl;

    public:
        /** @brief Default Constructor.
        */
        Service();

        /** @brief Destructor.
        */
        virtual ~Service();

        /** @brief Creates a responder to handle request received by a server.
        */
        Responder* getResponder(const Request&);
        
        /** @brief Destroys a responder created by a server.
        */
        void releaseResponder(Responder*);

        /** @brief Returns the signal emitted when a request upgrades to this service.

            The signal provides the accepted stream and the value of the request's
            %Upgrade header.
        */
        Signal<IOStream*, const std::string&>& upgradeRequested();

    protected:
        /** @brief Creates a responder to handle request received by a server.
        */
        virtual Responder* onGetResponder(const Request&) = 0;
        
        /** @brief Destroys a responder created by a server.
        */
        virtual void onReleaseResponder(Responder*) = 0;

        /** @brief Returns whether the service accepts an upgraded connection.

            @a stream is deleted when this function returns false. The caller of a
            connected %upgradeRequested signal slot owns @a stream when this
            function returns true.
        */

    private:
        // service specific options need to be set in Service ctor so it can
        // be used concurrently by server threads without locking
        Pt::varint_t _r0;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
        Signal<IOStream*, const std::string&> _upgradeRequested;
};

/** @brief Basic HTTP service implementation.
*/
template <typename R, typename Alloc = Allocator>
class BasicService : public Service
{
    public:
        /** @brief Default Constructor.
        */
        BasicService()
        { }

        /** @brief Destructor.
        */
        ~BasicService()
        { }

    protected:
        virtual Responder* onGetResponder(const Request&)
        {
            void* r = _alloc.allocate( sizeof(R) );
            return new(r) R(*this);
        }

        virtual void onReleaseResponder(Responder* r)
        {
            r->~Responder();
            _alloc.deallocate( r, sizeof(R) );
        }

    private:
        Alloc _alloc;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Service_h
