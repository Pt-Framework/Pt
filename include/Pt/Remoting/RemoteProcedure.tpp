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

#ifndef PT_REMOTING_REMOTEPROCEDURE_TPP
#define PT_REMOTING_REMOTEPROCEDURE_TPP

namespace Pt {

namespace Remoting {

template <typename R,
          typename A1 = Pt::Void,
          typename A2 = Pt::Void,
          typename A3 = Pt::Void,
          typename A4 = Pt::Void,
          typename A5 = Pt::Void,
          typename A6 = Pt::Void,
          typename A7 = Pt::Void,
          typename A8 = Pt::Void,
          typename A9 = Pt::Void,
          typename A10 = Pt::Void>
class RemoteProcedure : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        , _a5( &client.context() )
        , _a6( &client.context() )
        , _a7( &client.context() )
        , _a8( &client.context() )
        , _a9( &client.context() )
        , _a10( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
            _args[4] = _a5.decomposer();
            _args[5] = _a6.decomposer();
            _args[6] = _a7.decomposer();
            _args[7] = _a8.decomposer();
            _args[8] = _a9.decomposer();
            _args[9] = _a10.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                _a8.begin(a8, "", this->client().context());
                _a9.begin(a9, "", this->client().context());
                _a10.begin(a10, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 10);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                _a8.begin(a8, "", this->client().context());
                _a9.begin(a9, "", this->client().context());
                _a10.begin(a10, "", this->client().context());
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 10);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
            _a5.clear( &this->client().context() );
            _a6.clear( &this->client().context() );
            _a7.clear( &this->client().context() );
            _a8.clear( &this->client().context() );
            _a9.clear( &this->client().context() );
            _a10.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        RemoteArgument<A5> _a5;
        RemoteArgument<A6> _a6;
        RemoteArgument<A7> _a7;
        RemoteArgument<A8> _a8;
        RemoteArgument<A9> _a9;
        RemoteArgument<A10> _a10;
        Decomposer* _args[10]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8,
          typename A9>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        , _a5( &client.context() )
        , _a6( &client.context() )
        , _a7( &client.context() )
        , _a8( &client.context() )
        , _a9( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
            _args[4] = _a5.decomposer();
            _args[5] = _a6.decomposer();
            _args[6] = _a7.decomposer();
            _args[7] = _a8.decomposer();
            _args[8] = _a9.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                _a8.begin(a8, "", this->client().context());
                _a9.begin(a9, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 9);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                _a8.begin(a8, "", this->client().context());
                _a9.begin(a9, "", this->client().context());
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 9);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
            _a5.clear( &this->client().context() );
            _a6.clear( &this->client().context() );
            _a7.clear( &this->client().context() );
            _a8.clear( &this->client().context() );
            _a9.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        RemoteArgument<A5> _a5;
        RemoteArgument<A6> _a6;
        RemoteArgument<A7> _a7;
        RemoteArgument<A8> _a8;
        RemoteArgument<A9> _a9;
        Decomposer* _args[9]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        , _a5( &client.context() )
        , _a6( &client.context() )
        , _a7( &client.context() )
        , _a8( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
            _args[4] = _a5.decomposer();
            _args[5] = _a6.decomposer();
            _args[6] = _a7.decomposer();
            _args[7] = _a8.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                _a8.begin(a8, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 8);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                _a8.begin(a8, "", this->client().context());
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 8);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
            _a5.clear( &this->client().context() );
            _a6.clear( &this->client().context() );
            _a7.clear( &this->client().context() );
            _a8.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        RemoteArgument<A5> _a5;
        RemoteArgument<A6> _a6;
        RemoteArgument<A7> _a7;
        RemoteArgument<A8> _a8;
        Decomposer* _args[8]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        , _a5( &client.context() )
        , _a6( &client.context() )
        , _a7( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
            _args[4] = _a5.decomposer();
            _args[5] = _a6.decomposer();
            _args[6] = _a7.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 7);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                _a7.begin(a7, "", this->client().context());
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 7);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
            _a5.clear( &this->client().context() );
            _a6.clear( &this->client().context() );
            _a7.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        RemoteArgument<A5> _a5;
        RemoteArgument<A6> _a6;
        RemoteArgument<A7> _a7;
        Decomposer* _args[7]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        , _a5( &client.context() )
        , _a6( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
            _args[4] = _a5.decomposer();
            _args[5] = _a6.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 6);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                _a6.begin(a6, "", this->client().context());
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 6);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            return this->call(a1, a2, a3, a4, a5, a6);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
            _a5.clear( &this->client().context() );
            _a6.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        RemoteArgument<A5> _a5;
        RemoteArgument<A6> _a6;
        Decomposer* _args[6]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5>
