#ifndef RANGED_SEQUENCE__H__
#define RANGED_SEQUENCE__H__

#include <list>

#include "Frequency.h"
#include "Range.h"
#include "Sequence.h"

class RangedSequence
{
public:
    explicit RangedSequence(
        const Sequence & _sequence,
        const Range & _range,
        const Frequency & _frequency);

    const Sequence & sequence() const;
    const Range & range() const;
    const Frequency & frequency() const;

private:
    Sequence m_sequence;
    Range m_range;
    Frequency m_frequency;
};

typedef std::list < RangedSequence > ListRangedSequence;
typedef std::list < ListRangedSequence > ListListRangedSequence;

#endif
