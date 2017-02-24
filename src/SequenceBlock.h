#ifndef SEQUENCE_BLOCK__H__
#define SEQUENCE_BLOCK__H__

#include <list>
#include <map>

#include "Frequency.h"
#include "Interval.h"
#include "Range.h"
#include "Sequence.h"
#include "Size.h"
#include "Support.h"

class SequenceBlock;
typedef std::list < SequenceBlock > ListSequenceBlocks;
typedef std::map < Size, ListSequenceBlocks > MapSequenceBlocksByLength;

class SequenceBlock
{
public:
    explicit SequenceBlock(
        const Sequence & _sequence,
        const Range & _range,
        const Interval & _interval,
        const Support & _support);

    bool operator==(const SequenceBlock & _other) const;
    bool operator!=(const SequenceBlock & _other) const;

    const Sequence & sequence() const;
    const Range & range() const;
    const Interval & interval() const;

    Support support() const;
    Frequency frequency() const;
    Size area() const;

    bool contains(const SequenceBlock & _other) const;
    bool contains(
        const Range & _other_range,
        const Interval & _other_interval) const;

    bool hasSameCoordinates(const SequenceBlock & _other) const;

    struct LessThanComparer
    {
        bool operator() (
            const ListSequenceBlocks::iterator & _left,
            const ListSequenceBlocks::iterator & _right) const;
    };

    struct PositionComparer
    {
        explicit PositionComparer(size_t _x_start, size_t _y_start);

        bool operator() (
            const SequenceBlock & _left,
            const SequenceBlock & _right) const;

        size_t m_x_start;
        size_t m_y_start;
    };

private:
    Sequence m_sequence;
    Range m_range;
    Interval m_interval;
    Support m_support;
};

#endif
