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

#include <iostream>

#include <Pt/SourceInfo.h>
#include <Pt/Ssl/SSLContext.h>

namespace Pt {
namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PT_SSL_DEBUG
Pt::System::Logger& SSLContext::pt_ssl_logger()
{
    static Pt::System::Logger logger("Pt.SSL.Logger");
    return logger;
}
const std::string SSLContext::pt_ssl_gen_call_info(const char* className, const std::string& funcName)
{
    static int count = 0;

    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);
    a = f.find_last_of(" ");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, "[%s] %06d [%22s] ", className, count++, f.c_str());

    return buff;
}
#define PT_SSL_LOG(CODE) SSLContext::pt_ssl_logger().info() << SSLContext::pt_ssl_gen_call_info("SSLContext  ", PT_FUNCTION) << CODE << Pt::System::endlog
#else
#define PT_SSL_LOG(CODE)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BIO* SSLContext::_bioErr = 0;

int SSLContext::_passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata)
{
    // Get the SSLContext instance
    SSLContext& sslCtx = *reinterpret_cast<SSLContext*>(userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if((unsigned) num < sslCtx._pswd.length() + 1) return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &sslCtx._pswd[0]);
    return sslCtx._pswd.length();
}

SSLContext::SSLContext(const char* caCertFile, const char* certFile, const char* keyFile, const char* password, const char* sessionID)
: _pswd(password ? password : "")
{
    // Only need to perform these once for every application
    if(!SSLContext::_bioErr) {
        SSL_library_init();
        SSL_load_error_strings();
        SSLContext::_bioErr = BIO_new_fp(stderr, BIO_NOCLOSE);
#ifdef PT_SSL_DEBUG
        // Initialize logger
        Pt::System::LogTarget::get("Pt.SSL.Logger").setChannel("console://");
        Pt::System::LogTarget::get("Pt.SSL.Logger").setLogLevel(Pt::System::Trace);
#endif        
    }

    // Create a new SSL context that by default wants SSL version 3 but can fallback to SSL version 2
    _ctx = SSL_CTX_new(SSLv23_method());
    //_ctx = SSL_CTX_new(SSLv3_method());
    //_ctx = SSL_CTX_new(SSLv2_method());
    //_ctx = SSL_CTX_new(TLSv1_method());
  
    // Load the certificate chain file (if available)
    if(certFile) {
        PT_SSL_LOG("Loading certificate chain file = " << certFile);
        if(!SSL_CTX_use_certificate_chain_file(_ctx, certFile)) throw "Could not read certificate file!";
    }

    // Load the private key  file (if available)
    if(keyFile) {
        PT_SSL_LOG("Loading private key file = " << keyFile);
        SSL_CTX_set_default_passwd_cb(_ctx, _passwordCallback);
        SSL_CTX_set_default_passwd_cb_userdata(_ctx, this);
        if(!SSL_CTX_use_PrivateKey_file(_ctx, keyFile, SSL_FILETYPE_PEM)) throw "Could not read key file!";
    }

    // Check the private key (if needed)
    if(certFile && keyFile) {
        if(!SSL_CTX_check_private_key(_ctx)) throw "The private key does not agree with the corresponding public key in the certificate!";
    }

    // Load and verify CA list (if available)
    if(caCertFile) {
        PT_SSL_LOG("Loading CA certificate list file = " << caCertFile);
        if(!SSL_CTX_load_verify_locations(_ctx, caCertFile, 0)) throw "Could not read/verify CA list!";
    }
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx,1);
#endif

    // Set some options
    SSL_CTX_set_mode(_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
    SSL_CTX_set_options(_ctx, SSL_OP_SINGLE_DH_USE);
    if(sessionID) SSL_CTX_set_session_id_context(_ctx, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));
}

SSLContext::~SSLContext()
{ SSL_CTX_free(_ctx); }

} // namespace Pt
} // namespace Ssl
