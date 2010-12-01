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
#ifndef PT_SSL_SSLCONNECTOR2_H
#define PT_SSL_SSLCONNECTOR2_H

#include <string>
#include <Pt/Signal.h>
#include <Pt/System/IODevice.h>

#include "SSLContext.h"

namespace Pt {
namespace Ssl {

//!
//! \brief SSL connector.
//! By default this connector will act as an SSL connector server.
//! Call \ref connect() to convert it to an SSL conenctor client and initiate a connection to an SSL connector server.
class PT_SSL_API SSLConnector2 : public Connectable {
    public:
        //! \brief Construct an SSL connector that uses the given context.
        SSLConnector2(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID);

        //! \brief Standard dtor.
        virtual ~SSLConnector2();

        //! \brief Get the current status string of this SSL connector.
        const char* getStatusString() const;

        //! \brief Check if this SSL connector has been connected to the SSL connector at the other end.
        bool connectionEstablished() const;

        //! \brief Activate this SSL connector as an SSL connector client and initiate a connection to an SSL connector server.
        //! It is the responsibility of the developer to \ref pullData() from this SSL connector client and \ref pushData() to the correct SSL connector server.
        //! A derivative class that override this class must always calls the original implementation before executing any other SSL operation.
        virtual void connect();

        //! \brief Disconnect the connection.
        virtual void disconnect();

        //! \brief Get the peer CN (Common Name).
        const std::string getPeerCN() const;

        //! \brief Reset this SSL connector.
        void reset();

        //! \brief Write data to this SSL connector.
        //! The written data will be encrypted and sent to the SSL conenctor at the other end through the communication medium
        //! This functions return the number of bytes actually written.
        //! A derivative class that override this class must always calls the original implementation before executing any other SSL operation.
        virtual int write(const char* buff, int len);

        //! \brief Signal that will be called when a client has successfully connected to the server.
        Signal<SSLConnector2&> connected;

        //! \brief Signal that will be called when decrypted data is available.
        Signal<SSLConnector2&> decryptedDataAvailable;

        //! \brief Call this function to get the decrypted data.
        int readDecryptedData(char* buff, int size);

    protected:
        SSL*              _ssl;            // OpenSSL's SSL handle
        bool              _connected;      // A flag to indicate if a client has successfully connected to the server
        char              _sslBuff[32768]; // SSL records can be up to 16KB, so this is just for safety

        BIO*              _in;             // Input BIO
        std::string       _inBuff;         // Input buffer

        BIO*              _out;            // Output BIO
        std::string       _outBuff;        // Output buffer
        std::string       _decBuff;        // Decrypted data buffer

        System::IODevice& _iod;            // IO Device
        char              _iodBuff[32768]; // SSL records can be up to 16KB, so this is just for safety

        int _write(const char* buff, int len);
        int _pullData(char* buff, int buffSize) const;
        int _pushData(const char* buff, int len);
        void _doSSL();

        void _onIODOutput(System::IODevice& iod);
        void _onIODInput(System::IODevice& iod);
};


} // namespace Pt
} // namespace Ssl

#endif
