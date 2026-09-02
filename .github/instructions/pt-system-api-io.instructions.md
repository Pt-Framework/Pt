---
description: "Synchronous and asynchronous I/O, streams and archive formats"
---

- I/O device endpoint, sync/async read and write, seek:
  `include/Pt/System/IODevice.h`
- Stream buffer for I/O devices:
  `include/Pt/System/IOBuffer.h`
- Input, output and bidirectional streams for I/O devices:
  `include/Pt/System/IOStream.h`
- Wait on native handles or file descriptors:
  `include/Pt/System/IONotifier.h`
- Half-duplex pipe pair of I/O devices:
  `include/Pt/System/Pipe.h`
- Serial port, baud, parity and flow control:
  `include/Pt/System/SerialDevice.h`
- I/O and access-failed exceptions:
  `include/Pt/System/IOError.h`
- Parse URI protocol, user, host, port and path:
  `include/Pt/System/Uri.h`
- URI query fields, encode and decode:
  `include/Pt/System/UriQuery.h`
- Tar archive I/O group, Pax/UStar format:
  `include/Pt/System/Api-Tar.h`
- Tar entry metadata, type, path and content chunks:
  `include/Pt/System/TarEntry.h`
- Incremental tar reader, non-blocking advance:
  `include/Pt/System/TarReader.h`
- Write tar entries, stream file content, finish archive:
  `include/Pt/System/TarWriter.h`
