/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#include <Pt/Ssl/CipherStreamBuf.h>

#include <sstream>

namespace Pt {
namespace Ssl {

CipherStreamBuf::CipherStreamBuf()
: _ios(0), _cipher(0)
{}

CipherStreamBuf::CipherStreamBuf(std::iostream& ios, BasicCipher& cipher)
: _ios(0), _cipher(0)
{
    setIOStream(ios);
    setCipher(cipher);
}

CipherStreamBuf::~CipherStreamBuf()
{}

void CipherStreamBuf::setIOStream(std::iostream& ios)
{ _ios = &ios; }

void CipherStreamBuf::setCipher(BasicCipher& cipher)
{ _cipher = &cipher; }

std::streamsize CipherStreamBuf::import()
{
    const std::streamsize avail = _ios->rdbuf()->in_avail();
    if(!avail) return 0;

    if(underflow() == traits_type::eof()) return 0;

    return this->egptr() - this->gptr();
}

void CipherStreamBuf::finish()
{
}

CipherStreamBuf::int_type CipherStreamBuf::sync()
{
    return 0;
}

CipherStreamBuf::int_type CipherStreamBuf::underflow()
{
    return traits_type::eof();
}

CipherStreamBuf::int_type CipherStreamBuf::overflow(int_type ch)
{
    return traits_type::eof();
}

} // namespace Pt
} // namespace Ssl
