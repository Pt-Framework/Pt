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

#include "LogManager.h"
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/DateTime.h>
#include <memory>
#include <fstream>

log_define("Pt.System")

namespace {

inline std::string toString(Pt::System::LogLevel level)
{
    switch (level)
    {
        case Pt::System::None:  return "None";
        case Pt::System::Fatal: return "Fatal";
        case Pt::System::Error: return "Error";
        case Pt::System::Warn:  return "Warning";
        case Pt::System::Info:  return "Info";
        case Pt::System::Debug: return "Debug";
        case Pt::System::Trace: return "Trace";

        default:
            break;
    };

    return "None";
}

inline Pt::System::LogLevel toLogLevel(const std::string& str)
{
    if(str == "None")
    {
        return Pt::System::None;
    }
    else if(str == "Fatal")
    {
        return Pt::System::Fatal;
    }
    else if(str == "Error")
    {
        return Pt::System::Error;
    }
    else if(str == "Warn")
    {
        return Pt::System::Warn;
    }
    else if(str == "Info")
    {
        return Pt::System::Info;
    }
    else if(str == "Debug")
    {
        return Pt::System::Debug;
    }
    else if(str == "Trace")
    {
        return Pt::System::Trace;
    }

    throw std::invalid_argument("invalid log level: " + str);
    return Pt::System::None;
}

}


namespace Pt {

namespace System {

LogManagerStaticInit pt_system_log_manager_static_init;

LogManager* LogManager::_instance = 0;

LogManager::LogManager()
: _consolePlugin("console", "1.0.0")
, _filePlugin("file", "1.0.0")
, _serialPlugin("comm", "1.0.0")
, _rootTarget(0)
, _loggerCount(0)
, _msg(255, ' ')
, _msgPattern("%t [%c] %l - %m")
{
    // builtin plugins
    _pluginManager.registerPlugin( _consolePlugin );
    _pluginManager.registerPlugin( _filePlugin );
    _pluginManager.registerPlugin( _serialPlugin );

    // Set root target to logLevel 'Error' and output channel to 'console://'
    std::auto_ptr<LogTarget> rootTarget( new LogTarget("", 0) );
    _rootTarget = rootTarget.get();
    _targetMap[""] = _rootTarget;
    _rootTarget->assignLogLevel(Pt::System::Error, false);
    this->setChannel( *_rootTarget, "console://");

    rootTarget.release();
}


LogManager::~LogManager()
{
    // target hierachy
    std::map<std::string, LogTarget*>::iterator it;
    for( it = _targetMap.begin(); it != _targetMap.end(); ++it )
    {
        delete it->second;
    }

    _targetMap.clear();

    // channels
    std::map<std::string, LogChannel*>::iterator iter;
    for( iter = _channelMap.begin(); iter != _channelMap.end(); ++iter )
    {
        LogChannel* channel = iter->second;
        if( 0 == channel->unref() )
        {
            channel->close();
            _pluginManager.destroy( channel );
        }
    }

    _channelMap.clear();
}


void LogManager::init(const std::string& path)
{
    Pt::System::RecursiveLock lock( _mutex );

    std::ifstream fs( path.c_str() );
    Pt::TextIStream ts(fs, new Pt::Utf8Codec);
    Settings s;
    s.load(ts);

    const Settings& settings = s;

    Settings::ConstEntry entry;
    for(entry = settings.begin(); entry != settings.end(); ++entry)
    {
        LogTarget& target = this->target( entry.name() );
        this->initTarget(target, entry);
    }
}


void LogManager::init(const Settings& settings)
{
    Pt::System::RecursiveLock lock( _mutex );

    Settings::ConstEntry entry;
    for(entry = settings.begin(); entry != settings.end(); ++entry)
    {
        LogTarget& target = this->target( entry.name() );
        this->initTarget(target, entry);
    }
}


void LogManager::initTarget(LogTarget& target, const Settings::ConstEntry& entry)
{
    Pt::String value;

    if( entry["logLevel"].get(value) )
    {
        LogLevel level = toLogLevel( value.narrow() );
        this->setLogLevel( target, level );
    }

    if( entry["channel"].get(value) )
    {
        this->setChannel( target, value.narrow() );
    }
}


LogTarget& LogManager::target(const std::string& name)
{
    Pt::System::RecursiveLock lock( _mutex );

    // find requested logger amongst the existing ones
    std::map<std::string, LogTarget*>::iterator iter = _targetMap.find(name);
    if( iter != _targetMap.end() )
    {
        return *iter->second;
    }

    // logger needs to be created as a child of an existing logger
    LogTarget* foundTarget = _rootTarget;

    // parse the target name dot syntax
    // ad-hoc parsing code. We might want to replace this with a real
    // parser if it gets more complicated, like allowing wildcards etc
    std::size_t begin = 0;
    std::size_t end = 0;
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
            throw std::invalid_argument("Invalid logger name");
        }

        // insert a '.' before the next token unless its the first
        if( ! targetName.empty() )
        {
            targetName += '.';
        }
        targetName += token;

        // if end + 1 is outside the string we have a string ending with a '.'
        begin = end + 1;
        if( begin >= name.size() )
        {
            throw std::invalid_argument("Invalid logger name");
        }

