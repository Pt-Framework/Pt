#ifndef PT_SYSTEM_H
#define PT_SYSTEM_H


/** \dir
This module offers support for multithreaded programming, API's for
file system handling such as traversing through directories and files,
File-IO, shared memory and shared libraries. All classes and functions
are in the namespace system, which is nested in the ptv namespace.
*/

namespace Pt {

	/** The System Module.
	This module offers support for multithreaded programming, API's for
	file system handling such as traversing through directories and files,
	File-IO, shared memory and shared libraries. All classes and functions
	are in the namespace system, which is nested in the ptv namespace.
	 */
	namespace System {
		class FileSystemMode;
	}
}

/** \page "File System Operations"
!!! File System Operations
%File systems are typically node based and contain file sytem objects of various types, i.e. files, directories. PPR models this through the FileSystemNode interface,
which provides functions that are applicable to possible objects usually found as
nodes in file systems. The FileSystem singleton class serves as a factory, which can create
FileSytemNodes from a path. The two most important classes that implement the
FileSystemNode interface are File and Directory.

A File object is not meant to be used for File I/O, but only for file system operations
such as creating, renaming, moving or removing files. For File I/O use a FileDevice
instead.

The Directory class has a special meaning and can be seen as a container for
file system objects. The DirectoryIterator can be used to traverse through the
entries in a Directory. It can be used easily with all the iterator based
algorithms in the C++ standard library.
*/

/** \page "Multithreading"
!!! Multithreading
!! Reentrancy and thread-safety
Functions can provide two levels of thread safety: thread-safe and reentrant.

- A reentrant function can be called by multible threads, as long as all callers
work with unique data.
- A thread-safe function can be called from multiple threads.

Most C++ member functions are reentrant since they only work on member data.
A member function is not reentrant for example when multiple objects share one
implementation.

!! Synchronizing access
For a member function to become thread-safe, all access to the data it references
need to be synchronized by the use of a synchronisation object. PPR provides three
different synchronization objects, the Mutex, the Semaphore and the Spinlock.

The advantage of a Spinlock is that locking and unlocking is very cheap, but while
a Spinlock tries to lock on a resource in a busy loop. It is the synchronisation
object of choice when only very short operations need to be protected, like assigning
a int variable. Spinlocks are not recursive.

The Mutex does not run in a busy loop while it locks, but locking and
unlockingis more costly. It is used to synchronize longer operations or code
sections. Mutexes are recursive.

The Semaphore is a synchronization object that can protect multiple resources.
It allows a number of threads concurent access.On the other hand a Mutex can
be seen as a special case of a Semaphore that protectes only one resource.

!! Creating and using Threads
The Thread class provided by PPR can be used to spawn new threads. These threads can
either be detached or joinable. It is only possible to wait for joinable threads
to finish from another thread. Once a thread is detached it will keep running even
if the Thread object is destroyed. To create a new thread, either derive from
Thread and implement the Thread::run() method or pass a Runnable to the
Thread constructor.

The Condition object serves the purpose of letting threads sleep and be wakable
by other threads. One thread or a number of threads wait on a Condition until it
becomes signalled.

*/

#endif

