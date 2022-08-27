/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2012 by Marc Duerner
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

#include "CertificateStoreImpl.h"
#include "CertificateImpl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <cstdio>

#include <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFArray.h>

PT_LOG_DEFINE("Pt.Ssl.CertificateStore")

namespace Pt {

namespace Ssl {

CertificateStoreImpl::CertificateStoreImpl()
{
}


CertificateStoreImpl::~CertificateStoreImpl()
{
    for(std::vector<Certificate*>::iterator it = _allCerts.begin(); it != _allCerts.end(); ++it)
    {
        delete *it;
    }
}

//
// TODO: export end reimport selected key and certificate to build an identity later
//  SecExternalFormat dataFormat = kSecFormatWrappedPKCS8;
//  CFDataRef exportData = NULL;
//
//  SecItemImportExportKeyParameters keyParams = {};
//  keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
//  keyParams.passphrase = CFSTR("ExportImportPassphrase");
//
//  OSStatus status = SecItemExport(privateKey, dataFormat, 0, &keyParams, &exportData);
//  SecExternalFormat actualFormat = dataFormat;
//  SecExternalItemType actualType = kSecItemTypePrivateKey;
//  CFArrayRef outItems = NULL;
//
//  if (status == noErr)
//  {
//      status =
//          SecItemImport(exportData, nullptr, &actualFormat, &actualType, 0, &keyParams, targetKeychain, &outItems);
//  }
//  if (exportData != nullptr)
//      CFRelease(exportData);
//
//  CFRelease(keyParams.passphrase);
//  keyParams.passphrase = nullptr;
//
//  if (outItems != nullptr)
//      CFRelease(outItems);
//
//  return status;
//

void CertificateStoreImpl::loadPem(const char* pem, std::size_t len, const char* passwd)
{
    PT_LOG_DEBUG("loadPem: " << passwd);

    CFDataRef data = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(pem), len);
    if( ! data )
        throw std::runtime_error("CFDataCreate");

    SecItemImportExportKeyParameters keyParams = {};
    keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;

    CFStringRef password = NULL;
    if(passwd)
    {
      password = CFStringCreateWithCString(NULL, passwd, kCFStringEncodingUTF8);
      keyParams.passphrase = password;
    }

    CFStringRef extension = NULL;
    SecExternalFormat pemFormat = kSecFormatPEMSequence;
    SecExternalItemType* itemType = NULL;
    SecItemImportExportFlags flags = 0;
    SecKeychainRef keychain = NULL;
    CFArrayRef items = NULL;

    OSStatus status = SecItemImport(data, extension, &pemFormat, 
                                    itemType, flags, &keyParams, 
                                    keychain, &items);

    if(password)
        CFRelease(password);

    CFRelease(data);

    if(status != noErr)
    {
        if(items)
            CFRelease(items);
            
        throw InvalidCertificate("invalid PKCS12 data");
    }

    if( ! items)
        return;

    CFIndex count = CFArrayGetCount(items);

    for(CFIndex n = 0; n < count; ++n)
    {
        CFDictionaryRef item = (CFDictionaryRef) CFArrayGetValueAtIndex(items, n);

        SecIdentityRef identity = (SecIdentityRef) CFDictionaryGetValue(item, kSecImportItemIdentity);
        if(identity)
        {
            CFRetain(identity);
            Certificate* c = new Certificate( new CertificateImpl(identity) );
            _allCerts.push_back(c);
            
            PT_LOG_DEBUG("imported identity: " << c->subject());
        }

        CFArrayRef certs = (CFArrayRef) CFDictionaryGetValue(item, kSecImportItemCertChain);
        if(certs)
        {
            CFIndex certCount = CFArrayGetCount(certs);
            for(CFIndex i = 0; i < certCount; ++i)
            {
                SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(certs, i);

                CFRetain(cert);
                Certificate* c = new Certificate( new CertificateImpl(cert) );
                _allCerts.push_back(c);

                PT_LOG_DEBUG("imported certificate: " << c->subject());
            }
        }
    }

