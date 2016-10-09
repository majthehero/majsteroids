#include "main.h"

#define EXIT_GAME_OVER 1
#define EXIT_USER_ABORT 2

using namespace std;


// initialise data for one game
Game::Game() {
	game_loop = true;
	levelMsg = (char*)calloc(100, sizeof(char));
	nextLevelMsg = (char*)calloc(100, sizeof(char));
	uiHue = 0.0;
	destroyedAsteroids = 0;
	player = new Ship();
	level = 1;
	nextLevel = 10;
	srand(clock());
	asteroids = new vector<Asteroid*>();
	explosions = new vector<Explosion*>();
	bullets = new vector<Bullet*>();
}

// game loop
void Game::gameLoop() {
	while (game_loop) {
		handleInput();
		calcPhysics();
		render();
		burryDead();
		keepFPS();
		gameLogic();
	}

}

// get user input and react to it
void Game::handleInput() {
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
		endGame(EXIT_USER_ABORT);
	}
}

// game objects react to each other
void Game::calcPhysics() {
	player->move();
	for (size_t i = 0; i < bullets->size(); i++) {
		if (bullets->at(i)->isDead()) continue;
		bullets->at(i)->move();
		for (size_t j = 0; j < asteroids->size(); j++) {
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

// render everything
void Game::render() {
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
	for (Bullet *b : *bullets)
		b->draw();
	for (Asteroid *a : *asteroids)
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

// clean up memmory of used up game objects
void Game::burryDead() {
	bullets->erase(remove_if(bullets->begin(), bullets->end(), Bullet::toRemove),
		bullets->end());
	asteroids->erase(remove_if(asteroids->begin(), asteroids->end(), Asteroid::toRemove),
		asteroids->end());
	explosions->erase(remove_if(explosions->begin(), explosions->end(), Explosion::toRemove),
		explosions->end());
}

// keeps fps reasonable
// TODO currently retarded but kinda good enough
void Game::keepFPS() {
	al_rest(1.0 / 49.0);
}

// keeps track of levels, points etc
void Game::gameLogic() {
	if (destroyedAsteroids > nextLevel) {
		destroyedAsteroids = 0;
		nextLevel += level++;
	}
	while (asteroids->size() < nextLevel - destroyedAsteroids + 2) {
		asteroids->push_back(new Asteroid());
	}
	if (player->getLives() < 1) {
		endGame(EXIT_GAME_OVER);
	}
}

// ends game, returns to menu, cleans up game
void Game::endGame(int end_mode) {
	// TODONOW implement end game
	game_loop = false;
}
