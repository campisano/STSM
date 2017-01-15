#ifndef SEQUENCE_BLOCK__H__
#define SEQUENCE_BLOCK__H__

#include <list>

#include "Interval.h"
#include "Range.h"
#include "Sequence.h"
#include "Support.h"

class SequenceBlock
{
public:
    explicit SequenceBlock(
        const Sequence & _sequence,
        const Range & _range,
        const Interval & _interval,
        const Support & _support);

    const Sequence & sequence() const;
    const Range & range() const;
    const Interval & interval() const;
    const Support & support() const;

private:
    Sequence m_sequence;
    Range m_range;
    Interval m_interval;
    Support m_support;
};

typedef std::list < SequenceBlock > ListSequenceBlocks;

#endif
