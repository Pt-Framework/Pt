/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_IROW_H
#define PT_DB_IROW_H

#include <Pt/RefCounted.h>
#include <Pt/Db/Api.h>
#include <cstddef>

namespace Pt {

namespace Db {

  class Value;

    /** @brief Database-row backend.
    */
    class PT_DB_API IRow : public RefCounted
    {
        public:
            typedef std::size_t size_type;

            /** @brief Returns the number of columns.
            */
            virtual size_type size() const = 0;

            /** @brief Returns the value at @a field_num.
            */
            virtual Value getValue(size_type field_num) const = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_IROW_H
