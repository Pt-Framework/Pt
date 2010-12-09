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
#include "openssl/err.h"

namespace Pt {
namespace Ssl {

#define SSL_CALL_INFO _getType(this, PT_FUNCTION)

static const std::string _getType(const SSLConnector2* ssl, const std::string& funcName)
{
    static int count = 0;

    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, "%06d %s [%17s]", count++, (dynamic_cast<const SSLConnector2Server*>(ssl)) ? "(Server)" : "(Client)", f.c_str());

    return buff;
}

SSLConnector2::SSLConnector2(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID)
: _in       ( 0 ),
  _out      ( 0 ),
  _ssl      ( 0 ),
  _connected( false ),
  _ownIOSB  ( false ),
  _iosb     ( 0 )
{ this->_init(sslContext, sessionID, &ioDevice, 0); }

SSLConnector2::SSLConnector2(System::StreamBuffer& streamBuffer, SSLContext& sslContext, const char* sessionID)
: _in       ( 0 ),
  _out      ( 0 ),
  _ssl      ( 0 ),
  _connected( false ),
  _ownIOSB  ( true ),
  _iosb     ( 0 )
{ this->_init(sslContext, sessionID, 0, &streamBuffer); }

void SSLConnector2::_init(SSLContext& sslContext, const char* sessionID, System::IODevice* ioDevice, System::StreamBuffer* streamBuffer)
{
    // Create the SSL objects
    _in  = BIO_new(BIO_s_mem());
    _out = BIO_new(BIO_s_mem());
    _ssl = SSL_new(sslContext._ctx);

    // Create the stream buffer object
    _iosb = streamBuffer ? streamBuffer : (new System::StreamBuffer(*ioDevice, 8192, true));

    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);

    // Set session ID
    if(sessionID) SSL_set_session_id_context(_ssl, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));

    // Connect the signals
    _iosb->inputReady  += Pt::slot(*this, &SSLConnector2::_onIOSBInput  );
    _iosb->outputReady += Pt::slot(*this, &SSLConnector2::_onIOSBOutput );
}

SSLConnector2::~SSLConnector2()
{
    SSL_free(_ssl);
    if(_ownIOSB) delete _iosb;
}

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
    _sslWriteBuff.append(buff, len);
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Wrote " << len << " bytes to the output buffer" << std::endl;

    _doSSL();
    return len;
}

int SSLConnector2::readDecryptedData(char* buff, int size)
{
    if(size <= 0) return 0;

    const int avail = _sslDDataBuff.length();
    if(avail <= 0) return 0;

    if(avail <= size) {
        memcpy(buff, _sslDDataBuff.data(), avail);
        _sslDDataBuff.clear();
        std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Retrieved " << avail << " bytes from the decrypted data buffer" << std::endl;

        return avail;
    }

    memcpy(buff, _sslDDataBuff.data(), size);
    _sslDDataBuff.erase(0, size);
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Retrieved " << size << " bytes from the decrypted data buffer" << std::endl;

    return size;
}

int SSLConnector2::_pullData(char* buff, int buffSize) const
{
    int bytesRead = 0;

    if(!BIO_pending(_out)) return 0;

    while(!bytesRead) {
        bytesRead = BIO_read(_out, buff, buffSize);
        if(bytesRead < 0) {
            if(!BIO_should_retry(_out))
                throw "Output buffer error!";
            else
                std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Output BIO should retry R=" << BIO_should_read(_out) << " W=" << BIO_should_write(_out) << std::endl;
            continue;
        }
    }
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Pulled " << bytesRead << " bytes from the output BIO" << std::endl;

    return bytesRead;
}

int SSLConnector2::_pushData(const char* buff, int len) const
{
    int bytesWritten = 0;

    while(!bytesWritten) {
        bytesWritten = BIO_write(_in, buff, len);
        if(bytesWritten < 0) {
            if(!BIO_should_retry(_in))
                throw "Output buffer error!";
            else
                std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Input BIO should retry R=" << BIO_should_read(_in) << " W=" << BIO_should_write(_in) << std::endl;
            continue;
        }
    }
    BIO_flush(_in);
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Pushed " << bytesWritten << " bytes to the input BIO" << std::endl;

    return bytesWritten;
}

