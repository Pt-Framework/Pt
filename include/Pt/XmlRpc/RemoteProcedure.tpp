/*
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PT_XMLRPC_REMOTEPROCEDURE_TPP
#define PT_XMLRPC_REMOTEPROCEDURE_TPP

namespace Pt {

namespace XmlRpc {

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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        , _a5( & client.context() )
        , _a6( & client.context() )
        , _a7( & client.context() )
        , _a8( & client.context() )
        , _a9( & client.context() )
        , _a10( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            _a8.begin(a8, "");
            _a9.begin(a9, "");
            _a10.begin(a10, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[10] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9, &_a10 };
            this->client().beginCall(this->_r, *this, argv, 10);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            _a8.begin(a8, "");
            _a9.begin(a9, "");
            _a10.begin(a10, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[10] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9, &_a10 };
            this->client().call(this->_r, *this, argv, 10);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
        Decomposer<A8> _a8;
        Decomposer<A9> _a9;
        Decomposer<A10> _a10;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        , _a5( & client.context() )
        , _a6( & client.context() )
        , _a7( & client.context() )
        , _a8( & client.context() )
        , _a9( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            _a8.begin(a8, "");
            _a9.begin(a9, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[9] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9 };
            this->client().beginCall(this->_r, *this, argv, 9);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            _a8.begin(a8, "");
            _a9.begin(a9, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[9] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9 };
            this->client().call(this->_r, *this, argv, 9);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
        Decomposer<A8> _a8;
        Decomposer<A9> _a9;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        , _a5( & client.context() )
        , _a6( & client.context() )
        , _a7( & client.context() )
        , _a8( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            _a8.begin(a8, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[8] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8 };
            this->client().beginCall(this->_r, *this, argv, 8);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            _a8.begin(a8, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[8] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8 };
            this->client().call(this->_r, *this, argv, 8);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
        Decomposer<A8> _a8;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        , _a5( & client.context() )
        , _a6( & client.context() )
        , _a7( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[7] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7 };
            this->client().beginCall(this->_r, *this, argv, 7);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            _a7.begin(a7, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[7] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7 };
            this->client().call(this->_r, *this, argv, 7);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        , _a5( & client.context() )
        , _a6( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[6] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6 };
            this->client().beginCall(this->_r, *this, argv, 6);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            _a6.begin(a6, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[6] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6 };
            this->client().call(this->_r, *this, argv, 6);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            return this->call(a1, a2, a3, a4, a5, a6);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        , _a5( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[5] = { &_a1, &_a2, &_a3, &_a4, &_a5 };
            this->client().beginCall(this->_r, *this, argv, 5);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            _a5.begin(a5, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[5] = { &_a1, &_a2, &_a3, &_a4, &_a5 };
            this->client().call(this->_r, *this, argv, 5);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            return this->call(a1, a2, a3, a4, a5);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        , _a4( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[4] = { &_a1, &_a2, &_a3, &_a4 };
            this->client().beginCall(this->_r, *this, argv, 4);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            _a4.begin(a4, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[4] = { &_a1, &_a2, &_a3, &_a4 };
            this->client().call(this->_r, *this, argv, 4);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            return this->call(a1, a2, a3, a4);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        , _a3( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[3] = { &_a1, &_a2, &_a3 };
            this->client().beginCall(this->_r, *this, argv, 3);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            _a3.begin(a3, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[3] = { &_a1, &_a2, &_a3 };
            this->client().call(this->_r, *this, argv, 3);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3)
        {
            return this->call(a1, a2, a3);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
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
        , _a1( & client.context() )
        , _a2( & client.context() )
        { }

        void begin(const A1& a1, const A2& a2)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[2] = { &_a1, &_a2 };
            this->client().beginCall(this->_r, *this, argv, 2);
        }

        const R& call(const A1& a1, const A2& a2)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            _a2.begin(a2, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[2] = { &_a1, &_a2 };
            this->client().call(this->_r, *this, argv, 2);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2)
        {
            return this->call(a1, a2);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
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
        , _a1( & client.context() )
        { }

        void begin(const A1& a1)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");

            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { &_a1 };
            this->client().beginCall(this->_r, *this, argv, 1);
        }

        const R& call(const A1& a1)
        {
            this->_result.clearFault();

            _a1.begin(a1, "");
            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { &_a1 };
            this->client().call(this->_r, *this, argv, 1);
            return this->_result.get();
        }

        const R& operator()(const A1& a1)
        {
            return this->call(a1);
        }

    private:
        Decomposer<A1> _a1;
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
            this->_result.clearFault();

            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { 0 };
            this->client().beginCall(this->_r, *this, argv, 0);
        }

        const R& call()
        {
            this->_result.clearFault();

            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { 0 };
            this->client().call(this->_r, *this, argv, 0);
            return this->_result.get();
        }

        const R& operator()()
        {
            return this->call();
        }
};

}

}

#endif // PT_XMLRPC_REMOTEPROCEDURE_TPP
