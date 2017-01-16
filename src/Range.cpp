#include "Range.h"

Range::Range(
    const Point & _start,
    const Point & _end):
    Segment(_start, _end)
{
}

Range::Range(const Segment & _segment):
    Segment(_segment)
{
}
