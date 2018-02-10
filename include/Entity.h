#ifndef ENTITY_H
#define ENTITY_H


class Entity
{
    private:
        int health;
        int x, y;
        bool isAlive;

    public :
        Entity() {health = 1; x = 0; y = 0; isAlive = true;}
        Entity(int h) {health = h; x = 0; y = 0; isAlive = true;}
        Entity(int h, int sx, int sy) {health = h; x = sx; y = sy; isAlive = true;}

        bool Hurt(int damage) {health -= damage; if (health <= 0) isAlive = false; return isAlive;}
        void newPos(int nx, int ny) {x = nx; y = ny;}
        void setX(int nx) {x = nx;}
        void setY(int ny) {y = ny;}
        virtual ~Entity();
};

#endif // ENTITY_H
