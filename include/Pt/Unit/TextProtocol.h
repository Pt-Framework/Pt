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
#ifndef PT_UNIT_TEXTPROTOCOL_H
#define PT_UNIT_TEXTPROTOCOL_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/TestSuite.h>


namespace Pt {

namespace Unit {

/** @brief Implements a simple text protocol.

    The %TextProtocol is used for running tests defined in text
    protocol files.
 */
class PT_UNIT_API TextProtocol : public Pt::Unit::TestProtocol
{
    public:

        /**
         * Constructor.
         *
         * @param path initializes the component test protocol file name
         */
        TextProtocol(const std::string& path)
        : _path(path)
        {
        }

        /**
         * Loads and parses the component test protocol file, then runs the tests on the specified suite.
         *
         * @param suite the test suite to run the tests on
         */
        virtual void run(Pt::Unit::TestSuite& suite);

    private:
        std::string _path;
    };

}   // namespace Unit

}   // namespace Pt


#endif  // PT_UNIT_TEXTPROTOCOL_H
