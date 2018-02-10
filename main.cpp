#include <iostream>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "Entity.h"

using namespace std;

struct Projectile {
    int x, y;

    int damage;
};

const int MAXHEALTH = 5;
const int SCREEN_H = 480;
const int SCREEN_W = 720;
const float FPS = 60;

int main() {
    //Allegro initialization
	al_init();
	al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_KEYBOARD_STATE key_state;
	ALLEGRO_MOUSE_STATE mouse_state;

    Entity Player(MAXHEALTH, SCREEN_H/2, SCREEN_W/2);


    event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());;

	//Starting timer process
	al_start_timer(timer);

	vector<Projectile> projectiles;

    bool start = true;

    while (start) {

        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //Quits if ESC is pressed
      		start = false;
		}
        else if (ev.type = ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {

        }

    }


    return 0;
}
