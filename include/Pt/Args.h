// Copyright (C) 2000-2004 Marc Boris Drner <marcd __at arklinux __dot org>
// All rights reserved.
#ifndef Pt_Args_h
#define Pt_Args_h

#include <Pt/Any.h>
#include <Pt/Exception.h>
#include <vector>


//! \addtogroup Pt
namespace Pt {

class Args
{
    public:
        Args()
        {}

        template <typename A>
        Args(A a)
        {
            _args.push_back( Any(a) );
        }

        template <typename A, typename B>
        Args(A a, B b)
        {
            _args.push_back( Any(a) );
            _args.push_back( Any(b) );
        }

        template <typename A, typename B, typename C>
        Args(A a, B b, C c)
        {
            _args.push_back( Any(a) );
            _args.push_back( Any(b) );
            _args.push_back( Any(c) );
        }

        virtual ~Args()
        {
            //_args.push_back( Any(a) );
        }

        const Any& get(size_t n) const
        {
            if( n >= _args.size() )
                throw LogicError("No such argument", PT_SOURCEINFO);

            return _args[n];
        }

        template <typename T>
        void push_back(T value)
        {
            _args.push_back( Any(value) );
        }

    private:
        std::vector<Any> _args;
};


} // namespace Pt

#endif
