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
#ifndef PT_SSL_SSLCIPHERINFO_H
#define PT_SSL_SSLCIPHERINFO_H

//#undef NLOG
#define PT_SSL_LOGGER_CATEGORY "Pt.SSL.Logger"
#define PT_SSL_LOG_INFO(NAME, CODE) log_info(Pt::Ssl::SSLContext::_pt_ssl_gen_call_info(NAME, PT_FUNCTION) << CODE)

#include <Pt/Ssl/Exception.h>

// Forward declaration of some OpenSSL structures
struct ssl_ctx_st;
struct bio_st;
struct ssl_st;

namespace Pt {
namespace Ssl {

//! \brief Chipher information.
class PT_SSL_API SSLCipherInfo {
    public:
        unsigned long id;       //!< Numerical ID of the cipher.
        std::string   strid;    //!< Sring ID of the cipher.
        std::string   name;     //!< Name of the cipher.
        int           bits;     //!< Number of bits supported by the cipher.
        int           usedBits; //!< Number of bits actually used by the cipher.
        std::string   version;  //!< Version of the cipher.
        std::string   desc;     //!< Description of the cipher.

        //! \brief Convert the cipher information into a string.
        const std::string dump() const;

        friend class SSLContext;
        friend class SSLStreamBuf;

    private:
        inline SSLCipherInfo()
        : id(0), bits(0)
        {}

        inline SSLCipherInfo(unsigned long      id_,
                             const std::string& strid_,
                             const std::string& name_,
                             int                bits_,
                             const std::string& version_,
                             const std::string& desc_)
        : id(id_), strid(strid_), name(name_), bits(bits_), version(version_), desc(desc_)
        {}
};

inline bool operator==(const SSLCipherInfo& a, const SSLCipherInfo& b)
{
    return a.id == b.id;
}

} // namespace Pt
} // namespace Ssl

#endif
