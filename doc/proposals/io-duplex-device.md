---
proposal: io-duplex.md
---

# I/O Duplex Device

## Cross-direction guard

IODevice::beginRead()/read() reject a call only when a read is
already pending (_rbuf set). IODevice::beginWrite()/write() reject a
call only when a write is already pending (_wbuf set). endRead() and
endWrite() touch only their own buffer. isEof() and setEof() stay a
single flag scoped to the read path.

The backends a device runs on track and complete the two directions
independently on the same native handle. The select, poll, epoll, and
kqueue selectors hold separate read and write interest per IOHandle
(separate fd_sets, separate POLLIN/POLLOUT bits, separate
EPOLLIN/EPOLLOUT bits, separate EVFILT_READ/EVFILT_WRITE kevents), and
the Windows overlapped implementation uses two separate OVERLAPPED
structures, _readOv and _writeOv, whose completion is checked
independently through HasOverlappedIoCompleted(). Concrete onRun()
overrides such as Net::TcpSocket::onRun() branch on isReading() and
isWriting() independently and send inputReady() and outputReady()
separately. System::IOBuffer::beginRead() and beginWrite() guard only
their own direction.

A device that cannot actually run a read and a write at the same time
(a backend, or a device type, with no independent read/write interest
on its native handle) must reject the second direction from its own
onBeginRead()/onBeginWrite() override. It must not rely on IODevice to
serialize all devices on its behalf. Today every IODevice type either
has independent backend state per direction (sockets, files, serial
ports) or is split into two separate single-direction IODevice
instances (pipes), so no device currently needs its own override.

## Offset coupling {#offset-coupling}

OverlappedIODeviceImpl currently advances both _readOv.Offset and
_writeOv.Offset after either a read or a write completes. This couples
the two directions to one shared sequential position. The coupling is
harmless for non-seekable devices and wrong for a seekable device that
allows concurrent, independently positioned read and write.

Each direction keeps its own offset. A read advances only the read
OVERLAPPED offset; a write advances only the write OVERLAPPED offset.
Both offsets are 64-bit (Offset + OffsetHigh).

seek sets both offsets to the same absolute position. position() and
seek(0, ios::cur) return the read offset as the public current
position. On Windows the implementation must not use the OS file
pointer as the source of the current position: ReadFile and WriteFile
with OVERLAPPED do not update it. The OVERLAPPED offsets are the
source of truth. After computing a new absolute position, both offsets
are set and the file pointer may be synchronized with SetFilePointer
for compatibility.

IOBuffer::seekoff ends any pending transfer, calls the device seek,
and discards its buffers. The next transfer in either direction
therefore starts from the common position established by the seek.

This replaces the coupled advances in OverlappedIODeviceImpl and the
WCE FileDeviceImpl path, and makes FileDeviceImpl::seek and
position() consistent with independent offsets.
