/************************************************************************************
   Copyright (C) 2020 MariaDB Corporation AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not see <http://www.gnu.org/licenses>
   or write to the Free Software Foundation, Inc.,
   51 Franklin St., Fifth Floor, Boston, MA 02110, USA
*************************************************************************************/


#ifndef _CLIENTSIDEPREPAREDSTATEMENT_H_
#define _CLIENTSIDEPREPAREDSTATEMENT_H_

#include "Consts.h"

#include "BasePrepareStatement.h"
#include "MariaDbStatement.h"

#include "parameters/ParameterHolder.h"

namespace sql
{
namespace mariadb
{

class ClientSidePreparedStatement : public BasePrepareStatement
{
  static const Shared::Logger logger ; /*LoggerFactory.getLogger(typeid(ClientSidePreparedStatement))*/
  std::vector<std::vector<Shared::ParameterHolder>> parameterList;
  Shared::ClientPrepareResult prepareResult;
  SQLString sqlQuery;
  std::vector<Shared::ParameterHolder> parameters;
  Shared::ResultSetMetaData resultSetMetaData; /*NULL*/
  Shared::ParameterMetaData parameterMetaData ; /*NULL*/

  ClientSidePreparedStatement(
    MariaDbConnection* connection,
    int32_t resultSetScrollType,
    int32_t resultSetConcurrency,
    int32_t autoGeneratedKeys,
    Shared::ExceptionFactory& factory);
public:
  ClientSidePreparedStatement(
    MariaDbConnection* connection, const SQLString& sql,
    int32_t resultSetScrollType,
    int32_t resultSetConcurrency,
    int32_t autoGeneratedKeys,
    Shared::ExceptionFactory& factory);

  ClientSidePreparedStatement* clone(MariaDbConnection* connection);

  /* Need to define overloaded methods*/
  void addBatch(const SQLString& sql) { BasePrepareStatement::addBatch(sql); }

protected:
  bool executeInternal(int32_t fetchSize);

public:
  void addBatch();
  void clearBatch();
  sql::Ints& executeBatch();
  sql::Ints& getServerUpdateCounts();
  sql::Longs& executeLargeBatch();

private:
  void executeInternalBatch(std::size_t size);

public:
  sql::ResultSetMetaData* getMetaData();
  void setParameter(int32_t parameterIndex, ParameterHolder* holder);
  ParameterMetaData* getParameterMetaData();

private:
  void loadParametersData()
    ;
public:
  void clearParameters();
  void close();
  uint32_t getParameterCount();
  SQLString toString();

protected:
  ClientPrepareResult* getPrepareResult();
};
}
}
#endif