    CFRelease(items);
}

#ifdef PT_SSL_EXPERIMENTAL_KEY_IMPORT

// https://stackoverflow.com/questions/45997841/how-to-get-a-secidentityref-from-a-seccertificateref-and-a-seckeyref

static OSStatus AddKeyToKeychain(SecKeyRef privateKey, SecKeychainRef targetKeychain)
{
    // a) is used to put something INTO a keychain, instead of to take it out.
    // b) Doesn't assume that the input should be CFRelease()d and overwritten.
    // c) Doesn't return/emit the imported key reference.
    // d) Works on private keys.
    SecExternalFormat dataFormat = kSecFormatWrappedPKCS8;
    CFDataRef exportData = nullptr;

    SecItemImportExportKeyParameters keyParams = {};
    keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
    keyParams.passphrase = CFSTR("ExportImportPassphrase");

    OSStatus status = SecItemExport(privateKey, dataFormat, 0, &keyParams, &exportData);

    SecExternalFormat actualFormat = dataFormat;
    SecExternalItemType actualType = kSecItemTypePrivateKey;
    CFArrayRef outItems = nullptr;

    if (status == noErr)
    {
        status =
            SecItemImport(exportData, nullptr, &actualFormat, &actualType, 0, &keyParams, targetKeychain, &outItems);
    }

    if (exportData != nullptr)
        CFRelease(exportData);

    CFRelease(keyParams.passphrase);
    keyParams.passphrase = nullptr;

    if (outItems != nullptr)
        CFRelease(outItems);

    return status;
}


void CertificateStoreImpl::loadPem(const char* pem, std::size_t len, const char* passwd)
{
    //PT_LOG_DEBUG("loadPem: " << passwd);

    //std::cout.write(pem, len-1) << std::endl;

    std::ostringstream oss;

    oss << "-----BEGIN RSA PRIVATE KEY-----" << std::endl;
    oss << "MIIEpAIBAAKCAQEA3Yc68ZPIDSMhSWpMdgyOsLBfhqFrmGn3UK6o2lBnCQ3trCCT" << std::endl;
    oss << "3t50FvnQRxA2uV8Wv8ksQM/TqYemn0W/dpbmZBApqTDWzr9JPZg6Wqn19GqxpM78" << std::endl;
    oss << "LjDlu85BM3otmb7S2qMnRy9nrDXH11KMu3UzVVSdM/R9DQuR4FJxjS0mlHrrcxNr" << std::endl;
    oss << "9zgHSQZ2AgYoAt/THOUwlVq5xK/61uF8YtRb6oDbblZSAk3cfRR0rptJZLDZgon9" << std::endl;
    oss << "oPwAxlT5MEmb1X87sM7gPi44C/30mSC0mOQ3yILnAMtgl5eD/XV7BJdTXF7s0m3c" << std::endl;
    oss << "U379M5Y2/ZWTBJYfDzEI5hwhjrrHFIGwYKYjAQIDAQABAoIBAQDPZGn3fl38hpYE" << std::endl;
    oss << "MEMzBWCU5BoaZYbgotwJqeqksD82B9cpnToynVpbpDasQO6qru4SUNlgN/E1tqx0" << std::endl;
    oss << "Ug0EjhNVPJh0MCf6WLTNuHVxKcr8ruJuo+UNrXN2VotOESUd2bt3OuVrzGKno5Xa" << std::endl;
    oss << "7QqZ+Qg868FcwZhNHTbNpwFtpQaIW6mkX1V86IOTP539oeymiyneN9hawuw7BCEc" << std::endl;
    oss << "mvAS2UsyOQYuDCwxIT0ooW6STGWylpBNFT1ohPtiKtbXXvOCIQ63qBmCpcEhhiuW" << std::endl;
    oss << "YyxkfQdXqjmQ+DMg4nRHfp8KZVCd1NzK+UB82042tOA+/dQdtxsQwTFl/5FI3Wi1" << std::endl;
    oss << "NAwXso+RAoGBAPCaZg1BxWqFGumXcSdYnyOs4cGmId8qBdcforE39mrguGxeMuXQ" << std::endl;
    oss << "hf1IMIVU1kt7iWSIidI6bYCy/PDK/73uXkFNXdh5xamZ4f9i6FN1AhdLYIzSMPlD" << std::endl;
    oss << "E7B3CUchpL2UrMPXEvWrG9ahoBn9sGE44NZDsVPu+wr8ZQSbPIMN+vpVAoGBAOu0" << std::endl;
    oss << "V/58tfuTnoDf0/N1E2Inb14mf1W1Qyz6zzP9BMdHPfcxV1bcCQtQYmAu0Yv2JWEo" << std::endl;
    oss << "sTIbUl8mdPbmVaibom/ea7bF2/zluvxZODrsCrNOQjZJJjAzCGnGW3xUDXbA0Vix" << std::endl;
    oss << "wuwSuQgcfoxN22ScDtaczuUdT7bIe2WXuMGs0Mn9AoGANEOVv3N2Crl3Kx62ebGK" << std::endl;
    oss << "gBlhwMrxuKRMOHqqwIxDftvps8CSldFCCc7Qm2XeVu8y3R+7sGr4tmgrAbNW8k8B" << std::endl;
    oss << "MnY32Ho7Mag3q1400UwtdBnvkydlc//aGw8AeO8Z9gSBstjZSXwtw+W//WVaFwRf" << std::endl;
    oss << "8qspre3PoodI+kMuVdmHlzECgYAd1i5tYIwI1J7z4WGLJ/7UJfnLCUSFLPTb9MPV" << std::endl;
    oss << "6Ol58GK/r9dGf4QFkp9hXYvZE8eXjPchSKQct1mHB5FATrM5RgaMlG9MjCgnWYPs" << std::endl;
    oss << "sYSJPhbTmF7lKxCdkyZxjpJFvhLCD36FM7mIdVLvn8tt6PNX53GpPxSgJrC4I4SX" << std::endl;
    oss << "v7COYQKBgQCimn2yWFrdlbuIx1+CccKvmXnsxWGelS1wSGeUyUWmt0Vsn361eKJ2" << std::endl;
    oss << "nqDCuduZkrmyR71prIjUbZmJH9ccbzNCKgTAmUgl8BLdWWYkGQRs8tkU66IXRhro" << std::endl;
    oss << "FvTQbzqYTKK5NNKVVp7VMEotMFyIewjVPrwKlklkMBo/LJCawEtHEg==" << std::endl;
    oss << "-----END RSA PRIVATE KEY-----" << std::endl;

    std::ostringstream oss2;
    oss2 << "-----BEGIN CERTIFICATE-----" << std::endl;
    oss2 << "MIIDdDCCAlygAwIBAgIBZTANBgkqhkiG9w0BAQUFADBEMQswCQYDVQQGEwJVUzEa" << std::endl;
    oss2 << "MBgGA1UEBxMRQ2hleWVubmUgTW91bnRhaW4xGTAXBgNVBAoTEFN0YXJnYXRlIENv" << std::endl;
    oss2 << "bW1hbmQwIBcNMTIwNjI4MDAwOTQzWhgPMjExMjAyMjUwMDA5NDNaMEQxCzAJBgNV" << std::endl;
    oss2 << "BAYTAlVTMRowGAYDVQQHExFDaGV5ZW5uZSBNb3VudGFpbjEZMBcGA1UEChMQU3Rh" << std::endl;
    oss2 << "cmdhdGUgQ29tbWFuZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN2H" << std::endl;
    oss2 << "OvGTyA0jIUlqTHYMjrCwX4aha5hp91CuqNpQZwkN7awgk97edBb50EcQNrlfFr/J" << std::endl;
    oss2 << "LEDP06mHpp9Fv3aW5mQQKakw1s6/ST2YOlqp9fRqsaTO/C4w5bvOQTN6LZm+0tqj" << std::endl;
    oss2 << "J0cvZ6w1x9dSjLt1M1VUnTP0fQ0LkeBScY0tJpR663MTa/c4B0kGdgIGKALf0xzl" << std::endl;
    oss2 << "MJVaucSv+tbhfGLUW+qA225WUgJN3H0UdK6bSWSw2YKJ/aD8AMZU+TBJm9V/O7DO" << std::endl;
    oss2 << "4D4uOAv99JkgtJjkN8iC5wDLYJeXg/11ewSXU1xe7NJt3FN+/TOWNv2VkwSWHw8x" << std::endl;
    oss2 << "COYcIY66xxSBsGCmIwECAwEAAaNvMG0wCQYDVR0TBAIwADALBgNVHQ8EBAMCBLAw" << std::endl;
    oss2 << "EwYDVR0lBAwwCgYIKwYBBQUHAwIwEQYJYIZIAYb4QgEBBAQDAgeAMCsGA1UdHwQk" << std::endl;
    oss2 << "MCIwIKAeoByGGmh0dHA6Ly90ZXN0Y2EubG9jYWwvY2EuY3JsMA0GCSqGSIb3DQEB" << std::endl;
    oss2 << "BQUAA4IBAQA3zExU/mMi4mPbfrMJJlso0IAwWKBwVYNzY0+PDYRDwasJ/nGDRpYg" << std::endl;
    oss2 << "3t51RMy+RDq1D9o66NYT02EjOt2+FKIPa1rqSqlOOSp4JhwYI2snyNjwM1uyEfwu" << std::endl;
    oss2 << "5JDGqR4K8RUPGz2idI9ueYKkInfZkWtWLscX25Zt3NHybdp8pn91+/EfhRkzDQs5" << std::endl;
    oss2 << "PAQOP2+DjFI9cIbxsM+ww7LaD2TJCNuoL5L+r9bRN/2iLAQG6ds88NdPmsP/zoiL" << std::endl;
    oss2 << "Q597Co441X3otxORYviKnMisdq80Z9EPH64CsIVLycmwfRgZskoj4kO3SnAVuiI5" << std::endl;
    oss2 << "HzDpmPLQ57zce4ZMKHaq1zYQRSM35aNh" << std::endl;
    oss2 << "-----END CERTIFICATE-----" << std::endl;

    std::string pemString = oss.str();
    
    CFDataRef data = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(pemString.c_str()), 
                                                                       pemString.size());
    if( ! data )
        throw std::runtime_error("CFDataCreate");

