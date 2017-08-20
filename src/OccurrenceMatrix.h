#ifndef OCCURRENCE_MATRIX__H__
#define OCCURRENCE_MATRIX__H__

#include <string>

#include "Interval.h"
#include "Position.h"
#include "Range.h"
#include "RangedSequence.h"
#include "Support.h"

class OccurrenceMatrix
{
public:
    explicit OccurrenceMatrix(
        const RangedSequence & _ranged_sequence,
        const ListPositions & _positions);
    virtual ~OccurrenceMatrix();

    Support getSupport(
        const Range & _range,
        const Interval & _interval) const;

    std::string toString() const;

private:
    void detectBorders(const ListPositions & _positions);
    void initMatrix();
    void clearMatrix();

    bool ** m_matrix;
    size_t m_width, m_height;

    size_t m_x_start, m_y_start;
    size_t m_seq_size;
};

#endif
