/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/Lua/AsyncCall.h>
#include <Pt/Lua/Call.h>
#include <Pt/Lua/Context.h>

#include <lua.hpp>

namespace Pt {

namespace Lua {

AsyncCall::AsyncCall()
: _readyCb(0)
, _readyUser(0)
, _hasError(false)
, _rtype(0)
{
}


AsyncCall::~AsyncCall()
{
}


void AsyncCall::bind(Pt::Reflex::TypeManager& tm)
{
  _rtype = onBind(tm);
}


Pt::Reflex::Type* AsyncCall::rtype() const
{
  return _rtype;
}


void AsyncCall::beginCall(Pt::System::EventLoop& loop,
                        ReadyCallback readyCb, void* user)
{
  _readyCb   = readyCb;
  _readyUser = user;
  onBeginCall(loop);
}


Pt::Any AsyncCall::getResult()
{
  return onGetResult();
}


void AsyncCall::cancel()
{
  onCancel();
}


void AsyncCall::setReady()
{
  if(_readyCb)
    _readyCb(_readyUser);
}


bool AsyncCall::hasError() const
{
  return _hasError;
}


const std::string& AsyncCall::errorMessage() const
{
  return _errorMsg;
}


void AsyncCall::setError(const std::string& msg)
{
  _hasError = true;
  _errorMsg = msg;
}

} // namespace

} // namespace
