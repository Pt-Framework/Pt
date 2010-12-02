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

#include "SSLConnector2Server.h"
#include "SSLConnector2Client.h"

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
    _outBuff += std::string(buff, len);

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Wrote " << len << " bytes to the output buffer" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Wrote " << len << " bytes to the output buffer" << std::endl;

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

        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;

        return avail;
    }

    memcpy(buff, _decBuff.data(), size);
    _decBuff.erase(0, size);

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;

    return size;
}

int SSLConnector2::_write(const char* buff, int len)
{
    int bytesWritten = SSL_write(_ssl, buff, len);
    if(bytesWritten < 0) {
        if(!SSL_want_read(_ssl))
            throw "Connection error!";
        else {
            if(dynamic_cast<SSLConnector2Server*>(this))
                std::cerr << "[SSLConnector2] (Server) SSL wants read" << std::endl;
            else
                std::cerr << "[SSLConnector2] (Client) SSL wants read" << std::endl;
        }
        return 0;
    }

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Wrote " << bytesWritten << " bytes to the SSL handle" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Wrote " << bytesWritten << " bytes to the SSL handle" << std::endl;

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
            else {
                if(dynamic_cast<const SSLConnector2Server*>(this))
                    std::cerr << "[SSLConnector2] (Server) Output BIO should retry R=" << BIO_should_read(_out) << " W=" << BIO_should_write(_out) << std::endl;
                else
                    std::cerr << "[SSLConnector2] (Client) Output BIO should retry R=" << BIO_should_read(_out) << " W=" << BIO_should_write(_out) << std::endl;
            }
            continue;
        }
    }

    if(dynamic_cast<const SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Pulled " << bytesRead << " bytes from the output BIO" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Pulled " << bytesRead << " bytes from the output BIO" << std::endl;

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
            else {
                if(dynamic_cast<const SSLConnector2Server*>(this))
                    std::cerr << "[SSLConnector2] (Server) Input BIO should retry R=" << BIO_should_read(_in) << " W=" << BIO_should_write(_in) << std::endl;
                else
                    std::cerr << "[SSLConnector2] (Client) Input BIO should retry R=" << BIO_should_read(_in) << " W=" << BIO_should_write(_in) << std::endl;
            }
            continue;
        }
    }

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Pushed " << bytesWritten << " bytes to the input BIO" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Pushed " << bytesWritten << " bytes to the input BIO" << std::endl;

    if(!SSL_is_init_finished(_ssl)) {
        SSL_do_handshake(_ssl);
    }
    else {
        const int bytesRead = SSL_read(_ssl, _sslBuff, sizeof(_sslBuff));

        if(dynamic_cast<const SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) Read " << bytesRead << " bytes from the SSL handle" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) Read " << bytesRead << " bytes from the SSL handle" << std::endl;

        if(bytesRead > 0) {
            _decBuff += std::string(_sslBuff, bytesRead);

            if(dynamic_cast<SSLConnector2Server*>(this))
                std::cerr << "[SSLConnector2] (Server) Stored " << bytesRead << " bytes to the decrypted data buffer" << std::endl;
            else
                std::cerr << "[SSLConnector2] (Client) Stored " << bytesRead << " bytes to the decrypted data buffer" << std::endl;

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
    if(!_iod.reading()) {
        _iod.beginRead(_iodBuff, sizeof(_iodBuff));

        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) begin read from IO device" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) begin read from IO device" << std::endl;
    }

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) _doSSL() started" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) _doSSL() started" << std::endl;

    int byteCount = 0;

    if(_outBuff.length()) {
        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) Writing pending data in the output buffer to the SSL handle" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) Writing pending data in the output buffer to the SSL handle" << std::endl;

        byteCount = _write(_outBuff.data(), _outBuff.length());
        if(byteCount > 0) _outBuff.erase(0, byteCount);
    }

    if(!_inBuff.length()) {
        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) Trying to pull data from the output BIO" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) Trying to pull data from the output BIO" << std::endl;

        byteCount = _pullData(_sslBuff, sizeof(_sslBuff));
        if(byteCount > 0) {
            _iod.beginWrite(_sslBuff, byteCount);

            if(dynamic_cast<SSLConnector2Server*>(this))
                std::cerr << "[SSLConnector2] (Server) begin write to IO device" << std::endl;
            else
                std::cerr << "[SSLConnector2] (Client) begin write to IO device" << std::endl;
        }
    }

    while(_inBuff.length()) {
        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) Pushing pending data in the input buffer to the input BIO" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) Pushing pending data in the input buffer to the input BIO" << std::endl;

        byteCount = _pushData(_inBuff.data(), _inBuff.length());
        if(byteCount > 0) _inBuff.erase(0, byteCount);

        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) Trying to pull data from the output BIO" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) Trying to pull data from the output BIO" << std::endl;

        byteCount = _pullData(_sslBuff, sizeof(_sslBuff));
        if(byteCount > 0) {
            _iod.beginWrite(_sslBuff, byteCount);

            if(dynamic_cast<SSLConnector2Server*>(this))
                std::cerr << "[SSLConnector2] (Server) begin write to IO device" << std::endl;
            else
                std::cerr << "[SSLConnector2] (Client) begin write to IO device" << std::endl;
        }
    }

    if(!_connected) {
        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) SSL status = " << getStatusString() << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) SSL status = " << getStatusString() << std::endl;

        if(connectionEstablished()) {
            _connected = true;
            connected(*this);
        }
    }

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) _doSSL() ended" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) _doSSL() ended" << std::endl;
}

void SSLConnector2::_onIODOutput(System::IODevice& iod)
{
    const int byteCount = _iod.endWrite();

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Wrote " << byteCount << " bytes to the IO device" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Wrote " << byteCount << " bytes to the IO device" << std::endl;

    if(!_iod.reading()) {
        _iod.beginRead(_iodBuff, sizeof(_iodBuff));

        if(dynamic_cast<SSLConnector2Server*>(this))
            std::cerr << "[SSLConnector2] (Server) begin read from IO device" << std::endl;
        else
            std::cerr << "[SSLConnector2] (Client) begin read from IO device" << std::endl;
    }
}

void SSLConnector2::_onIODInput(System::IODevice& iod)
{
    const int byteCount = _iod.endRead();

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Read " << byteCount << " bytes from the IO device" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Read " << byteCount << " bytes from the IO device" << std::endl;

    if(byteCount > 0) _inBuff += std::string(_iodBuff, byteCount);

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] (Server) Wrote " << byteCount << " bytes to the input buffer" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) Wrote " << byteCount << " bytes to the input buffer" << std::endl;

    _doSSL();
}

} // namespace Pt
} // namespace Ssl
