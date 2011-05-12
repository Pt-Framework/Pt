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

#include <Pt/Ssl/AESCipher.h>

namespace Pt {
namespace Ssl {

AESCipher::AESCipher(std::iostream& ios, KeySize keySize, Mode mode)
: BasicSymmetricCipher(ios)
{
    setKeySize(keySize);
    setMode(mode);
}

AESCipher::~AESCipher()
{}

void AESCipher::setKeySize(KeySize keySize)
{ _keySize = keySize; }

void AESCipher::setMode(Mode mode)
{ _mode = mode;  }

const char* AESCipher::getOpenSSLCipherName() const
{
    switch(_keySize) {
        case K128 :
            switch(_mode) {
                case ECB  : return "aes-128-ecb";
                case CBC  : return "aes-128-cbc";
                case CFB1 : return "aes-128-cfb1";
                case CFB8 : return "aes-128-cfb8";
                case CFB  : return "aes-128-cfb";
                case OFB  : return "aes-128-ofb";
                default   : throw SSLRuntimeError("Invalid mode of operation!", PT_SOURCEINFO);
            }
            break;

        case K192 :
            switch(_mode) {
                case ECB  : return "aes-192-ecb";
                case CBC  : return "aes-192-cbc";
                case CFB1 : return "aes-192-cfb1";
                case CFB8 : return "aes-192-cfb8";
                case CFB  : return "aes-192-cfb";
                case OFB  : return "aes-192-ofb";
                default   : throw SSLRuntimeError("Invalid mode of operation!", PT_SOURCEINFO);
            }
            break;

        case K256 :
            switch(_mode) {
                case ECB  : return "aes-256-ecb";
                case CBC  : return "aes-256-cbc";
                case CFB1 : return "aes-256-cfb1";
                case CFB8 : return "aes-256-cfb8";
                case CFB  : return "aes-256-cfb";
                case OFB  : return "aes-256-ofb";
                default   : throw SSLRuntimeError("Invalid cipher mode!", PT_SOURCEINFO);
            }
            break;

        default:
            throw SSLRuntimeError("Invalid key size!", PT_SOURCEINFO);
    }
}

} // namespace Pt
} // namespace Ssl
