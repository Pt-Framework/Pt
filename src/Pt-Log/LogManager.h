/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
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
#ifndef Pt_LogManager_h
#define Pt_LogManager_h

#include "Pt/Settings.h"
#include <Pt/Log/Api.h>
#include <Pt/Log/Target.h>
#include <Pt/Singleton.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Plugin.h>
#include <string>
#include <map>


namespace Pt {

namespace Log {

class Channel;
class Message;
class Logger;


class PT_LOG_API LogManager : public Pt::Singleton<LogManager>
{
    friend class Pt::Singleton<LogManager>;

    protected:
        LogManager();

    public:
        ~LogManager();

        Target& target(const std::string& name = "");

        void setChannel(Target& target, const std::string& url);

        void log(Target& target, const Message& message, bool isAsync = false);

    protected:
        Channel& channel(const std::string& url);

    private:
        Target* _rootTarget;
        std::map<std::string, Target*> _targetMap;
        std::map<std::string, Channel*> _channelMap;
        Pt::System::Mutex _mutex;
        Settings _settings;
        Pt::System::PluginManager<Channel> _pluginManager;
        Logger* _logger;
};


inline bool operator>>(const SerializationData& data, LogManager& manager)
{
    Target& ptLog = Target::get("Pt-Log");
    data >> ptLog;
    return true;
}

}

}

#endif

