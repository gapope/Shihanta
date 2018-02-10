#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Entity.h"

using namespace std;

struct Projectile {
    short type;
    int x, y;
    int damage, health;
    ALLEGRO_BITMAP* sprite;
};

const int MAXLIVES = 3;
const int SCREEN_H = 480;
const int SCREEN_W = 720;
const float FPS = 60;

Projectile fireProjectile(short type, int x, int y);
bool checkCollision(const Projectile &projectile, const Projectile &enemy);

int main() {
    //Allegro initialization
	if (!al_init()){
    	std::cerr << "failed to initialize allegro! "  << std::endl;
		return -1;
	}

	al_init_image_addon();

	if (!al_install_audio()) {
        std::cerr << "failed to install audio! "  << std::endl;
		return -1;
	}
    if(!al_init_acodec_addon()) {
        std::cerr << "failed to install codec! " << std::endl;
        return -1;
    }


	ALLEGRO_SAMPLE* background = NULL;
	if (!al_reserve_samples(1)){
      std::cerr << "failed to reserve sample! " << std::endl;
      return -1;
    }

    background = al_load_sample("data/sounds/Background.wav");
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

    event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());;

	//Setting back colour
	al_clear_to_color(al_map_rgb(0, 0, 255));
	al_flip_display();

	//Starting timer process
	al_start_timer(timer);

	/* Loop the sample until the display closes. */
    al_play_sample(background, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);

	vector<Projectile> projectiles;
    vector<Projectile> enemies;

    bool redraw = false;

    unsigned int timeCount = 0;

    Entity player(-1, MAXLIVES, SCREEN_W/2, SCREEN_H/2, al_load_bitmap("data/images/Player.png"));

    //Setting mouse at player
	al_grab_mouse(display);
	al_set_mouse_xy(display, SCREEN_W/2, SCREEN_H/2);

	srand(time(NULL));

    while (true) {

        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) { //acting on time
            redraw = true;
            timeCount++;

            //Enemy spawning
            if (timeCount % 300 == 0) {
                enemies.push_back(fireProjectile(11, SCREEN_W + 10, rand() % (SCREEN_H - 15) + 15)); //CannonFodder
                enemies.push_back(fireProjectile(12, SCREEN_W + 10, rand() % (SCREEN_H - 15) + 15)); //Fishman
            }
        }
        //Escape cases
        else if (player.getHealth() <= 0 ||ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //Quits if ESC is pressed
      		break;
		}
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) { //Moving player with mouse
        	player.newPos(ev.mouse.x, ev.mouse.y);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { //Click to shoot
            if (ev.mouse.button == 1)
                projectiles.push_back(fireProjectile(1, player.getX() + al_get_bitmap_width(player.getSprite())/2, player.getY() + 12));
        }

        //Drawing things once changes have happened
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            //Running through enemies for movement and attacking
            for (unsigned int i = 0; i < enemies.size(); i++) {
                if (enemies[i].type == 11 && timeCount % 5 == 0)
                    enemies[i].x -= 3;
                else if (enemies[i].type == 12 && timeCount % 10 == 0)
                    enemies[i].x -= 3;

                if (enemies[i].x < 0) {
                    enemies.erase(enemies.begin() + i);
                }

                if (enemies[i].type == 12 && timeCount % 180 == 0)
                    projectiles.push_back(fireProjectile(-1, enemies[i].x, enemies[i].y));
            }

            //Moving projectiles
            if (timeCount % 2 == 0) {
                for (unsigned int i = 0; i < projectiles.size(); i++) {
                    if (projectiles[i].type > 0)
                        projectiles[i].x += 3; //Friendly projectiles
                    else if (projectiles[i].type < 0)
                        projectiles[i].x -= 3; //Enemy projectiles

                    //Deleting off-screen projectiles
                    if (projectiles[i].x > SCREEN_W || projectiles[i].x < 0) {
                        projectiles.erase(projectiles.begin() + i);
                    }
                }
            }

            //Collision checks
            for (unsigned int i = 0; i < enemies.size(); i++) {
                for (unsigned int j = 0; j < projectiles.size(); j++) {
                    if (projectiles[i].type > 0 && checkCollision(projectiles[j], enemies[i])) {
                            enemies[i].health -= projectiles[j].damage;
                            projectiles.erase(projectiles.begin() + j);


                            if (enemies[i].health <= 0) {
                                enemies.erase(enemies.begin() + i);

                            }
                    }
                }
            }

            //Collision checking with player
            for (unsigned int i = 0; i < enemies.size(); i ++) {
                static Projectile playerP;
                playerP.sprite = player.getSprite();
                playerP.x = player.getX();
                playerP.y = player.getY();

                if (checkCollision(enemies[i], playerP)) {
                    player.Hurt(enemies[i].damage);
                    if (enemies[i].type == 11) {
                        enemies.erase(enemies.begin() + i);
                        i--;
                    }
                }
            }

            //Background
            al_clear_to_color(al_map_rgb(0, 0, 255));

            //Drawing enemies
            for (unsigned int i = 0; i < enemies.size(); i++)
                al_draw_bitmap(enemies[i].sprite, enemies[i].x - al_get_bitmap_width(enemies[i].sprite), enemies[i].y - al_get_bitmap_height(enemies[i].sprite)/2, 0);

            //Drawing projectiles
            for (unsigned int i = 0; i < projectiles.size(); i++)
                al_draw_bitmap(projectiles[i].sprite, projectiles[i].x - al_get_bitmap_width(projectiles[i].sprite), projectiles[i].y - al_get_bitmap_height(projectiles[i].sprite)/2, 0);

            //Drawing player
            al_draw_bitmap(player.getSprite(), player.getX() - al_get_bitmap_width(player.getSprite())/2, player.getY() - al_get_bitmap_height(player.getSprite())/2, 0);
        }

        al_flip_display();
    }

    //BYE!!
    cout << "Thanks for playing" << endl;

    return 0;
}

