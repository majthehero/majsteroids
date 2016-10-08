#include "main.h"

using namespace std;

int main() {
	initAllegro();
	initData();
	while (1) {
		if (game_state == GAME) {
			while (game_loop) {
				handleInput();
				calcPhysics();
				render();
				burryDead();
				keepFPS();
				gameLogic();
			}
		}
		else if (game_state == MENU) {
			Menu *menu = new Menu();
			while (menu->isShowMenu()) {
				menu->handleInput();
				menu->render();
				menu->execute();
			}
		}
	}
}

int initData()
{
	game_state = GAME;
	game_loop = true;
	levelMsg = (char*)calloc(100, sizeof(char));
	nextLevelMsg = (char*)calloc(100, sizeof(char));
	uiFont = al_load_ttf_font("font.ttf", 32, NULL);
	uiHue = 0.0;
	destroyedAsteroids = 0;
	player = new Ship();
	level = 1;
	nextLevel = 10;
	asteroids = new vector<Asteroid*>();
	explosions = new vector<Explosion*>();
	bullets =new vector<Bullet*>();
	space_bg = al_load_bitmap("128px_stars_bg.tga");
	spaceshipSprite = al_load_bitmap("32px_spaceship.tga");
	asteroidSprite = al_load_bitmap("32px_64f_asteroid.tga");
	bulletSprite = al_load_bitmap("32px_4f_bullet.tga");
	explosionSprite = al_load_bitmap("64px_64f_explosion.tga");
	heartSprite = al_load_bitmap("32px_heart.tga");
	if (!space_bg || !spaceshipSprite || !asteroidSprite || !bulletSprite || 
		!explosionSprite || !heartSprite) {
		cerr << "Failed to load sprites." << "\n";
		exit(5);
	}
	return 0;
}

int initAllegro()
{
	if (!al_init()) {
		cerr << "Failed to init allegro.\n";
		exit(1);
	}
	if (!al_init_image_addon()) {
		cerr << "Failed to init allegro image addon.\n";
		exit(1);
	}
	if (!al_init_font_addon()) {
		cerr << "Failed to init allegro font addon.\n";
		exit(1);
	}
	if (!al_init_ttf_addon()) {
		cerr << "Failed to init allegro true type font addon.\n";
		exit(1);
	}
	display = al_create_display(WIDTH, HEIGHT);
	if (!display) {
		cerr << "Failed to create display.\n";
		exit(1);
	}
	if (!al_install_keyboard()) {
		cerr << "Failed to init keyboard input.\n";
		exit(1);
	}
	if (!al_install_mouse()) {
		cerr << "Failed to init mouse input.\n";
		exit(1);
	}
	keyboard_state = (ALLEGRO_KEYBOARD_STATE*)malloc(sizeof(ALLEGRO_KEYBOARD_STATE));
	return 0;
}

void handleInput() {
	al_get_keyboard_state(keyboard_state);
	if (!keyboard_state) {
		cerr << "Failed to get keyboard state.\n";
		exit(2);
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_W)) {
		player->accelerate(PC_ACCELERATION_SPEED);
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_A)) {
		player->changeDirection(PC_ROTATION_SPEED * -1.0);
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_D)) {
		player->changeDirection(PC_ROTATION_SPEED);
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_S)) {
		player->accelerate(PC_ACCELERATION_SPEED * -1.0);
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_SPACE)) {
		bullets->push_back(player->shoot());
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_ESCAPE)) {
		game_loop = false;
		game_state = MENU;
	}
}

void calcPhysics() {
	player->move();
	for (size_t i = 0; i < bullets->size(); i++) {
		if (bullets->at(i)->isDead()) continue;
		bullets->at(i)->move();
		for (size_t j = 0; j < asteroids->size() ; j++) {
			if (asteroids->at(j)->distanceTo(bullets->at(i)->x, bullets->at(i)->y) < 15) {
				asteroids->at(j)->kill();
				explosions->push_back(new Explosion(asteroids->at(j)->x, asteroids->at(j)->y));
				bullets->at(i)->kill();
				destroyedAsteroids++;
			}
		}
	}
	for (size_t i = 0; i < asteroids->size(); i++) {
		if (asteroids->at(i)->isDead()) continue;
		asteroids->at(i)->move();
		if (asteroids->at(i)->distanceTo(player->x, player->y) < 15)
			player->kill();
		for (size_t j = 0; j < asteroids->size(); j++) {
			if (i == j) continue;
			if (asteroids->at(j)->isDead()) continue;
		}
	}
}

void render() {
	// black
	al_clear_to_color(al_map_rgb(0, 0, 0));
	// game sprites and all
	if (space_bg) {
		float x = 0;
		float y = 0;
		while (x < WIDTH) {
			while (y < HEIGHT) {
				al_draw_bitmap(space_bg, x, y, NULL);
				y += 128;
			}
			y = 0;
			x += 128;
		}
	}
	for (Bullet *b: *bullets)
		b->draw();
	for (Asteroid *a: *asteroids)
		a->draw();
	for (Explosion *e : *explosions)
		e->draw(); 
	player->draw();
	// lives
	for (int i = 0; i < player->getLives(); i++) {
		al_draw_bitmap(heartSprite, i * 33 + 10, 48, NULL);
	}
	// texts
	uiHue += (float)level;
	if (uiHue >= 360.0) uiHue = 0.0;
	uiColor = al_color_hsv(uiHue, 1.0, 1.0);
	sprintf_s(levelMsg, 100, "LEVEL %d", level);
	al_draw_text(uiFont, uiColor, 20, 20, NULL, levelMsg);
	sprintf_s(nextLevelMsg, 100, "to next level: %d", 
		nextLevel - destroyedAsteroids);
	al_draw_text(uiFont, uiColor, 360, 20, NULL, nextLevelMsg);
	// flip
	al_flip_display();
}

void keepFPS() {
	al_rest(1.0 / 65.0);
}

void burryDead() {
	bullets->erase(remove_if(bullets->begin(), bullets->end(), Bullet::toRemove), 
		bullets->end());
	asteroids->erase(remove_if(asteroids->begin(), asteroids->end(), Asteroid::toRemove), 
		asteroids->end());
	explosions->erase(remove_if(explosions->begin(), explosions->end(), Explosion::toRemove), 
		explosions->end());
}

void gameLogic() {
	if (destroyedAsteroids > nextLevel) {
		destroyedAsteroids = 0;
		nextLevel += level++;
	}
	while (asteroids->size() < nextLevel - destroyedAsteroids + 2) {
		asteroids->push_back(new Asteroid());
	}
	if (player->getLives() < 1) {

	}
}