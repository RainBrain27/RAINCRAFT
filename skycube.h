#ifndef SKYCUBE_H
#define SKYCUBE_H

#include "baseobject.h"

class skycube : public baseobject
{
public:
    skycube(size_t shaID, size_t texID, size_t program = 0);
};

#endif // SKYCUBE_H
