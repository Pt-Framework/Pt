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

#include <Pt/Ssl/TripleDESCipher.h>

namespace Pt {
namespace Ssl {

TripleDESCipher::TripleDESCipher(std::iostream& ios, Mode mode)
: BasicSymmetricCipher(ios)
{ setMode(mode); }

TripleDESCipher::~TripleDESCipher()
{}

void TripleDESCipher::setMode(Mode mode)
{
    // Note: Actually OpenSSL support CFB1 in TripleDES,
    //       however it seems to always produce corrupted data.
    if(mode == ECB || mode == CFB1)
        throw SSLRuntimeError("Triple-DES cipher does not support the ECB and CFB1 modes!", PT_SOURCEINFO);

    _mode = mode;
}

const char* TripleDESCipher::getOpenSSLCipherName() const
{
    switch(_mode) {
        case CBC  : return "des-ede3-cbc";
        case CFB8 : return "des-ede3-cfb8";
        case CFB  : return "des-ede3-cfb";
        case OFB  : return "des-ede3-ofb";
        default   : throw SSLRuntimeError("Invalid mode of operation!", PT_SOURCEINFO);
    }
}

} // namespace Pt
} // namespace Ssl
