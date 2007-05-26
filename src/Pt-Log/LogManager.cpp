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
#include "LogManager.h"
#include "ConsoleChannel.h"
#include "SerialChannel.h"
#include "Pt/Exception.h"
#include "Pt/System/MutexLock.h"
#include <Pt/Log/Target.h>
#include <Pt/Log/Logger.h>
#include <memory>


static Pt::System::BasicPlugin<Pt::Log::ConsoleChannel, Pt::Log::Channel> consolePlugin("console", "0.0.1");
static Pt::System::BasicPlugin<Pt::Log::SerialChannel, Pt::Log::Channel> serialPlugin("comm", "0.0.1");


namespace Pt {

namespace Log {


LogManager::LogManager()
: _rootTarget(0)
, _logger(0)
{
    // builtin plugins
    _pluginManager.registerPlugin( consolePlugin );
    _pluginManager.registerPlugin( serialPlugin );

    // root of the target hierachy
    std::auto_ptr<Target> rootTarget( new Target("", 0) );
    _rootTarget = rootTarget.get();
    _targetMap[""] = _rootTarget;

    // logger for Pt::Log
    std::auto_ptr<Target> logTarget( new Target("Pt-Log", _rootTarget) );
    _targetMap["Pt-Log"] = logTarget.get();

    //
    // TODO: initialise Targets with properties
    //

    _logger = new Logger( *logTarget );
    *_logger << info << "Logging system initialized" << endlog;

    logTarget.release();
    rootTarget.release();
}


LogManager::~LogManager()
{
    *_logger << info << "Logging system shutdown" << endlog;

    // logger for Pt::Log
    delete _logger;
    _logger = 0;

    // target hierachy
    std::map<std::string, Target*>::iterator it;
    for( it = _targetMap.begin(); it != _targetMap.end(); ++it )
    {
        delete it->second;
    }

    _targetMap.clear();

    // channels
    std::map<std::string, Channel*>::iterator iter;
    for( iter = _channelMap.begin(); iter != _channelMap.end(); ++iter )
    {
        iter->second->close();
        _pluginManager.destroy( iter->second );
    }

    _channelMap.clear();
}


Target& LogManager::target(const std::string& name)
{
    Pt::System::MutexLock lock( _mutex );

    PT_LOG_LOGGEDSCOPE(*_logger, Pt::Log::Trace);

    // find requested logger amongst the existing ones
    std::map<std::string, Target*>::iterator it = _targetMap.find(name);
    if( it != _targetMap.end() )
    {
        return *it->second;
    }

    // logger needs to be created as a child of an existing logger
    Target* foundTarget = _rootTarget;

    // ad-hoc parsing code. We might want to replace this with a state
    // machine if it gets more complicated, like allowing wildcards etc
    size_t begin = 0;
    size_t end = 0;
    std::string targetName;
    std::string token;
    while(end != std::string::npos)
    {
        // get next token either until '.' or rest if the string
        end = name.find('.', begin);
        if(end == std::string::npos)
        {
            token = name.substr( begin );
        }
        else
        {
            token = name.substr( begin, end - begin );
        }

        if( token.empty() )
        {
            *_logger << error << "invalid target: " << name << endlog;
            throw std::invalid_argument("Invalid logger name" + PT_SOURCEINFO);
        }

        // insert a '.' before the next token unless its the first
        if( !targetName.empty() )
        {
            targetName += ".";
        }
        targetName += token;

        // if end + 1 is outside the string we have a string ending with a '.'
        begin = end + 1;
        if( begin >= name.size() )
        {
            *_logger << error << "invalid target: " << name << endlog;
            throw std::invalid_argument("Invalid logger name" + PT_SOURCEINFO);
        }

        // create the logger if not existing. We might want to iterate the
        // Target hierachy directly, but caller knows that this method is
        // costly in either way
        std::map<std::string, Target*>::iterator it = _targetMap.find(targetName);
        if( it != _targetMap.end() )
        {
            *_logger << debug << "Found target: " << targetName << endlog;
            foundTarget = it->second;
        }
        else
        {
            *_logger << info << "New target: " << targetName << ", parent: " << foundTarget->name() << endlog;
            foundTarget = new Target(targetName, foundTarget);
            _targetMap[targetName] = foundTarget;
        }
    }

    return *foundTarget;
}


void LogManager::setChannel(Target& target, const std::string& url)
{
    Pt::System::MutexLock lock( _mutex );

    PT_LOG_LOGGEDSCOPE(*_logger, Pt::Log::Trace);

    *_logger << info << target.name()<< " set to " << url << endlog;

    Channel& chan = this->channel(url);
    target.assignChannel( chan );
}


void LogManager::log(Target& target, const Message& message, bool isAsync)
{
    Pt::System::MutexLock lock( _mutex );

    Target* current = &target;
    while( current != 0 )
    {
        if( current->_channel )
        {
            current->_channel->write(message, isAsync);
            return;
        }
        current = current->_parent;
    }
}


Channel& LogManager::channel(const std::string& url)
{
    PT_LOG_LOGGEDSCOPE(*_logger, Pt::Log::Trace);

    std::map<std::string, Channel*>::iterator it = _channelMap.find(url);
    if( it != _channelMap.end() )
    {
        return *it->second;
    }

    size_t colon = url.find(':');
    if(colon == std::string::npos)
    {
        *_logger << error << "invalid channel url: " << url << endlog;
        throw  std::invalid_argument("Invalid channel url" + PT_SOURCEINFO);
    }

    std::string protocol = url.substr(0, colon);

    Channel* ch = _pluginManager.create(protocol);
    if(ch == 0)
    {
        *_logger << error << "No such channel: " << url << endlog;
        throw std::invalid_argument("No such channel" + PT_SOURCEINFO);
    }

    ch->open(url);
    *_logger << info << "Opened channel: " << url << endlog;

    _channelMap[url] =  ch;
    return *ch;
}

}

}
