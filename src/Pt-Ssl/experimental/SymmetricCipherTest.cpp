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
#include <Pt/Ssl/AESCipher.h>
#include <Pt/Ssl/TripleDESCipher.h>
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

        // Test texts
        const std::string text  = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vitae quam quis velit gravida vestibulum.";
        const std::string text2 = text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text
                                + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text
                                + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text + ' ' + text;

        std::stringstream ss1(text2, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        std::stringstream ss2(text2, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

        // Start encryption test #1
        ss1.str(""); ss1.clear();
        Pt::Ssl::TripleDESCipher cipher1(ss1, Pt::Ssl::TripleDESCipher::CBC);
        std::iostream            scIOS1(&cipher1);
        cipher1.startEncrypt("my_password_1", Pt::Ssl::TripleDESCipher::NoSalt);
        scIOS1.write(text.c_str(), text.length());
        scIOS1.write(" ", 1);

        // Start encryption test #2
        ss2.str(""); ss2.clear();
        Pt::Ssl::AESCipher  cipher2(ss2, Pt::Ssl::AESCipher::K256, Pt::Ssl::AESCipher::CBC);
        std::iostream       scIOS2(&cipher2);
        const std::string&  salt2 = cipher2.startEncrypt("my_password_2", Pt::Ssl::AESCipher::StrongSalt);
        scIOS2.write(text.c_str(), text.length());
        scIOS2.write(" ", 1);
        scIOS2.write(text2.c_str(), text2.length());
        
        // End the encryption tests and get a copy of the encrypted string
        cipher1.finish();
        cipher2.finish();
        const std::string tenc1 = ss1.str();
        const std::string tenc2 = ss2.str();
        std::cerr << std::endl;

        char buff[1024];
        
        // Start decryption test #1
        ss1.str(tenc1); ss1.clear();
        cipher1.startDecrypt("my_password_1");

        std::string tdec1;
        scIOS1.clear();
        while(!scIOS1.eof()) { // Test with blocking
            scIOS1.read(buff, sizeof(buff));
            if(scIOS1.gcount()) tdec1 += std::string(buff, scIOS1.gcount());
        }

        // Start decryption test #2
        ss2.str(tenc2); ss2.clear();
        cipher2.startDecrypt("my_password_2", salt2);

        std::string tdec2;
        scIOS2.clear();
        while(!scIOS2.eof()) { // Test with blocking
            scIOS2.read(buff, sizeof(buff));
            if(scIOS2.gcount()) tdec2 += std::string(buff, scIOS2.gcount());
        }

        // End the decryption tests
        cipher1.finish();
        cipher2.finish();

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
