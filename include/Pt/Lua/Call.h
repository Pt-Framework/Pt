/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_CALL_H
#define PT_LUA_CALL_H

#include <Pt/Lua/Api.h>
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/PropertyInfo.h>
#include <Pt/Reflex/ConstructorInfo.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Any.h>

#include <string>
#include <vector>

namespace Pt {

namespace Lua {

/** @brief Synchronous native call from Lua.

    %Call is the work a %Script runs when Lua invokes a reflected
    method, property, or constructor. The script owns the call and
    deletes it after %call() returns. Application code does not
    construct a %Call; the binding closures do.

    %call() performs the invocation and returns the result as
    %Any. A thrown %std::exception is stored as an error:
    %hasError() is true and %errorMessage() holds the text.
    %rtype() is the Reflex result type, or a null pointer when
    the call has no result to push.

    @ingroup Pt-Lua-Calls
*/
class Call
{
  public:
    /** @brief Destroys the call.
    */
    virtual ~Call()
    {}

    /** @brief Performs the invocation and returns the result.
    */
    virtual Pt::Any call() = 0;

    /** @brief Returns the Reflex result type, or a null pointer.
    */
    virtual Pt::Reflex::Type* rtype() const = 0;

    /** @brief Returns true when the invocation stored an error.
    */
    bool hasError() const
    { return ! _errorMsg.empty(); }

    /** @brief Returns the stored error text.
    */
    const std::string& errorMessage() const
    { return _errorMsg; }

  protected:
    Call() = default;

    Call(const Call&) = delete;

    Call& operator=(const Call&) = delete;

    /** @brief Stores @a msg as the error of this call.
    */
    void setError(const std::string& msg)
    { _errorMsg = msg; }

  private:
    std::string _errorMsg;
};


/** @brief Reflected method invocation from Lua.

    @ingroup Pt-Lua-Calls
*/
class MethodCall : public Call
{
  public:
    /** @brief Creates a call of @a mi on @a self with @a args.
    */
    MethodCall(Pt::Reflex::MethodInfo* mi, void* self,
               std::vector<Pt::Reflex::Argument> args)
    : _mi(mi)
    , _self(self)
    , _args(args)
    {}

    Pt::Any call() override
    {
      try
      {
        Pt::Reflex::ArgumentList arglist(
          _args.empty() ? static_cast<Pt::Reflex::Argument*>(0) : &_args[0],
          _args.size());
        return _mi->call(_self, arglist);
      }
      catch(const std::exception& e) { setError(e.what()); }

      return Pt::Any();
    }

    Pt::Reflex::Type* rtype() const override
    { return &_mi->rtype(); }

  private:
    Pt::Reflex::MethodInfo*           _mi;
    void*                             _self;
    std::vector<Pt::Reflex::Argument> _args;
};


/** @brief Reflected property read from Lua.

    @ingroup Pt-Lua-Calls
*/
class PropertyGetCall : public Call
{
  public:
    /** @brief Creates a read of @a pi on @a self.
    */
    PropertyGetCall(Pt::Reflex::PropertyInfo* pi, void* self)
    : _pi(pi)
    , _self(self)
    {}

    Pt::Any call() override
    {
      try { return _pi->get(_self); }
      catch(const std::exception& e) { setError(e.what()); }

      return Pt::Any();
    }

    Pt::Reflex::Type* rtype() const override
    { return &_pi->type(); }

  private:
    Pt::Reflex::PropertyInfo* _pi;
    void*                     _self;
};


/** @brief Reflected property write from Lua.

    @ingroup Pt-Lua-Calls
*/
class PropertySetCall : public Call
{
  public:
    /** @brief Creates a write of @a value to @a pi on @a self.
    */
    PropertySetCall(Pt::Reflex::PropertyInfo* pi, void* self,
                    Pt::Reflex::Argument value)
    : _pi(pi)
    , _self(self)
    , _value(value)
    {}

    Pt::Any call() override
    {
      try { _pi->set(_self, _value.toAny(), _value.type()); }
      catch(const std::exception& e) { setError(e.what()); }

      return Pt::Any();
    }

    Pt::Reflex::Type* rtype() const override
    { return 0; }

  private:
    Pt::Reflex::PropertyInfo* _pi;
    void*                     _self;
    Pt::Reflex::Argument      _value;
};


/** @brief Reflected constructor invocation from Lua.

    @ingroup Pt-Lua-Calls
*/
class ConstructorCall : public Call
{
  public:
    /** @brief Creates a construction of @a instance with @a ci and @a args.
    */
    ConstructorCall(Pt::Reflex::ConstructorInfo* ci, void* instance,
                    std::vector<Pt::Reflex::Argument> args)
    : _ci(ci)
    , _instance(instance)
    , _args(args)
    {}

    Pt::Any call() override
    {
      try
      {
        Pt::Reflex::ArgumentList arglist(
          _args.empty() ? static_cast<Pt::Reflex::Argument*>(0) : &_args[0],
          _args.size());
        _ci->call(_instance, arglist);
      }
      catch(const std::exception& e) { setError(e.what()); }

      return Pt::Any();
    }

    Pt::Reflex::Type* rtype() const override
    { return 0; }

  private:
    Pt::Reflex::ConstructorInfo*      _ci;
    void*                             _instance;
    std::vector<Pt::Reflex::Argument> _args;
};

} // namespace

} // namespace

#endif // PT_LUA_CALL_H
