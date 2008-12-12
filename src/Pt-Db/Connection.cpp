/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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
#include "Pt/Db/Connection.h"
#include "Pt/Db/Result.h"
#include "Pt/Db/Row.h"
#include "Pt/Db/Value.h"
#include "Pt/Db/Statement.h"
#include "sqlite/Connector.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Db {

    void Connection::beginTransaction()
    {
        _connection->beginTransaction();
    }

    void Connection::commitTransaction()
    {
        _connection->commitTransaction();
    }

    void Connection::rollbackTransaction()
    {
        _connection->rollbackTransaction();
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
        return _connection->execute(query);
    }

    Result Connection::select(const std::string& query)
    {
        return _connection->select(query);
    }

    Row Connection::selectRow(const std::string& query)
    {
        return _connection->selectRow(query);
    }

    Value Connection::selectValue(const std::string& query)
    {
        return _connection->selectValue(query);
    }

    Statement Connection::prepare(const std::string& query)
    {
        return _connection->prepare(query);
    }

    Statement Connection::prepareCached(const std::string& query)
    {
        return _connection->prepareCached(query);
    }

    Statement IStmtCacheConnection::prepareCached(const std::string& query)
    {
        StatementCache::iterator it = _stmtCache.find(query);

        if (it == _stmtCache.end())
        {
            Statement stmt = prepare(query);
            IStatement* istmt = const_cast<IStatement*>(stmt.getImpl());
            _stmtCache.insert(StatementCache::value_type(query, istmt));
            return stmt;
        }
        else
        {
            return Statement(it->second);
        }
    }

    void IStmtCacheConnection::clearStatementCache()
    {
        _stmtCache.clear();
    }

    long long Connection::insertId() {
        return _connection->insertId();
    }



  //typedef std::map<std::string, LibraryManager> librariesType;
  //static librariesType libraries;

  Connection connect(const std::string& url)
  {
    //log_debug("connect(\"" << url << "\")");

    std::string::size_type n = url.find(':');

/*    if (n == std::string::npos)
#ifdef WITH_STATIC_POSTGRESQL
      return connectionManager_postgresql.connect(url);
#elif WITH_STATIC_MYSQL
      return connectionManager_mysql.connect(url);
#elif WITH_STATIC_SQLITE
      return connectionManager_sqlite.connect(url);
#else
      throw Error("invalid url \"" + url + '"');
#endif*/

    /*if (n == std::string::npos)
        return connectionManager_sqlite.connect(url);*/

    std::string driverName =  url.substr(0, n);

/*#ifdef WITH_STATIC_POSTGRESQL
    if (driverName == "postgresql")
      return connectionManager_postgresql.connect(url.substr(n + 1));
#elif WITH_STATIC_MYSQL
    if (driverName == "mysql")
      return connectionManager_mysql.connect(url.substr(n + 1));
#elif WITH_STATIC_SQLITE
    if (driverName == "sqlite")
      return connectionManager_sqlite.connect(url.substr(n + 1));
#endif*/

    /*if (driverName == "sqlite")
      return connectionManager_sqlite.connect(url.substr(n + 1));*/

    if(driverName == "sqlite") {
        Pt::Db::sqlite::Connector ctor;
        return ctor.connect( url.substr(n + 1) );
    }

    throw std::logic_error("No DB driver found for " + driverName + PT_SOURCEINFO);

    /*std::string libraryUrl = url.substr(n + 1);
    //log_debug("driver \"" << driverName << "\" url=\"" << libraryUrl << '"');

    // lookup library-manager
    LibraryManager libraryManager;
    librariesType::const_iterator it = libraries.find( driverName );
    if (it == libraries.end())
    {
      libraryManager = LibraryManager(driverName);
      libraries[driverName] = libraryManager;
    }
    else
      libraryManager = it->second;

    return libraryManager.connect(libraryUrl);*/
  }

  /*static ConnectionPool connectionPool;

  Connection connectCached(const std::string& url)
  {
    log_debug("connectCached(\"" << url << "\")");
    return connectionPool.connect(url);
  }

  void dropCached(unsigned keep)
  {
    connectionPool.drop(keep);
  }

  void dropCached(const std::string& url, unsigned keep)
  {
    connectionPool.drop(url, keep);
  }
  */

} // namespace Db

} // namespace Pt

