\page Pt-Core-Page Core Module

@copydetails Pt-Core

This chapter covers program options, application settings and events,
fixed-size integers, dates and times, atomic operations, coroutines, the
core module's custom allocators, its unicode text processing, its
signal and delegate callback mechanism, its type traits and conversion
utilities, and its serialization framework.

- @ref Pt-Core-Page-Basics covers program options, application settings, events, fixed-size integers and atomic operations.
- @ref Pt-Core-Page-DateTime covers dates and times.
- @ref Pt-Core-Page-Coroutines covers tasks, awaitables and generators.
- @ref Pt-Core-Page-Allocator covers custom allocation strategies.
- @ref Pt-Core-Page-Text covers unicode text processing and regular expressions.
- @ref Pt-Core-Page-Signals covers signals, slots and delegates.
- @ref Pt-Core-Page-TypeTraits covers type traits and type information.
- @ref Pt-Core-Page-Convert covers conversion utilities.
- @ref Pt-Core-Page-Serialization covers serialization.

@section Pt-Core-Page-Basics Basic Types
@copydetails Pt-Basics

@subsection Pt-Core-Page-Event Events
@copydetails Pt::Event

@subsection Pt-Core-Page-Arg Command Line Arguments
@copydetails Pt::Arg

@subsection Pt-Core-Page-Settings Application Settings
@copydetails Pt::Settings

@subsection Pt-Core-Page-IntTypes Fixed-Size Integers
@copydetails Pt-IntTypes

@subsection Pt-Core-Page-Atomics Atomic Operations
@copydetails Pt-Atomics

@section Pt-Core-Page-DateTime Dates and Times
@copydetails Pt-DateTime

@subsection Pt-Core-Page-Timespan Timespan
@copydetails Pt::Timespan

@subsection Pt-Core-Page-Date Date
@copydetails Pt::Date

@subsection Pt-Core-Page-Time Time
@copydetails Pt::Time

@subsection Pt-Core-Page-DateTimeClass DateTime
@copydetails Pt::DateTime

@section Pt-Core-Page-Coroutines Coroutines
@copydetails Pt-Coroutines

@subsection Pt-Core-Page-Task Tasks
@copydetails Pt::Task

@subsection Pt-Core-Page-Awaitables Awaitables
@copydetails Pt::Awaiter

@copydetails Pt::BasicAwaiter

@subsection Pt-Core-Page-Generators Generators
@copydetails Pt::Generator

@section Pt-Core-Page-Allocator Allocators
@copydetails Pt-Allocator

@subsection Pt-Core-Page-AllocatorInterface The Allocator Interface
@copydetails Pt::Allocator

@subsection Pt-Core-Page-Pool Pool Allocation
@copydetails Pt::PoolAllocator

@copydetails Pt::MemoryPool

@subsection Pt-Core-Page-Page Page Allocation
@copydetails Pt::PageAllocator

@section Pt-Core-Page-Text Text Processing
@copydetails Pt-Text

@subsection Pt-Core-Page-Characters Characters and Strings
@copydetails Pt::Char

@copydetails Pt::String

@subsection Pt-Core-Page-TextStreams Text Streams and Codecs
@copydetails Pt::TextCodec

@copydetails Pt::BasicTextIStream

@copydetails Pt::BasicTextOStream

@subsection Pt-Core-Page-Base64 Base-64 Encoding
@copydetails Pt::Base64Codec

@subsection Pt-Core-Page-Regex Regular Expressions
@copydetails Pt::Regex

@section Pt-Core-Page-Signals Signals and Delegates
@copydetails Pt-Signals

@subsection Pt-Core-Page-Signal Signals
@copydetails Pt::Signal

@subsection Pt-Core-Page-Delegate Delegates
@copydetails Pt::Delegate

@section Pt-Core-Page-TypeTraits Type Traits and Information
@copydetails Pt-TypeTraits

@subsection Pt-Core-Page-TypeInfo Type Information
@copydetails Pt::TypeInfo

@section Pt-Core-Page-Convert Conversions
@copydetails Pt-Convert

@section Pt-Core-Page-Serialization Serialization
@copydetails Pt-Serialization
