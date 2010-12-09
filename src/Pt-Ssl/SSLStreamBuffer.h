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
#ifndef PT_SSL_SSLSTREAMBUFFER_H
#define PT_SSL_SSLSTREAMBUFFER_H

#include <string>
#include <Pt/Signal.h>
#include <Pt/System/StreamBuffer.h>

#include "SSLContext.h"

namespace Pt {
namespace Ssl {

//!
//! \brief SSL connector.
class PT_SSL_API SSLStreamBuffer : public Connectable, public std::streambuf {
    public:
        //! \brief Construct an SSL connector that uses the given IO device and context.
        SSLStreamBuffer(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID);

        //! \brief Construct an SSL connector that uses the given stream buffer and context.
        SSLStreamBuffer(System::StreamBuffer& streamBuffer, SSLContext& sslContext, const char* sessionID);

        //! \brief Standard dtor.
        virtual ~SSLStreamBuffer();

        //! \brief Check if this SSL connector has been connected to the SSL connector at the other end.
        bool connectionEstablished() const;

        //! \brief Get the current status string of this SSL connector.
        const char* getStatusString() const;

        //! \brief Reset this SSL connector.
        void reset();

        //! \brief Write data to this SSL connector.
        //! The written data will be encrypted and sent to the SSL conenctor at the other end through the communication medium
        //! This functions return the number of bytes actually written.
        //! A derivative class that override this class must always calls the original implementation before executing any other SSL operation.
        virtual int write(const char* buff, int len);

        //! \brief Signal that will be called when a client has successfully connected to the server.
        Signal<SSLStreamBuffer&> connected;

        //! \brief Signal that will be called when decrypted data is available.
        Signal<SSLStreamBuffer&> decryptedDataAvailable;

        //! \brief Call this function to get the decrypted data.
        int readDecryptedData(char* buff, int size);


        virtual std::streambuf::int_type overflow(std::streambuf::int_type ch);
        virtual std::streambuf::int_type underflow();
        int sync();

        static const int SB_SIZE = 16;
        static const int SB_PUTB =  4;

        char _sbBuffer[SB_SIZE];

    protected:
        BIO*                  _in;              // Input BIO
        BIO*                  _out;             // Output BIO
        SSL*                  _ssl;             // OpenSSL SSL handle
        std::string           _inBuff;          // Data to be written to the input BIO
        std::string           _sslWriteBuff;    // User writes the plain-data to be sent to this buffer
        std::string           _sslDDataBuff;    // User reads the received decrypted-data from this buffer
        bool                  _connected;       // A flag to indicate if a client has successfully connected to the server
        bool                  _ownIOSB;         // A flag to indicate if this class own the stream buffer
        System::StreamBuffer* _iosb;            // IO stream buffer
        char                  _readBuff[32768]; // Generic read buffer; SSL records can be up to 16KB, so this is just for safety

        void _init(SSLContext& sslContext, const char* sessionID, System::IODevice* ioDevice, System::StreamBuffer* streamBuffer);

        int  _pullData (      char* buff, int buffSize) const;
        int  _pushData (const char* buff, int len     ) const;
        void _readSSL  ();
        int  _writeSSL (const char* buff, int len     ) const;
        void _doSSL    ();

        void _onIOSBOutput(System::StreamBuffer&);
        void _onIOSBInput (System::StreamBuffer&);
};


} // namespace Pt
} // namespace Ssl

#endif
