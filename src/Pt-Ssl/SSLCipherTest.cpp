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
        // Start the test
        PT_SSL_LOG_M("OpenSSL test progam started");
        PT_SSL_LOG_M("################################################################################");

        // Load certificate and private key
        Pt::Ssl::SSLCertificateList serverCertChain;
        serverCertChain.loadFromFile("server.pem");
        
        Pt::Ssl::SSLPrivateKey serverPrivKey("password");
        serverPrivKey.loadFromFile("server.key");

        Pt::Ssl::SSLPublicKey serverPubKey = serverCertChain.getPublicKey();
        

        const std::string text  = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vitae quam quis velit gravida vestibulum.";
        const std::string text2 = text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text;

        // Verification test
        const std::string& tsig = serverPrivKey.signString(text);
        const bool         vres = serverPubKey.verifyStringSignature(text, tsig);
        PT_SSL_LOG_M("\n\n##### SIGNING TEXT #####"
                     << "\n\nInput text:\n" << text
                     << "\n\nResulting signature:\n" << tsig
                     << "\n\nVerification result:\n" << (vres ? "OK" : "FAILED") << "\n");

        // Start encryption test #1
        std::string tenc1;
        serverPubKey.beginEncryptString(Pt::Ssl::SSLPublicKey::RSA_PKCS1);
        tenc1 += serverPubKey.tryEncryptString(text);
        tenc1 += serverPubKey.tryEncryptString(" ");

        // Start encryption test #2
        Pt::Ssl::SSLPublicKey serverPubKey2(serverPubKey);
        std::string           tenc2;
        serverPubKey2.beginEncryptString(Pt::Ssl::SSLPublicKey::RSA_PKCS1_OAEP);
        tenc2 += serverPubKey2.tryEncryptString(text);
        tenc2 += serverPubKey2.tryEncryptString(" ");
        //tenc2 += serverPubKey2.tryEncryptString(text2);

        // End the encryption tests
        tenc1 += serverPubKey .endEncryptString();
        tenc2 += serverPubKey2.endEncryptString();

        std::string tdec1;
        serverPrivKey.beginDecryptString(Pt::Ssl::SSLPrivateKey::RSA_PKCS1);
        tdec1 += serverPrivKey.tryDecryptString(tenc1);

        // Start decryption test #2
        Pt::Ssl::SSLPrivateKey serverPrivKey2(serverPrivKey);
        std::string            tdec2;
        serverPrivKey2.beginDecryptString(Pt::Ssl::SSLPrivateKey::RSA_PKCS1_OAEP);
        tdec2 += serverPrivKey2.tryDecryptString(tenc2);

        // End the decryption tests
        tdec1 += serverPrivKey .endDecryptString();
        tdec2 += serverPrivKey2.endDecryptString();
  
        // Check if the decrypted texts are the same with the source texts
        PT_SSL_LOG_M("\n\n##### STRING ENCRYPTION #####"
                     << "\nDecryption #1 status: " << ( ( (text + " " ) == tdec1 ) ? "OK" : "FAILED")
                     << "\nDecryption #2 status: " << ( ( (text + " " + text2) == tdec2 ) ? "OK" : "FAILED") << "\n"
                    );

        // Done
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
