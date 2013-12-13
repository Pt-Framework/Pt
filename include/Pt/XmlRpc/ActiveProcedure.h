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

#ifndef PT_XMLRPC_ACTIVEPROCEDURE_H
#define PT_XMLRPC_ACTIVEPROCEDURE_H

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/ServiceProcedure.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>
#include <Pt/Void.h>

namespace Pt {

namespace XmlRpc {

class Responder;

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
class ActiveProcedure : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef A5 Arg5T;
        typedef A6 Arg6T;
        typedef A7 Arg7T;
        typedef A8 Arg8T;
        typedef A9 Arg9T;
        typedef A10 Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9, _v10);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4,
                                 const A5& a5, const A6& a6,
                                 const A7& a7, const A8& a8,
                                 const A9& a9, const A10& a10) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;
        A5 _v5;
        A6 _v6;
        A7 _v7;
        A8 _v8;
        A9 _v9;
        A10 _v10;

        Composer* _args[11];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicComposer<A5> _a5;
        BasicComposer<A6> _a6;
        BasicComposer<A7> _a7;
        BasicComposer<A8> _a8;
        BasicComposer<A9> _a9;
        BasicComposer<A10> _a10;
        BasicDecomposer<R> _r;
};


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
class ActiveProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, 
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef A5 Arg5T;
        typedef A6 Arg6T;
        typedef A7 Arg7T;
        typedef A8 Arg8T;
        typedef A9 Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4,
                                 const A5& a5, const A6& a6,
                                 const A7& a7, const A8& a8,
                                 const A9& a9) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;
        A5 _v5;
        A6 _v6;
        A7 _v7;
        A8 _v8;
        A9 _v9;

        Composer* _args[10];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicComposer<A5> _a5;
        BasicComposer<A6> _a6;
        BasicComposer<A7> _a7;
        BasicComposer<A8> _a8;
        BasicComposer<A9> _a9;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7,
           typename A8>
class ActiveProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, 
                      Pt::Void,
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef A5 Arg5T;
        typedef A6 Arg6T;
        typedef A7 Arg7T;
        typedef A8 Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4,
                                 const A5& a5, const A6& a6,
                                 const A7& a7, const A8& a8) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;
        A5 _v5;
        A6 _v6;
        A7 _v7;
        A8 _v8;

        Composer* _args[9];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicComposer<A5> _a5;
        BasicComposer<A6> _a6;
        BasicComposer<A7> _a7;
        BasicComposer<A8> _a8;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7>
class ActiveProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef A5 Arg5T;
        typedef A6 Arg6T;
        typedef A7 Arg7T;
        typedef Void Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _a7(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = 0;
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4, _v5, _v6, _v7);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4,
                                 const A5& a5, const A6& a6,
                                 const A7& a7) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;
        A5 _v5;
        A6 _v6;
        A7 _v7;

        Composer* _args[8];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicComposer<A5> _a5;
        BasicComposer<A6> _a6;
        BasicComposer<A7> _a7;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6>
class ActiveProcedure<R, A1, A2, A3, A4, A5, A6,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef A5 Arg5T;
        typedef A6 Arg6T;
        typedef Void Arg7T;
        typedef Void Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _a6(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = 0;
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4, _v5, _v6);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4,
                                 const A5& a5, const A6& a6) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;
        A5 _v5;
        A6 _v6;

        Composer* _args[7];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicComposer<A5> _a5;
        BasicComposer<A6> _a6;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class ActiveProcedure<R, A1, A2, A3, A4, A5,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef A5 Arg5T;
        typedef Void Arg6T;
        typedef Void Arg7T;
        typedef Void Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _a5(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = 0;
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4, _v5);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4,
                                 const A5& a5) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;
        A5 _v5;

        Composer* _args[6];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicComposer<A5> _a5;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class ActiveProcedure<R, A1, A2, A3, A4,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef A4 Arg4T;
        typedef Void Arg5T;
        typedef Void Arg6T;
        typedef Void Arg7T;
        typedef Void Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _a4(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = 0;
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

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3, _v4);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;
        A4 _v4;

        Composer* _args[5];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicComposer<A4> _a4;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2,
           typename A3>
