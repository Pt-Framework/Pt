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

%FileDevice keeps one logical position that reserves ranges for both
directions in transfer-start order. Starting a read or write reserves
the requested byte count, even when the transfer later completes with
fewer bytes because it reaches end of file or reports a short result.
The next transfer starts at the end of that requested range. A caller
can therefore begin a read and a write concurrently without either
operation changing the other operation's native position.

The logical position is the public current position. %position() and
seek(0, ios::cur) return it; they do not derive it from a native file
pointer or from a completed byte count. A seek establishes a new
logical position for later transfers. It cannot alter an operation
that is already in flight. Direct device seeks reject that state, while
%IOBuffer ends its pending transfer and discards its buffers before
asking the device to seek.

Each native transfer receives its own position context. On Windows the
context is the %OVERLAPPED structure prepared immediately before
%ReadFile or %WriteFile. On POSIX the file backend restores the
transfer's reserved offset before delegating to the generic I/O
operation. These native positions serve only their one transfer; they
are not the public cursor.

%FileDevice::peek() reads at the current logical position without
reserving a range. A following read therefore observes the same bytes
and advances the logical position normally.
