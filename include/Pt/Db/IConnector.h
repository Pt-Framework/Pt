/* Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_ICONNECTOR_H
#define PT_DB_ICONNECTOR_H

#include <Pt/Db/Api.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace Db {

class Connection;
class IConnection;

/** @brief Creates %IConnection objects for a driver.
*/
class PT_DB_API IConnector : private NonCopyable
{
    public:
        virtual ~IConnector() { }

        /** @brief Creates an %IConnection.

            The caller takes ownership. Call %open() to establish a
            session.
        */
        virtual IConnection* get() = 0;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_ICONNECTOR_H