class ActiveProcedure<R, A1, A2, A3,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void,
                      Pt::Void> : public ServiceProcedure
{
    public:
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef A3 Arg3T;
        typedef Void Arg4T;
        typedef Void Arg5T;
        typedef Void Arg6T;
        typedef Void Arg7T;
        typedef Void Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _a3(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = 0;

        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            
            return _args;
        }

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2, _v3);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2,
                                 const A3& a3) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;
        A3 _v3;

        Composer* _args[4];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicComposer<A3> _a3;
        BasicDecomposer<R> _r;
};


template < typename R,
           typename A1,
           typename A2>
class ActiveProcedure<R, A1, A2,
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
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef A2 Arg2T;
        typedef Void Arg3T;
        typedef Void Arg4T;
        typedef Void Arg5T;
        typedef Void Arg6T;
        typedef Void Arg7T;
        typedef Void Arg8T;
        typedef Void Arg9T;
        typedef Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _a2(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = 0;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            
            return _args;
        }

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1, _v2);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop,
                                 const A1& a1, const A2& a2) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        A2 _v2;

        Composer* _args[3];
        BasicComposer<A1> _a1;
        BasicComposer<A2> _a2;
        BasicDecomposer<R> _r;
};


template <typename R, 
          typename A1>
class ActiveProcedure<R, A1,
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
        typedef R ReturnT;
        typedef A1 Arg1T;
        typedef Pt::Void Arg2T;
        typedef Pt::Void Arg3T;
        typedef Pt::Void Arg4T;
        typedef Pt::Void Arg5T;
        typedef Pt::Void Arg6T;
        typedef Pt::Void Arg7T;
        typedef Pt::Void Arg8T;
        typedef Pt::Void Arg9T;
        typedef Pt::Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _a1(&resp.context())
        , _r(&resp.context())
        {
            _args[0] = &_a1;
            _args[1] = 0;
        }

    protected:
        Composer** onBeginArgs()
        {
            _a1.begin(_v1);
            return _args;
        }

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop, _v1);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop, const A1& a1) = 0;

        virtual const R& onResult() = 0;

    private:
        A1 _v1;
        Composer* _args[2];
        BasicComposer<A1> _a1;
        BasicDecomposer<R> _r;
};


template <typename R>
class ActiveProcedure<R, 
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
        typedef R ReturnT;
        typedef Pt::Void Arg1T;
        typedef Pt::Void Arg2T;
        typedef Pt::Void Arg3T;
        typedef Pt::Void Arg4T;
        typedef Pt::Void Arg5T;
        typedef Pt::Void Arg6T;
        typedef Pt::Void Arg7T;
        typedef Pt::Void Arg8T;
        typedef Pt::Void Arg9T;
        typedef Pt::Void Arg10T;

    public:
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _r( &resp.context() )
        {
            _args[0] = 0;
        }

    protected:
        Composer** onBeginArgs()
        {
            return _args;
        }

        virtual void onBeginCall(System::EventLoop& loop)
        {
            onInvoke(loop);
        }

        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        virtual void onInvoke(System::EventLoop& loop) = 0;

        virtual const R& onResult() = 0;

    private:
        Composer* _args[1];
        BasicDecomposer<R> _r;
};


template <typename CallT>
class ActiveProcedureDef : public ServiceProcedureDef
{
    public:
        ActiveProcedureDef(const Callable<CallT*, Responder&>& cb)
        : _cb(0)
        {
            _cb = cb.clone();
        }

        ~ActiveProcedureDef()
        {
            delete _cb;
        }

    protected:
        virtual ServiceProcedure* onCreateProcedure(Responder& resp) const
        {
            typedef typename CallT::ReturnT R;
            typedef typename CallT::Arg1T A1;
            typedef typename CallT::Arg2T A2;
            typedef typename CallT::Arg3T A3;
            typedef typename CallT::Arg4T A4;
            typedef typename CallT::Arg5T A5;
            typedef typename CallT::Arg6T A6;
            typedef typename CallT::Arg7T A7;
            typedef typename CallT::Arg8T A8;
            typedef typename CallT::Arg9T A9;
            typedef typename CallT::Arg10T A10;

            ActiveProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>* proc = _cb->call(resp);
            return proc;
        }

    private:
        const Callable<CallT*, Responder&>* _cb;
};

} // namespace XmlRpc

} // namespace Pt

#endif // PT_XMLRPC_ACTIVEPROCEDURE_H
