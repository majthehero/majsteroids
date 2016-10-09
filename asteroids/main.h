#pragma once

#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>

#include<allegro5\allegro.h>
#include<allegro5\allegro_font.h>
#include<allegro5\allegro_ttf.h>
#include<allegro5\allegro_image.h>
#include<allegro5\allegro_color.h>

// constants
#define PI 3.14159265359

#define WIDTH 600
#define HEIGHT 400
#define DESIRED_FPS 32

#define PC_ACCELERATION_SPEED 0.1
#define PC_ROTATION_SPEED 0.07
#define PC_MAX_SPEED 5.0

#define BULLET_SPEED_BASE 10
#define BULLET_SPEED_MODIFIER 2

#define GRAVITY_DIST 300
#define GRAVITY_CONSTANT 10000

#define ASTEROID_SPEED_MODIFIER 0.5
#define ASTEROID_SPEED_BASE 3

#define EDGE_UP 1
#define EDGE_RIGHT 2
#define EDGE_DOWN 3
#define EDGE_LEFT 4

//game_state
#define GAME 1
#define MENU 2

// functions
int main();
int initAllegro();

// allegro is tha bast
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_KEYBOARD_STATE *keyboard_state;
extern ALLEGRO_COLOR uiColor;
extern ALLEGRO_FONT *uiFont;
extern float uiHue;

// sprites
extern ALLEGRO_BITMAP *spaceshipSprite;
extern ALLEGRO_BITMAP *bulletSprite;
extern ALLEGRO_BITMAP *asteroidSprite;
extern ALLEGRO_BITMAP *explosionSprite;
extern ALLEGRO_BITMAP *space_bg;
extern ALLEGRO_BITMAP *heartSprite;

// data structures

class Direction {
private:
	float phi;
	float length;
public:
	Direction() {
		float phi = (float)rand() / (float)RAND_MAX * 2 * PI;
		float length = 1.0;
		this->phi = phi;
		this->length = length;
	}
	Direction(float phi, float length) {
		if (length < 0) {
			length *= -1;
			phi += 2 * PI;
		}
		this->length = length;
		while (phi > 2 * PI) phi -= 2 * PI;
		while (phi < 2 * PI) phi += 2 * PI;
		this->phi = phi;
	}
	float getPhi() { return phi; }
	float getLength() { return length; }
	void changePhi(float dPhi) {
		phi += dPhi;
	}
	void changeLength(float dLength) {
		length += dLength;
	}
	void replacePhi(float phi) {
		this->phi = phi;
	}
	void replaceLength(float length) {
		this->length = length;
	}
	float *getVector() {
		return getVector(phi, length);
	}
	void add(Direction* dir) {
		phi += dir->getPhi();
		length *= dir->length;
	}
	static float *getVector(float phi, float length) {
		float* retval = new float[2];
		retval[0] = sin(phi) * length;
		retval[1] = cos(phi) * length;
		return retval;
	}
	static Direction *weightedSum(Direction *dir1, float w1, Direction *dir2, float w2) {
		float wPhi = dir1->getPhi()*w1 + dir2->getPhi()*w2;
		float wLength = dir1->getLength()*w1 + dir2->getLength()*w2;
		return new Direction(wPhi, wLength);
	}
	static Direction *negate(Direction *dir) {
		return new Direction(dir->getPhi() + 2 * PI, dir->getLength());
	}
	static float makePhi(float x, float y) {
		return atan2(y, x);
	}
};

class Bullet {
private:
	Direction *dir;
	int animationFrame;
	bool alive;
public:
	float x;
	float y;

	Bullet(float x, float y, Direction *dir, int weaponLevel) {
		this->dir = new Direction(dir->getPhi(), dir->getLength());
		this->dir->changeLength(BULLET_SPEED_MODIFIER + BULLET_SPEED_BASE); // dodaj level bulleta
		this->x = x;
		this->y = y;
		this->alive = true;
	}

	void kill() {
		alive = false;
	}

	bool isDead() {
		return !alive;
	}

	static bool toRemove(Bullet *b) {
		return b->isDead();
	}

	void move() {
		x += dir->getVector()[0];
		y -= dir->getVector()[1];
	}

	bool draw() {
		if (x > WIDTH + 100 || x < -100 || y > HEIGHT + 100 || y < -100) kill();
		if (!alive) return false;
		int srcx = animationFrame * 32;
		ALLEGRO_BITMAP *subsprite = al_create_sub_bitmap(bulletSprite, srcx, 0, 32, 32);
		al_draw_rotated_bitmap(subsprite, 16, 16, x, y, dir->getPhi(), NULL);
		al_destroy_bitmap(subsprite);
		subsprite = NULL;
		animationFrame = (animationFrame + 1) % 4;
		return true;
	}
};

class Asteroid {
private:
	bool alive;
public:
	float x;
	float y;
	Direction *dir;
	int animationFrame;

