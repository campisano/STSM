#include "Interval.h"

Interval::Interval(
    const Point & _start,
    const Point & _end):
    Segment(_start, _end)
{
}

Interval::Interval(const Segment & _segment):
    Segment(_segment)
{
}
