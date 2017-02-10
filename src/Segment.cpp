#include "Segment.h"

#include <algorithm>

Segment::Segment(
    const Point & _start,
    const Point & _end):
    m_start(_start),
    m_end(_end)
{
}

Segment::Segment(const Segment & _segment):
    m_start(_segment.start()),
    m_end(_segment.end())
{
}

bool Segment::operator==(const Segment & _other) const
{
    return m_start == _other.m_start && m_end == _other.m_end;
}

bool Segment::operator!=(const Segment & _other) const
{
    return ! ((*this) == _other);
}

const Point & Segment::start() const
{
    return m_start;
}

void Segment::start(const Point & _start)
{
    m_start = _start;
}

const Point & Segment::end() const
{
    return m_end;
}

void Segment::end(const Point & _end)
{
    m_end = _end;
}

bool Segment::contains(const Point & _point) const
{
    return _point >= m_start && _point <= m_end;
}

bool Segment::contains(const Segment & _other) const
{
    return (
        m_start <= _other.m_start &&
        m_end >= _other.m_end
        );
}

Size Segment::size() const
{
    return m_end - m_start + 1;
}

bool Segment::intersects(
    const Segment & _segment,
    Segment & _resulting_segment) const
{
    Point max_start = std::max(m_start, _segment.start());
    Point min_end = std::min(m_end, _segment.end());

    if(max_start > min_end)
    {
        return false;
    }

    _resulting_segment.start(max_start);
    _resulting_segment.end(min_end);

    return true;
}

void Segment::unify(
    const Segment & _segment,
    Segment & _resulting_segment) const
{
    _resulting_segment.start(std::min(m_start, _segment.start()));
    _resulting_segment.end(std::max(m_end, _segment.end()));
}
