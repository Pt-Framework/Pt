/*
 * Copyright (C) 2026 by Marc Boris Duerner
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

#ifndef PT_SSL_PKCS12PARSER_H
#define PT_SSL_PKCS12PARSER_H

#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <cstddef>
#include <vector>

namespace Pt {

namespace Ssl {

// Parse a PKCS#12 PFX (DER-encoded) and extract the private key, leaf
// certificate and CA chain. On success the caller owns all output objects
// and must free them using the matching mbedtls_*_free + delete sequence.
// On failure all out parameters are left as NULL / empty.
//
// Returns true on success, false on any parse or decryption error.
bool parsePkcs12(const unsigned char*            data,
                 std::size_t                     len,
                 const char*                     passwd,
                 mbedtls_pk_context**            pkey,
                 mbedtls_x509_crt**              cert,
                 std::vector<mbedtls_x509_crt*>& ca);

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_PKCS12PARSER_H
