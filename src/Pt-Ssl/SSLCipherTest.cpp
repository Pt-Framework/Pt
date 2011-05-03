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
#include <Pt/Ssl/SecureDigest.h>
#include <Pt/Ssl/RSACipher.h>
#include <Pt/System/Logger.h>

#include <sstream>

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
        
        // Test texts
        const std::string text  = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vitae quam quis velit gravida vestibulum.";
        const std::string text2 = text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text;
        
        std::stringstream ss1(text2, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        std::stringstream ss2(text2, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

        // Start signing test #1
        Pt::Ssl::SecureDigest secureDigest1(serverPrivKey, Pt::Ssl::SecureDigest::MD5_Digest);
        secureDigest1.update(ss1);
       
        // Start signing test #2
        Pt::Ssl::SecureDigest secureDigest2(serverPrivKey, Pt::Ssl::SecureDigest::SHA1_Digest);
        secureDigest2.update(text);
        secureDigest2.update(text);

        // End the signing tests
        secureDigest1.finish();
        secureDigest2.finish();
        const std::string& tsig1 = secureDigest1.getSignature();
        const std::string& tsig2 = secureDigest2.getSignature();

        // Start verification test #1
        secureDigest1.start(serverPubKey, tsig1, Pt::Ssl::SecureDigest::MD5_Digest);
        secureDigest1.update(text2.substr(  0, 100));
        secureDigest1.update(text2.substr(100, 200));
        secureDigest1.update(text2.substr(300));

        // Start verification test #2
        secureDigest2.start(serverPubKey, tsig2, Pt::Ssl::SecureDigest::SHA1_Digest);
        secureDigest2.update(text);
        secureDigest2.update(text);

        // End the the verification tests
        const bool tsig1ok = secureDigest1.finish();
        const bool tsig2ok = secureDigest2.finish();
        
        // Check if the decrypted texts are the same with the source texts
        PT_SSL_LOG_M("\n\n##### STRING SIGNING #####"
                     << "\nVerification #1 status: " << ( tsig1ok ? "OK" : "FAILED")
                     << "\nVerification #2 status: " << ( tsig2ok ? "OK" : "FAILED") << "\n"
                    );

        PT_SSL_LOG_M("################################################################################");

        // Start encryption test #1
        ss1.str(""); ss1.clear();
        Pt::Ssl::RSACipher rsaCipher1(ss1, serverPubKey, Pt::Ssl::RSACipher::RSA_PKCS1);
        std::iostream      rsaIOS1(&rsaCipher1);
        rsaIOS1.write(text.c_str(), text.length());
        rsaIOS1.write(" ", 1);

        // Start encryption test #2
        ss2.str(""); ss2.clear();
        Pt::Ssl::RSACipher rsaCipher2(ss2, serverPubKey, Pt::Ssl::RSACipher::RSA_PKCS1_OAEP);
        std::iostream      rsaIOS2(&rsaCipher2);
        rsaIOS2.write(text.c_str(), text.length());
        rsaIOS2.write(" ", 1);
        rsaIOS2.write(text2.c_str(), text2.length());

        // End the encryption tests and get a copy of the encrypted string
        rsaCipher1.finish();
        rsaCipher2.finish();
        const std::string tenc1 = ss1.str();
        const std::string tenc2 = ss2.str();

        char buff[1024];
        
        // Start decryption test #1
        ss1.str(tenc1); ss1.clear();
        rsaCipher1.startDecrypt(serverPrivKey, Pt::Ssl::RSACipher::RSA_PKCS1);

        std::string tdec1;
        rsaIOS1.clear();
        while(rsaIOS1.eof()) { // Test with blocking
            rsaIOS1.read(buff, sizeof(buff));
            if(rsaIOS1.gcount()) tdec1 += std::string(buff, rsaIOS1.gcount());
            std::cerr << "@@@ GOT : " << std::string(buff, rsaIOS1.gcount()) << std::endl;
        }

        // Start decryption test #2
        ss2.str(tenc2); ss2.clear();
        rsaIOS2.clear();
        rsaCipher2.startDecrypt(serverPrivKey, Pt::Ssl::RSACipher::RSA_PKCS1_OAEP);

        std::string tdec2;
        while(rsaIOS2.eof()) { // Test with non-blocking
            const size_t got = rsaIOS2.readsome(buff, sizeof(buff));
            if(got) tdec2+= std::string(buff, got);
            std::cerr << "@@@ GOT : " << std::string(buff, got) << std::endl;
        }
        
        // End the decryption tests
        rsaCipher1.finish();
        rsaCipher2.finish();
  
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
