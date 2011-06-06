/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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
#include <Pt/Ssl/CipherStreamBuf.h>
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

        // Load certificate 
        Pt::Ssl::SSLCertificateList serverCertChain;
        serverCertChain.loadFromFile("server.pem");

        // Extract the public key from the certificate
        Pt::Ssl::SSLPublicKey serverPubKey = serverCertChain.getPublicKey();

        // Load private key
        Pt::Ssl::SSLPrivateKey serverPrivKey("password");
        serverPrivKey.loadFromFile("server.key");

        // Test texts
        const std::string textShort = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vitae quam quis velit gravida vestibulum.";
        const std::string textLong  = textShort + ' ' + textShort + ' ' + textShort + ' '
                                    + textShort + ' ' + textShort + ' ' + textShort + ' '
                                    + textShort + ' ' + textShort;

        std::cerr << "textShort = " << textShort.size() << " bytes." << std::endl;
        std::cerr << "textLong  = " << textLong .size() << " bytes." << std::endl;

        // Instantiate the ciphers
        Pt::Ssl::RSACipher cipher1(serverPubKey, serverPrivKey, Pt::Ssl::RSACipher::RSA_PKCS1);
        Pt::Ssl::RSACipher cipher2(serverPubKey, serverPrivKey, Pt::Ssl::RSACipher::RSA_PKCS1_OAEP);

        // Instantiate the string-streams
        std::stringstream ss1("", std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        std::stringstream ss2("", std::ios_base::in | std::ios_base::out | std::ios_base::binary);

        // Instantiate the cipher-stream-buffers
        Pt::Ssl::CipherStreamBuf csb1(ss1, cipher1);
        Pt::Ssl::CipherStreamBuf csb2(ss2, cipher2);

        // Instantiate the io-streams
        std::iostream ios1(&csb1);
        std::iostream ios2(&csb2);
        
        // Start encryption test
        ios1.write(textShort.c_str(), textShort.length());
        ios1.write(" ", 1);
        csb1.finish();

        ios2.write(textShort.c_str(), textShort.length());
        ios2.write(" ", 1);
        ios2.write(textLong.c_str(), textLong.length());
        
        std::cerr << "input1 = " << (textShort + " " ).size() << " bytes." << std::endl;
        std::cerr << "input2 = " << (textShort + " " + textLong).size() << " bytes." << std::endl;

        // End the encryption tests and get a copy of the encrypted string
        csb1.finish();
        csb2.finish();
        const std::string tenc1 = ss1.str();
        const std::string tenc2 = ss2.str();

        std::cerr << "tenc1 = " << tenc1.size() << " bytes." << std::endl;
        std::cerr << "tenc2 = " << tenc2.size() << " bytes." << std::endl;

        // Reset the stream for decryption test
        char buff[1024];
        ss1.str(tenc1); ss1.clear(); ios1.clear();
        ss2.str(tenc2); ss2.clear(); ios2.clear();

        // Start decryption test
        std::string tdec1;
        while(!ios1.eof()) { // Test with blocking
            ios1.read(buff, sizeof(buff));
            if(ios1.gcount()) tdec1 += std::string(buff, ios1.gcount());
        }

        std::string tdec2;
        while(!ios2.eof()) { // Test with non-blocking
            if(csb2.import() <= 0) break;
            const size_t got = ios2.readsome(buff, sizeof(buff));
            if(got <= 0) break;
            tdec2+= std::string(buff, got);
        }
        
        // End the decryption tests
        csb1.finish();
        csb2.finish();
        std::cerr << "tdec1 = " << tdec1.size() << " bytes." << std::endl;
        std::cerr << "tdec2 = " << tdec2.size() << " bytes." << std::endl;

        // Check if the decrypted texts are the same with the source texts
        PT_SSL_LOG_M("\n\n##### STRING ENCRYPTION #####"
                     << "\nDecryption #1 status: " << ( ( (textShort + " " ) == tdec1 ) ? "OK" : "FAILED")
                     << "\nDecryption #2 status: " << ( ( (textShort + " " + textLong) == tdec2 ) ? "OK" : "FAILED") << "\n"
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
