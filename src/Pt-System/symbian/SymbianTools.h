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

// Symbian native APIs
#include <e32base.h>
#include <f32file.h>
#include <vector>

namespace Pt {

namespace System {

namespace SymbianTools {

static char* wchar2char(const TUint16* in, char* out, int len) // UCS2 -> UTF8
{
    int i = 0, j = 0;
    while (i < len)
    {
        TUint16 c = in[i++];
        if(c <= 0x7f)
        {
            out[j++] = c;
        }
        else if(c <= 0x7ff)
        {
            out[j++] = 0xc0 | (c >>   6); // first 5 bits
            out[j++] = 0x80 | (c & 0x3f); // last 6 bits
        }
        else if(c <= 0x7fff)
        {
            out[j++] = 0xe0 | (c >>  12);         // first 5 bits
            out[j++] = 0x80 | ((c>>6) & 0x3f);    // next 6 bits
            out[j++] = 0x80 | (c & 0x3f);         // last 6 bits
        }
    }
    out[j] = 0;
    return out;
}

// safe but not very efficient
static std::string TPtrC2string(TPtrC src) 
{
    std::vector<char> dst(src.Length()*2);    

    wchar2char(src.Ptr(), &dst[0], src.Length());
    
    return std::string(&dst[0]);
}

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