    std::string pemString2 = oss2.str();
    
    CFDataRef data2 = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(pemString2.c_str()), 
                                                                        pemString2.size());
    if( ! data2 )
        throw std::runtime_error("CFDataCreate");

    SecItemImportExportKeyParameters keyParams = {};
    keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
    keyParams.accessRef = NULL;
    keyParams.passphrase = NULL;
    
    CFStringRef password = NULL;
    // if(passwd)
    // {
    //   std::cout << passwd << std::endl;
    //   password = CFStringCreateWithCString(NULL, passwd, kCFStringEncodingUTF8);
    //   keyParams.passphrase = password;
    // }

    CFStringRef extension = NULL;
    SecExternalFormat format = kSecFormatPEMSequence;
    SecExternalItemType* itemType = NULL;
    SecItemImportExportFlags flags = 0;
    SecKeychainRef keychain = NULL;
    CFArrayRef items = NULL;
    CFArrayRef items2 = NULL;

    //SecKeychainCopyDefault(&keychain);

    OSStatus os = SecKeychainOpen("pt-test", &keychain);
    if(keychain)
    {
        std::cout << "keychain open: " << os << " " << (void*) keychain << std::endl;

        OSStatus ds = SecKeychainDelete(keychain);
        std::cout << "keychain delete: " << ds << std::endl;
        keychain = NULL;
    }

    if( ! keychain )
    {
        OSStatus kcs = SecKeychainCreate("pt-test", 0, "", FALSE, NULL, &keychain);
        std::cout << "keychain create: " << kcs << " " << (void*) keychain << std::endl;

        //SecKeychainCopyDefault(&keychain);
    }

    SecKeychainRef imoprtKeychain = NULL;
    //SecKeychainRef imoprtKeychain = keychain;

    std::cout << "importing..." << std::endl;
    OSStatus status = SecItemImport(data, extension, NULL, 
                                    itemType, flags, &keyParams, 
                                    imoprtKeychain, &items);

    // std::cout << "importing..." << std::endl;
    // OSStatus status2 = SecItemImport(data2, extension, NULL, 
    //                                 itemType, flags, &keyParams, 
    //                                 imoprtKeychain, &items2);

    // std::cout << "imported: " << status2 << std::endl;
    
    if(password)
        CFRelease(password);

    CFRelease(data);
    CFRelease(data2);

    if(status != noErr)
    {
        std::cout << "SSL ERROR" << std::endl;
        if(items)
            CFRelease(items);

        CFStringRef what = SecCopyErrorMessageString(status, NULL);
        char str[64] = {}; 
        CFStringGetCString(what, str, 64, kCFStringEncodingUTF8);
        std::cout << status << " " << str << std::endl;
        //throw InvalidCertificate("invalid PEM data");
    }

    if( ! items)
    {
        std::cout << "no items" << std::endl;
        return;
    }

    std::cout << "got items" << std::endl;
    CFIndex count = CFArrayGetCount(items);
    std::cout << "array count: " << count << std::endl;

    SecCertificateRef cert = NULL;
    SecKeyRef key = NULL;

    for(CFIndex n = 0; n < count; ++n)
    {
        SecKeychainItemRef item = (SecKeychainItemRef) CFArrayGetValueAtIndex(items, n);

        CFTypeID type = CFGetTypeID(item);
        std::cout << "got item " << type << std::endl;
        
        if( type == SecCertificateGetTypeID() )
        {
            cert = (SecCertificateRef) item;
            SecKeyRef pubKey = SecCertificateCopyKey(cert);
            CFDataRef data = SecKeyCopyExternalRepresentation(pubKey, NULL);

            std::cout << "got certificate: " << CFDataGetLength(data) << std::endl;
        }
        
        if( type == SecKeyGetTypeID() )
        {
            key = (SecKeyRef) item;
            
            CFDataRef data = SecKeyCopyExternalRepresentation(key, NULL);
            std::cout << "got key data: " << (void*) key << " " << (void*) data << std::endl;
            
            CFMutableDictionaryRef attrs = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                    &kCFTypeDictionaryKeyCallBacks, 
                                                                    &kCFTypeDictionaryValueCallBacks);
            CFDictionarySetValue(attrs, kSecAttrKeyType, kSecAttrKeyTypeRSA);
            CFDictionarySetValue(attrs, kSecAttrKeyClass, kSecAttrKeyClassPrivate);
            SecKeyRef copy = SecKeyCreateWithData(data, attrs, NULL);
            if(copy)
            {
                SecKeyRef pubKey = SecKeyCopyPublicKey(copy);
                std::cout << "got key: " << (void*) key << " " << (void*) pubKey << std::endl;
            }
        }
        if( type == SecIdentityGetTypeID() )
        {
            std::cout << "got identity" << std::endl;
        }
    }

    // count = CFArrayGetCount(items2);
    // std::cout << "array count: " << count << std::endl;
    // for(CFIndex n = 0; n < count; ++n)
    // {
    //     SecKeychainItemRef item = (SecKeychainItemRef) CFArrayGetValueAtIndex(items2, n);

    //     CFTypeID type = CFGetTypeID(item);
    //     std::cout << "got item " << type << std::endl;
        
    //     if( type == SecCertificateGetTypeID() )
    //     {
    //         cert = (SecCertificateRef) item;
    //         SecKeyRef pubKey = SecCertificateCopyKey(cert);
    //         CFDataRef data = SecKeyCopyExternalRepresentation(pubKey, NULL);

    //         std::cout << "got certificate: " << CFDataGetLength(data) << std::endl;
    //     }
    //     if( type == SecKeyGetTypeID() )
    //     {
    //         key = (SecKeyRef) item;
    //         SecKeyRef pubKey = SecKeyCopyPublicKey(key);
    //         //CFDataRef data = SecKeyCopyExternalRepresentation(pubKey, NULL);

    //         std::cout << "got key: " << (void*) pubKey << std::endl;
    //     }
    //     if( type == SecIdentityGetTypeID() )
    //     {
    //         std::cout << "got identity" << std::endl;
    //     }
    // }

    //CFDataRef hash = CFDataCreate(NULL, reinterpret_cast<const UInt8*>("123456"), 5);
    //CFDataRef hash2 = CFDataCreate(NULL, reinterpret_cast<const UInt8*>("123456‚"), 6);
    //CFDataRef hash3 = CFDataCreate(NULL, reinterpret_cast<const UInt8*>("123456‚"), 6);

    // //
    // // Add certificate
    // //
    // CFMutableDictionaryRef attrs2 = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
    //                                                          &kCFTypeDictionaryKeyCallBacks, 
    //                                                          &kCFTypeDictionaryValueCallBacks);
    // CFDictionarySetValue(attrs2, kSecClass, kSecClassCertificate);
    // CFDictionarySetValue(attrs2, kSecValueRef, cert);
    // CFDictionarySetValue(attrs2, kSecUseKeychain, keychain);

    // OSStatus acs = SecItemAdd(attrs2, NULL);
    // {
    //     CFStringRef what = SecCopyErrorMessageString(acs, NULL);
    //     char str[64] = {}; 
    //     CFStringGetCString(what, str, 64, kCFStringEncodingUTF8);
    //     std::cout << acs << " added cert " << str << std::endl;
    // }

    // //
    // // Search added certificate
    // //
    // CFMutableDictionaryRef attrsCert = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
    //                                                              &kCFTypeDictionaryKeyCallBacks, 
    //                                                              &kCFTypeDictionaryValueCallBacks);
    // CFDictionarySetValue(attrsCert, kSecClass, kSecClassCertificate);
    // CFDictionarySetValue(attrsCert, kSecMatchSearchList, 
    //                             CFArrayCreate(NULL, (const void**) &keychain, 1, NULL) );
    // CFDictionarySetValue(attrsCert, kSecMatchLimit, kSecMatchLimitOne);
    // CFDictionarySetValue(attrsCert, kSecReturnRef, kCFBooleanTrue);
    // CFDictionarySetValue(attrsCert, kSecReturnAttributes, kCFBooleanTrue);
    // //CFDictionarySetValue(attrs3, kSecAttrLabel, CFSTR("123456"));
    
    // CFTypeRef resultCert = NULL;
    // status = SecItemCopyMatching(attrsCert, &resultCert);
    // {
    //     CFStringRef what = SecCopyErrorMessageString(status, NULL);
    //     char str[64] = {}; 
    //     CFStringGetCString(what, str, 64, kCFStringEncodingUTF8);
    //     std::cout << status << " match cert " << str << std::endl;
    // }

    // CFDataRef publicHash = NULL;

    // if(resultCert)
    // {
    //     std::cout << " match cert RESULT " << CFGetTypeID(resultCert) << std::endl;

    //     if(CFGetTypeID(resultCert) == CFDictionaryGetTypeID() )
    //     {
    //         CFDictionaryRef dict = (CFDictionaryRef) resultCert;
    //         publicHash = (CFDataRef) CFDictionaryGetValue(dict, kSecAttrPublicKeyHash);
    //         if(publicHash)
    //         {
    //             std::cout << "  FOUND CERT HASH: " << CFDataGetLength(publicHash) << std::endl;
    //             UInt8 buffer[64];
    //             CFDataGetBytes(publicHash, CFRangeMake(0, CFDataGetLength(publicHash)), buffer);
    //             for(unsigned n = 0; n < 20; ++n)
    //             {
    //                 std::cout << (int)buffer[n] << " ";
    //             }
    //             std::cout << std::endl;
    //         }

    //         CFStringRef keyLabel = (CFStringRef) CFDictionaryGetValue(dict, kSecAttrLabel);
    //         if(keyLabel)
    //         {
    //             CFStringRef desc = CFCopyTypeIDDescription( CFGetTypeID(keyLabel) );

    //             char str[64] = {}; 
    //             CFStringGetCString(desc, str, 64, kCFStringEncodingUTF8);
    //             std::cout <<  "  LABEL TYPE: " << str << std::endl;

    //             if( CFGetTypeID(keyLabel) == CFStringGetTypeID() )
    //             {
    //                 std::cout << "  FOUND LABEL: " << CFStringGetLength(keyLabel) << std::endl;

    //                 char str2[64] = {}; 
    //                 CFStringGetCString(keyLabel, str2, 64, kCFStringEncodingUTF8);
    //                 std::cout <<  "  LABEL TEXT: " << str2 << std::endl;
    //             }
    //         }
    //     }
    // }

    //
    // Add key
    //
    CFMutableDictionaryRef attrs = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                             &kCFTypeDictionaryKeyCallBacks, 
                                                             &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(attrs, kSecClass, kSecClassKey);
    CFDictionarySetValue(attrs, kSecValueRef, key);
    CFDictionarySetValue(attrs, kSecUseKeychain, keychain);
    CFDictionarySetValue(attrs, kSecAttrLabel, CFSTR("Imported Private Key"));

    //if(publicHash)
    //    CFDictionarySetValue(attrs, kSecAttrApplicationLabel, publicHash);

    OSStatus as = SecItemAdd(attrs, NULL);
    {
        CFStringRef what = SecCopyErrorMessageString(as, NULL);
        char str[64] = {}; 
        CFStringGetCString(what, str, 64, kCFStringEncodingUTF8);
        std::cout << as << " added key " << str << std::endl;
    }

    //
    // Search added key
    //
    CFMutableDictionaryRef attrs3 = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                             &kCFTypeDictionaryKeyCallBacks, 
                                                             &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(attrs3, kSecClass, kSecClassKey);
    CFDictionarySetValue(attrs3, kSecMatchSearchList, 
                                CFArrayCreate(NULL, (const void**) &keychain, 1, NULL) );
    CFDictionarySetValue(attrs3, kSecMatchLimit, kSecMatchLimitOne);
    CFDictionarySetValue(attrs3, kSecReturnRef, kCFBooleanTrue);
    CFDictionarySetValue(attrs3, kSecReturnAttributes, kCFBooleanTrue);
    //CFDictionarySetValue(attrs3, kSecAttrLabel, CFSTR("123456"));
    CFTypeRef result = NULL;
    status = SecItemCopyMatching(attrs3, &result);
    {
        CFStringRef what = SecCopyErrorMessageString(status, NULL);
        char str[64] = {}; 
        CFStringGetCString(what, str, 64, kCFStringEncodingUTF8);
        std::cout << status << " match key " << str << std::endl;
    }
    
    if(result)
    {
        std::cout << " match key RESULT " << CFGetTypeID(result) << std::endl;
        if(CFGetTypeID(result) == SecIdentityGetTypeID() )
        {
            SecIdentityRef ident = (SecIdentityRef) result;
            CertificateImpl ci(ident);
            std::cout << " FOUND IDENTITY: " << ci.subject() << std::endl;
        }
        if(CFGetTypeID(result) == SecCertificateGetTypeID() )
        {
            SecCertificateRef certi = (SecCertificateRef) result;
            CertificateImpl ci(certi);
            std::cout << " FOUND CERTIFICATE: " << ci.subject() << std::endl;
        }

        if(CFGetTypeID(result) == CFDictionaryGetTypeID() )
        {
            CFDictionaryRef dict = (CFDictionaryRef) result;
            CFDataRef keyHash = (CFDataRef) CFDictionaryGetValue(dict, kSecAttrApplicationLabel);
            if(keyHash)
            {
                std::cout << "  FOUND KEY HASH: " << CFDataGetLength(keyHash) << std::endl;
                UInt8 buffer[64];
                CFDataGetBytes(keyHash, CFRangeMake(0, CFDataGetLength(keyHash)), buffer);
                for(unsigned n = 0; n < 20; ++n)
                {
                    std::cout << (int)buffer[n] << " ";
                }
                std::cout << std::endl;
            }

            CFStringRef keyLabel = (CFStringRef) CFDictionaryGetValue(dict, kSecAttrLabel);
            if(keyLabel)
            {
                CFStringRef desc = CFCopyTypeIDDescription( CFGetTypeID(keyLabel) );

                char str[64] = {}; 
                CFStringGetCString(desc, str, 64, kCFStringEncodingUTF8);
                std::cout <<  "  LABEL TYPE: " << str << std::endl;

                if( CFGetTypeID(keyLabel) == CFStringGetTypeID() )
                {
                    std::cout << "  FOUND LABEL: " << CFStringGetLength(keyLabel) << std::endl;

                    char str2[64] = {}; 
                    CFStringGetCString(keyLabel, str2, 64, kCFStringEncodingUTF8);
                    std::cout <<  "  LABEL TEXT: " << str2 << std::endl;
                }
            }
        }
    }
    
    std::cout << "creating identity" << std::endl;
    SecIdentityRef ident = NULL;
    OSStatus ids = SecIdentityCreateWithCertificate(keychain, cert, &ident);
    std::cout << "ident: " << ids << " " << (void*) ident << std::endl;
    {
        CFStringRef what = SecCopyErrorMessageString(ids, NULL);
        char str[64] = {}; 
        CFStringGetCString(what, str, 64, kCFStringEncodingUTF8);
        std::cout << ids << " ident " << str << std::endl;
    }

    OSStatus ds = SecKeychainDelete(keychain);
    std::cout << "keychain delete: " << ds << std::endl;

    CFRelease(items);
    //CFRelease(items2);
}

