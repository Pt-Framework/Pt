#ifndef SSL_CONNECTOR_H
#define SSL_CONNECTOR_H

#include "SSLContext.h"

//!
//! \brief SSL connector.
//! By default this connector willa act as an SSL connector server.
//! Call \ref connect() to convert it to an SSL conenctor client and initiate a connection to an SSL connector server.
class SSLConnector {
    public:
        //! \brief Construct an SSL connector that uses the given context.
        SSLConnector(SSLContext& sslContext);

        //! \brief Standard dtor.
        virtual ~SSLConnector();

        //! \brief Get the current status string of this SSL connector.
        const char* getStatusString() const;

        //! \brief Check if this SSL connector has been connected to the SSL connector at the other end.
        bool connectionEstablished() const;

        //! \brief Activate this SSL connector as an SSL connector client and initiate a connection to an SSL connector server.
        //! It is the responsibility of the developer to \ref pullData() from this SSL connector client and \ref pushData() to the correct SSL connector server.
        //! A derivative class that override this class must always calls the original implementation before executing any other SSL operation.
        virtual void connect();

        //! \brief Write data to this SSL connector.
        //! The written data will be encrypted and sent to the SSL conenctor at the other end through the communication medium
        //! This functions return the number of bytes actually written.
        //! A derivative class that override this class must always calls the original implementation before executing any other SSL operation.
        virtual int write(const char* buff, int len);

        //! \brief Ovevride this function to receive decrypted data.
        virtual void onRecvData(const char* buff, int len) const = 0;

        //! \brief Pull data from the output buffer of this SSL connector.
        //! The pulled data must be send through the communication medium and written to the input buffer of the SSL conenctor at the other end.
        //! This functions return the number of bytes actually read.
        int pullData(char* buff, int buffSize) const;

        //! \brief Push data to the input buffer of this SSL connector.
        //! The pushed data must be the data received from the output buffer of the SSL conenctor at the other end through the communication medium.
        //! This functions return the number of bytes actually written.
        int pushData(const char* buff, int len);

    protected:
        BIO* _in;  // Input BIO
        BIO* _out; // Output BIO
        SSL* _ssl; // OpenSSL's SSL handle
};

#endif
