#ifndef ENTITY_H
#define ENTITY_H

#include <allegro5/allegro.h>

class Entity
{
    private:
        short type;
        short health;
        int x, y;
        ALLEGRO_BITMAP* sprite;

    public :
        Entity() {health = 1; x = 0; y = 0;}
        Entity(short sx, int sy) {type = 0; health = 1; x = sx; y = sy; sprite = nullptr;}
        Entity(short t, int sx, int sy) {type = t; health = 1; x = sx; y = sy; sprite = nullptr;}
        Entity(short t, short h, int sx, int sy, ALLEGRO_BITMAP* image) {type = t; health = h; x = sx; y = sy; sprite = image;}

        short getType() {return type;}
        short getHealth() {return health;}
        void setHealth(short h) {health = h;}
        bool Hurt(int damage) {health -= damage; return (health <= 0);}
        void newPos(int nx, int ny) {x = nx; y = ny;}
        void setX(int nx) {x = nx;}
        void setY(int ny) {y = ny;}
        int getX() {return x;}
        int getY() {return y;}
        void setSprite(ALLEGRO_BITMAP* image) {sprite = image;}
        ALLEGRO_BITMAP* getSprite() {return sprite;}
        virtual ~Entity();
};

#endif // ENTITY_H

