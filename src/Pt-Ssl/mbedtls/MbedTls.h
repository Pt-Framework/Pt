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

#ifndef PT_SSL_MBEDTLS_H
#define PT_SSL_MBEDTLS_H

#include <Pt/Ssl/Api.h>
#include <Pt/SmartPtr.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>

namespace Pt {

namespace Ssl {

class FreeMbedX509Crt
{
    protected:
        void destroy(mbedtls_x509_crt* ptr)
        {
            mbedtls_x509_crt_free(ptr);
            delete ptr;
        }
};

typedef Pt::AutoPtr<mbedtls_x509_crt, FreeMbedX509Crt> X509CrtAutoPtr;


class FreeMbedPk
{
    protected:
        void destroy(mbedtls_pk_context* ptr)
        {
            mbedtls_pk_free(ptr);
            delete ptr;
        }
};

typedef Pt::AutoPtr<mbedtls_pk_context, FreeMbedPk> PkAutoPtr;

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_MBEDTLS_H
