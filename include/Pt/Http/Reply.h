/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef Pt_Http_Reply_h
#define Pt_Http_Reply_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Message.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

namespace Http {

/** @brief HTTP reply message.
*/
class PT_HTTP_API Reply : public Message
{
    friend class Connection;

    public:
        /** @brief HTTP reply status code.
        */
        enum StatusCode
        {
            Continue = 100,              //!< Continue
            OK = 200,                    //!< OK
            MultipleChoices = 300,       //!< Multiple choices for request
            BadRequest = 400,            //!< Bad request received
            Unauthorized = 401,          //!< Unauthorized access
            RequestEntityTooLarge = 413, //!< %Request entity too large
            InternalServerError = 500    //!< Internal server error occured
        };

    public:
        /** @brief Construct with connection.
        */
        explicit Reply(Http::Connection& conn)
        : Message(conn)
        , _statusCode(200)
        , _statusText("OK")
        { }
        
        /** @brief Sets the HTTP status.
        */
        void setStatus(unsigned code, const std::string& txt)
        {
            _statusCode = code;
            _statusText = txt;
        }

        /** @brief Sets the HTTP status.
        */
        void setStatus(unsigned code, const char* txt)
        {
            _statusCode = code;
            _statusText = txt;
        }

        //! @brief Returns the HTTP status code.
        unsigned statusCode() const
        { return _statusCode; }

        //! @brief Returns the HTTP status text.
        const std::string& statusText() const
        { return _statusText; }

        //! @internal
        void receive();

        //! @internal
        void beginReceive();

        //! @internal
        MessageProgress endReceive();

        //! @brief Begin sending the reply.
        void beginSend(bool finish = true);

        //! @internal
        MessageProgress endSend();

        //! @internal
        Signal<Reply&>& inputReceived()
        { return _inputReceived; }

        //! @internal
        Signal<Reply&>& outputSent()
        { return _outputSent; }

        /** @brief Clears all content.
        */
        void clear();

    protected:
        //! @internal
        void onInput()
        { _inputReceived.send(*this); }

        //! @internal
        void onOutput()
        { _outputSent.send(*this); }

    private:
        unsigned _statusCode;
        std::string _statusText;
        Signal<Reply&> _inputReceived;
        Signal<Reply&> _outputSent;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Reply_h
