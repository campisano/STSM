#ifndef CANDIDATE__H__
#define CANDIDATE__H__

#include <list>

#include "Kernel.h"
#include "Range.h"
#include "Sequence.h"
#include "Serie.h"

class Candidate
{
public:
    explicit Candidate(
        const Sequence & _sequence,
        const Range & _range,
        const ListKernels & _kernels);

    const Sequence & sequence() const;
    const Range & range() const;
    const ListKernels & kernels() const;

    void updateCandidateKernels(
        const Serie & _serie,
        const Point _position,
        const Frequency _min_spatial_frequency);

    void mergeKernels(
        const Frequency _min_spatial_freq);

private:
    Sequence m_sequence;
    Range m_range;
    ListKernels m_kernels;

    bool m_kernel_closed;
    Kernel * m_current;
};

typedef std::list<Candidate> ListCandidates;

#endif
