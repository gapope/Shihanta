#include "Entity.h"


Entity::~Entity()
{
    //dtor
    al_destroy_bitmap(sprite);
}
