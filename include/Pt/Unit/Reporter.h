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

		Reporter(std::ostream* outputStream = &std::cerr)
		: outStream(outputStream)
		{
		}

		virtual ~Reporter()
		{
		}

		virtual void message(const std::string& msg)
		{
			*outStream << msg << std::endl;
		}

		virtual void success(const Test& test)
		{
			*outStream << test.name() <<  ": OK."<< std::endl;
		}

		virtual void assertion(const Test& test, const Assertion& a)
		{
			*outStream << test.name() << ": Assertion!" << std::endl;
			*outStream << '\t' << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
			*outStream << '\t' << a.sourceInfo().func() << std::endl;
		}

		virtual void exception(const Test& test, const std::exception& ex)
		{
			*outStream << test.name() << ": Exception!" << std::endl;
			*outStream << '\t' << ex.what() << std::endl;
		}

		virtual void error(const Test& test)
		{
			*outStream << test.name() << ": Error!" << std::endl;
		}


	protected:

		std::ostream* outStream;
	};


	class FileReporter : public Reporter
	{
	public:

		FileReporter(std::string outFileName, std::ofstream::openmode mode = std::ofstream::out | std::ofstream::trunc, bool writeTimestamp = false)
		{
			outFileStream.open(outFileName.c_str(), mode);

			if(outFileStream)
			{
				outStream = &outFileStream;
			}

			else
			{
				outStream = &std::cerr;

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
