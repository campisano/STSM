#ifndef POSITION__H__
#define POSITION__H__

#include <list>
#include <set>

#include "Point.h"

typedef std::pair < Point, Point > Position;
typedef std::list < Position > ListPositions;
typedef std::set < Position > SetPositions;

#endif
