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

#include <vector>
#include <cstring>

#include <Pt/Singleton.h>
#include <Pt/Ssl/SSLContext.h>

namespace Pt {
namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
log_define("Pt.SSL.Logger");
#define PT_SSL_LOG(CODE) log_info(SSLContext::pt_ssl_gen_call_info("SSLContext  ", PT_FUNCTION) << CODE)

#ifndef NLOG
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
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChiperList : public Singleton<ChiperList> {
    public:
        struct CipherInfo {
            unsigned long id;
            std::string   strid;
            std::string   name;
            std::string   desc;

            inline CipherInfo(unsigned long id_, const std::string& strid_, const std::string& name_, const std::string& desc_)
            : id(id_), strid(strid_), name(name_), desc(desc_)
            {}
        };

        friend class Singleton<ChiperList>;
        
    private:
        ChiperList();
        void _getCiphers(std::vector<CipherInfo>& dst, const SSL_METHOD* sslMethod);
        
        BIO*                    _bioErr;
        std::vector<CipherInfo> _sslV2Chipers;
        std::vector<CipherInfo> _sslV3Chipers;
        std::vector<CipherInfo> _tlsV1Chipers;
};

ChiperList::ChiperList()
: _bioErr(0)
{
    // Initialize logger
    Pt::System::LogTarget::get("Pt.SSL.Logger").setChannel("console://");
    Pt::System::LogTarget::get("Pt.SSL.Logger").setLogLevel(Pt::System::Trace);

    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    _bioErr = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);

    // Get the list of ciphers for all the supported protocols
    _getCiphers(_sslV2Chipers, SSLv2_method());
    _getCiphers(_sslV3Chipers, SSLv3_method());
    _getCiphers(_tlsV1Chipers, TLSv1_method());

    std::cerr << "########## Supported SSLv2 Ciphers ##########" << std::endl;
    for(uint i = 0; i < _sslV2Chipers.size(); ++i) {
        std::cerr << _sslV2Chipers[i].id << " " << _sslV2Chipers[i].strid << " " << _sslV2Chipers[i].name << " | " << _sslV2Chipers[i].desc << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "########## Supported SSLv3 Ciphers ##########" << std::endl;
    for(uint i = 0; i < _sslV3Chipers.size(); ++i) {
        std::cerr << _sslV3Chipers[i].id << " " << _sslV3Chipers[i].strid << " " << _sslV3Chipers[i].name << " | " << _sslV3Chipers[i].desc << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "########## Supported TLSv1 Ciphers ##########" << std::endl;
    for(uint i = 0; i < _tlsV1Chipers.size(); ++i) {
        std::cerr << _tlsV1Chipers[i].id << " " << _tlsV1Chipers[i].strid << " " << _tlsV1Chipers[i].name << " | " << _tlsV1Chipers[i].desc << std::endl;
    }
    std::cerr << std::endl;
}

void ChiperList::_getCiphers(std::vector<CipherInfo>& dst, const SSL_METHOD* sslMethod)
{
    // Get the available ciphers
    SSL_CTX*              ctx = SSL_CTX_new(sslMethod);
    SSL*                  ssl = SSL_new(ctx);
    STACK_OF(SSL_CIPHER)* chp = SSL_get_ciphers(ssl);

    // Walk trough the ciphers
    for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i) {
        // Skip if not valid
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);
        if(!c->valid) continue;
        // Get the ID and split it
        const unsigned long id  = c->id;
        const int           id0 = (int) (  id >> 24);
        const int           id1 = (int) ( (id >> 16) & 0xFFL );
        const int           id2 = (int) ( (id >>  8) & 0xFFL );
        const int           id3 = (int) (  id        & 0xFFL );
        // Convert the ID to a readable string
        char strid[64];
             if((id & 0xFF000000L) == 0x02000000L) sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
        else if((id & 0xFF000000L) == 0x03000000L) sprintf(strid, "0x%02X,0x%02X", id2, id3);
        else                                       sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);
        // Get some information
        char desc[512];
        SSL_CIPHER_description(c, desc, sizeof(desc));
        const int dlen = strlen(desc);
        if(desc[dlen - 1] == '\n') desc[dlen - 1] = 0;
        // Store the chiper
        dst.push_back(CipherInfo(id, strid, c->name, desc));
    }

    // Clear all
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

static ChiperList& chiperList = ChiperList::instance();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    // Create a new SSL context that by default wants SSL version 3 but can fallback to SSL version 2
    _ctx = SSL_CTX_new(SSLv23_method());
    //_ctx = SSL_CTX_new(SSLv3_method());
    //_ctx = SSL_CTX_new(SSLv2_method());
    //_ctx = SSL_CTX_new(TLSv1_method());

    //int SSL_CTX_set_ssl_version(SSL_CTX *ctx, const SSL_METHOD *meth);
  
    // Load the certificate chain file (if available)
    if(certFile) {
        PT_SSL_LOG("Loading certificate chain file = " << certFile);
        if(!SSL_CTX_use_certificate_chain_file(_ctx, certFile))
            throw SSLRuntimeError("Could not read certificate file!", PT_SOURCEINFO);
    }
    
    // Load the private key  file (if available)
    if(keyFile) {
        PT_SSL_LOG("Loading private key file = " << keyFile);
        SSL_CTX_set_default_passwd_cb(_ctx, _passwordCallback);
        SSL_CTX_set_default_passwd_cb_userdata(_ctx, this);
        if(!SSL_CTX_use_PrivateKey_file(_ctx, keyFile, SSL_FILETYPE_PEM))
            throw SSLRuntimeError("Could not read key file!", PT_SOURCEINFO);
    }

    // Check the private key (if needed)
    if(certFile && keyFile) {
        if(!SSL_CTX_check_private_key(_ctx))
            throw SSLRuntimeError("The private key does not agree with the corresponding public key in the certificate!", PT_SOURCEINFO);
    }

    // Load and verify CA list (if available)
    if(caCertFile) {
        PT_SSL_LOG("Loading CA certificate list file = " << caCertFile);
        if(!SSL_CTX_load_verify_locations(_ctx, caCertFile, 0))
            throw SSLRuntimeError("Could not read/verify CA list!", PT_SOURCEINFO);
    }
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx, 1);
#endif


    // Set some options
    SSL_CTX_set_mode(_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
    SSL_CTX_set_options(_ctx, SSL_OP_SINGLE_DH_USE);
  //SSL_CTX_set_read_ahead(_ctx, 1);
    if(sessionID) SSL_CTX_set_session_id_context(_ctx, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));

    //SSL_SESSION *SSL_get1_session(SSL *ssl); /* obtain a reference count */
    //int SSL_set_session(SSL *to, SSL_SESSION *session);

    //int SSL_CTX_set_cipher_list(SSL_CTX *ctx, const char *str);

    // QUESTION: How to actually store the session data (SSL_SESSION*) to file???
}

SSLContext::~SSLContext()
{ SSL_CTX_free(_ctx); }

} // namespace Pt
} // namespace Ssl
