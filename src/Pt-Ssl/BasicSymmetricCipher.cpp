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

#include <Pt/Ssl/BasicSymmetricCipher.h>

#include "Utils.h"

#include <openssl/rand.h>

namespace Pt {
namespace Ssl {

BasicSymmetricCipher::BasicSymmetricCipher(const std::string& password, OperationMode operMode)
{
}

BasicSymmetricCipher::~BasicSymmetricCipher()
{
}

void BasicSymmetricCipher::setPassword(const std::string& password)
{
}

size_t BasicSymmetricCipher::saltLength() const
{
}

void BasicSymmetricCipher::setSalt(const std::string& salt)
{
}

const std::string& BasicSymmetricCipher::getSalt() const
{
}

void BasicSymmetricCipher::genSalt(SaltType saltType)
{
}

int BasicSymmetricCipher::encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next)
{
}

int BasicSymmetricCipher::decode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next)
{
}

} // namespace Pt
} // namespace Ssl
