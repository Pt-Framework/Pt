/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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

#include <Pt/System/Clock.h>
#include <Pt/Unit/Assertion.h>

#include <iostream>
#include <fstream>
#include <sstream>


namespace Pt {

namespace Unit {

	class Reporter
	{
	public:
		Reporter(std::ostream* out = &std::cerr)
		: _out(out)
		{}

		virtual ~Reporter()
		{}

		virtual void started(const Test& test)
		{
			*_out << test.name() << ": ";
		}

		virtual void finished(const Test& test)
		{
			//*_out << std::endl;
		}

		virtual void message(const std::string& msg)
		{
			*_out << msg << std::endl;
		}

		virtual void success(const Test& test)
		{
			*_out << "OK"<< std::endl;
		}

		virtual void assertion(const Test& test, const Assertion& a)
		{
			*_out << "ASSERTION" << std::endl;
			*_out << '\t' << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
			*_out << '\t' << a.sourceInfo().func() << std::endl;
		}

		virtual void exception(const Test& test, const std::exception& ex)
		{
			*_out << test.name() << "EXCEPTION" << std::endl;
			*_out << '\t' << ex.what() << std::endl;
		}

		virtual void error(const Test& test)
		{
			*_out << test.name() << ": ERROR" << std::endl;
		}

	protected:
		std::ostream* _out;
	};

    //TODO: is this obsolete? Andreas suspected so before and I have a 
    //       feeling that he is right.
    //       -Marc
	class FileReporter : public Reporter
	{
	public:

		FileReporter(std::string outFileName, std::ofstream::openmode mode = std::ofstream::out | std::ofstream::trunc, bool writeTimestamp = false)
		{
			outFileStream.open(outFileName.c_str(), mode);

			if(outFileStream)
			{
				_out = &outFileStream;
			}

			else
			{
				_out = &std::cerr;

				std::cerr << std::endl;
				std::cerr << "FileReporter::FileReporter(std::string outFileName)" << std::endl;
				std::cerr << "--> Cannot open out file: " << outFileName << std::endl;
				std::cerr << "--> Using std::cerr instead" << std::endl;
				std::cerr << std::endl;
			}


			if(writeTimestamp)
			{
				std::stringstream msg;

				msg << std::endl;
				msg << std::endl;
				msg << "--------------------------------------------------" << std::endl;
				msg << outFileName << std::endl;
				msg << "Test started: " << Pt::System::Clock::getCurrentTime().toIsoString() << std::endl;
				//msg << "Test started: " << "TODO: Insert current time" << std::endl;
				msg << "--------------------------------------------------" << std::endl;
				msg << std::endl;

				message(msg.str());
			}
		}


		virtual ~FileReporter()
		{
			if(outFileStream)
			{
				outFileStream.close();
			}
		}


	private:

		std::ofstream outFileStream;
	};

} // namespace Unit

} // namespace Pt

#endif
