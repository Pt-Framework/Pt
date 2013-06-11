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

//
// BasicServiceProcedure with 10 arguments
//
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _a5(&ctx.sctx())
        , _a6(&ctx.sctx())
        , _a7(&ctx.sctx())
        , _a8(&ctx.sctx())
        , _a9(&ctx.sctx())
        , _a10(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
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

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _a5(&ctx.sctx())
        , _a6(&ctx.sctx())
        , _a7(&ctx.sctx())
        , _a8(&ctx.sctx())
        , _a9(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
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

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _a5(&ctx.sctx())
        , _a6(&ctx.sctx())
        , _a7(&ctx.sctx())
        , _a8(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
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

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _a5(&ctx.sctx())
        , _a6(&ctx.sctx())
        , _a7(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
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

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5, A6>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _a5(&ctx.sctx())
        , _a6(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);

            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4, A5>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _a5(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);

            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3, A4>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _a4(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);

            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2, A3>& cb, Context& ctx )
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _a3(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);

            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1, A2>& cb, Context& ctx )
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _a2(&ctx.sctx())
        , _r(&ctx.sctx())
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

        IComposer** beginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);

            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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
        BasicServiceProcedure( const Callable<R, A1>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _a1(&ctx.sctx())
        , _r(&ctx.sctx())
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        IComposer** beginArgs()
        {
            _a1.begin(_v1);

            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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


//
// BasicServiceProcedure with 0 arguments
//

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
        BasicServiceProcedure(const Callable<R>& cb, Context& ctx)
        : ServiceProcedure(ctx)
        , _cb(0)
        , _r(&ctx.sctx())
        {
            _cb = cb.clone();

            _args[0] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        IComposer** beginArgs()
        {
            return _args;
        }

        virtual void beginCall()
        {
            this->setReady();
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


//
// AsyncServiceProcedure with 1 arguments
//

template <typename R, 
          typename A1 = Pt::Void>
class AsyncProcedure : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;

    public:
        AsyncProcedure(Context& ctx)
        : ServiceProcedure(ctx)
        , _a1(&ctx.sctx())
        , _r(&ctx.sctx())
        {
            _args[0] = &_a1;
            _args[1] = 0;
        }

        IComposer** beginArgs()
        {
            _a1.begin(_v1);
            return _args;
        }

        virtual void beginCall()
        {
            onBeginCall(_v1);
        }

        IDecomposer* endCall()
        {
            const R& r = onEndCall();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onBeginCall(const A1& a1) = 0;

        virtual const R& onEndCall() = 0;

    private:
        A1 _v1;
        IComposer* _args[2];
        Composer<A1> _a1;
        Decomposer<R> _r;
};


//
// AsyncServiceProcedure with 0 arguments
//

template <typename R>
class AsyncProcedure<R, 
                     Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef Pt::Void Arg1T;

    public:
        AsyncProcedure(Context& ctx)
        : ServiceProcedure(ctx)
        , _r( &ctx.sctx() )
        {
            _args[0] = 0;
        }

        IComposer** beginArgs()
        {
            return _args;
        }

        virtual void beginCall()
        {
            onBeginCall();
        }

        IDecomposer* endCall()
        {
            const R& r = onEndCall();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onBeginCall() = 0;

        virtual const R& onEndCall() = 0;

    private:
        IComposer* _args[1];
        Decomposer<R> _r;
};


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
           typename A10 = Pt::Void >
class BasicProcedureDef : public ServiceProcedureDef
{
    public:
        BasicProcedureDef(const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
        : _cb(0)
        {
            _cb = cb.clone();
        }

        ~BasicProcedureDef()
        {
            delete _cb;
        }

    protected:
        virtual ServiceProcedure* onCreateProcedure(Context& ctx) const
        { 
            return new BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( *_cb, ctx ); 
        }

    private:
        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>* _cb;
};


template <typename CallT>
class AsyncProcedureDef : public ServiceProcedureDef
{
    public:
        AsyncProcedureDef(const Callable<CallT*, Context&>& cb)
        : _cb(0)
        {
            _cb = cb.clone();
        }

        ~AsyncProcedureDef()
        {
            delete _cb;
        }

    protected:
        virtual ServiceProcedure* onCreateProcedure(Context& ctx) const
        {
            typedef typename CallT::ReturnT R;
            typedef typename CallT::Arg1T A1;

            AsyncProcedure<R, A1>* proc = _cb->call(ctx);
            return proc;
        }

    private:
        const Callable<CallT*, Context&>* _cb;
};

}

}

#endif // PT_XMLRPC_SERVICE_TPP
