#ifndef RANGED_SEQUENCE__H__
#define RANGED_SEQUENCE__H__

#include <list>
#include <map>

#include "Frequency.h"
#include "Range.h"
#include "Sequence.h"
#include "Size.h"
#include "Support.h"

class RangedSequence
{
public:
    explicit RangedSequence(
        const Sequence & _sequence,
        const Range & _range,
        const Support & _support);

    const Sequence & sequence() const;
    const Range & range() const;
    const Support & support() const;

    Frequency frequency() const;

private:
    Sequence m_sequence;
    Range m_range;
    Support m_support;
};

typedef std::list < RangedSequence > ListRangedSequence;
typedef std::map < Size, ListRangedSequence > MapRangedSequencesByLength;

#endif
