#ifndef KERNEL__H__
#define KERNEL__H__

#include <list>

#include "Frequency.h"
#include "Point.h"
#include "Range.h"
#include "Segment.h"
#include "Support.h"

class Kernel : public Segment
{
public:
    explicit Kernel(
        const Range & _range,
        const Frequency & _min_frequency);

    explicit Kernel(
        const Point & _start,
        const Point & _end,
        const Frequency & _min_frequency);

    const Support & support() const;
    void support(const Support & _support);

    const Frequency & frequency() const;
    void frequency(const Frequency & _frequency);

private:
    Frequency m_min_frequency;
    Support m_support;
    Frequency m_frequency;
};

typedef std::list < Kernel > ListKernels;

#endif
