/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
 *
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

#ifndef Pt_System_LogManager_h
#define Pt_System_LogManager_h

#include <Pt/System/Api.h>
#include <ConsoleChannel.h>
#include <FileChannel.h>
#include <SerialChannel.h>
#include <Pt/System/LogLevel.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Plugin.h>
#include <Pt/NonCopyable.h>
#include <Pt/Settings.h>
#include <string>
#include <map>

namespace Pt {

namespace System {

class LogTarget;
class LogRecord;

class LogManager : private NonCopyable
{
    protected:
        LogManager();

    public:
        ~LogManager();

        void init(const std::string& path);
        
        void init(const Settings& settings);

        LogTarget& target(const std::string& name = std::string());

        void setPattern(const std::string& pattern)
        { _msgPattern= pattern; }

        std::string getChannel(const LogTarget& target);

        void setChannel(LogTarget& target, const std::string& url);

        void log(LogTarget& target, const LogRecord& record);

        void setLogLevel(LogTarget &target, LogLevel level);

        static LogManager& instance()
        {
            if( ! _instance )
            {
                _instance = new LogManager();
            }

            return *_instance;
        }

        static LogManager& get()
        {
            LogManager& mgr = instance();
            Pt::System::RecursiveLock lock(mgr._mutex);
            ++mgr._loggerCount;
            return mgr;
        }

        static void release()
        {
            LogManager& mgr = instance();
            Pt::System::RecursiveLock lock(mgr._mutex);

            if(0 == --mgr._loggerCount)
            {
                lock.unlock();
                delete _instance;
                _instance = 0;
            }
        }

    protected:
        void initTarget(LogTarget& target, const Settings::ConstEntry& entry);

        void updateChildren(LogTarget &target, LogLevel level);

    private:
        Pt::System::BasicPlugin<ConsoleChannel, LogChannel> _consolePlugin;
        Pt::System::BasicPlugin<FileChannel, LogChannel> _filePlugin;
        Pt::System::BasicPlugin<SerialChannel, LogChannel> _serialPlugin;
        Pt::System::PluginManager<LogChannel> _pluginManager;
        LogTarget* _rootTarget;
        std::map<std::string, LogTarget*> _targetMap;
        std::map<std::string, LogChannel*> _channelMap;
        Pt::System::RecursiveMutex _mutex;
        size_t _loggerCount;
        std::string _msg;
        std::string _msgPattern;

    private:
        static LogManager* _instance;
};

class LogManagerStaticInit
{
    public:
        LogManagerStaticInit()
        {
            LogManager::get();
        }

        ~LogManagerStaticInit()
        {
            LogManager::release();
        }
};

} // namespace System

} // namespace Pt

#endif // Pt_System_LogManager_h

