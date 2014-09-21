/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
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

#ifndef Pt_System_FileDevice_h
#define Pt_System_FileDevice_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <string>
#include <ios>

namespace Pt {

namespace System {

/** @brief Read and write to files.

    A Pt::System::FileDevice reads and writes files in the filesystem either
    synchronously or asynchronously. If used synchronously, it offers similar
    functionality like a std::fstream or the file I/O functions from the C
    library (fopen...). However, most applications need to perform input and
    output asynchronously, which makes using a %FileDevice attractive. Since it
    inherits Pt::System::IODevice, it can be used as the endpoint of an
    Pt::System::IOStream or Pt::System::IOBuffer, respectively. If no buffering
    is required, a %FileDevice can be used on its own, as shown in the following
    example:

    @code
    void onOpen(Pt::System::FileDevice& file);

    void onOutput(Pt::System::IODevice& device);

    int main(int argc, char** argv)
    {
        try
        {
            Pt::System::MainLoop loop;
    
            Pt::System::FileDevice file;
            file.setActive(loop);
            file.opened() += Pt::slot( &onOpen );
            file.outputReady() += Pt::slot( &onOutput );

            Pt::System::Path path = "tmpfile.txt";
            file.beginOpen(path, std::ios::out);

            loop.run();
            return 0;
        }
        catch(Pt::System::IOError& e)
        {
            std::cerr << "I/O error: " << e.what() << std::endl;
        }

        return -1;
    }
    @endcode

    An %EventLoop is required for all asynchronous operations. This includes
    not only reading and writing, but also opening the file. The function
    @link Pt::System::FileDevice::beginOpen beginOpen()@endlink begins to open
    a file and the signal returned by @link Pt::System::FileDevice::opened
    opened()@endlink is sent when the file was opened. It is connected to the
    slot shown in the next example:

    @code
    void onOpen(Pt::System::FileDevice& file)
    {
        file.endOpen();
        file.beginWrite("Hello world!", 12);
    }
    @endcode

    The asynchronous open operation is ended by calling 
    @link Pt::System::FileDevice::endOpen() endOpen()@endlink, and a write
    operation is started with @link Pt::System::FileDevice::beginWrite()
    beginWrite()@endlink to write bytes from a buffer to the file. The signal
    @link Pt::System::FileDevice::outputReady() outputReady()@endlink is sent,
    when data was written to the file. A slot is connected to that signal to
    handle output:

    @code
    void onOutput(Pt::System::IODevice& device)
    {
        std::size_t n = device.endWrite();
        std::cout << "wrote: " << n << "bytes." << std::endl;
    }
    @endcode

    The signal is inherited from %IODevice, so the signature of the slot
    needs a reference to a %IODevice as parameter. The write operation is
    ended by @link Pt::System::FileDevice::endWrite() endWrite()@endlink,
    which returns the number of bytes written to the file. This might be
    less than what was requested by beginWrite(), in which case another
    write operation has to be started.

    All functions to begin or end asynchronous operations throw an exception
    of type Pt::System::IOError on failure. If IOErrors are not catched and
    handled in the slots, they will propagate through the %EventLoop into the
    main() function and end the program. Normally, larger applications will
    need to process errors in the slots, so the %EventLoop is not stopped.

    @ingroup FileSystem
*/
class PT_SYSTEM_API FileDevice : public IODevice 
{
    public:
        /** @brief Default Constructor.
        */
        FileDevice();

        /** @brief Construct with path to file.
        */
        FileDevice(const Path& path, std::ios::openmode mode);

        /** @brief Destructor.
        */
        ~FileDevice();

        /** @brief Opens the file.
        */
        void open(const Path& path, std::ios::openmode mode);

        /** @brief Begin opening the file.
        */
        void beginOpen(const Path& path, std::ios::openmode mode);

        /** @brief End opening the file.
        */
        void endOpen();

        /** @brief Notifies that the file was opened.
        */
        Signal<FileDevice&>& opened()
        { return _opened; }

        /** @brief Returns true if file is open.
        */
        bool isOpen() const
        { return _isOpen; }

    protected:
        // inherit docs
        std::size_t onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        // inherit docs
        std::size_t onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        // inherit docs
        std::size_t onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n);

        // inherit docs
        std::size_t onEndWrite(EventLoop& loop, const char* buffer, std::size_t n);

        // inherit docs
        void onClose();

        // inherit docs
        void onCancel();

        // inherit docs
        void onSetTimeout(std::size_t timeout);

        // inherit docs
        bool onSeekable() const;

        // inherit docs
        pos_type onSeek(off_type offset, std::ios::seekdir sd);

        // inherit docs
        std::size_t onRead(char* buffer, std::size_t count, bool& eof);

        // inherit docs
        std::size_t onWrite(const char* buffer, std::size_t count);

        // inherit docs
        std::size_t onPeek(char* buffer, std::size_t count);

        // inherit docs
        void onSync() const;

        // inherit docs
        virtual bool onRun();

    private:
        class FileDeviceImpl* _impl;
        Signal<FileDevice&> _opened;
        bool _opening;
        bool _isOpen;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_FileDevice_h
