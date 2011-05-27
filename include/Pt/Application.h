/*
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
#ifndef PT_APPLICATION_H
#define PT_APPLICATION_H

#include <Pt/Api.h>
#include <Pt/Connectable.h>
#include <stdexcept>

namespace Pt {

class Application : public Pt::Connectable
{
    public:
        explicit Application(int argc = 0, char** argv = 0)
        : _argc(argc)
        , _argv(argv)
        {
            if( getAppPtr() )
                throw std::logic_error("application already initialized");

            getAppPtr() = this;
        }

        virtual ~Application()
        {
            getAppPtr() = 0;
        }

        static Application& instance()
        {
            Application* app = getAppPtr();
            if( ! app )
                throw std::logic_error("application not initialized");

            return *app;
        }

        int argc() const
        { return _argc; }

        char** argv() const
        { return _argv; }

    private:
        static Application*& getAppPtr()
        {
            static Application* _app = 0;
            return _app;
        }

    private:
        int     _argc;
        char**  _argv;
};

} // namespace Pt

#endif
