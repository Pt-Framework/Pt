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
#ifndef PT_CMA_ITESTSUITE_H
#define PT_CMA_ITESTSUITE_H

#include <Pt/Cma/IUnknown.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/Reporter.h>


namespace Pt {

namespace Cma {

    /**
     * This is the interface used for writing component tests.
     */
    class ITestSuite : public Pt::Cma::IUnknown, public Pt::Unit::TestSuite
    {
    public:

        ITestSuite(const std::string& name)
        : Pt::Unit::TestSuite(name)
        , m_inputDirectory("")
        , m_outputDirectory("")
        {
        }

        virtual ~ITestSuite()
        { }

        static TypeId typeId()
        {
            static TypeId _typeId("ITestSuite");
            return _typeId;
        }

        std::string protocolFileName()
        {
            return m_protocolFileName;
        }

        void setInputDirectory(const std::string& dir)
        {
            m_inputDirectory = dir;
        }

        void setOutputDirectory(const std::string& dir)
        {
            m_outputDirectory = dir;
        }

    protected:
        std::string m_protocolFileName;
        std::string m_inputDirectory;
        std::string m_outputDirectory;
    };

}   // namespace Cma

}   // namespace Pt

#endif  // PT_CMA_ITESTSUITE_H
