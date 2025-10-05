#include "../../include/shapes.h"

plane_t plane(void)
{
    plane_t p;
    shape(&p, SHAPE_PLANE);
    return p;
}