//Function to create projectiles
Projectile fireProjectile(short type, int x, int y) {
    Projectile temp;

    temp.type = type;
    temp.x = x;
    temp.y = y;

    switch (type) {
        case 11:
            temp.health = 1;
            temp.damage = 1;
            temp.sprite = al_load_bitmap("data/images/CannonFodder.png");
            break;
        case 12:
            temp.health = 6;
            temp.damage = 1;
            temp.sprite = al_load_bitmap("data/images/Fishman.png");
            break;
        case 1:
            temp.health = 1;
            temp.damage = 1;
            temp.sprite = al_load_bitmap("data/images/Torpedo.png");

            static ALLEGRO_SAMPLE* shot = nullptr;
            if (!al_reserve_samples(1)){
                std::cerr << "failed to reserve sample! " << std::endl;
                break;
            }
            shot = al_load_sample("Successful_Hit.wav");
            al_play_sample(shot, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
            break;

        case -1:
            temp.health = 1;
            temp.damage = 1;
            temp.sprite = al_load_bitmap("data/images/Spear.png");
            break;
    }

    if (!temp.sprite)
        cerr << "error loading sprite for projectile " << type << endl;

    return temp;
}

//Collision checking... pretty self explanatory
bool checkCollision(const Projectile &projectile, const Projectile &enemy) {
    int projectile_top = projectile.y - al_get_bitmap_height(projectile.sprite)/2;
	int projectile_bottom = projectile.y + al_get_bitmap_height(projectile.sprite)/2;
	int projectile_left = projectile.x - al_get_bitmap_width(projectile.sprite)/2;
	int projectile_right = projectile.x + al_get_bitmap_width(projectile.sprite)/2;

	int enemy_top = enemy.y - al_get_bitmap_height(enemy.sprite)/2;
	int enemy_bottom = enemy.y + al_get_bitmap_height(enemy.sprite)/2;
	int enemy_left = enemy.x - al_get_bitmap_width(enemy.sprite)/2;
	int enemy_right = enemy.x + al_get_bitmap_width(enemy.sprite)/2;

	if (enemy_bottom < projectile_top || enemy_top > projectile_bottom ||
		enemy_right < projectile_left || enemy_left > projectile_right) {
		return false;
	}
	else {
        true;
	}
}
