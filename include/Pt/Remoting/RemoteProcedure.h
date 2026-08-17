/*
  Copyright (C) 2009-2026 by Marc Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_REMOTING_REMOTEPROCEDURE_H
#define PT_REMOTING_REMOTEPROCEDURE_H

#include <Pt/Remoting/Api.h>
#include <Pt/Remoting/Client.h>
#include <Pt/SerializationContext.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>
#include <Pt/String.h>
#include <cstddef>
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

        /** @brief Returns the client used for communication.
        */
        Client& client()
        { return *_client; }

        /** @brief Returns the name of the procedure.
        */
        const String& name() const
        { return _name; }

        /** @brief Returns true if the procedure failed.
        */
        bool isFailed() const
        { return _client->isFailed(); }

        /** @brief Cancels the remote call.
        */
        void cancel();

        /** @internal Sends finish notifcation.
        */
        void setReady();

        /** @internal Ends the remote call via the client.
        */
        void endCall();

    protected:
        /** @internal Sends finish notifcation.
        */
        virtual void onReady() = 0;

        /** @internal Releases temporary call resources without discarding the result.
        */
        virtual void onReset() = 0;

        /** @internal Clears all call resources and the stored result.
        */
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

        /** @brief Returns the result of the call.
        */
        Result<R>& result()
        {
            return _result;
        }

        /** @brief Returns the result of the call.
        */
        const Result<R>& result() const
        {
            return _result;
        }

        /** @brief Reports that the result has been received.
        */
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


template <typename... Ts>
class RemoteArguments;


template <>
class RemoteArguments<>
{
    public:
        explicit RemoteArguments(SerializationContext* /*ctx*/)
        { }

        void begin(SerializationContext& /*ctx*/)
        { }

        void fill(std::size_t i, Decomposer** args)
        {
            args[i] = 0;
        }

        void clear(SerializationContext* /*ctx*/)
        { }
};


template <typename T, typename... Ts>
class RemoteArguments<T, Ts...>
{
    public:
        explicit RemoteArguments(SerializationContext* ctx)
        : _head(ctx)
        , _tail(ctx)
        { }

        void begin(SerializationContext& ctx, const T& a, const Ts&... as)
        {
            _head.begin(a, "", ctx);
            _tail.begin(ctx, as...);
        }

        void fill(std::size_t i, Decomposer** args)
        {
            args[i] = _head.decomposer();
            _tail.fill(i + 1, args);
        }

        void clear(SerializationContext* ctx)
        {
            _head.clear(ctx);
            _tail.clear(ctx);
        }

    private:
        RemoteArgument<T> _head;
        RemoteArguments<Ts...> _tail;
};


/** @brief Performs a remote procedure call.

    This class is a function object to perform remote procedure call. It is
    a class template where the template parameters imply the signature of the
    remote procedure. The first tempate parameter is the return value type,
    followed by the argument types, if any. For example, a
    %RemoteProcedure<double, int, int> calls a remote procedure which returns
    a double and takes two ints as arguments. A %RemoteProcedure<std::string>
    calls a remote procedure without arguments, only returning a std::string.

    Synchronous operation is supported by means of the operator() and the call()
    method. Asynchronous calls can be made by calling begin and connecting to
    the finished signal, which is sent when the result has been received.
    The finshed signal has one parameter of type Result<R>, which contains
    the return value.

    The %RemoteProcedure uses a Client for communication with the service. In
    many cases this will be a HttpClient, but other transport protocols can
    be supported by other types of clients. The Client must live longer than
    the remote procedures using it.
*/
template <typename R, typename... As>
class RemoteProcedure : public RemoteProcedureBase<R>
{
    public:
        /** @brief Construct with procedure name and client.
        */
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _argv( &client.context() )
        {
            _argv.fill(0, _args);
        }

        /** @brief Destructor.
        */
        virtual ~RemoteProcedure()
        { }

        /** @brief Begins an asynchronous remote call.
        */
        void begin(const As&... args)
        {
            try
            {
                _argv.begin(this->client().context(), args...);
                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, sizeof...(As));
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        /** @brief Blocking remote call.
        */
        const R& call(const As&... args)
        {
            try
            {
                _argv.begin(this->client().context(), args...);
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, sizeof...(As));
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }

            return this->result().value();
        }

        /** @brief Blocking remote call
        */
        const R& operator()(const As&... args)
        {
            return this->call(args...);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();
            _argv.clear( &this->client().context() );
        }

    private:
        RemoteArguments<As...> _argv;
        Decomposer* _args[sizeof...(As) + 1];
};

} // namespace Remoting

} // namespace Pt

#endif // PT_REMOTING_REMOTEPROCEDURE_H
