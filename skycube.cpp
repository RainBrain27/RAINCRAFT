#include "skycube.h"

#include <stdio.h>

skycube::skycube(size_t shaID, size_t texID, size_t program)
    :baseobject(shaID,texID,program)
{
    saveModelMatrix=ModelMatrix;
}
