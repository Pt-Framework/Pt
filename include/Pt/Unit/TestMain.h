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
#include<Pt/Unit/Reporter.h>
#include<Pt/Unit/TestCase.h>

#include <cstring>


int main(int argc, char** argv)
{
  // CppUnit(mini) test launcher
  // command line option syntax:
  // test [OPTIONS]
  // where OPTIONS are
  //  -t=CLASS[::TEST]    run the test class CLASS or member test CLASS::TEST
  //  -x=CLASS[::TEST]    run all except the test class CLASS or member test CLASS::TEST
  //  -f=FILE             save output in file FILE instead of stdout

	int num_errors = 0;
	char* fileName = 0;
	char* testName = "";
	char* xtestName = "";
	
	for(int i = 1; i < argc; ++i)
	{
		if(argv[i][0] != '-')
			break;

		if( !strncmp(argv[i], "-t=", 3) ) 
		{
			testName = argv[i] + 3;
		}
		else if( !strncmp(argv[i], "-f=", 3) ) 
		{
			fileName = argv[i] + 3;
		}
		else if ( !strncmp(argv[i], "-x=", 3) ) 
		{
			xtestName = argv[i] + 3;
		}
	}

	num_errors = Pt::Unit::TestCase::start(std::cerr);

	//reporter->printSummary();
	//delete reporter;

	return num_errors;
}
