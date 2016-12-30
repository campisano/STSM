#ifndef SEQUENCE__H__
#define SEQUENCE__H__

#include <list>
#include <string>
#include <vector>

#include "Item.h"
#include "Point.h"
#include "Serie.h"
#include "Size.h"

class Sequence
{
public:
    explicit Sequence();
    explicit Sequence(const Item& _item);
    explicit Sequence(const std::string & _string_representation);
    virtual ~Sequence();

    bool operator==(const Sequence & _other);
    void set(const std::string & _string_representation);

    void clear();
    void append(const Item & _item);

    Size size() const;

    const Item & getFirst() const;
    const Item & getLast() const;

    void getCopyExceptPos(Point _pos, Sequence & _copy_seq) const;
    void getSubsequenceDroppingFirstItem(Sequence & _subseq) const;
    void getSubsequenceDroppingLastItem(Sequence & _subseq) const;

    bool supportedBy(const Serie & _serie) const;

    std::string toString() const;
    std::string toStringOfItems() const;

private:
    std::vector<Item> m_items;
};

#endif
