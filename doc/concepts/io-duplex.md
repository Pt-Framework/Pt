# I/O Duplex

Pt::System::IODevice attaches to an EventLoop and runs asynchronous
transfers through beginRead()/endRead() and beginWrite()/endWrite().
This concept states what it takes for a device, and for a streambuf
built on a device, to run one outstanding read and one outstanding
write at the same time, each completing independently of the other.

## Transport layer

A device attached to an EventLoop can have at most one outstanding
beginRead() and one outstanding beginWrite() at the same time. The two
complete independently: a write finishing does not require a read to
be idle, and a read finishing does not require a write to be idle. The
device reports each direction with its own signal, inputReady() and
outputReady(), and a caller can have both pending without either one
waiting for the other.

This is a property of the device, not of one backend. Whatever
mechanism drives the EventLoop (select, poll, epoll, kqueue, or an
overlapped Windows handle) tracks read interest and write interest for
the same native handle independently, and reports their completion
independently. A device built on such a backend must expose that
independence instead of adding its own cross-direction restriction.

## Buffering layer

A streambuf that only moves bytes between its own buffer and the
device, without transforming them, keeps full duplex when it tracks a
read in progress and a write in progress as separate state and guards
a direction only against a second start of the same direction. It must
not refuse to start a read because a write is in progress, or refuse to
start a write because a read is in progress.

## Transform layer

A streambuf stacked on a buffering layer that transforms bytes
(encryption, compression, re-encoding) is full duplex only if each
direction's transform state is independent of the other: an unfinished
encode must survive a decode call, and an unfinished decode must
survive an encode call. A transform that keeps one piece of state
shared by both directions is not full duplex; it is unidirectional per
session, and starting the other direction before the current one
finishes must be a defined, reported error rather than silently
discarded or corrupted data.

A transform may still serialize the underlying conversion call itself;
it does not have to accept a reentrant call from one direction while
the other is in progress. What it must not do is turn a legitimate,
protocol-internal need to touch the other direction (an alert, a key
update, a renegotiation) into an unrecoverable failure. The
caller-visible contract for that case is the same one used for a
device direction that is not yet ready: no progress yet, retry later.

## Out of scope

This concept addresses one EventLoop thread alternating or overlapping
operations on one device or streambuf. It does not address two threads
calling into the same device or streambuf at the same time.
