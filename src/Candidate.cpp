#include "Candidate.h"

Candidate::Candidate(
    const Sequence & _sequence,
    const Range & _range,
    const ListKernels & _kernels):
    m_sequence(_sequence),
    m_range(_range),
    m_kernels(_kernels)
{
    m_kernel_closed = true;
    m_current = NULL;
}

const Sequence & Candidate::sequence() const
{
    return m_sequence;
}

const Range & Candidate::range() const
{
    return m_range;
}


const ListKernels & Candidate::kernels() const
{
    return m_kernels;
}

void Candidate::updateCandidateKernels(
    const Serie & _serie,
    const Point & _position,
    const Frequency & _min_spatial_freq)
{
    bool supported = m_sequence.supportedBy(_serie);

    // if it is the first kernel or the previous kernel is closed
    if(m_kernel_closed)
    {
        // and the candidate sequence is contained in the time series d
        if(supported)
        {
            // start a new kernel
            m_kernel_closed = false;
            m_kernels.push_back(
                Kernel(
                    _position,
                    _position));
            m_current = & m_kernels.back();
            m_current->support(1);
            m_current->frequency(1.0);
        }
    }
    // if the previous kernel is open
    // and the sequence is contained in the time series d
    else if(supported)
    {
        // extend the current kernel
        m_current->end(_position);
        m_current->support(m_current->support() + 1);
        m_current->frequency(
            m_current->support() / m_current->size());
    }
    else
    {
        Frequency frequency = m_current->support() / m_current->size();

        // check validity of current kernel
        if (frequency < _min_spatial_freq)
        {
            m_kernel_closed = true;
        }
    }
}

void Candidate::mergeKernels(
    const Frequency & _min_spatial_freq)
{
    ListKernels to_add;
    ListKernels::iterator ka_it;
    std::list<ListKernels::iterator> to_del;
    std::list<ListKernels::iterator>::iterator kd_it;

    ListKernels::iterator kq_it, kr_it;
    Support new_support;
    Point min_start, max_end;
    Frequency new_frequency;

    bool mergeable;

    do
    {
        mergeable = false;

        // for each kernel range q in the candidate kernels
        for(
            kq_it = m_kernels.begin();
            kq_it != m_kernels.end();
            ++kq_it)
        {
            // for each other ranges... where r > q
            for(
                kr_it = kq_it, ++kr_it; // equivalent to kr_it = kq_it + 1
                kr_it != m_kernels.end();
                ++kr_it)
            {
                // ...that can be merged reaching the minimum spatial frequency
                new_support = kq_it->support() + kq_it->support();
                min_start = std::min(kq_it->start(), kq_it->start());
                max_end = std::max(kq_it->end(), kq_it->end());
                new_frequency = new_support / (max_end - min_start + 1);

                if(new_frequency >= _min_spatial_freq)
                {
                    // generate a new merged kernel
                    to_add.push_back(
                        Kernel(
                            min_start,
                            max_end));
                    Kernel & kernel = to_add.back();
                    kernel.support(new_support);
                    kernel.frequency(new_frequency);

                    // mark the two smaller kernels to be removed
                    to_del.push_back(kq_it);
                    to_del.push_back(kr_it);

                    // stop the merging and update the kernel list
                    mergeable = true;
                    break;
                }
            }

            // stop the merging and update the kernel list
            if(mergeable)
            {
                break;
            }
        }

        for(kd_it = to_del.begin(); kd_it != to_del.end(); ++kd_it)
        {
            m_kernels.erase(*kd_it);
        }

        to_del.clear();

        for(ka_it = to_add.begin(); ka_it != to_add.end(); ++ka_it)
        {
            m_kernels.push_back(*ka_it);
        }

        to_add.clear();
    }
    while (mergeable);
}
