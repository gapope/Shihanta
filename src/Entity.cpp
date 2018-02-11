#include "Entity.h"


Entity::~Entity()
{
    //dtor
    al_destroy_bitmap(spriteA);
    al_destroy_bitmap(spriteB);
}
