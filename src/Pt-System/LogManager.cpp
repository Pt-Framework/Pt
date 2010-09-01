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
#include <Pt/System/LogTarget.h>
#include <Pt/System/Logger.h>
#include <Pt/Text/TextStream.h>
#include <Pt/Text/Utf8Codec.h>
 #include <Pt/System/Clock.h>
#include <memory>
#include <fstream>

log_define("Pt.System")

namespace Pt {

namespace System {

LogManager::LogManager()
: _consolePlugin("console", "1.0.0")
, _filePlugin("file", "1.0.0")
, _serialPlugin("comm", "1.0.0")
, _rootTarget(0)
//, _logger(0)
{
    // builtin plugins
    _pluginManager.registerPlugin( _consolePlugin );
    _pluginManager.registerPlugin( _filePlugin );
    _pluginManager.registerPlugin( _serialPlugin );

    // initialise completely if .settings exist for backward
    // compatibility wit old code
    //std::ifstream fs("Pt-Log.settings");
    //if( fs )
    //{
    //    Pt::Text::TextIStream ts(fs, new Pt::Text::Utf8Codec);
    //    _settings.load(ts);
    //    _init = true;
    //}

    // Set root target to logLevel 'Error' and output channel to 'console://'
    std::auto_ptr<LogTarget> rootTarget( new LogTarget("", 0) );
    _rootTarget = rootTarget.get();
    _targetMap[""] = _rootTarget;
    _rootTarget->assignLogLevel(Pt::System::Error, false);
    this->setChannel( *_rootTarget, "console://");

    //if(_init)
    //    _settings >>= *_rootTarget;

    //// logger for Pt::Log
    //std::auto_ptr<LogTarget> logTarget( new LogTarget("Pt-Log", _rootTarget) );
    //_targetMap["Pt-Log"] = logTarget.get();

    //if(_init)
    //    _settings.getObject( *logTarget, "Pt-Log" );

    //std::auto_ptr<Logger> logger( new Logger( *logTarget ) );
    //_logger = logger.get();

    //_logger->info(PT_SOURCEINFO) << "Logging system initialized" << endlog;

    //logger.release();
    //logTarget.release();
    rootTarget.release();
}


LogManager::~LogManager()
{
    //_logger->info(PT_SOURCEINFO) << "Logging system shutdown" << endlog;

    //logger for Pt::Log
    //delete _logger;
    //_logger = 0;

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
        iter->second->close();
        _pluginManager.destroy( iter->second );
    }

    _channelMap.clear();
}


void LogManager::init(const std::string& path)
{
    Pt::System::RecursiveLock lock( _mutex );

    std::ifstream fs( path.c_str() );
    Pt::Text::TextIStream ts(fs, new Pt::Text::Utf8Codec);
    _settings.load(ts);

    Settings::ConstEntry entry;
    for(entry = _settings.begin(); entry != _settings.end(); ++entry)
    {
        //std::map<std::string, LogTarget*>::iterator iter = _targetMap.find( entry.name() );
        //if( iter == _targetMap.end() )
        //    continue;

        //LogTarget* target = iter->second;
        //this->initTarget(*target, entry);
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
    std::map<std::string, LogTarget*>::iterator it = _targetMap.find(name);
    if( it != _targetMap.end() )
    {
        return *it->second;
    }

    // logger needs to be created as a child of an existing logger
    LogTarget* foundTarget = _rootTarget;

    // parse the target name dot syntax
    // ad-hoc parsing code. We might want to replace this with a real
    // parser if it gets more complicated, like allowing wildcards etc
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
            //_logger->beginLog(PT_SOURCEINFO) << debug << "Found target: " << targetName << endlog;
            foundTarget = it->second;
        }
        else
        {
            //_logger->beginLog(PT_SOURCEINFO) << info << "New target: " << targetName
            //                                 << ", parent: " << foundTarget->name() << endlog;

            // The target inherits the log level of the parent upon contruction
            foundTarget = new LogTarget(targetName, foundTarget);
            _targetMap[targetName] = foundTarget;

            // The settings for the target might override the inherited log level
            //this->initTarget(*foundTarget, _settings.entry(targetName) );
        }
    }

    return *foundTarget;
}


void LogManager::setLogLevel(LogTarget &target, LogLevel level)
{
    Pt::System::RecursiveLock lock( _mutex );
    target.assignLogLevel(level, false);
    this->updateChildren(target, level);
}


void LogManager::updateChildren(LogTarget &target, LogLevel level)
{
    // Find the direct children of this target and set their LogLevels,
    // if their LogLevels haven't been set explicitely
    const std::string targetName = target.name() + ".";

    std::string::size_type startPos;
    std::string::size_type endPos;

    std::map<std::string, LogTarget*>::iterator it;
    for (it = _targetMap.upper_bound(targetName); it != _targetMap.end(); ++it)
    {
        LogTarget* foundTarget = it->second;

        const std::string& childTargetName = foundTarget->name();
        startPos = childTargetName.find(targetName);

        if (startPos != 0)
        {
            break;  // It's not a child. Leave this loop/method.
        }

        // For each direct child, update the target's log level and descent recursivly.
        // It is a direct child if we do not find another '.' after the parent name
        endPos = childTargetName.find( ".", targetName.size() );
        if( endPos == std::string::npos && foundTarget->inheritsLogLevel() )
        {
            foundTarget->assignLogLevel( target.logLevel(), true );
            LogManager::instance().updateChildren( *foundTarget, level );
        }
    }
}


void LogManager::setChannel(LogTarget& target, const std::string& url)
{
    Pt::System::RecursiveLock lock( _mutex );
    LogChannel& chan = this->channel(url);
    target.assignChannel( chan );

    //if(_logger)
    //    _logger->beginLog(PT_SOURCEINFO) << info << target.name()<< " set to " << url << endlog;
}


void LogManager::log(LogTarget& target, const LogMessage& message)
{
    Pt::System::RecursiveLock lock( _mutex );
    ;
    // search target hierachy upwards for a valid channel
    for( LogTarget* current = &target; current != 0; current = current->_parent )
    {
        if( current->_channel )
        {
            // format the message string
            std::string level = toString( message.logLevel() );
            Pt::DateTime time = System::Clock::getLocalTime();
            std::string str = time.toIsoString() + " [" + target.name() + "] " +
                              level + " - "  + message.text() + "\n";

            // write data to channel
            current->_channel->write(str);
            return;
        }
    }
}


LogChannel& LogManager::channel(const std::string& url)
{
    std::map<std::string, LogChannel*>::iterator it = _channelMap.find(url);
    if( it != _channelMap.end() )
    {
        return *it->second;
    }

    // use the url schema to create a new channel
    size_t colon = url.find(':');
    if(colon == std::string::npos)
    {
        throw  std::invalid_argument("Invalid channel url");
    }

    std::string protocol = url.substr(0, colon);

    LogChannel* ch = _pluginManager.create(protocol);
    if(ch == 0)
    {
        throw std::invalid_argument("No such channel");
    }


    // TODO: handle exceptions from open()
    ch->open(url);
    //if(_logger)
    //    _logger->beginLog(PT_SOURCEINFO) << info << "Opened channel: " << url << endlog;

    _channelMap[url] =  ch;
    return *ch;
}

}

}
