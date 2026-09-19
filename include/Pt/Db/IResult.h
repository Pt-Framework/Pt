/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_IRESULT_H
#define PT_DB_IRESULT_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/RefCounted.h>
#include <Pt/Db/Api.h>


namespace Pt {

namespace Db {

    class Row;

    /** @brief Buffered query-result backend.
    */
    class PT_DB_API IResult : public RefCounted
    {
    public:
        typedef std::size_t size_type;
        typedef Row value_type;

    public:
        virtual ~IResult()
        { }

        /** @brief Returns the row at @a tup_num.
        */
        virtual Row getRow(size_type tup_num) const = 0;

        /** @brief Returns the number of rows.
        */
        virtual size_type size() const = 0;

        /** @brief Returns the number of columns.
        */
        virtual size_type getFieldCount() const = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_IRESULT_H
