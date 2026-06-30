/*
 * Copyright (C) 2006-2014 Marc Boris Duerner
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

#include <Pt/Utf8Codec.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

namespace System {

static TextCodec<Pt::Char, char>* getSystemCodec()
{
/*
    #include <langinfo.h>
    char *charset = nl_langinfo(CODESET);

    if "C" or empty check next of:
    LC_ALL -> LC_TYPE -> LANG

    parse codeset from
    lang.codeset@bbb

    match codeset against list, if empty match lang against list
*/

    // nullptr means UTF-8
    return 0;
}


static TextCodec<Pt::Char, char>* systemCodec()
{
    static TextCodec<Pt::Char, char>* _systemCodec = getSystemCodec();
    return _systemCodec;
}


static struct InitSystemCodec
{
    InitSystemCodec()
    { systemCodec(); }
} _initSystemCodec ;


inline void toLocalPath(const Pt::Char* from, std::size_t size, std::string& local)
{
    TextCodec<Pt::Char, char>* codec = systemCodec();

    Utf8Codec utf8Codec;
    if( ! codec )
        codec = &utf8Codec;;

    char to[32];
    char* toEnd = to + 32;
    const Pt::Char* fromEnd = from + size;
    MBState state;
    std::codecvt_base::result r;

    do
    {
        char* toNext = to;
        r = codec->out(state, from, fromEnd, from, to, toEnd, toNext);

        if( r == std::codecvt_base::error )
            local.append(5, '?');
        else
            local.append(to, toNext);
    }
    while(r == std::codecvt_base::partial);
}


inline void fromLocalPath(const char* from, std::size_t size, Pt::String& ustr)
{
    TextCodec<Pt::Char, char>* codec = systemCodec();

    Utf8Codec utf8Codec;
    if( ! codec )
        codec = &utf8Codec;;

    Pt::Char to[32];
    Pt::Char* toEnd = to + 32;
    const char* fromEnd = from + size;
    MBState state;
    std::codecvt_base::result r;

    do
    {
        Pt::Char* toNext = to;
        r = codec->in(state, from, fromEnd, from, to, toEnd, toNext);

        if (r == std::codecvt_base::error)
            ustr.append(5, '?');
        else
            ustr.append(to, toNext);
    }
    while(r == std::codecvt_base::partial);
}


class PathImpl
{
    public:
        typedef std::string::size_type size_type;

    public:
        PathImpl()
        { }

        void clear()
        { _path.clear(); }

        bool empty() const
        { return _path.empty(); }

        std::size_t size() const
        { return _path.size(); }

        const char* c_str() const
        { return _path.c_str(); }

        char front() const
        { return _path[0]; }

        char back() const
        { return _path[ _path.size()-1 ]; }

        void assign(const char* s)
        { _path = s; }

        void append(const PathImpl& p)
        {
            _path += p._path;
        }

        void append(const Pt::Char* s, std::size_t n)
        {
            toLocalPath(s, n, _path);
        }

        bool append(const char* s, std::size_t n)
        {
            TextCodec<Pt::Char, char>* codec = systemCodec();
            if( ! codec )
            {
                // no codec means local encoding is UTF-8
                _path.append(s, n);
            }

            return ! codec;
        }

        void push_back(char c)
        { _path.push_back(c); }

        static size_type npos()
        { return std::string::npos; }

        size_type rfind(char ch, size_type pos = npos()) const
        {
            return _path.rfind(ch, pos);
        }

        Pt::String substr(size_type pos, size_type n)
        {
            Pt::String str;
            fromLocalPath(_path.c_str() + pos, n, str);
            return str;
        }

        int compare(const PathImpl& p) const
        {
            return _path.compare(p._path);
        }

        Pt::String toString() const
        {
            Pt::String str;
            fromLocalPath(_path.c_str(), _path.size(), str);
            return str;
        }

        Pt::String toGeneric() const
        {
            return toString();
        }

        const std::string& toLocal() const
        {
            return _path;
        }

        static char dirsep()
        { return '/'; }

        static char extsep()
        { return '.'; }

    private:
        std::string _path;
};

} // namespace System

} // namespace Pt
