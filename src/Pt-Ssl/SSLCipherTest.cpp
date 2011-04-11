/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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

#include <Pt/Ssl/SSLContext.h>
#include <Pt/Ssl/SSLCertificateList.h>
#include <Pt/Ssl/SSLPrivateKey.h>
#include <Pt/System/Logger.h>

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG_M(CODE) PT_SSL_LOG_INFO("@@ main() @@", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    try {
        PT_SSL_LOG_M("OpenSSL test progam started");

        Pt::Ssl::SSLCertificateList serverCertChain;
        serverCertChain.loadFromFile("server.pem");
        
        Pt::Ssl::SSLPrivateKeyPtr serverPrivKey(Pt::Ssl::SSLPrivateKey::newPrivateKey("password"));
        serverPrivKey->loadFromFile("server.key");

        Pt::Ssl::SSLPublicKeyPtr serverPubKey = serverCertChain.getPublicKey();
        
        PT_SSL_LOG_M("################################################################################");

        const std::string& text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vitae quam quis velit gravida vestibulum.";
        const std::string& tsig = serverPrivKey->signString(text);
        PT_SSL_LOG_M("\n\nSIGNING TEXT:\n" << text << "\n\nRESULTING SIGNATURE:\n" << tsig << "\n\n");

        PT_SSL_LOG_M("################################################################################");

        PT_SSL_LOG_M("OpenSSL test progam ended");
        return 0;
    }
    catch(const std::exception& ex)
    {
        PT_SSL_LOG_M("Error: " << ex.what());
    }
    catch(const char* ex)
    {
        PT_SSL_LOG_M("Error: " << ex);
    }
    return 1;
}
