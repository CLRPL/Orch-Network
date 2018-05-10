//******************************************************************************
//******************************************************************************

#include "logger.h"
#include "settings.h"
#include "xbridge/xuiconnector.h"

#include "util.h"

#include <string>
#include <sstream>
#include <fstream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

boost::mutex logLocker;

//******************************************************************************
//******************************************************************************
// static
std::string LOG::m_logFileName;

//******************************************************************************
//******************************************************************************
LOG::LOG(const char reason)
    : std::basic_stringstream<char, std::char_traits<char>,
                    boost::pool_allocator<char> >()
    , m_r(reason)
{
    *this << "\n" << "[" << (char)std::toupper(m_r) << "] "
          << boost::posix_time::second_clock::local_time()
          << " [0x" << boost::this_thread::get_id() << "] ";
}

//******************************************************************************
//******************************************************************************
// static
std::string LOG::logFileName()
{
    return m_logFileName;
}

//******************************************************************************
//******************************************************************************
LOG::~LOG()
{
    boost::lock_guard<boost::mutex> lock(logLocker);

    // const static std::string path     = settings().logPath().size() ? settings().logPath() : settings().appPath();
    const static bool logToFile       = true; // !path.empty();
    static boost::gregorian::date day =
            boost::gregorian::day_clock::local_day();
    if (m_logFileName.empty())
    {
        m_logFileName    = makeFileName();
    }

    // std::cout << str().c_str();

    try
    {
        if (logToFile)
        {
            boost::gregorian::date tmpday =
                    boost::gregorian::day_clock::local_day();

            if (day != tmpday)
            {
                m_logFileName = makeFileName();
                day = tmpday;
            }

            std::ofstream file(m_logFileName.c_str(), std::ios_base::app);
            file << str().c_str();
        }
    }
    catch (std::exception &)
    {
    }
}

//******************************************************************************
//******************************************************************************
// static
std::string LOG::makeFileName()
{
    boost::filesystem::path directory = GetDataDir(false) / "log";
    boost::filesystem::create_directory(directory);

    return directory.string() + "/" +
            "xbridgep2p_" +
            boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) +
            ".log";
}
