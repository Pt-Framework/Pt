/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_API_LUA_H
#define PT_LUA_API_LUA_H

/** @defgroup Pt-Lua Lua Scripting

    @brief Lua runtime, reflected bindings and script execution.

    This module embeds a Lua runtime against a catalog of reflected
    C++ types, so a caller runs Lua source that constructs those
    types, calls their methods, and reads their properties without
    writing Lua C API glue for each binding. It is not a wrapper
    whose unit of work is a raw lua_State. The unit of work is a
    %Script, which loads source into a %Context that already holds
    the bindings.

    A %TypeManager is the catalog. It is a %Pt::Reflex::TypeManager
    that already knows the scalar types Lua can convert (int, long,
    float, double, bool, and string), void, and %AsyncCall*. User
    types and functions are registered with the Reflex API before a
    %Context is constructed. How a C++ type declares constructors,
    methods, and properties is the Reflex module; this module binds
    that catalog into Lua.

    %Context opens a Lua state, loads the standard libraries, and
    binds every eligible type and asynchronous function in the
    catalog, including entries that live on a parent type manager.
    A bound type becomes a Lua global of that type's name.
    Calling the global constructs a userdata instance. Methods use
    the Lua method syntax, and properties are fields. Scalar
    arguments and results convert to and from Lua booleans, numbers,
    and strings. A result that is a registered object is
    copy-constructed into userdata that Lua owns.

    %Script loads source as a coroutine of that context. Only one
    script may use a context at a time. The script is a
    %Selectable. Blocking %advance() runs it on the calling thread
    until the next yield, native call, success, or error.
    Asynchronous work attaches the script with %setActive() so an
    %EventLoop can drive %beginAdvance() and %endAdvance(), and
    %advanced() reports each step. The loop does not own the
    script: the code that creates it keeps it alive while a step
    is still waiting. A C++20 awaitable wraps the same
    asynchronous step.

    When Lua calls a reflected method, property, or constructor,
    the script reports %NativeCall and runs a %Call. When Lua
    calls a function or method that returns %AsyncCall*, the
    script starts that call on the event loop and resumes when
    the call is ready. Blocking %advance() cannot start an
    asynchronous call.

    After %ScriptOk, %Result reads a named Lua global from the
    context's state. A name that is already bound at construction
    throws %std::logic_error. A script or native failure is
    %ScriptError, and %errorMessage() holds the text.

    The example registers a Point type, binds it, runs a chunk
    that constructs a point and calls sum, and reads the result.

    @code
    struct Point
    {
        Point(int x_, int y_)
        : x(x_)
        , y(y_)
        { }

        int x;
        int y;
    };

    class PointType : public Pt::Reflex::BasicType<Point>
    {
      public:
        PointType()
        : Pt::Reflex::BasicType<Point>("Point")
        { }

        void define(Pt::Reflex::TypeManager& tm)
        {
            this->registerConstructor(tm, *this, &PointType::construct);
            this->registerMethod(tm, "sum", &sum);
        }

      private:
        void construct(void* mem, int x, int y)
        { new (mem) Point(x, y); }

        static int sum(Point& p)
        { return p.x + p.y; }
    };

    Pt::Lua::TypeManager tm;
    PointType pointType;
    tm.registerType(pointType);
    pointType.define(tm);

    Pt::Lua::Context ctx(tm);
    Pt::Lua::Script script(ctx,
        "local p = Point(3, 4)\n"
        "result = p:sum()\n");

    Pt::Lua::Script::Status status = script.advance();
    while(status == Pt::Lua::Script::Yield ||
          status == Pt::Lua::Script::NativeCall)
    {
        status = script.advance();
    }

    if(status == Pt::Lua::Script::ScriptError)
        throw std::runtime_error(script.errorMessage());

    Pt::Lua::Result result(ctx.state());
    int sum = result.get("result");
    @endcode

    The rest of this chapter is the runtime and bindings, then
    scripts, then native calls.
*/

/** @defgroup Pt-Lua-Runtime Runtime and Bindings

    @ingroup Pt-Lua
*/

/** @defgroup Pt-Lua-Scripts Scripts

    @ingroup Pt-Lua
*/

/** @defgroup Pt-Lua-Calls Native Calls

    @ingroup Pt-Lua
*/

#endif
