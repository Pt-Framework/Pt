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

#ifndef PT_XMLRPC_SERVICE_TPP
#define PT_XMLRPC_SERVICE_TPP

namespace Pt {

namespace XmlRpc {

// BasicServiceProcedure with 10 arguments
template < typename R,
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
class BasicServiceProcedure : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _a5(ctx)
        , _a6(ctx)
        , _a7(ctx)
        , _a8(ctx)
        , _a9(ctx)
        , _a10(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = &_a8;
            _args[8] = &_a9;
            _args[9] = &_a10;
            _args[10] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);
            _a8.begin(_v8);
            _a9.begin(_v9);
            _a10.begin(_v10);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9, _v10);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<A8>::Value V8;
        typedef typename TypeTraits<A9>::Value V9;
        typedef typename TypeTraits<A10>::Value V10;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;
        V8 _v8;
        V9 _v9;
        V10 _v10;

        IComposer* _args[11];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Composer<V8> _a8;
        Composer<V9> _a9;
        Composer<V10> _a10;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 9 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7,
           typename A8,
           typename A9>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _a5(ctx)
        , _a6(ctx)
        , _a7(ctx)
        , _a8(ctx)
        , _a9(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = &_a8;
            _args[8] = &_a9;
            _args[9] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);
            _a8.begin(_v8);
            _a9.begin(_v9);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<A8>::Value V8;
        typedef typename TypeTraits<A9>::Value V9;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;
        V8 _v8;
        V9 _v9;

        IComposer* _args[10];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Composer<V8> _a8;
        Composer<V9> _a9;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 8 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7,
           typename A8>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _a5(ctx)
        , _a6(ctx)
        , _a7(ctx)
        , _a8(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = &_a8;
            _args[8] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);
            _a8.begin(_v8);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<A8>::Value V8;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;
        V8 _v8;

        IComposer* _args[9];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Composer<V8> _a8;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 7 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _a5(ctx)
        , _a6(ctx)
        , _a7(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;

        IComposer* _args[8];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 6 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _a5(ctx)
        , _a6(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;

        IComposer* _args[7];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 5 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _a5(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;

        IComposer* _args[6];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 4 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class BasicServiceProcedure<R, A1, A2, A3, A4,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _a4(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;

        IComposer* _args[5];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 3 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3>
class BasicServiceProcedure<R, A1, A2, A3,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2, A3>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _a3(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;

        IComposer* _args[4];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 2 arguments
template < typename R,
           typename A1,
           typename A2>
class BasicServiceProcedure<R, A1, A2,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1, A2>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _a2(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;

        IComposer* _args[3];
        Composer<V1> _a1;
        Composer<V2> _a2;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 1 arguments
template < typename R,
           typename A1>
class BasicServiceProcedure<R, A1,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, A1>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _a1(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1);
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;

        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1>* _cb;
        RV _rv;
        V1 _v1;

        IComposer* _args[2];
        Composer<V1> _a1;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 0 arguments
template < typename R>
class BasicServiceProcedure<R,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R>& cb, SerializationContext* ctx = 0 )
        : ServiceProcedure()
        , _cb(0)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new BasicServiceProcedure(*_cb, ctx);
        }

        IComposer** beginCall()
        {

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call();
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<R>::Value RV;

        Callable<R>* _cb;
        RV _rv;

        IComposer* _args[1];
        Decomposer<RV> _r;
};


template <typename R, typename A1>
class AsyncServiceProcedure : public ServiceProcedure
{
    public:
        AsyncServiceProcedure(const Callable<R, A1>& cb, SerializationContext* ctx = 0)
        : ServiceProcedure()
        , _a1(ctx)
        , _r(ctx)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = 0;
        }

        ~AsyncServiceProcedure()
        { }

        ServiceProcedure* clone(SerializationContext* ctx) const
        {
            return new AsyncServiceProcedure(ctx);
        }

        IComposer** beginCall()
        {
            _a1.begin(_v1);
            return _args;
        }

        IDecomposer* endCall() // TODO: pass EventLoop
        {
            this->endExec(_rv);
            _r.begin(_rv, "");
            return &_r;
        }

        void beginAsync(System::EventLoop& loop)
        {
            this->exec(_a1);
        }

        IDecomposer* endAsync()
        {
            this->endExec(_rv);
            _r.begin(_rv, "");
            return &_r;
        }

        virtual void exec(A1 a1)
        { this->responder().replyFinished(0); }

        virtual void endExec(R& r) = 0;

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R>* _cb;
        RV _rv;
        V1 _v1;

        IComposer* _args[2];
        Composer<V1> _a1;
        Decomposer<RV> _r;
};


class PT_XMLRPC_API Service : public Http::Service
{
        friend class XmlRpcResponder;

    public:
        Service()
        { }

        virtual ~Service();

        template <typename R>
        void registerFunction(const std::string& name, R (*fn)())
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1>
        void registerFunction(const std::string& name, R (*fn)(A1))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2>
        void registerFunction(const std::string& name, R (*fn)(A1, A2))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R>
        void registerCallable(const std::string& name, const Callable<R>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1>
        void registerCallable(const std::string& name, const Callable<R, A1>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2>
        void registerCallable(const std::string& name, const Callable<R, A1, A2>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, class C>
        void registerMethod(const std::string& name, C& obj, R (C::*method)() )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

    protected:
        virtual Http::Responder* createResponder(const Http::Request&);

        virtual void releaseResponder(Http::Responder* resp);

        ServiceProcedure* getProcedure(const std::string& name, SerializationContext* ctx);

        void releaseProcedure(ServiceProcedure* proc);

        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};

}

}

#endif // PT_XMLRPC_SERVICE_TPP
