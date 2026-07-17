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

#ifndef PT_LUA_ASYNCCALL_H
#define PT_LUA_ASYNCCALL_H

#include <Pt/Lua/Api.h>
#include <Pt/Signal.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Any.h>

#include <exception>
#include <string>

namespace Pt {

namespace Lua {

class PT_LUA_API AsyncCall
{
  public:
    AsyncCall();

    virtual ~AsyncCall();

    void bind(Pt::Reflex::TypeManager& tm);

    Pt::Reflex::Type* rtype() const;

    void beginCall(Pt::System::EventLoop& loop);

    Pt::Any getResult();

    Pt::Signal<>& finished();

    void cancel();

    bool hasError() const;

    const std::string& errorMessage() const;

  protected:
    void setReady();

    void setError(const std::string& msg);

  private:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) = 0;

    virtual void onBeginCall(Pt::System::EventLoop& loop) = 0;

    virtual Pt::Any onGetResult() = 0;

    virtual void onCancel() = 0;

  private:
    Pt::Signal<>      _finished;
    bool              _hasError;
    std::string       _errorMsg;
    Pt::Reflex::Type* _rtype;
};


template <typename R>
class BasicAsyncCall : public AsyncCall
{
  protected:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) override
    {
      return tm.getType( typeid(R) );
    }

    virtual R onResult() = 0;

  private:
    Pt::Any onGetResult() override
    {
      R r = onResult();
      return Pt::Any(r);
    }
};


template <>
class BasicAsyncCall<void> : public AsyncCall
{
  protected:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) override
    {
      return tm.getType( typeid(void) );
    }

  private:
    Pt::Any onGetResult() override
    { return Pt::Any(); }
};

} // namespace

} // namespace

#endif // PT_LUA_ASYNCCALL_H
