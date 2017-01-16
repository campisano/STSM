#ifndef INTERVAL__H__
#define INTERVAL__H__

#include "Segment.h"

class Interval : public Segment
{
public:
    explicit Interval(
        const Point & _start,
        const Point & _end);

    explicit Interval(const Segment & _segment);
};

#endif
