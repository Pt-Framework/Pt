/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_API_SIGNALS_H
#define PT_API_SIGNALS_H

/** @defgroup Pt-Signals Signals and Delegates

    @brief Type-safe callback mechanism for connecting signals to slots and delegates.

    Callback mechanisms for event handling have become ubiqitous in todays
    application frameworks. Older examples are the use of function pointers as
    callbacks or the message maps found in the MFC toolkit. More modern approaches
    include the .NET delegates and so-called signal-slot techniques. When signals
    and slots are used, objects can communicate with each other by connecting a
    signal of one object to the slot of another object. In most cases connection
    management features are built-in so that an object closes all it connections
    automatically when it gets destroyed. Once the connection has been established,
    all connected slots are called when a signal is send. Connecting signals to
    slots is type-safe i.e. a signal can only be connected to a slot that matches
    the signal's signature. At the same time it allows a great deal of flexibility
    (loose coupling), since the caller has no intimite knowledge of the callee. A
    simple but real example might look like this:

    @code
    int main()
    {
        Pt::System::Application app;

        Pt::System::Timer timer;
        timer.setActive( app.loop() );
        timer.start(1000);
        timer.timeout() += Pt::slot(app, &Pt::System::Application::exit);

        return app.run();
    }
    @endcode

    This program will simply exit, when the timer expires after 1000 ms. The
    application object is the callee and the member function Application::exit
    serves as a slot. The timer is the caller, which has a signal called timeout.

    Non-generic lambdas and function objects derive their slot signature from
    operator(). Generic lambdas use an explicitly stated slot signature. The
    callable must be copyable because a connection clones its slot. A lambda
    slot cannot be removed with operator-= because captured values do not have
    general equality. Retain the returned %Connection and close it when no
    context is supplied.

    @code
    Pt::Signal<int> signal;
    Pt::Connection connection = signal +=
      Pt::slot([this](int) { add(42); });

    Pt::Delegate<int, int> delegate;
    delegate += Pt::slot<int, int>([](auto value) { return value + 1; });
    int result = delegate.call(41);

    connection.close();

    Pt::Connectable context;
  signal += Pt::slot(context, [this](int) { add(42); });
    @endcode

    Destroying the context closes its bound connections automatically. The
    context must outlive every unsafe capture while the connection remains open.
*/

#endif
