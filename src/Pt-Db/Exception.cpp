/*
 * Copyright (C) 2004-2006 Marc Boris Duerner
 * Copyright (C) 2005-2006 Aloysius Indrayanto
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
#include "Pt/Db/Exception.h"


namespace Pt {

namespace Db {

AccessError::AccessError(const std::string& what, const SourceInfo& si) throw()
: std::logic_error(what + si)
{ }


AccessError::~AccessError() throw()
{ }


DatabaseException::DatabaseException( const std::string& what, const std::string& statement, const SourceInfo& si ) throw()
: std::logic_error( what + si )
, m_dbError(DatabaseException::UNSPECIFIED)
, m_statement(statement)
{
    
}
                
DatabaseException::DatabaseException( const std::string& what, const std::string& statement, const DatabaseError& error, const SourceInfo& si ) throw()
: std::logic_error( what + si )
, m_dbError(error)
, m_statement(statement)
{
}
            
DatabaseException::~DatabaseException() throw()
{
}
            
const DatabaseException::DatabaseError& DatabaseException::databaseError() const
{
    return m_dbError;
}

const std::string& DatabaseException::statement() const
{
    return m_statement;
}


} // namespace Db

} // namespace Ptv
