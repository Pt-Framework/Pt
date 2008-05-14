/***************************************************************************
 *   Copyright (C) 2008 by PTV AG                                          *
 *   Copyright (C) 2008 by Peter Barth                                     *
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
#ifndef PT_SYMBIANTOOLS_H_
#define PT_SYMBIANTOOLS_H_

#include <vector>
#include <pthread.h>

// Symbian native APIs
#include <e32base.h>
#include <f32file.h>

namespace Pt {

namespace System {

namespace SymbianTools {

// UCS2 -> UTF8
static char* wchar2char(const TUint16* in, char* out, int len, int bufferSize) 
{
    int i = 0, j = 0;
    while (i < len)
    {
        TUint16 c = in[i++];
        if (c <= 0x7f)
        {
            if (j < bufferSize)
                out[j++] = c;
        }
        else if (c <= 0x7ff)
        {
            if (j < bufferSize)
                out[j++] = 0xc0 | (c >>   6); // first 5 bits
            if (j < bufferSize)
                out[j++] = 0x80 | (c & 0x3f); // last 6 bits
        }
        else if (c <= 0x7fff)
        {
            if (j < bufferSize)
                out[j++] = 0xe0 | (c >>  12);         // first 5 bits
            if (j < bufferSize)
                out[j++] = 0x80 | ((c>>6) & 0x3f);    // next 6 bits
            if (j < bufferSize)
                out[j++] = 0x80 | (c & 0x3f);         // last 6 bits
        }
    }
    
    if ((j == bufferSize) && (bufferSize > 0))
        out[bufferSize-1] = 0;
    else
        out[j] = 0;
    
    return out;
}

// safe but not very efficient
static std::string TPtrC2string(TPtrC src) 
{
    const int maxSize = src.Length()*4;
    std::vector<char> dst(maxSize);    

    wchar2char(src.Ptr(), &dst[0], src.Length(), maxSize);
    
    return std::string(&dst[0]);
}

// msecs == -1 => wait infinitely
static bool WaitForRequestWithTimeOut(TRequestStatus& status, int msecs)
{
    if (msecs == -1)
    {
        User::WaitForRequest(status);  
        return (status.Int() == KErrNone || 
                status.Int() == KErrEof);                    
    }
    
    RTimer timer;
    TRequestStatus timerStatus;
    timer.CreateLocal();
    timer.After(timerStatus, msecs * 1000);
    User::WaitForRequest(status, timerStatus);  
    
    TInt timerCompletionCode = timerStatus.Int();
    TInt readCompletionCode = status.Int();
    
    // timed out and read is not finished
    if (timerCompletionCode == KErrNone &&
        readCompletionCode == KRequestPending)
    {                    
        return false;
    }

    // cancel timer if read is done 
    if ((readCompletionCode != KRequestPending) &&
        timerCompletionCode != KErrNone)
    {
        timer.Cancel();
    }
    
    return (readCompletionCode == KErrNone || 
            readCompletionCode == KErrEof);                
}

// This class is used to monitor a symbian TRequestStatus
// in a separate pthread.
class StatusRequestWatcher
{
public:
    struct NotificationListener
    {
        virtual void OnStatusRequestComplete(StatusRequestWatcher& src) = 0;
    };
    
    StatusRequestWatcher(const TRequestStatus& status)
    : _status(status), _running(false), _notificationListener(0)
    {
        ::pthread_mutex_init(&_mutex, NULL);
        ::pthread_cond_init(&_cond, NULL);
        ::pthread_cond_init(&_startCond, NULL);        
    }
    
    ~StatusRequestWatcher()
    {
        stop();
        
        ::pthread_cond_destroy(&_startCond);
        ::pthread_cond_destroy(&_cond);
        ::pthread_mutex_destroy(&_mutex);
    }
    
    // do not call when watcher is running
    void setNotificationListener(NotificationListener* notificationListener)
    { 
        if (_running)
        {
            throw std::logic_error("Watcher already running" + PT_SOURCEINFO);
        }
        _notificationListener = notificationListener; 
    }
    
    bool start()
    {
        if (_running)
            return false;
        
        int rc = ::pthread_create(&_thread, NULL, WatcherThreadEntry, 
                reinterpret_cast<void*>(this));
        
        if (!rc)
        {
            ::pthread_mutex_lock(&_mutex);
            ::pthread_cond_wait(&_startCond, &_mutex);
            ::pthread_mutex_unlock(&_mutex);
        
            return true;
        }
                       
        return false;
    }
    
    bool stop()
    {
        if (!_running)
            return false;
        
        ::pthread_mutex_lock(&_mutex);
        _running = false;
        ::pthread_mutex_unlock(&_mutex);
        
        int rc = ::pthread_join(_thread, NULL);
        
        return rc == 0;
    }
    
private:
    static void* WatcherThreadEntry(void* threadID)
    {
        StatusRequestWatcher* self = 
            reinterpret_cast<StatusRequestWatcher*>(threadID);
    
        self->watcherThread();
        
        ::pthread_exit(NULL);
        
        return NULL;
    }

    void watcherThread()
    {
        ::pthread_mutex_lock(&_mutex);
        _running = true;
        ::pthread_mutex_unlock(&_mutex);
        
        ::pthread_cond_signal(&_startCond);
        
        while (_running && _status.Int() == KRequestPending)
        {
            // Wait a bit, otherwise this is a very busy loop
            User::After(1000);
            //printf("Waiting for request\n");
        }
        
        if (_notificationListener)
            _notificationListener->OnStatusRequestComplete(*this);
        
        ::pthread_mutex_lock(&_mutex);
        _running = false;
        ::pthread_mutex_unlock(&_mutex);
    }
    
    const TRequestStatus& _status;
    
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_cond_t _startCond;
    pthread_t _thread;
    
    bool _running;

    NotificationListener* _notificationListener;
    
};

// obsolete
//static void rename(const std::string& oldName, const std::string& newName)
//{
//    if (oldName.length() > (unsigned)KMaxPath)
//    {
//        throw std::logic_error("Old name too long");        
//    }
//        
//    if (newName.length() > (unsigned)KMaxPath)
//    {
//        throw std::logic_error("New name too long");        
//    }
//    
//    TPtrC8 ptrOldName(reinterpret_cast<const TUint8*>(oldName.c_str()));
//    TBuf<KMaxPath> descOldName;
//    descOldName.Copy(ptrOldName);
//
//    TPtrC8 ptrNewName(reinterpret_cast<const TUint8*>(newName.c_str()));
//    TBuf<KMaxPath> descNewName;
//    descNewName.Copy(ptrNewName);
//    
//    // creating a temporary file server session connection
//    // this is apparently costly, but when renaming files/directories 
//    // performance should not be an issue
//    
//    // Connect session
//    RFs fsSession;
//    
//    if (fsSession.Connect() != KErrNone)
//    {
//        throw SystemError("Could not establish file server connection", PT_SOURCEINFO);
//    }
//
//    TInt drive = EDriveC;
//    
//    TParse parser;
//    if (parser.SetNoWild(RProcess().FileName(), 0, 0) == KErrNone) 
//    {
//        if (parser.Drive().Length() > 0)
//            RFs::CharToDrive(parser.Drive()[0], drive);
//            
//        if (fsSession.CreatePrivatePath(drive) != KErrNone) 
//        {
//            drive = EDriveC;
//            fsSession.CreatePrivatePath(drive);
//        }
//        
//        fsSession.SetSessionToPrivate(drive);
//    }
//    
//    // Create file management object
//    // NewL factory should be trapped in case of a leave, so we can throw
//    // a true c++ exception when a leave occurs
//    CFileMan* fileMan;
//    TRAPD(createError, fileMan = CFileMan::NewL(fsSession));
//    if (createError)
//    {
//        throw SystemError("CFileMan::NewL() failed", PT_SOURCEINFO);        
//    }
//        
//    // TODO Rename only works when both directories 
//    // are located on the same drive. When they are located on different drives
//    // call Move instead of Rename
//    // TODO If destination directory exists Rename will fail
//    if (fileMan->Rename(descOldName, descNewName) != KErrNone)
//    {
//        throw SystemError("Could not move/rename directory/file '" + oldName + "' to '" + newName + "'", PT_SOURCEINFO);        
//    }
//
//    // close file server session
//    fsSession.Close();      
//}

}

}

}

#endif /*PT_SYMBIANTOOLS_H_*/
