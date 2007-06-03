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
