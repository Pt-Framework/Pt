/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

/** @brief Asynchronous native call from Lua.

    %AsyncCall is native work that Lua starts and that finishes
    later on an %EventLoop. A reflected function or method whose
    return type is %AsyncCall* returns a heap instance; the
    %Script takes ownership, binds it, starts it, and resumes
    when %finished() is emitted.

    Subclass %BasicAsyncCall rather than this type. The script
    calls %bind() with the context's type manager so %rtype() is
    known, then %beginAdvance() on the loop, which reaches
    %onBeginCall(). When the work is done, %setReady() emits
    %finished(). %setError() records a failure instead. The
    script then calls %getResult() and pushes it to Lua, or
    becomes %ScriptError. %cancel() reaches %onCancel().

    @ingroup Pt-Lua-Calls
*/
class PT_LUA_API AsyncCall
{
  public:
    /** @brief Creates an idle asynchronous call.
    */
    AsyncCall();

    /** @brief Destroys the call.
    */
    virtual ~AsyncCall();

    /** @brief Binds the result type from @a tm.
    */
    void bind(Pt::Reflex::TypeManager& tm);

    /** @brief Returns the Reflex result type, or a null pointer before bind.
    */
    Pt::Reflex::Type* rtype() const;

    /** @brief Starts the call on @a loop.
    */
    void beginCall(Pt::System::EventLoop& loop);

    /** @brief Returns the result after the call has finished.
    */
    Pt::Any getResult();

    /** @brief Signal emitted when the call is ready or has failed.
    */
    Pt::Signal<>& finished();

    /** @brief Cancels the call.
    */
    void cancel();

    /** @brief Returns true when the call stored an error.
    */
    bool hasError() const;

    /** @brief Returns the stored error text.
    */
    const std::string& errorMessage() const;

  protected:
    /** @brief Emits %finished() to mark the call ready.
    */
    void setReady();

    /** @brief Stores @a msg as the error of this call.
    */
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


/** @brief Asynchronous native call with result type @a R.

    %BasicAsyncCall looks up typeid(R) at bind time. Override
    %onBeginCall() to start work on the loop, %onResult() to
    produce R, and %onCancel() to abort. Call %setReady() or
    %setError() when the work ends.

    @ingroup Pt-Lua-Calls
*/
template <typename R>
class BasicAsyncCall : public AsyncCall
{
  protected:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) override
    {
      return tm.getType( typeid(R) );
    }

    /** @brief Returns the result of the finished call.
    */
    virtual R onResult() = 0;

  private:
    Pt::Any onGetResult() override
    {
      R r = onResult();
      return Pt::Any(r);
    }
};


/** @brief Asynchronous native call with no result.

    Override %onBeginCall() and %onCancel(). There is no
    %onResult(). Call %setReady() or %setError() when the work
    ends.

    @ingroup Pt-Lua-Calls
*/
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