void SSLConnector2::_readSSL()
{
    if(!SSL_is_init_finished(_ssl)) {
        SSL_do_handshake(_ssl);
    }
    else {
        const int bytesRead = SSL_read(_ssl, _readBuff, sizeof(_readBuff));
        std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Read " << bytesRead << " bytes from the SSL handle" << std::endl;

        if(bytesRead < 0){
            long lerr = ERR_get_error();
            if(lerr) {
                char buf[255];
                ERR_error_string_n(lerr, buf, sizeof(buf));
                std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " ERROR " << lerr << ": " << buf << std::endl;
            }
        }
        else if(bytesRead > 0) {
            _sslDDataBuff.append(_readBuff, bytesRead);
            std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Stored " << bytesRead << " bytes to the decrypted data buffer" << std::endl;

            decryptedDataAvailable(*this);
        }
        else if(SSL_get_shutdown(_ssl) & SSL_RECEIVED_SHUTDOWN) {
            SSL_shutdown(_ssl);
        }
    }
}

int SSLConnector2::_writeSSL(const char* buff, int len) const
{
    int bytesWritten = SSL_write(_ssl, buff, len);
    if(bytesWritten < 0) {
        if(!SSL_want_read(_ssl))
            throw "Connection error!";
        else
            std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " SSL wants read" << std::endl;
        return 0;
    }
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Wrote " << bytesWritten << " bytes to the SSL handle" << std::endl;

    return bytesWritten;
}

void SSLConnector2::_doSSL()
{
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " _doSSL() started" << std::endl;

    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " begin read from IO device" << std::endl;
    _iosb->beginRead();

    int byteCount = 0;

    // TODO: May need to add mechanism to 'redo' this so that a very long message will be sent automatically
    if(_sslWriteBuff.length()) {
        std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Writing pending data in the output buffer to the SSL handle" << std::endl;
        byteCount = _writeSSL(_sslWriteBuff.data(), _sslWriteBuff.length());
        if(byteCount > 0) _sslWriteBuff.erase(0, byteCount);
    }

    // TODO: May need to defer the invocation of the 'decryptedDataAvailable' until at the end of this function
    while(_inBuff.length()) {
        std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Pushing pending data in the input buffer to the input BIO" << std::endl;
        byteCount = _pushData(_inBuff.data(), _inBuff.length());
        if(byteCount > 0) _inBuff.erase(0, byteCount);
        _readSSL();
    }

    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Trying to pull data from the output BIO" << std::endl;
    byteCount = _pullData(_readBuff, sizeof(_readBuff));
    if(byteCount > 0) {
        std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " begin write to IO device" << std::endl;
        _iosb->sputn(_readBuff, byteCount);
        _iosb->beginWrite();
    }

    if(!_connected) {
        std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " SSL status = " << getStatusString() << std::endl;

        if(connectionEstablished()) {
            _connected = true;
            connected(*this);
        }
    }

    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " _doSSL() ended" << std::endl;
}

void SSLConnector2::_onIOSBOutput(System::StreamBuffer&)
{
    const int byteCount = _iosb->endWrite();
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Wrote " << byteCount << " bytes to the IO device" << std::endl;

    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " begin read from IO device" << std::endl;
    _iosb->beginRead();
}

void SSLConnector2::_onIOSBInput(System::StreamBuffer&)
{
    _iosb->endRead();
    const int byteCount = std::min<size_t>(_iosb->in_avail(), sizeof(_readBuff));
    _iosb->sgetn(_readBuff, byteCount);
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Read " << byteCount << " bytes from the IO device" << std::endl;

    if(byteCount > 0) _inBuff.append(_readBuff, byteCount);
    std::cerr << "[SSLConnector2] " << SSL_CALL_INFO << " Wrote " << byteCount << " bytes to the input buffer" << std::endl;

    _doSSL();
}

} // namespace Pt
} // namespace Ssl