        // create the logger if not existing. We might want to iterate the
        // LogTarget hierachy directly, but caller knows that this method is
        // costly in either way
        std::map<std::string, LogTarget*>::iterator it = _targetMap.find(targetName);
        if( it != _targetMap.end() )
        {
            foundTarget = it->second;
        }
        else
        {
            // The target inherits the log level of the parent upon construction
            foundTarget = new LogTarget(targetName, foundTarget);
            _targetMap[targetName] = foundTarget;
        }
    }

    return *foundTarget;
}


void LogManager::setLogLevel(LogTarget &t, LogLevel level)
{
    Pt::System::RecursiveLock lock( _mutex );
    t.assignLogLevel(level, false);
    this->updateChildren(t, level);
}


void LogManager::updateChildren(LogTarget& t, LogLevel level)
{
    // Find the direct children of this target and set their LogLevels,
    // if their LogLevels haven't been set explicitely
    const std::string targetName = t.name() + '.';

    // if target name is empty (root target) we must consider all entries
    // in the target map, otherwise jump to the first possible child target
    std::map<std::string, LogTarget*>::iterator it;
    it = t.name().empty() ? _targetMap.begin()
                          : _targetMap.upper_bound(targetName);

    for( ; it != _targetMap.end(); ++it)
    {
        LogTarget* foundTarget = it->second;
        const std::string& childTargetName = foundTarget->name();

        if( t.name().size() > 0 && 0 != childTargetName.find(targetName) )
        {
            break; // target is not a child
        }

        // For each direct child, update the target's log level and descent recursivly.
        // It is a direct child if we do not find another '.' after the parent name
        std::string::size_type endPos = childTargetName.find( '.', targetName.size() );
        if( endPos == std::string::npos && foundTarget->inheritsLogLevel() )
        {
            foundTarget->assignLogLevel( t.logLevel(), true );
            LogManager::instance().updateChildren( *foundTarget, level );
        }
    }
}


std::string LogManager::getChannel(const LogTarget& t)
{
    Pt::System::RecursiveLock lock( _mutex );

    // search target hierachy upwards for a valid channel
    for( const LogTarget* current = &t; current != 0; current = current->parent() )
    {
        if( current->channel() )
        {
            return current->channel()->url();
        }
    }

    return std::string();
}


void LogManager::setChannel(LogTarget& t, const std::string& url)
{
    Pt::System::RecursiveLock lock( _mutex );

    if( ! t.inheritsChannel() )
    {
        std::map<std::string, LogChannel*>::iterator it;
        for(it = _channelMap.begin(); it != _channelMap.end(); ++it)
        {
            LogChannel* channel = it->second;
            if( channel == t.channel() )
            {
                t.removeChannel();

                if( 0 == channel->unref() )
                {
                    channel->close();
                    _pluginManager.destroy( channel );
                    _channelMap.erase(it);
                }

                break;
            }
        }
    }

    LogChannel* ch = 0;

    std::map<std::string, LogChannel*>::iterator it = _channelMap.find(url);
    if( it != _channelMap.end() )
    {
        ch = it->second;
    }
    else
    {
        // use the url schema to create a new channel
        std::size_t colon = url.find(':');
        if(colon == std::string::npos)
        {
            throw  std::invalid_argument("Invalid channel url");
        }

        std::string protocol = url.substr(0, colon);

        ch = _pluginManager.create(protocol);
        if(ch == 0)
        {
            throw std::invalid_argument("No such channel");
        }

        try
        {
            ch->open(url);
            _channelMap[url] =  ch;
        } 
        catch(...)
        {
            _pluginManager.destroy(ch);
            throw;
        }
    }

    ch->ref();
    t.assignChannel( *ch );
}


void LogManager::log(LogTarget& t, const LogRecord& record)
{
    Pt::System::RecursiveLock lock( _mutex );
    Pt::DateTime timeOfLog;
    bool timeOfLogUpdated = false;

    // search target hierachy upwards for a valid channel
    for( LogTarget* current = &t; current != 0; current = current->parent() )
    {
        if( current->channel() )
        {
            _msg.clear();
            
            bool percent = false;
            //const char* c = "%t [%c] %l - %m";

            std::string::const_iterator it;
            
            for( it = _msgPattern.begin(); it != _msgPattern.end(); ++it)
            {
              if(*it == '%')
              {
                percent = true;
                continue;
              }
              if( ! percent)
              {
                _msg += *it;
                continue;
              }

              percent = false;
                
              switch(*it)
              {
                case'l':
                  _msg += toString( record.logLevel() ); 
                  break;
             
                case'd':
                  if( ! timeOfLogUpdated )
                      timeOfLog = System::Clock::getLocalTime();
                  
                  _msg += timeOfLog.date().toIsoString();
                  break; 
                
                case't':
                  if( ! timeOfLogUpdated )
                      timeOfLog = System::Clock::getLocalTime();
                  
                  _msg += timeOfLog.time().toIsoString();
                  break;             
                
                case 'm':
                  _msg += record.text(); 
                  break;
                  
                case 'c':
                  _msg += t.name(); 
                  break;
                  
                default:
                  break;
              }
            }

            _msg += "\n";
            // write data to channel
            current->channel()->write(_msg);
            return;
        }
    }
}

} // namespace System

} // namespace Pt
