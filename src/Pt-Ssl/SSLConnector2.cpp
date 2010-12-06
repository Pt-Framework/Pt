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

#include <Pt/SourceInfo.h>
#include <iostream>

#include "SSLConnector2Server.h"
#include "SSLConnector2Client.h"

namespace Pt {
namespace Ssl {

static const std::string _getType(const SSLConnector2* ssl, const std::string& funcName)
{
    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, "%s [%17s]", (dynamic_cast<const SSLConnector2Server*>(ssl)) ? "(Server)" : "(Client)", f.c_str());

    return buff;
}

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

bool SSLConnector2::connectionEstablished() const
{ return SSL_get_state(_ssl) == SSL_ST_OK; }

const char* SSLConnector2::getStatusString() const
{ return SSL_state_string_long(_ssl); }

void SSLConnector2::reset()
{
    BIO_reset(_in);
    BIO_reset(_out);
    SSL_clear(_ssl);
}

int SSLConnector2::write(const char* buff, int len)
{
    _outBuff.append(buff, len);
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Wrote " << len << " bytes to the output buffer" << std::endl;

    _doSSL();
    return len;
}

int SSLConnector2::readDecryptedData(char* buff, int size)
{
    if(size <= 0) return 0;

    const int avail = _decBuff.length();
    if(avail <= 0) return 0;

    if(avail <= size) {
        memcpy(buff, _decBuff.data(), avail);
        _decBuff.clear();
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;

        return avail;
    }

    memcpy(buff, _decBuff.data(), size);
    _decBuff.erase(0, size);
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;

    return size;
}

int SSLConnector2::_write(const char* buff, int len)
{
    int bytesWritten = SSL_write(_ssl, buff, len);
    if(bytesWritten < 0) {
        if(!SSL_want_read(_ssl))
            throw "Connection error!";
        else
            std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " SSL wants read" << std::endl;
        return 0;
    }
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Wrote " << bytesWritten << " bytes to the SSL handle" << std::endl;

    return bytesWritten;
}

int SSLConnector2::_pullData(char* buff, int buffSize) const
{
    int bytesRead = 0;

    while(!bytesRead) {
        bytesRead = BIO_read(_out, buff, buffSize);
        if(bytesRead < 0) {
            if(!BIO_should_retry(_out))
                throw "Output buffer error!";
            else
                std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Output BIO should retry R=" << BIO_should_read(_out) << " W=" << BIO_should_write(_out) << std::endl;
            continue;
        }
    }
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Pulled " << bytesRead << " bytes from the output BIO" << std::endl;

    return bytesRead;
}

int SSLConnector2::_pushData(const char* buff, int len)
{
    int bytesWritten = 0;

    while(!bytesWritten) {
        bytesWritten = BIO_write(_in, buff, len);
        if(bytesWritten < 0) {
            if(!BIO_should_retry(_in))
                throw "Output buffer error!";
            else
                std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Input BIO should retry R=" << BIO_should_read(_in) << " W=" << BIO_should_write(_in) << std::endl;
            continue;
        }
    }
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Pushed " << bytesWritten << " bytes to the input BIO" << std::endl;

    if(!SSL_is_init_finished(_ssl)) {
        SSL_do_handshake(_ssl);
    }
    else {
        const int bytesRead = SSL_read(_ssl, _sslBuff, sizeof(_sslBuff));
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Read " << bytesRead << " bytes from the SSL handle" << std::endl;

        if(bytesRead > 0) {
            _decBuff.append(_sslBuff, bytesRead);
            std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Stored " << bytesRead << " bytes to the decrypted data buffer" << std::endl;

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
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " _doSSL() started" << std::endl;

    if(!_iod.reading()) {
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " begin read from IO device" << std::endl;
        _iod.beginRead(_iodBuff, sizeof(_iodBuff));
    }

    int byteCount = 0;

    if(_outBuff.length()) {
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Writing pending data in the output buffer to the SSL handle" << std::endl;
        byteCount = _write(_outBuff.data(), _outBuff.length());
        if(byteCount > 0) _outBuff.erase(0, byteCount);
    }

    while(_inBuff.length()) {
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Pushing pending data in the input buffer to the input BIO" << std::endl;
        byteCount = _pushData(_inBuff.data(), _inBuff.length());
        if(byteCount > 0) _inBuff.erase(0, byteCount);

    }

    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Trying to pull data from the output BIO" << std::endl;
    byteCount = _pullData(_sslBuff, sizeof(_sslBuff));

    if(byteCount > 0) {
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " begin write to IO device" << std::endl;
        _iod.beginWrite(_sslBuff, byteCount);
    }

    if(!_connected) {
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " SSL status = " << getStatusString() << std::endl;

        if(connectionEstablished()) {
            _connected = true;
            connected(*this);
        }
    }

    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " _doSSL() ended" << std::endl;
}

void SSLConnector2::_onIODOutput(System::IODevice& iod)
{
    const int byteCount = _iod.endWrite();
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Wrote " << byteCount << " bytes to the IO device" << std::endl;

    if(!_iod.reading()) {
        std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " begin read from IO device" << std::endl;
        _iod.beginRead(_iodBuff, sizeof(_iodBuff));
    }
}

void SSLConnector2::_onIODInput(System::IODevice& iod)
{
    const int byteCount = _iod.endRead();
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Read " << byteCount << " bytes from the IO device" << std::endl;

    if(byteCount > 0) _inBuff.append(_iodBuff, byteCount);
    std::cerr << "[SSLConnector2] " << _getType(this, PT_FUNCTION) << " Wrote " << byteCount << " bytes to the input buffer" << std::endl;

    _doSSL();
}

} // namespace Pt
} // namespace Ssl
