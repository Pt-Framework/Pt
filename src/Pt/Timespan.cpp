#include "Pt/Timespan.h"
#include <algorithm>


namespace Pt {

const Pt::int64_t Timespan::Milliseconds = 1000;
const Pt::int64_t Timespan::Seconds      = 1000 * Timespan::Milliseconds;
const Pt::int64_t Timespan::Minutes      =   60 * Timespan::Seconds;
const Pt::int64_t Timespan::Hours        =   60 * Timespan::Minutes;
const Pt::int64_t Timespan::Days         =   24 * Timespan::Hours;


Timespan::Timespan():
	_span(0)
{
}


Timespan::Timespan(Pt::int64_t microseconds)
: _span(microseconds)
{
}


Timespan::Timespan(long seconds, long microseconds)
: _span(Pt::int64_t(seconds)*Seconds + microseconds)
{
}


Timespan::Timespan(int days, int hours, int minutes, int seconds, int microseconds)
: _span( Pt::int64_t(microseconds) +
         Pt::int64_t(seconds)*Seconds +
         Pt::int64_t(minutes)*Minutes +
         Pt::int64_t(hours)*Hours +
         Pt::int64_t(days)*Days )
{
}


Timespan::Timespan(const Timespan& timespan):
	_span(timespan._span)
{
}


Timespan::~Timespan()
{
}


Timespan& Timespan::operator = (const Timespan& timespan)
{
	_span = timespan._span;
	return *this;
}


Timespan& Timespan::operator = (Pt::int64_t microseconds)
{
	_span = microseconds;
	return *this;
}


Timespan& Timespan::set(int days, int hours, int minutes, int seconds, int microseconds)
{
	_span = Pt::int64_t(microseconds) +
            Pt::int64_t(seconds)*Seconds +
            Pt::int64_t(minutes)*Minutes +
            Pt::int64_t(hours)*Hours +
            Pt::int64_t(days)*Days;
	return *this;
}


Timespan& Timespan::set(long seconds, long microseconds)
{
	_span = Pt::int64_t(seconds)*Seconds + Pt::int64_t(microseconds);
	return *this;
}


void Timespan::swap(Timespan& timespan)
{
	std::swap(_span, timespan._span);
}


Timespan Timespan::operator + (const Timespan& d) const
{
	return Timespan(_span + d._span);
}


Timespan Timespan::operator - (const Timespan& d) const
{
	return Timespan(_span - d._span);
}


Timespan& Timespan::operator += (const Timespan& d)
{
	_span += d._span;
	return *this;
}


Timespan& Timespan::operator -= (const Timespan& d)
{
	_span -= d._span;
	return *this;
}


Timespan Timespan::operator + (Pt::int64_t microseconds) const
{
	return Timespan(_span + microseconds);
}


Timespan Timespan::operator - (Pt::int64_t microseconds) const
{
	return Timespan(_span - microseconds);
}


Timespan& Timespan::operator += (Pt::int64_t microseconds)
{
	_span += microseconds;
	return *this;
}


Timespan& Timespan::operator -= (Pt::int64_t microseconds)
{
	_span -= microseconds;
	return *this;
}


} // namespace Pt
