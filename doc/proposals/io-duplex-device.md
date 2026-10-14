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

OverlappedIODeviceImpl::endRead() advances both _readOv.Offset and
_writeOv.Offset by the number of bytes read, and the write path
advances both the same way. This assumes one shared, sequential file
position used by both directions. That assumption is harmless for a
device that is not seekable (socket, pipe, serial port) and wrong for
a seekable device driven with concurrent, independently positioned
read and write. FileDeviceImpl must decide whether it keeps one
coupled position, which forces sequential, non-overlapping use of read
and write, or gives each direction its own offset once concurrent read
and write are allowed on a seekable device.
