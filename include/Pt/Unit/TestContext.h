/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_UNIT_TESTCONTEXT_H
#define PT_UNIT_TESTCONTEXT_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>

#include <string>


namespace Pt {

    class SerializationInfo;

namespace Unit {

    class PT_UNIT_API TestContext
    {
        public:
            TestContext(TestFixture& fixture, Test& test,
                        const SerializationInfo* args = 0, size_t argCount = 0);

            virtual ~TestContext();

            virtual std::string testName() const;

            void run();

        private:
            TestFixture& _fixture;
            Test& _test;
            const SerializationInfo* _args;
            size_t _argCount;
            bool _setUp;
    };

} // namespace Unit

} // namespace Pt

#endif
