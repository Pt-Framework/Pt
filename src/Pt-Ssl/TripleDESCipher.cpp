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

TripleDESCipher::TripleDESCipher(std::iostream& ios)
: BasicSymmetricCipher(ios)
{
}

TripleDESCipher::~TripleDESCipher()
{
}

const char* TripleDESCipher::getOpenSSLCipherName() const
{
    /*
        http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation

        Electronic Codebook Book (ECB)
            + The simplest of the encryption modes.
            + Message is broken into independent blocks which are encrypted.
            + Each block is a value which is substituted, like a codebook, hence the name.
            + Each block is encoded independently of the other blocks .
            + Uses: secure transmission of small data.
            + Provide little or no defense against modification
            + Identical plaintext blocks are encrypted to identical ciphertext blocks

        cipher-block chaining (CBC)
            + Message is broken into blocks
            + Each previous cipher blocks is chained with current plaintext block, hence the name.
            + Each block of plaintext is XOR-ed with the previous ciphertext block before being encrypted.
            + A change in the message affects all ciphertext blocks after the change as well as the original block.
            + Uses: secure transmission of bulk data.
            + An error in (or attack upon) one ciphertext block impacts two plaintext blocks upon decryption.

        cipher feedback (CFB)
        output feedback (OFB)
            + Make the block cipher into a self-synchronizing stream cipher.
            + Message is treated as a stream of bits.
            + Result is feed back for next stage, hence the name.
            + Uses: secure transmission of stream/bulk data.
            + Flipping a bit in the ciphertext produces a flipped bit in the plaintext at the same location.
            + Errors propagate for several blocks after the error.
    */

    //return "aes-128-cbc";
    //return "aes-128-cfb";
    //return "aes-128-cfb1";
    //return "aes-128-cfb8";
    //return "aes-128-ofb";
      
    //return "aes-192-cbc";
    //return "aes-192-cfb";
    //return "aes-192-cfb1";
    //return "aes-192-cfb8";
    //return "aes-192-ofb";
  
    //return "aes-256-cbc";
    //return "aes-256-cfb";
    //return "aes-256-cfb1";
    //return "aes-256-cfb8";
    //return "aes-256-ofb";

    return "des-ede3-cbc";
    //return "des-ede3-cfb";
    //return "des-ede3-cfb1"; // ERROR!
    //return "des-ede3-cfb8";
    //return "des-ede3-ofb";
    
}

} // namespace Pt
} // namespace Ssl
