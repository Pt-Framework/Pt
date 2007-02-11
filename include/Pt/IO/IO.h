#ifndef PTV_IO_H
#define PTV_IO_H

/** \dir
The I/O module extends the IOStreams of the C++ standard library. A
Ptv::IOStream implements formatted IO through its std::locale and all
overloads of the input and output operator are usable as in the C++
standard library. The Ptv::IOStream has additional peeking capability
compared to standard C++ IOStreams, so it can be peeked for multiple
characters at once.
Classes derived from Ptv::StreamBuffer are used to implement a buffering
strategy for an IOStream. The IOBuffer included with the IO module
implements buffered IO with a linear buffer area.
IOStreams operate on IODevices, which are meant to implement low-level IO.
Examples of typical IODevices are the FileDevice in the Sytstem module
or the Socket classes in the Net module.
A Ptv::IOStream combines a Ptv::StreamBuffer and a Ptv::IODevice and allows
for transparent IO this way.
All classes and functions are in the namespace io which is nested in the
ptv namespace.
*/

namespace Pt {

    namespace IO {
    }

}

#endif

