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

#include "SSLConnector2.h"

namespace Pt {
namespace Ssl {

SSLConnector2::SSLConnector2(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID)
: _ssl      ( SSL_new(sslContext._ctx) ),
  _connected( false ),
  _in       ( BIO_new(BIO_s_mem()) ),
  _out      ( BIO_new(BIO_s_mem()) ),
  _iod      ( ioDevice )
{
    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);

    // Set session ID
    if(sessionID) SSL_set_session_id_context(_ssl, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));

    // Connect the signals
    _iod.inputReady  += Pt::slot(*this, &SSLConnector2::_onIODInput  );
    _iod.outputReady += Pt::slot(*this, &SSLConnector2::_onIODOutput );
}

SSLConnector2::~SSLConnector2()
{ SSL_free(_ssl); }

const char* SSLConnector2::getStatusString() const
{ return SSL_state_string_long(_ssl); }

bool SSLConnector2::connectionEstablished() const
{ return SSL_get_state(_ssl) == SSL_ST_OK; }

void SSLConnector2::accept()
{
    SSL_set_accept_state(_ssl);
    _iod.beginRead(_iodBuff, sizeof(_iodBuff));
}

void SSLConnector2::connect()
{
    SSL_set_connect_state(_ssl);
    SSL_do_handshake(_ssl);
    _iod.beginRead(_iodBuff, sizeof(_iodBuff));
    _doSSL();
}

void SSLConnector2::disconnect()
{
    SSL_shutdown(_ssl);
    _doSSL();
}

void SSLConnector2::reset()
{
    BIO_reset(_in);
    BIO_reset(_out);
    SSL_clear(_ssl);
}

const std::string SSLConnector2::getPeerCN() const
{
    if(SSL_get_verify_result(_ssl) != X509_V_OK) return "";

    X509* peer;
    peer = SSL_get_peer_certificate(_ssl);

    char peerCN[256];
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
    return peerCN;
}

int SSLConnector2::write(const char* buff, int len)
{
    _outBuff += std::string(buff, len);
    _doSSL();
    return len;
}

int SSLConnector2::readDecryptedData(char* buff, int size)
{
    const int avail = _decBuff.length();
    if(!avail) return 0;

    if(avail <= size) {
        memcpy(buff, _decBuff.data(), avail);
        _decBuff.clear();
        return avail;
    }

    memcpy(buff, _decBuff.data(), size);
    _decBuff.erase(0, size);
    return size;
}

int SSLConnector2::_write(const char* buff, int len)
{
    int bytesWritten = SSL_write(_ssl, buff, len);

    if(bytesWritten < 0) {
        if(!SSL_want_read(_ssl)) throw "Connection error!";
        return 0;
    }

    return bytesWritten;
}

int SSLConnector2::_pullData(char* buff, int buffSize) const
{
    const int bytesRead = BIO_read(_out, buff, buffSize);

    if(bytesRead < 0) {
        if(!BIO_should_retry(_out)) throw "Output buffer error!";
        return 0;
    }

    return bytesRead;
}

int SSLConnector2::_pushData(const char* buff, int len)
{
    const int bytesWritten = BIO_write(_in, buff, len);

    if(!SSL_is_init_finished(_ssl)) {
        SSL_do_handshake(_ssl);
    }
    else {
        char rbuff[8192];
        const int bytesRead = SSL_read(_ssl, rbuff, sizeof(rbuff));
        if(bytesRead > 0) {
            _decBuff += std::string(rbuff, bytesRead);
            decryptedDataAvailable(*this);
        }
        else if(SSL_get_shutdown(_ssl) & SSL_RECEIVED_SHUTDOWN) {
            SSL_shutdown(_ssl);
        }
    }

    return bytesWritten;
}

void SSLConnector2::_doSSL()
{
    int byteCount = 0;

    if(_outBuff.length()) {
        byteCount = _write(_outBuff.data(), _outBuff.length());
        if(byteCount > 0) _outBuff.erase(0, byteCount);
    }

    if(!_inBuff.length()) {
        byteCount = _pullData(_sslBuff, sizeof(_sslBuff));
        if(byteCount > 0) _iod.beginWrite(_sslBuff, byteCount);
    }

    while(_inBuff.length()) {
        byteCount = _pushData(_inBuff.data(), _inBuff.length());
        if(byteCount > 0) _inBuff.erase(0, byteCount);
        std::cerr << "[SSLConnector2] SSL status = " << getStatusString() << std::endl;

        byteCount = _pullData(_sslBuff, sizeof(_sslBuff));
        if(byteCount > 0) _iod.beginWrite(_sslBuff, byteCount);
    }

    if(!_connected && connectionEstablished()) {
        _connected = true;
        connected(*this);
    }
}

void SSLConnector2::_onIODOutput(System::IODevice& iod)
{
    const int byteCount = _iod.endWrite();
    std::cerr << "[SSLConnector2] Wrote " << byteCount << " bytes to the IO device" << std::endl;

    _iod.beginRead(_iodBuff, sizeof(_iodBuff));
}

void SSLConnector2::_onIODInput(System::IODevice& iod)
{
    const int byteCount = _iod.endRead();
    std::cerr << "[SSLConnector2] Read " << byteCount << " bytes from the IO device" << std::endl;

    if(byteCount > 0) _inBuff += std::string(_iodBuff, byteCount);
    _doSSL();
}

} // namespace Pt
} // namespace Ssl