#endif

void CertificateStoreImpl::loadPkcs12(const char* pkcs12, std::size_t len, const char* passwd)
{
    PT_LOG_DEBUG("loadPkcs12: " << passwd);

    CFDataRef data = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(pkcs12), len);
    if( ! data)
        throw std::runtime_error("CFDataCreate");

    CFStringRef password = CFStringCreateWithCString(NULL, passwd, kCFStringEncodingUTF8);
    
    const void* keys[]   = { kSecImportExportPassphrase };
    const void* values[] = { password };

    CFIndex hasPassword = CFStringGetLength(password) > 0 ? 1 : 0;

    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, values, hasPassword, NULL, NULL);
    if( ! options)
        throw std::runtime_error("CFDictionaryCreate");

    CFArrayRef items = NULL;
    OSStatus securityError = SecPKCS12Import(data, options, &items);
    PT_LOG_TRACE("SecPKCS12Import: " <<  securityError);

    CFRelease(password);
    CFRelease(options);
    CFRelease(data);
    
    if(securityError != noErr)
    {
        if(items)
            CFRelease(items);
            
        throw InvalidCertificate("invalid PKCS12 data");
    }
    
    if( ! items)
        return;

    CFIndex count = CFArrayGetCount(items);

    for(CFIndex n = 0; n < count; ++n)
    {
        CFDictionaryRef item = (CFDictionaryRef) CFArrayGetValueAtIndex(items, n);

        SecIdentityRef identity = (SecIdentityRef) CFDictionaryGetValue(item, kSecImportItemIdentity);
        if(identity)
        {
            CFRetain(identity);
            Certificate* c = new Certificate( new CertificateImpl(identity) );
            _allCerts.push_back(c);
            
            PT_LOG_DEBUG("imported identity: " << c->subject());
        }

        CFArrayRef certs = (CFArrayRef) CFDictionaryGetValue(item, kSecImportItemCertChain);
        if(certs)
        {
            CFIndex certCount = CFArrayGetCount(certs);
            for(CFIndex i = 0; i < certCount; ++i)
            {
                SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(certs, i);

                CFRetain(cert);
                Certificate* c = new Certificate( new CertificateImpl(cert) );
                _allCerts.push_back(c);

                PT_LOG_DEBUG("imported certificate: " << c->subject());
            }
        }
    }

    CFRelease(items);
}


const Certificate* CertificateStoreImpl::findCertificate(const std::string& subject)
{
    PT_LOG_TRACE("find certificate: " << subject);
    
    for(std::vector<Certificate*>::const_iterator it = _allCerts.begin(); it != _allCerts.end(); ++it) 
    {
        if( (*it)->subject().find(subject) != std::string::npos )
            return *it;
    }

    return 0;
}

} // namespace Ssl

} // namespace Pt
