#ifndef SEGMENT__H__
#define SEGMENT__H__

#include "Point.h"
#include "Size.h"

class Segment
{
public:
    explicit Segment(
        const Point & _start,
        const Point & _end);

    const Point & start() const;
    void start(const Point & _pos);

    const Point & end() const;
    void end(const Point & _end);

    bool contains(const Point & _point) const;
    Size size() const;

    bool intersect(
        const Segment & _segment,
        Segment & _resulting_segment) const;

protected:
    Point m_start;
    Point m_end;
};

#endif
