#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "Entity.h"

using namespace std;

struct Projectile {
    short type;
    int x, y;
    int damage, health, counter, created;
    ALLEGRO_BITMAP* spriteA;
    ALLEGRO_BITMAP* spriteB;
    ALLEGRO_BITMAP* currentS;
};

const int MAXLIVES = 5;
const int SCREEN_H = 480;
const int SCREEN_W = 720;
const float FPS = 60;
const ALLEGRO_COLOR BACKGROUND = al_map_rgb(13, 21, 57);
char const *fontName  = "data/courbd.ttf";

enum Projectiles {torpedo = 1, spear = -1, laser = -2, fodder = 11, fishman = 12, langler = 13, miniBoss = 14};

Projectile fireProjectile(short type, int x, int y, const unsigned long &timeCount);
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

	ALLEGRO_SAMPLE* backgroundMusic = NULL;
	ALLEGRO_SAMPLE* shotP = NULL;
	ALLEGRO_SAMPLE* shotE = NULL;
	ALLEGRO_SAMPLE* shotMB = NULL;
	ALLEGRO_SAMPLE* hitP = NULL;
	ALLEGRO_SAMPLE* hitE = NULL;
	ALLEGRO_SAMPLE* death = NULL;

	if (!al_reserve_samples(6)){
      std::cerr << "failed to reserve samples! " << std::endl;
      return -1;
    }

    backgroundMusic = al_load_sample("data/sounds/Background.wav");
    if (!backgroundMusic){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    shotP = al_load_sample("data/sounds/Friendly_Bullet.wav");
    if (!shotP){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    shotE = al_load_sample("data/sounds/Enemy_Bullet.wav");
    if (!shotE){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    shotMB = al_load_sample("data/sounds/Spawn_Thing.wav");
    if (!shotMB){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    hitP = al_load_sample("data/sounds/Player_got_hit.wav");
    if (!hitP){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    hitE = al_load_sample("data/sounds/Successful_Hit.wav");
    if (!hitE){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    death = al_load_sample("data/sounds/Death_sound.wav");
    if (!death){
        std::cerr << "failed to load sample! " << std::endl;
        return -1;
    }

    al_install_keyboard();
    if (!al_install_keyboard()) {
   		std::cerr << "failed to initialize the keyboard! "  << std::endl;
		return -1;
	}
    al_install_mouse();


	al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_FONT* text = al_load_ttf_font(fontName, 36, 0);

	ALLEGRO_DISPLAY *display = NULL;enum Enemy {fodder = 11, fishman = 12, langler = 13};
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


	ALLEGRO_BITMAP* menuBackground = al_load_bitmap("data/images/Menu.png");
	ALLEGRO_BITMAP* startButton = al_load_bitmap("data/images/Start.png");
	ALLEGRO_BITMAP* quitButton = al_load_bitmap("data/images/Quit.png");

	ALLEGRO_BITMAP* backgroundImage = al_load_bitmap("data/images/Background.png");
    ALLEGRO_BITMAP* rocks = al_load_bitmap("data/images/Rock.png");

    ALLEGRO_BITMAP* gameOver = al_load_bitmap("data/images/GameOver.png");
    ALLEGRO_BITMAP* retryButton = al_load_bitmap("data/images/Retry.png");

    al_draw_bitmap(menuBackground, 0, 0, 0);
    al_draw_bitmap(startButton, SCREEN_W/2 - al_get_bitmap_width(startButton)/2, SCREEN_H/2, 0);
    al_draw_bitmap(quitButton, SCREEN_W/2 - al_get_bitmap_width(quitButton)/2, SCREEN_H/2 + 100, 0);

    al_flip_display();

    bool start = false;

    //Menu loop
    while (!start) {
        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 &&
            (ev.mouse.x >= SCREEN_W/2 - al_get_bitmap_width(startButton)/2 && ev.mouse.x <= SCREEN_W/2 + al_get_bitmap_width(startButton)/2) &&
            (ev.mouse.y >= SCREEN_H/2 && ev.mouse.y <= SCREEN_H/2 + al_get_bitmap_height(startButton)/2)) {
            start = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ((ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) ||
                (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 &&
                (ev.mouse.x >= SCREEN_W/2 - al_get_bitmap_width(quitButton)/2 && ev.mouse.x <= SCREEN_W/2 + al_get_bitmap_width(quitButton)/2) &&
                (ev.mouse.y >= SCREEN_H/2 + 100 && ev.mouse.y <= SCREEN_H/2 + 100 + al_get_bitmap_height(quitButton)/2)))) {
                break;
        }
    }

    START:
    if (start) {
        //Setting background
        al_draw_bitmap(backgroundImage, 0, 0, 0);
        al_draw_bitmap(rocks, 0, 0, 0);
        al_flip_display();

        //Starting timer process
        al_start_timer(timer);

        // Loop the sample until the display closes.
        al_play_sample(backgroundMusic, 0.5, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    }

	vector<Projectile> projectiles;
    vector<Projectile> enemies;

    bool redraw = false, dead = false;

    unsigned long timeCount = 0;

    Entity player(-1, MAXLIVES, SCREEN_W/2, SCREEN_H/2, al_load_bitmap("data/images/PlayerA.png"), al_load_bitmap("data/images/PlayerB.png"));
    ALLEGRO_BITMAP* playerCurrent = player.getSprite(1);

    ALLEGRO_BITMAP* heart = al_load_bitmap("data/images/Heart.png");

    //Setting mouse at player
    al_hide_mouse_cursor(display);
	al_grab_mouse(display);
	al_set_mouse_xy(display, SCREEN_W/2, SCREEN_H/2);

	srand(time(NULL));

    while (start && !dead) {
        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) { //acting on time
            redraw = true;
            timeCount++;

            //Enemy spawning
            if (timeCount % 450 == 0)
                enemies.push_back(fireProjectile(fodder, SCREEN_W + 10, rand() % (SCREEN_H - 60) + 15, timeCount)); //CannonFodder
            if (timeCount % 600 == 0)
                enemies.push_back(fireProjectile(fishman, SCREEN_W + 10, rand() % (SCREEN_H - 60) + 15, timeCount)); //Fishman
            if (timeCount % 720 == 0)
                enemies.push_back(fireProjectile(langler, SCREEN_W + 10, rand() % (SCREEN_H - 60) + 15, timeCount)); //Langler

            if (timeCount % 3600 == 0) {
                enemies.push_back(fireProjectile(miniBoss, SCREEN_W + 10, rand() % (SCREEN_H - 60) + 15, timeCount)); //Miniboss
            }

        }
        //Escape cases
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) { //Quits if ESC is pressed
      		break;
		}
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) { //Moving player with mouse
        	if (ev.mouse.y > SCREEN_H - 60)
                player.setX(ev.mouse.x);
            else
                player.newPos(ev.mouse.x, ev.mouse.y);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { //Click to shoot
            if (ev.mouse.button == 1) {
                projectiles.push_back(fireProjectile(torpedo, player.getX() + al_get_bitmap_width(playerCurrent)/2, player.getY() + 12, timeCount));
                al_play_sample(shotP, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
            }
        }
        //Drawing things once changes have happened
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            if (timeCount % 20 == 0) {
                playerCurrent = (playerCurrent == player.getSprite(1)) ? player.getSprite(2) : player.getSprite(1);
            }

            for (unsigned int i = 0; i < enemies.size(); i++) {
                if (enemies[i].type == miniBoss && enemies[i].currentS == enemies[i].spriteA && (timeCount - enemies[i].created) % 360 == 0) {
                    enemies[i].currentS = enemies[i].spriteB;
                }
                else if (enemies[i].type == miniBoss && enemies[i].currentS == enemies[i].spriteB && (timeCount - enemies[i].created) % 120 == 0) {
                    enemies[i].currentS = enemies[i].spriteA;
                }
                else if (enemies[i].type != miniBoss && (timeCount - enemies[i].created) % 30 == 0) {
                    enemies[i].currentS = (enemies[i].currentS == enemies[i].spriteA) ? enemies[i].spriteB : enemies[i].spriteA;
                }
            }

            for (unsigned int i = 0; i < projectiles.size(); i++) {
                if ((timeCount - projectiles[i].created) % 30 == 0) {
                    projectiles[i].currentS = (projectiles[i].currentS == projectiles[i].spriteA) ? projectiles[i].spriteB : projectiles[i].spriteA;
                }
            }

            //Running through enemies for movement and attacking
            for (unsigned int i = 0; i < enemies.size(); i++) {
                if (enemies[i].type == fodder && timeCount % 5 == 0) //Cannon fodder movement
                    enemies[i].x -= 3;
                else if (enemies[i].type == fishman && timeCount % 10 == 0) { //Fishmen moving
                    enemies[i].x -= 3;
                    if (enemies[i].x < SCREEN_W - SCREEN_W/3 || rand() % 80 == 0) { //Swapping to up and down
                        enemies[i].type += 10;
                    }
                }
                else if ((enemies[i].type == fishman + 10 || enemies[i].type == miniBoss) && timeCount % 10 == 0) { //Up and down fishmen shifting
                    if (enemies[i].counter == 0)
                        enemies[i].counter = (rand() % 2 == 0) ? -6 : 6;

                    if (enemies[i].counter < 0) {
                        enemies[i].y -= 3;
                        enemies[i].counter++;

                        if (enemies[i].y < 10)
                            enemies[i].counter *= -1;
                    }
                    else if (enemies[i].counter > 0) {
                        enemies[i].y += 3;
                        enemies[i].counter--;

                        if (enemies[i].y > SCREEN_H - 60)
                            enemies[i].counter *= -1;
                    }
                }

                //Removing off-screen enemies
                if (enemies[i].x < 0) {
                    enemies.erase(enemies.begin() + i);enemies.push_back(fireProjectile(fishman, SCREEN_W + 10, rand() % (SCREEN_H - 60) + 15, timeCount)); //Fishman
                }

                if ((enemies[i].type % 10 == 2) && timeCount % 180 == 0) {
                    projectiles.push_back(fireProjectile(spear, enemies[i].x, enemies[i].y, timeCount));
                    al_play_sample(shotE, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                }
                else if (enemies[i].type == miniBoss && (timeCount - enemies[i].created) % 360 == 0) {
                    enemies.push_back(fireProjectile(fodder, enemies[i].x - 200, enemies[i].y, timeCount));
                    enemies.push_back(fireProjectile(fodder, enemies[i].x - 175, enemies[i].y - 25, timeCount));
                    enemies.push_back(fireProjectile(fodder, enemies[i].x - 175, enemies[i].y + 25, timeCount));
                    al_play_sample(shotMB, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                }

                else if ((enemies[i].type == langler) && timeCount % 720 == 0) {
                    projectiles.push_back(fireProjectile(laser, enemies[i].x - 60, enemies[i].y, timeCount));
                }
            }

            //Moving projectiles
            if (timeCount % 2 == 0) {
                for (unsigned int i = 0; i < projectiles.size(); i++) {
                    if (projectiles[i].type > 0)
                        projectiles[i].x += 3; //Friendly projectiles
                    else if (projectiles[i].type != laser && projectiles[i].type < 0)
                        projectiles[i].x -= 3; //Enemy projectiles

                    //Deleting off-screen projectiles
                    if (projectiles[i].x > SCREEN_W || projectiles[i].x < 0) {
                        projectiles.erase(projectiles.begin() + i);
                    }
                }
            }

            //Collision checks
            for (unsigned int i = 0; i < projectiles.size(); i++) {
                for (unsigned int j = 0; j < enemies.size(); j++) {
                    if (projectiles[i].type > 0 && checkCollision(projectiles[i], enemies[j])) {
                        enemies[j].health -= projectiles[i].damage;
                        projectiles.erase(projectiles.begin() + i);

                        al_play_sample(hitE, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);

                        if (enemies[j].health <= 0) {
                            enemies.erase(enemies.begin() + j);
                        }
                    }

                    static Projectile playerP;
                    playerP.currentS = playerCurrent;
                    playerP.x = player.getX();
                    playerP.y = player.getY();

                    //Laser timeout
                    if (projectiles[i].type == laser && (timeCount - projectiles[i].created) > 0 && (timeCount - projectiles[i].created) % 240 == 0) {
                        projectiles.erase(projectiles.begin() + i);
                        break;
                    }
                    else if (checkCollision(projectiles[i], playerP)) {
                        if (projectiles[i].type < 0 && projectiles[i].type != laser) {
                            player.Hurt(projectiles[i].damage);
                            projectiles.erase(projectiles.begin() + i);
                            al_play_sample(hitP, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                            break;
                        }
                    }
                    else if (projectiles[i].type == laser && player.getY() >= projectiles[i].y - al_get_bitmap_height(projectiles[i].currentS)/2 && player.getY() <= projectiles[i].y + al_get_bitmap_height(projectiles[i].currentS)/2) {
                        if (projectiles[i].counter == 0 || (timeCount - projectiles[i].counter != 0 && (timeCount - projectiles[i].counter) % 120 == 0)) {
                            player.Hurt(projectiles[i].damage);
                            al_play_sample(hitP, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                            projectiles[i].counter = timeCount;
                        }
                    }

                }
            }

            if (player.getHealth() <= 0) {//Quit at death
                al_play_sample(death, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                dead = true;
                break;
            }

            //Collision checking with player
            for (unsigned int i = 0; i < enemies.size(); i ++) {
                static Projectile playerP;
                playerP.currentS = playerCurrent;
                playerP.x = player.getX();
                playerP.y = player.getY();

                if (checkCollision(enemies[i], playerP)) {
                    player.Hurt(enemies[i].damage);
                    if (enemies[i].type == fodder) {
                        enemies.erase(enemies.begin() + i);
                    }
                }
            }

            //Background
            al_draw_bitmap(backgroundImage, 0, 0, 0);

            //Drawing enemies
            for (unsigned int i = 0; i < enemies.size(); i++)
                al_draw_bitmap(enemies[i].currentS, enemies[i].x - al_get_bitmap_width(enemies[i].currentS), enemies[i].y - al_get_bitmap_height(enemies[i].currentS)/2, 0);

            //Drawing projectiles
            for (unsigned int i = 0; i < projectiles.size(); i++) {
                al_draw_bitmap(projectiles[i].currentS, projectiles[i].x - al_get_bitmap_width(projectiles[i].currentS), projectiles[i].y - al_get_bitmap_height(projectiles[i].currentS)/2, 0);
            }


            //Drawing player
            al_draw_bitmap(playerCurrent, player.getX() - al_get_bitmap_width(playerCurrent)/2, player.getY() - al_get_bitmap_height(playerCurrent)/2, 0);
            for (int i = 0; i < player.getHealth(); i++) {
                al_draw_bitmap(heart, 10 * (i+1) + (i * al_get_bitmap_width(heart)), 10, 0);
            }

            //Looping rocks for the bottom
            al_draw_bitmap_region(rocks, 0 + timeCount % 1080, 0, 1080, 480, 0, 0, 0);
            al_draw_bitmap(rocks, 0 + 1080 - timeCount % 1080, 0, 0);
            al_flip_display();
        }
    }

    al_show_mouse_cursor(display);
    al_ungrab_mouse();
    al_stop_samples();


    if (dead) {
        al_draw_bitmap(gameOver, 0, 0, 0);
        al_draw_textf(text, al_map_rgb(153, 130, 31), 440, 215, ALLEGRO_ALIGN_CENTRE, "%d", timeCount/120);
        al_draw_bitmap(retryButton, SCREEN_W/3 - al_get_bitmap_width(retryButton)/2, SCREEN_H - 100, 0);
        al_draw_bitmap(quitButton, SCREEN_W - SCREEN_W/3 - al_get_bitmap_width(retryButton)/2, SCREEN_H - 100, 0);
        al_flip_display();
    }

    //Game over screen loop
    while (dead) {
        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

        if ((ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 &&
            (ev.mouse.x >= SCREEN_W/3 - al_get_bitmap_width(retryButton)/2 && ev.mouse.x <= SCREEN_W/3 + al_get_bitmap_width(retryButton)/2) &&
            (ev.mouse.y >= SCREEN_H - 100 && ev.mouse.y <= SCREEN_H - 100 + al_get_bitmap_height(retryButton)/2))) {
            start = true;
            goto START;
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ((ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) ||
                (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 &&
                (ev.mouse.x >= SCREEN_W - SCREEN_W/3 - al_get_bitmap_width(quitButton)/2 && ev.mouse.x <= SCREEN_W - SCREEN_W/3 + al_get_bitmap_width(quitButton)/2) &&
                (ev.mouse.y >= SCREEN_H - 100 && ev.mouse.y <= SCREEN_H - 100 + al_get_bitmap_height(quitButton)/2)))) {
                break;
        }
    }

    //BYE!!
    cout << "Thanks for playing, this game was made in 18 hours by Nick Leiper, Gregory Pope, and Kevin Chen" << endl;

    return 0;
}

//Function to create projectiles/enemies
Projectile fireProjectile(short type, int x, int y, const unsigned long &timeCount) {
    Projectile temp;

    temp.type = type;
    temp.x = x;
    temp.y = y;
    temp.counter = 0;
    temp.created = timeCount;

    switch (type) {
        case fodder:
            temp.health = 1;
            temp.damage = 1;
            temp.spriteA = al_load_bitmap("data/images/FodderA.png");
            temp.spriteB = al_load_bitmap("data/images/FodderB.png");

            break;

        case fishman:
            temp.health = 3;
            temp.damage = 1;
            temp.spriteA = al_load_bitmap("data/images/FishmanA.png");
            temp.spriteB = al_load_bitmap("data/images/FishmanB.png");
            break;

        case langler:
            temp.health = 6;
            temp.damage = 2;
            temp.spriteA = al_load_bitmap("data/images/LanglerA.png");
            temp.spriteB = al_load_bitmap("data/images/LanglerB.png");
            break;

        case miniBoss:
            temp.health = 12;
            temp.damage = 3;
            temp.spriteA = al_load_bitmap("data/images/MiniBossA.png");
            temp.spriteB = al_load_bitmap("data/images/MiniBossB.png");
            break;

        case torpedo:
            temp.health = 1;
            temp.damage = 1;
            temp.spriteA = al_load_bitmap("data/images/Torpedo.png");
            temp.spriteB = temp.spriteA;
            break;

        case spear:
            temp.health = 1;
            temp.damage = 1;
            temp.spriteA = al_load_bitmap("data/images/Spear.png");
            temp.spriteB = temp.spriteA;
            break;

        case laser:
            temp.health = 1;
            temp.damage = 1;
            temp.spriteA = al_load_bitmap("data/images/LaserA.png");
            temp.spriteB = al_load_bitmap("data/images/LaserB.png");
            break;

    }

    temp.currentS = temp.spriteA;

    if (!temp.currentS)
        cerr << "error loading sprite for projectile " << type << endl;

    return temp;
}

//Collision checking... pretty self explanatory
bool checkCollision(const Projectile &projectile, const Projectile &enemy) {
    int projectile_top = projectile.y - al_get_bitmap_height(projectile.currentS)/2;
	int projectile_bottom = projectile.y + al_get_bitmap_height(projectile.currentS)/2;
	int projectile_left = projectile.x - al_get_bitmap_width(projectile.currentS)/2;
	int projectile_right = projectile.x + al_get_bitmap_width(projectile.currentS)/2;

	int enemy_top = enemy.y - al_get_bitmap_height(enemy.currentS)/2;
	int enemy_bottom = enemy.y + al_get_bitmap_height(enemy.currentS)/2;
	int enemy_left = enemy.x - al_get_bitmap_width(enemy.currentS)/2;
	int enemy_right = enemy.x + al_get_bitmap_width(enemy.currentS)/2;

	if (enemy_bottom < projectile_top || enemy_top > projectile_bottom ||
		enemy_right < projectile_left || enemy_left > projectile_right) {
		return false;
    }

    return true;
}
