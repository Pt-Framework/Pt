/*
 * Copyright (C) 2009-2013 by Marc Duerner
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

#ifndef Pt_XmlRpc_BasicProcedure_h
#define Pt_XmlRpc_BasicProcedure_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/ServiceProcedure.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>
#include <Pt/TypeTraits.h>
#include <Pt/Void.h>

namespace Pt {

namespace XmlRpc {

class Responder;

//
// BasicProcedure with 10 arguments
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
class BasicProcedure : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _a7(&resp.context())
        , _a8(&resp.context())
        , _a9(&resp.context())
        , _a10(&resp.context())
        , _r(&resp.context())
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

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
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

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[11];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicComposer<V5> _a5;
        BasicComposer<V6> _a6;
        BasicComposer<V7> _a7;
        BasicComposer<V8> _a8;
        BasicComposer<V9> _a9;
        BasicComposer<V10> _a10;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 9 arguments
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
class BasicProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _a7(&resp.context())
        , _a8(&resp.context())
        , _a9(&resp.context())
        , _r(&resp.context())
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

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
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

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[10];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicComposer<V5> _a5;
        BasicComposer<V6> _a6;
        BasicComposer<V7> _a7;
        BasicComposer<V8> _a8;
        BasicComposer<V9> _a9;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 8 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7,
           typename A8>
class BasicProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _a7(&resp.context())
        , _a8(&resp.context())
        , _r(&resp.context())
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

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
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

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[9];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicComposer<V5> _a5;
        BasicComposer<V6> _a6;
        BasicComposer<V7> _a7;
        BasicComposer<V8> _a8;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 7 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7>
class BasicProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _a7(&resp.context())
        , _r(&resp.context())
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

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
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

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[8];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicComposer<V5> _a5;
        BasicComposer<V6> _a6;
        BasicComposer<V7> _a7;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 6 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6>
class BasicProcedure<R, A1, A2, A3, A4, A5, A6,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4, A5, A6>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _r(&resp.context())
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

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);

            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[7];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicComposer<V5> _a5;
        BasicComposer<V6> _a6;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 5 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class BasicProcedure<R, A1, A2, A3, A4, A5,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4, A5>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _r(&resp.context())
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = 0;
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);

            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[6];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicComposer<V5> _a5;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 4 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class BasicProcedure<R, A1, A2, A3, A4,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3, A4>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _r(&resp.context())
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = 0;
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);

            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[5];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicComposer<V4> _a4;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 3 arguments
template < typename R,
           typename A1,
           typename A2,
           typename A3>
class BasicProcedure<R, A1, A2, A3,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public ServiceProcedure
{
    public:
        BasicProcedure( const Callable<R, A1, A2, A3>& cb, Responder& resp )
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _r(&resp.context())
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = 0;
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);

            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[4];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicComposer<V3> _a3;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 2 arguments
template < typename R,
           typename A1,
           typename A2>
class BasicProcedure<R, A1, A2,
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
        BasicProcedure( const Callable<R, A1, A2>& cb, Responder& resp )
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _r(&resp.context())
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = 0;
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);

            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[3];
        BasicComposer<V1> _a1;
        BasicComposer<V2> _a2;
        BasicDecomposer<RV> _r;
};


// BasicProcedure with 1 arguments
template < typename R,
           typename A1>
class BasicProcedure<R, A1,
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
        BasicProcedure( const Callable<R, A1>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _a1(&resp.context())
        , _r(&resp.context())
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = 0;
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);

            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
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

        Composer* _args[2];
        BasicComposer<V1> _a1;
        BasicDecomposer<RV> _r;
};


//
// BasicProcedure with 0 arguments
//

template < typename R>
class BasicProcedure<R,
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
        BasicProcedure(const Callable<R>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(0)
        , _r(&resp.context())
        {
            _cb = cb.clone();

            _args[0] = 0;
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
        {
            _rv = _cb->call();
            _r.begin(_rv, "");
            return &_r;
        }

    private:
        typedef typename TypeTraits<R>::Value RV;

        Callable<R>* _cb;
        RV _rv;

        Composer* _args[1];
        BasicDecomposer<RV> _r;
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
        virtual ServiceProcedure* onCreateProcedure(Responder& resp) const
        { 
            return new BasicProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( *_cb, resp ); 
        }

    private:
        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>* _cb;
};

} // namespace XmlRpc

} // namespace Pt

#endif // PT_XMLRPC_SERVICEPROCEDURE_H

#include <Pt/XmlRpc/ActiveProcedure.h>
