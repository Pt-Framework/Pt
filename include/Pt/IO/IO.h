#ifndef PT_IO_H
#define PT_IO_H

namespace Pt {

    /** @namespace Pt::IO
        @brief Stream-based and raw I/O

        The I/O module extends the IOStreams of the C++ standard library. A
        Pt::IOStream implements formatted IO through its std::locale and all
        overloads of the input and output operator are usable as in the C++
        standard library. The Pt::IOStream has additional peeking capability
        compared to standard C++ IOStreams, so it can be peeked for multiple
        characters at once. A Pt::IOStream combines a Pt::StreamBuffer and a
        Pt::IODevice and allows for transparent IO this way.
        Classes derived from Pt::StreamBuffer are used to implement a buffering
        strategy for an IOStream. The IOBuffer included with the IO module
        implements buffered IO with a linear buffer area.
        IOStreams operate on IODevices, which are meant to implement low-level IO.
        Examples of typical IODevices are the FileDevice or the Socket classes.

    */
    namespace IO {
    }

}

#endif