class RemoteProcedure<R, A1, A2, A3, A4, A5,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        , _a5( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
            _args[4] = _a5.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 5);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                _a5.begin(a5, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 5);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            return this->call(a1, a2, a3, a4, a5);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
            _a5.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        RemoteArgument<A5> _a5;
        Decomposer* _args[5]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4>
class RemoteProcedure<R, A1, A2, A3, A4,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        , _a4( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
            _args[3] = _a4.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());

                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 4);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                _a4.begin(a4, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 4);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            return this->call(a1, a2, a3, a4);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
            _a4.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        RemoteArgument<A4> _a4;
        Decomposer* _args[4]; 
};


template <typename R,
          typename A1,
          typename A2,
          typename A3>
class RemoteProcedure<R, A1, A2, A3,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        , _a3( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
            _args[2] = _a3.decomposer();
        }

        void begin(const A1& a1, const A2& a2, const A3& a3)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 3);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                _a3.begin(a3, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 3);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3)
        {
            return this->call(a1, a2, a3);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
            _a3.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        RemoteArgument<A3> _a3;
        Decomposer* _args[3]; 
};


template <typename R,
          typename A1,
          typename A2>
class RemoteProcedure<R, A1, A2,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        , _a2( &client.context() )
        { 
            _args[0] = _a1.decomposer();
            _args[1] = _a2.decomposer();
        }

        void begin(const A1& a1, const A2& a2)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();

                this->client().beginCall(r, *this, _args, 2);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1, const A2& a2)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                _a2.begin(a2, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();

                this->client().call(r, *this, _args, 2);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            return this->result().value();
        }

        const R& operator()(const A1& a1, const A2& a2)
        {
            return this->call(a1, a2);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();

            _a1.clear( &this->client().context() );
            _a2.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        RemoteArgument<A2> _a2;
        Decomposer* _args[2]; 
};


template <typename R,
          typename A1>
class RemoteProcedure<R, A1,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        , _a1( &client.context() )
        {
            _args[0] = _a1.decomposer();
        }

        void begin(const A1& a1)
        {
            try
            {
                // TODO: pass instance name to format()/onFormat() and 
                //                             beginFormat()/onBeginFormat()

                _a1.begin(a1, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();
                
                this->client().beginCall(r, *this, _args, 1);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call(const A1& a1)
        {
            try
            {
                _a1.begin(a1, "", this->client().context());
                
                BasicComposer<R>& r = this->beginResult();
                
                this->client().call(r, *this, _args, 1);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()(const A1& a1)
        {
            return this->call(a1);
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();
            _a1.clear( &this->client().context() );
        }

    private:
        RemoteArgument<A1> _a1;
        Decomposer* _args[1]; 
};


template <typename R>
class RemoteProcedure<R,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin()
        {
            try
            {
                BasicComposer<R>& r = this->beginResult();
                
                this->client().beginCall(r, *this, 0, 0);
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
        }

        const R& call()
        {
            try
            {
                BasicComposer<R>& r = this->beginResult();
                
                this->client().call(r, *this, 0, 0);
                this->onReset();
            }
            catch(...)
            {
                this->onClear();
                throw;
            }
            
            return this->result().value();
        }

        const R& operator()()
        {
            return this->call();
        }

    protected:
        void onReset()
        {
            RemoteProcedureBase<R>::onReset();
        }
};

} // namespace Remoting

} // namespace Pt

#endif // PT_REMOTING_REMOTEPROCEDURE_TPP
