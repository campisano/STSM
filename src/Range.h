#ifndef RANGE__H__
#define RANGE__H__

#include "Segment.h"

class Range : public Segment
{
public:
    explicit Range(
        const Point & _start,
        const Point & _end);
};

#endif
