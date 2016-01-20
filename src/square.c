
#include <stdio.h>
#include <stdlib.h>

#include "square.h"


Square CreatSquare(int chipset, int type)
{
    Square sqr;

    sqr.chipset=chipset;

    sqr.type=type;

    return sqr;
}