	Asteroid() {
		alive = true;
		float phi = 0;
		int sourceEdge = rand() % 4;
		if (sourceEdge == EDGE_UP) {
			y = -30;
			x = rand() % WIDTH;
			phi = (float)rand() / (float)RAND_MAX * PI + PI/2;
		}
		else if (sourceEdge == EDGE_DOWN) {
			y = HEIGHT + 30;
			x = rand() % WIDTH;
			phi = (float)rand() / (float)RAND_MAX * PI - PI/2 ;
		}
		else if (sourceEdge == EDGE_LEFT) {
			x = -30;
			y = rand() % HEIGHT;
			phi = (float)rand() / (float)RAND_MAX *  PI;
		}
		else if (sourceEdge == EDGE_RIGHT) {
			x = WIDTH + 30;
			y = rand() % HEIGHT;
			phi = (float)rand() / (float)RAND_MAX * PI + PI;
		}
		dir = new Direction(phi, (float)ASTEROID_SPEED_MODIFIER + ASTEROID_SPEED_BASE); // dodaj level modifier
		float length = (float)ASTEROID_SPEED_MODIFIER + ASTEROID_SPEED_BASE;
	}

	void kill() {
		alive = false;
	}

	bool isDead() {
		return !alive;
	}

	static bool toRemove(Asteroid *a) {
		return a->isDead();
	}

	bool draw() {
		if (x > WIDTH + 100 || x < -100 || y > HEIGHT + 100 || y < -100) kill();
		if (!alive) return false;
		float srcx = animationFrame % 8 * 32;
		float srcy = animationFrame / 8 * 32;
		al_draw_bitmap_region(asteroidSprite, srcx, srcy, 32, 32, x, y, NULL);
		animationFrame = (animationFrame + 1) % 64;
		return true;
	}

	void move() {
		x += dir->getVector()[0];
		y -= dir->getVector()[1];
	}

	float distanceTo(float x, float y) {
		float dist = (this->x - x)*(this->x - x) + (this->y - y)*(this->y - y);
		dist = sqrt(dist);
		return dist;
	}

	void accelerate(Direction *dDir) {
		dir->add(dDir);
	}
};

class Ship {
private:
	int weaponLevel;
	int lives;
	int invincibility;
	ALLEGRO_COLOR colorDmg;
	int shootCounter;
public:
	float x;
	float y;
	Direction *dir;

	Ship() {
		shootCounter = 0;
		lives = 5;
		invincibility = 0;
		colorDmg = al_map_rgba_f(1.0, 0, 0, 0.5);
		weaponLevel = 1;
		x = WIDTH / 2;
		y = HEIGHT / 2;
		dir = new Direction(0, 1.0);
		dir->changeLength(0.0);
	}

	bool draw() {
		// lower counters
		if (invincibility > 0) invincibility--;
		if (shootCounter > 0) shootCounter--;
		// smart draw (tm)
		if (invincibility % 2 == 1)
			al_draw_tinted_rotated_bitmap(spaceshipSprite, colorDmg, 16, 16, x, y, dir->getPhi(), NULL);
		else 
			al_draw_rotated_bitmap(spaceshipSprite, 16, 16, x, y, dir->getPhi(), NULL);
		return true;
	}

	void kill() {
		if (invincibility != 0) return;
		lives--;
		invincibility = 20;
	}

	int getLives() { return lives; }

	void changeDirection(float angle) {
		dir->changePhi(angle);
	}

	void accelerate(float a) {
		dir->changeLength(a);
		if (dir->getLength() > PC_MAX_SPEED)
			dir->replaceLength(PC_MAX_SPEED);
		if (dir->getLength() < 0.0)
			dir->replaceLength(0.0);
	}

	void move() {
		x += dir->getVector()[0];
		y -= dir->getVector()[1];
	}

	Bullet *shoot() {
		Bullet *b =  new Bullet(x, y, dir, weaponLevel);
		if (shootCounter != 0) b->kill();
		else shootCounter = 8;
		return b;
	}
};

class Explosion {
private:
	float x;
	float y;
	int animationFrame;
	bool alive;
	void kill() {
		alive = false;
	}
public:
	Explosion(float x, float y) {
		this->x = x;
		this->y = y;
		animationFrame = 0;
		alive = true;
	}

	bool isDead() {
		return !alive;
	}

	static bool toRemove(Explosion *e) {
		return e->isDead();
	}

	bool draw() {
		if (animationFrame > 63) kill();
		if (!alive) return false;
		float srcx = animationFrame % 8 * 64;
		float srcy = animationFrame / 8 * 64;
		al_draw_bitmap_region(explosionSprite, srcx, srcy, 64, 64, x, y, NULL);
		animationFrame = animationFrame + 1;
		return true;
	}
};

class Game {
	Ship *player;
	std::vector<Asteroid*> *asteroids;
	std::vector<Bullet*> *bullets;
	std::vector<Explosion*> *explosions;

	int level;
	int destroyedAsteroids;
	int nextLevel;
	bool game_loop;

	char* levelMsg;
	char* nextLevelMsg;

	void handleInput();
	void calcPhysics();
	void render();
	void burryDead();
	void keepFPS();
	void gameLogic();
	void endGame(int end_mode);
public:
	Game();
	void gameLoop();
};

class Menu {
private:
	char **items;
	int selectedItem;
	bool menu_loop;
	bool toExecute;

	void exitBeautifully();
	void startGameBeautifully();
	void handleInput();
	void render();
	void execute();
	void keepFPS();
public:
	Menu();
	void menuLoop();
};
