#include "main.h"
using namespace std;

ALLEGRO_BITMAP *spaceshipSprite;
ALLEGRO_BITMAP *bulletSprite;
ALLEGRO_BITMAP *asteroidSprite;
ALLEGRO_BITMAP *explosionSprite;
ALLEGRO_BITMAP *space_bg;
ALLEGRO_BITMAP *heartSprite;

ALLEGRO_DISPLAY *display;
ALLEGRO_KEYBOARD_STATE *keyboard_state;
ALLEGRO_COLOR uiColor;
ALLEGRO_FONT *uiFont;
float uiHue;

int main() {
	initAllegro();
	// game- menu- loop
	while (true) {
		Menu *menu = new Menu();
		menu->menuLoop();
		Game *game = new Game();
		game->gameLoop();
	}
	al_destroy_display(display);
	exit(0);
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

	uiFont = al_load_ttf_font("font.ttf", 32, NULL);
	if (!uiFont) {
		cerr << "Failed to load font." << "\n";
		exit(5);
	}

	return 0;
}
