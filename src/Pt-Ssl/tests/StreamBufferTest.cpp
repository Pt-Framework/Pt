/*
 * Copyright (C) 2006 - 2011 by Marc Boris Duerner
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

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Ssl/CertificateList.h"
#include "Pt/Ssl/Context.h"
#include "Pt/Ssl/SSLStreamBuf.h"
#include "Pt/System/Logger.h"
#include <sstream>

const char clientPemData [] = 
"-----BEGIN CERTIFICATE-----\n"
"MIIDkjCCAnqgAwIBAgIBZTANBgkqhkiG9w0BAQUFADBVMQswCQYDVQQGEwJVUzEa\n"
"MBgGA1UEBxMRQ2hleWVubmUgTW91bnRhaW4xGTAXBgNVBAoTEFN0YXJnYXRlIENv\n"
"bW1hbmQxDzANBgNVBAMTBlNHQyBDQTAeFw0xMTA2MDcwMjU0NDdaFw0xMjA2MDYw\n"
"MjU0NDdaMFMxCzAJBgNVBAYTAlhYMRAwDgYDVQQHEwdMYW50YW5hMRYwFAYDVQQK\n"
"Ew1BdGxhbnRpcyBDaXR5MRowGAYDVQQDExFBbmNpZW50IE1haW5mcmFtZTCCASIw\n"
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL1wssBiOCiEHc033rcg7XPzx/ka\n"
"Ol1XPvowBwPhOBrNpVMogC+CU9f9C4qFzjPwYXd07CE/wMcbEepbYNqW5u810rWF\n"
"SM1y6E5f1Ow5d+lb81ZAtrauhZsrNheJ9qJWPgcOhQ4RfCP9JNW/hUkzXuBN+G5F\n"
"swpI9xwksjW8AOLXyn0ayFGcVQT/4Lz7eFHGdr7rVVi5GtnV6UmKkQe6dvjBqKxn\n"
"s3V/gtFffZDsZj9IxWmNubdUiUHru1PTmHpGZ+684w86+ldJEb3RCeteaGnJR1kR\n"
"+qliCA06O0DVQ685hrnfuH4Dbk8mmBsNAi/MfSjExE3ZLP7Y/2vKukHCX70CAwEA\n"
"AaNvMG0wCQYDVR0TBAIwADALBgNVHQ8EBAMCBLAwEwYDVR0lBAwwCgYIKwYBBQUH\n"
"AwIwEQYJYIZIAYb4QgEBBAQDAgeAMCsGA1UdHwQkMCIwIKAeoByGGmh0dHA6Ly90\n"
"ZXN0Y2EubG9jYWwvY2EuY3JsMA0GCSqGSIb3DQEBBQUAA4IBAQC2RzOA/D5XPKfi\n"
"46oznIVx13cHMxoAf/0ACWZVpcXtyfXLr1/SzuLisXf5nCuk0jSKhbo0eeQAXxEW\n"
"/xFFo1bABBoKRQnFy0eyGeTicUf6o0O2V9vEs6rc8PM9IZo9mLIVuJBOJMDxFG7u\n"
"YHSgY9ZgQ1nkoOE8oO1gPSjCsYHI2SlUJ8kURedKwLxErwUpkXTquEclgQZWW5f6\n"
"niGdGfNUWlP/y4KTd+RyGXNfrITKG63uCEqFBeJuBfA0FjLroxxyL6umI6XRgiH0\n"
"nAHRivMOTNZjyft/nmHlbpGeHuQ6dBTMCGhHc+krzm/uf2vyy8guphBZNO/1A4SQ\n"
"jhoJkRV8\n"
"-----END CERTIFICATE-----\n";

const char serverPemData [] =
"-----BEGIN CERTIFICATE-----\n"
"MIIDmzCCAoOgAwIBAgIBZDANBgkqhkiG9w0BAQUFADBVMQswCQYDVQQGEwJVUzEa\n"
"MBgGA1UEBxMRQ2hleWVubmUgTW91bnRhaW4xGTAXBgNVBAoTEFN0YXJnYXRlIENv\n"
"bW1hbmQxDzANBgNVBAMTBlNHQyBDQTAeFw0xMTA2MDcwMjU5MTlaFw0xMjA2MDYw\n"
"MjU5MTlaMFwxCzAJBgNVBAYTAlVTMRowGAYDVQQHExFDaGV5ZW5uZSBNb3VudGFp\n"
"bjEZMBcGA1UEChMQU3RhcmdhdGUgQ29tbWFuZDEWMBQGA1UEAxMNU0dDIE1haW5m\n"
"cmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL16SINcIyPdd/KO\n"
"lIAzGY/nTdmYLganfk1YPKCCwXbC47G8yRP7TgAIe1Ri8GSrzqQx14959hL9aRqf\n"
"XXcL75OausPOV4rDpzE4dncLYFxBhOfaP8mOtYNJx+iGWNh+FOqNjdYOmP0xk0Ie\n"
"o/WD6F4r1OVyD010a51AeOxrToVcBTL5dZ385cKGiVaghe+zn94QS/lRUoOLzMPL\n"
"TP42VQOcX2GnAU4dRhW5bBQmf+M6AxjbfS1Op0wfVzTU4LbprOHJy6f/mdoWTRYD\n"
"qwSLs11IDzxoSP/ED/2Abd186F5UMO23k8UN0XNsLYaCcUgdXSBKatlfFCVyNf0H\n"
"u+6jq9kCAwEAAaNvMG0wCQYDVR0TBAIwADALBgNVHQ8EBAMCBLAwEwYDVR0lBAww\n"
"CgYIKwYBBQUHAwEwEQYJYIZIAYb4QgEBBAQDAgZAMCsGA1UdHwQkMCIwIKAeoByG\n"
"Gmh0dHA6Ly90ZXN0Y2EubG9jYWwvY2EuY3JsMA0GCSqGSIb3DQEBBQUAA4IBAQBw\n"
"7oBoiraWvpp7t9Hq/ZYyYKkqOQkozqr1ekWidut8JLmFH5BwXNJzxotbb26YJhJ2\n"
"npxbcOzTpe/Na6eVmg3j6br2vDeyByfqxjmCiuhyu3ml9J6MCDXRlSruTDnbCGhs\n"
"z6RJAk5sE36bMyXEW5dnSj6wOQGpE6AIXuuYwr600U08voEHkcXSGRxHy9q8ytPC\n"
"PhuZ4NmG0ucghyGApuNzoJTXJfBYJVf4+87y9QleGcAjT0q/CLnnenABISIR1dKN\n"
"1CPDt3p1AKUer8fc532071nBNuPhg+SVZ38DvZIkppLL20ky/PqBKya6sSljSg6S\n"
"sIxgcXDCTonHiI2mOQX1\n"
"-----END CERTIFICATE-----\n";

const char caPemData [] =
"-----BEGIN CERTIFICATE-----\n"
"MIIEEzCCAvugAwIBAgIJAKr553j8TuyUMA0GCSqGSIb3DQEBBQUAMFUxCzAJBgNV\n"
"BAYTAlVTMRowGAYDVQQHExFDaGV5ZW5uZSBNb3VudGFpbjEZMBcGA1UEChMQU3Rh\n"
"cmdhdGUgQ29tbWFuZDEPMA0GA1UEAxMGU0dDIENBMB4XDTExMDYwNzAyNTIxOFoX\n"
"DTIxMDYwNDAyNTIxOFowVTELMAkGA1UEBhMCVVMxGjAYBgNVBAcTEUNoZXllbm5l\n"
"IE1vdW50YWluMRkwFwYDVQQKExBTdGFyZ2F0ZSBDb21tYW5kMQ8wDQYDVQQDEwZT\n"
"R0MgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDcdqQK4BNFZVfW\n"
"+koDaXMO3Qn0O7MK7t2rOLApIGDOoB4WG5vNgyCM0ZpyAVR1JJ7SHdgBwy+vEmfk\n"
"HN8ywYwIQLrjCDsEdBiijbhhtGWz5Cvvpc/OTmL2q+eUYH0AMFJIkUb4Did9J+7y\n"
"4oPlVLyrKGM64GNTm/jDhVILgY5W+/0M086l/M0W0NiN4diVR2j0X7FoorkbC1kY\n"
"SRZ6IyzwpzCFsn/83UzvGdRBt8SIHdyMOVSPDgiWxeCNtobgIfTDqyPyJlBjgT1j\n"
"89gcPtNzCrDHY9tKK6dELjHHUmS5CBJZ8E23d0lxbN4HAgxnRZMVL7WZrOiv1iCt\n"
"9udZ1j0BAgMBAAGjgeUwgeIwHQYDVR0OBBYEFFe61tTKhKGc55kJYuqvXYXR3/WL\n"
"MIGFBgNVHSMEfjB8gBRXutbUyoShnOeZCWLqr12F0d/1i6FZpFcwVTELMAkGA1UE\n"
"BhMCVVMxGjAYBgNVBAcTEUNoZXllbm5lIE1vdW50YWluMRkwFwYDVQQKExBTdGFy\n"
"Z2F0ZSBDb21tYW5kMQ8wDQYDVQQDEwZTR0MgQ0GCCQCq+ed4/E7slDAMBgNVHRME\n"
"BTADAQH/MCsGA1UdHwQkMCIwIKAeoByGGmh0dHA6Ly90ZXN0Y2EubG9jYWwvY2Eu\n"
"Y3JsMA0GCSqGSIb3DQEBBQUAA4IBAQDE7Va3mam/9v0586YO5QsKLERgYJpy+oRH\n"
"8dNPgI35mTc+OHdHszR35JhJSS6SvUGvC64HOSZQGKFL43FbAy4oO35vayJCQ5xT\n"
"0PQsqVHMMYQ+HtHrywj6zFGgD7gHTV+oSeNgfqHWJQclCZIa+FWW8sSkgmIREU0T\n"
"HXhKn2EoaA57VGipB3tR99R1XPXjcSrU8Pgfoex/CKQvSG2ymOFmMCEsiULtHdEe\n"
"+PHq5rDredAJw3tRHWjSHFKvHmbQNp+6YOvsQHnEJ3RNQ6rdrnTd18n/0IcxGbBn\n"
"oQyMCGKClipQE/0/dZETCuiTDqYKCJcDzdvkiyMYIwXuzI6HBOBJ\n"
"-----END CERTIFICATE-----\n";

const char serverKeyData [] =
"-----BEGIN RSA PRIVATE KEY-----\n"
"Proc-Type: 4,ENCRYPTED\n"
"DEK-Info: DES-EDE3-CBC,AEE7BA0B95B40038\n"
"\n"
"dxDd9QKCDXPxUvfquRAi04huz835S8x2MrshjYQYq2nRSFzJ+6gppN0vPrnLggwu\n"
"IXh3rGriFRtUKztxSYqwAWnfspzkfY+Q2EMJ3WjwkcsFvm7PWJXv+B5tsU47XSLj\n"
"Qyux5a3MsK0mlCVHzSFUp50Mj9Ph1mWVVw4ryWvKHz1GF9BgnX1eT4noUN4ngCcG\n"
"wyp2sjMVvdaNtlYeYJZSZ6HjBQ7TVVDdHfBTMdJlicwYaCO9YvirshHH+zXg9aTK\n"
"aFXLLV3ML76Ky5+pFLjubJdPJxXZEQRT+tXPrr3XQd89xfI9KFPksQh+GJ+ZAEF1\n"
"HFEMOlDTOMCMfF0K9++M21UYKLbqRL8+7TJyW+Mpq0bRa/+K5CIz87nDgikZjmp5\n"
"L8hVy9Sap+70lr/fee6zpSPXSjgN/g+v46LttIeLXUIEFMBbz1sg2cpQTmpYQvZm\n"
"wO8zwEx/jfM6pzeVnfoh9XnLg//Q8eV8FOuz/71tEJqRgnpnIhn2S1PuEiyjNrz+\n"
"d3K2sW61Uyg/tv4BprgiC2lfytolrnayXjVUBICaXphCoGv4OQIUJkf5vmO2NldA\n"
"4zUvlGII/ewxXPUBF83NnBrSfhKmmr8kPuNyas03+zaM8FvKne2Zr/sYqqQhHerc\n"
"JOTzndzVwiF10HJUbvc3wafKFUDiJRGkThK7BZWwbd3ORzAIY78HLFuev7py+7QR\n"
"Ii5W9q/NN8MZcRnh0ISu/HEbJ8u5sV4EQ8o5b5ClxzSMVQWQHtqFoKdtbCwO7lFq\n"
"kMgKqQFgJzkZ0It8BM7C5lFGuFe/710at1hpjhMMbfzlcolpWl+JOhey8YsBk5Xa\n"
"6iNvwSsq4UT8EcgO2VQt5FoiXd4Uayce18rGrM6YhRtn1VMahPw3WHpLPDf2OZZj\n"
"FE803xkg712deqeGyCvR1z1vO0ZVB6m1a3TBFV+zncJaMEVmXFJL26uQhELYnaT1\n"
"5CzSrjTUVHQ2S6uYjN8FEsi/XwDooyhqlOjuoGfcsC0GjMw/e8fhwmMWtvcwnfEN\n"
"r9L5o3FvyUg2e3f5GEReoAHKhrCrfrkv9/BEyIliQsvBGtV1jCoXyapjNdDNeQa2\n"
"HMgGSLZ+5MqNTsrxIcm8VVkTABhz9jzz/HovvlLCJnfpii2ZG1G5KDaoNJx633Bi\n"
"54joHixEK2Om53CEZECMQ+IVs/icTb4i+bjRljD7AOauiTwChrL+tdgddHULfu8d\n"
"m3id5cN1g5m/0ChtpI4AFLksKozIDdxOQx5N95PQL5GushV0VgEXboCEc8wxwZsx\n"
"jzEv3NbeG3Ob9m3Hza5gGHhIFwRUB4qohIrzVCx29dq+EUe3mn3CVrlsfYMfmQHu\n"
"kFO5TMjo4+PVoFrXRZvNRm8vzY6aS1X9nHndfn5fDQJoaawoecrZHqxL6wxDnLGj\n"
"Dm+yRCdb4RnavSHHSNzUFtGfd59W6OSz1zmdH2Fmup9e/3a2yJ4OLpStlKBs70NP\n"
"rg61BCuU3L8xoUhcjTb+IazKNAjcL6HtbXHMj51SuUz9mJAsjYkFC9dlXDjis2NU\n"
"KT1pxmEd40l6mWyHBUWVT4I9TVP9RZNCblAznJqnAHFxnLT5ga2cEA==\n"
"-----END RSA PRIVATE KEY-----";

const char clientKeyData [] =
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEpAIBAAKCAQEAvXCywGI4KIQdzTfetyDtc/PH+Ro6XVc++jAHA+E4Gs2lUyiA\n"
"L4JT1/0LioXOM/Bhd3TsIT/AxxsR6ltg2pbm7zXStYVIzXLoTl/U7Dl36VvzVkC2\n"
"tq6Fmys2F4n2olY+Bw6FDhF8I/0k1b+FSTNe4E34bkWzCkj3HCSyNbwA4tfKfRrI\n"
"UZxVBP/gvPt4UcZ2vutVWLka2dXpSYqRB7p2+MGorGezdX+C0V99kOxmP0jFaY25\n"
"t1SJQeu7U9OYekZn7rzjDzr6V0kRvdEJ615oaclHWRH6qWIIDTo7QNVDrzmGud+4\n"
"fgNuTyaYGw0CL8x9KMTETdks/tj/a8q6QcJfvQIDAQABAoIBAAYhhJd3VkvxYqmG\n"
"ml2g4i5zgFBOif8FWNNuBHwaqN8tMqGLWQKwnBgMZ3g3NmI18GGWViIJVyradGz6\n"
"Qhsw8Q5i1ovrYVzM0DnhHhn7DiduQ2TSOVeWNFq+PkTqwSGl2uINlT7MUBE2GyAT\n"
"z7N7xyFJRLYE6XN2wxCVG3Njvz+YIAGslHGp7dUNusJ98+fsNHevKh5XHhuLI81k\n"
"jyYbc4tF5spbzeFyntkj2ksXt1NysdWn6WN2nQlaBJDcYJLIQzIfSMJ7PMdlt/IH\n"
"HbjrCn4oW/Sds/T38q2a77N6+q4FPINA39sr2Pd2EOHxT2iF8y/Y6GN6P+hmscNQ\n"
"T+hRkcECgYEA4yGwLxJK0WhcKJFhnZCqYRkMLavLb3uNVfWPxqJrIRWNLAE5I8Dw\n"
"29qIoNYn7AMwI6LOhni4Safvcmb4O8GZ+M0KP7w1wEfpTvV9t4QdgwBro7ey4Tdm\n"
"MWcDDlhCc8TQphxTlEKiq/AEtnTpEJHTIIlfNsn37WPC1PSSijntBNECgYEA1YSf\n"
"z+Te6XufCOPw80VhjzQ4AynxW2L1+fpB79oxkLNa3HiojtR7TgnyFm4Ap7fPF8bH\n"
"Ckhi+jRZ9NBCJPCTkrDoe8OC1356CBf6hLrs9iojVBfwG+YbQzXkdeFexagJOGye\n"
"O6RM09XlIQOobXkeNalnNs7BlEHReBLKNIp91y0CgYEAxtPEwVAIAsyScEjj5OSw\n"
"4ZLloE1VJuBHV0pBdCbWFP4iuLTRakUKE8t38yPFiss3ptpsZDblTEUgTUtqXhUa\n"
"Tjxfjm+Zthq9WAt6491ZYEZL8+NpSdIPOYvtlu8YLPPLmr4NRCoY8N6JoZcyNvkQ\n"
"ktjuaq3exQahlX4LkcknfSECgYBo4/0qQPHs20j2MhwVNhlGI0385+bkkQnrYrBv\n"
"ps3Amwtg0XlkqSdl3+boc+OogNiggosXFxZgecVTSdekk/a7EmMzIYmuWTiTr0vn\n"
"VnjAOVUfo57V5cWwz9HKwwsuqGzXJizZec4wJpyoCkb8eIqJrA3DU/Cu52KCzSA+\n"
"jWZihQKBgQDWy3z7rK6r5IhzASvTfojd78jUc05rMEXA9NqCiPiqmnbT38kz5LlA\n"
"esK5pJBxXJXFVYRsCk0IjlYhCOflW1o9eUb+p9qwISwfHabkkdKrk78qvtgG2rNP\n"
"fqEbB/sshSwLfvltzAkcUttGhCQ2XeuSPlTbYhk8EUuZGkJExIhebA==\n"
"-----END RSA PRIVATE KEY-----\n";

class StreamBufferTest : public Pt::Unit::TestSuite
{
    public:
        StreamBufferTest()
        : Pt::Unit::TestSuite("StreamBufferTest")
        {
            //Pt::System::Logger::getTarget("Pt.Ssl").setLogLevel(Pt::System::Trace);

            this->registerMethod("Handshake", *this, &StreamBufferTest::Handshake);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void Handshake();
};


Pt::Unit::RegisterTest<StreamBufferTest> register_StreamBufferTest;


void StreamBufferTest::Handshake()
{
    std::stringstream data;

    Pt::Ssl::CertificateList caCert;
    caCert.fromPem(caPemData, sizeof(caPemData));

    // server-side SSL context
    Pt::Ssl::CertificateList serverCert;
    serverCert.fromPem(serverPemData, sizeof(serverPemData));

    Pt::Ssl::PrivateKey serverPrivKey("abc123");
    serverPrivKey.fromPem(serverKeyData, sizeof(serverKeyData));

    Pt::Ssl::Context serverContext;
    serverContext.setCACertificates(caCert);
    serverContext.setCertificateChain(serverCert);
    serverContext.setPrivateKey(serverPrivKey);

    // client-side SSL context
    Pt::Ssl::CertificateList clientCert;
    clientCert.fromPem(clientPemData, sizeof(clientPemData));

    Pt::Ssl::PrivateKey clientPrivKey("");
    clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));

    Pt::Ssl::Context clientContext;
    clientContext.setCACertificates(caCert);
    clientContext.setCertificateChain(clientCert);
    clientContext.setPrivateKey(clientPrivKey);

    // client begins the handshake
    Pt::Ssl::SSLStreamBuf client(data, clientContext);
    client.beginClientHandshake(true);

    // server begins the handskake
    Pt::Ssl::SSLStreamBuf server(data, serverContext);
    server.beginServerHandshake(true, true);

    for( ; ; )
    {
        // client handshake progress
        while( client.writeHandshake() )
            ;
    
        PT_UNIT_ASSERT( ! client.handshakeError() );
        PT_UNIT_ASSERT(data.str().size() > 0);
        data.clear();

        // server handshake progress
        while( server.readHandshake() )
            ;
    
        PT_UNIT_ASSERT( ! server.handshakeError() );
        data.clear();
        data.str( std::string() );
    
        while( server.writeHandshake() )
            ;
    
        PT_UNIT_ASSERT( ! server.handshakeError() );
        PT_UNIT_ASSERT(data.str().size() > 0);
        data.clear();

        // client handshake progress
        while( client.readHandshake() )
            ;

        PT_UNIT_ASSERT( ! client.handshakeError() );
        data.clear();
        data.str( std::string() );

        if( client.connected() )
            break;
    }

    PT_UNIT_ASSERT( client.connected() );
    PT_UNIT_ASSERT( server.connected() );
}