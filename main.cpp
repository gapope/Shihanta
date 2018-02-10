#include <iostream>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include "Entity.h"

using namespace std;

struct Projectile {
    int x, y;
    int damage;
    ALLEGRO_BITMAP* sprite;
};

const int MAXLIVES = 3;
const int SCREEN_H = 480;
const int SCREEN_W = 720;
const float FPS = 60;

Projectile fireProjectile(short type, int x, int y);

int main() {
    //Allegro initialization
	if (!al_init()) {
    	std::cerr << "failed to initialize allegro! "  << std::endl;
		return -1;
	}

	al_init_image_addon();

	if (!al_install_audio()) {
        std::cerr << "failed to install audio! "  << std::endl;
		return -1;
	}
	ALLEGRO_SAMPLE* background = NULL;
	if (!al_reserve_samples(1)){
      std::cerr << "failed to reserve sample! " << std::std::cerr << "failed to reserve sample! " << std::endl;endl;
      return -1;
    }

    background = al_load_sample("data/sounds/Background.mp3");
    if (!background){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    al_install_keyboard();
    if (!al_install_keyboard()) {
   		std::cerr << "failed to initialize the keyboard! "  << std::endl;
		return -1;
	}
    al_install_mouse();

	ALLEGRO_DISPLAY *display = NULL;
	display = al_create_display(SCREEN_W, SCREEN_H); //Creating screen
   	if (!display) {
      std::cerr << "failed to create display!" << std::endl;

      return -1;
	}

	ALLEGRO_EVENT_QUEUE *event_queue = NULL;

	ALLEGRO_TIMER *timer = NULL;
	timer = al_create_timer(0.5 / FPS);
	if (!timer) {
      std::cerr << "failed to create timer!" << std::endl;

      return -1;
	}

	ALLEGRO_KEYBOARD_STATE key_state;
	ALLEGRO_MOUSE_STATE mouse_state;

    event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());;

	//Setting back cProjectile fireProjectile(short type, int x, int y)olour
	al_clear_to_color(al_map_rgb(0, 0, 255));
	al_flip_display();

	//Starting timer process
	al_start_timer(timer);

	/* Loop the sample until the display closes. */
   al_play_sample(background, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);

	vector<Projectile> projectiles;

    bool start = true;

    Entity player(MAXLIVES, SCREEN_W/2, SCREEN_H/2, al_load_bitmap("data/images/player.png"));

    //Setting mouse at player
	al_grab_mouse(display);
	al_set_mouse_xy(display, SCREEN_W/2, SCREEN_H/2);

    while (start) {

        al_clear_to_color(al_map_rgb(0, 0, 255));

        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //std::cerr << "failed to reserve sample! " << std::endl;Quitting if window is closed
        	break;
      	}
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //Quits if ESC is pressed
      		start = false;
		}
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) { //Moving player with mouse
        	player.newPos(ev.mouse.x, ev.mouse.y);
        	al_draw_bitmap(player.getSprite(), player.getX() - al_get_bitmap_width(player.getSprite())/2, player.getY() - al_get_bitmap_height(player.getSprite())/2, 0);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { //Click to shoot
            projectiles.push_back(fireProjectile(0, player.getX() + al_get_bitmap_width(player.getSprite())/2, player.getY()));
        }

        al_flip_display();

    }


    return 0;
}

Projectile fireProjectile(short type, int x, int y) {
    Projectile temp;


    switch (type) {

        case 0:
            temp.x = x;
            temp.y = y;
            temp.damage = 1;
            break;
    }

    return temp;
}
