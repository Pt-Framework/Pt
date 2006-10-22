/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Dürner                      *
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
#ifndef PT_UNIT_REPORTER_H
#define PT_UNIT_REPORTER_H

#include <Pt/Unit/Assertion.h>

#include <iostream>


namespace Pt {

namespace Unit {

	class Reporter
	{
		public:
			Reporter()
			{}

			virtual ~Reporter()
			{}

			virtual void message(const std::string& msg)
			{
				std::cerr << msg << std::endl;
			}

			virtual void success( const std::string testName)
			{
				std::cerr << testName <<  ": OK."<< std::endl;
			}

			virtual void assertion(const std::string& testName, const Assertion& a)
			{
				std::cerr << testName << ": Assertion!" << std::endl;
				std::cerr << '\t' << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
			}

			virtual void exception( const std::string testName, const std::exception& ex)
			{
				std::cerr << testName << ": Exception!" << std::endl;
				std::cerr << '\t' << ex.what() << std::endl;
			}

			virtual void error(const std::string& testName)
			{
				std::cerr << testName << ": Error!" << std::endl;
			}
	};

} // namespace Unit

} // namespace Pt

#endif
