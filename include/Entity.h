#ifndef ENTITY_H
#define ENTITY_H

#include <allegro5/allegro.h>

class Entity
{
    private:
        short type;
        short health;
        int x, y;
        ALLEGRO_BITMAP* spriteA;
        ALLEGRO_BITMAP* spriteB;

    public :
        Entity() {health = 1; x = 0; y = 0;}
        Entity(short t, short h, int sx, int sy, ALLEGRO_BITMAP* imageA, ALLEGRO_BITMAP* imageB) {type = t; health = h; x = sx; y = sy; spriteA = imageA; spriteB = imageB;}

        short getType() {return type;}
        short getHealth() {return health;}
        void setHealth(short h) {health = h;}
        bool Hurt(int damage) {health -= damage; return (health <= 0);}
        void newPos(int nx, int ny) {x = nx; y = ny;}
        void setX(int nx) {x = nx;}
        void setY(int ny) {y = ny;}
        int getX() {return x;}
        int getY() {return y;}
        void setSprite(ALLEGRO_BITMAP* image, short i) {(i == 1 ? spriteA : spriteB) = image;}
        ALLEGRO_BITMAP* getSprite(short i) {return (i == 1) ? spriteA : spriteB;}
        virtual ~Entity();
};

#endif // ENTITY_H

