/*
 * Copyright (C) 2009-2014 by Dr. Marc Boris Duerner
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

#ifndef Pt_Remoting_RemoteProcedure_h
#define Pt_Remoting_RemoteProcedure_h

#include <Pt/Remoting/Api.h>
#include <Pt/Remoting/Client.h>
#include <Pt/SerializationContext.h>
#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

namespace Remoting {

/** @internal Documented externally.
*/
class PT_REMOTING_API RemoteCall : private Pt::NonCopyable
{
    public:
        RemoteCall(Client& client, const String& name);

        RemoteCall(Client& client, const std::string& name);

        RemoteCall(Client& client, const char* name);

        virtual ~RemoteCall();

        Client& client()
        { return *_client; }

        const String& name() const
        { return _name; }

        bool isFailed() const
        { return _client->isFailed(); }

        void cancel();

        void setReady();

        void endCall();

    protected:
        virtual void onReady() = 0;

        virtual void onReset() = 0;

        virtual void onClear() = 0;

    private:
        Client* _client;
        String _name;
};

/** @brief %Result of a remote procedure call.
*/
template <typename R>
class Result : private Pt::NonCopyable
{
    public:
        /** @brief Constructor.
        */
        explicit Result(RemoteCall* call = 0)
        : _call(call)
        , _result(0)
        { 
            _result = new (_mem) R;
        }

        ~Result()
        {
            _result->~R();
        }

        void init(RemoteCall* call)
        { _call = call; }

        /** @brief Indicates if the procedure has failed.

            If this method returns false, get() will not throw an excption.
        */
        bool isFailed() const
        {
            return _call->isFailed();
        }

        /** @brief The return value.
        */
        R& value()
        { return *_result; }

        /** @brief Ends a remote procedure call.

            This method ends a remote procedure call when the RemoteProcedure
            sends the finished signal. If the procedure has failed, an exception
            of type Fault is thrown. Other exceptions might be raised depending
            on the used Client.
        */
        const R& get() const
        {
            _call->endCall();
            return *_result;
        }

        void clear()
        {
            _result->~R();
            _result = new (_mem) R;
        }

    private:
        RemoteCall* _call;
        char _mem[ sizeof(R) ];
        R* _result;
};

/** @internal Documented externally.
*/
template <typename R>
class RemoteProcedureBase : public RemoteCall
{
    public:
        RemoteProcedureBase(Client& client, const std::string& name)
        : RemoteCall(client, name)
        , _result(0)
        , _r(0)
        { 
          _result.init(this);
        }

        ~RemoteProcedureBase()
        {
            if(_r)
                _r->~BasicComposer<R>();
        }

        Result<R>& result()
        { 
            return _result; 
        }

        const Result<R>& result() const
        {
            return _result;
        }

        Signal< const Result<R>& >& finished()
        { return _finished; }

    protected:
        void onReady()
        { _finished.send(_result); }

        BasicComposer<R>& beginResult()
        {
            if(_r)
            {
                _r->~BasicComposer<R>();
                _r = 0;
            }

            _r = new (_mem) BasicComposer<R>( &client().context() );
            _r->begin( result().value() );
            return *_r;
        }

        virtual void onReset()
        {
            if(_r)
            {
                _r->~BasicComposer<R>();
                _r = 0;
            }
        }

        virtual void onClear()
        {
            this->onReset();
            _result.clear();
        }

    private:
        Signal< const Result<R>& > _finished;
        Result<R> _result;
        char _mem[ sizeof(BasicComposer<R>) ];
        BasicComposer<R>* _r;
};


template <typename A>
class RemoteArgument : private Pt::NonCopyable
{
    public:
        RemoteArgument(SerializationContext* )
        : _decomposer(0)
        { }

        ~RemoteArgument()
        {
            if(_decomposer)
                _decomposer->~BasicDecomposer<A>();
        }

        void begin(const A& a, const char* name, SerializationContext& ctx)
        {
            if(_decomposer)
            {
                _decomposer->~BasicDecomposer<A>();
                _decomposer = 0;
            }

            _decomposer = new (_mem) BasicDecomposer<A>(&ctx);
            _decomposer->begin(a, name);
        }

        void clear(SerializationContext* )
        {
            if(_decomposer)
            {
                _decomposer->~BasicDecomposer<A>();
                _decomposer = 0;
            }
        }

        // TODO: return decomposer pointer from begin
        BasicDecomposer<A>* decomposer()
        { return reinterpret_cast<BasicDecomposer<A>*>(_mem); }

    private:
        char _mem[ sizeof(BasicDecomposer<A>) ];
        BasicDecomposer<A>* _decomposer;
};

} // namespace Remoting

} // namespace Pt

#include <Pt/Remoting/RemoteProcedure.tpp>

#endif // Pt_Remoting_RemoteProcedure_h
