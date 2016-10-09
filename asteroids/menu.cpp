#include "main.h"

using namespace std;

Menu::Menu() {
	menu_loop = true;
	items = (char**)calloc(2, sizeof(char*));
	char *newGameItem = "New Game";
	char *exitItem = "Exit Game";
	items[0] = newGameItem;
	items[1] = exitItem;
}

void Menu::menuLoop() {
	while (menu_loop) {
		handleInput();
		render();
		keepFPS();
	}
}

void Menu::handleInput() {
	al_get_keyboard_state(keyboard_state);
	if (!keyboard_state) {
		cerr << "Failed to get keyboard state.\n";
		exit(2);
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_W))
	{
		selectedItem = 0;
	}
	if (al_key_down(keyboard_state, ALLEGRO_KEY_S))
	{
		selectedItem = 1;
	}
	else if (al_key_down(keyboard_state, ALLEGRO_KEY_SPACE) ||
		al_key_down(keyboard_state, ALLEGRO_KEY_ENTER))  {
		cout << "Execute " << selectedItem << "\n";
		execute();
	}
}

void Menu::render() {
	al_clear_to_color(al_map_rgb(0, 0, 0));
	float x, y;
	uiHue += 0.8;
	if (uiHue >= 360) uiHue = 0.0;
	uiColor = al_color_hsv(uiHue, 1.0, 1.0);
	x = ceil(WIDTH / 3);
	y = ceil(HEIGHT / 3);
	al_draw_text(uiFont, uiColor, x, y, NULL, items[0]);
	if (selectedItem == 0) {
		al_draw_rotated_bitmap(spaceshipSprite, 
			16, 16, x - 32, y, PI / 2, NULL);
	}
	y = ceil(HEIGHT * 2 / 3);
	if (selectedItem == 1) {
		al_draw_rotated_bitmap(spaceshipSprite,
			16, 16, x - 32, y, PI / 2, NULL);
	}
	al_draw_text(uiFont, uiColor, x, y, NULL, items[1]);
	al_flip_display();
}

void Menu::execute() {
	if (selectedItem == 0) {
		startGameBeautifully();
	}
	else if (selectedItem == 1) {
		exitBeautifully();
	}
}

void Menu::exitBeautifully() {
	float x, y;
	int framesToExit = 60;
	x = ceil(WIDTH / 3);
	float shipx = ceil(WIDTH / 3);
	y = ceil(HEIGHT * 2 / 3);
	int frame = 0;
	float a = 1.0;
	while (frame++ <= framesToExit) {
		al_clear_to_color(al_map_rgb(0, 0, 0));
		uiColor = al_color_hsv(uiHue, 1.0, a);
		a -= 1.0 / 60.0;
		al_draw_text(uiFont, uiColor, x, y, NULL, items[1]);
		shipx += frame / 2.0;
		al_draw_rotated_bitmap(spaceshipSprite,
			16, 16, shipx - 32, y, PI / 2, NULL);
		al_flip_display();
		keepFPS();
	}
	exit(0);
}

void Menu::startGameBeautifully() {
	float x, y;
	int framesToExit = 60;
	x = ceil(WIDTH / 3);
	float shipx = ceil(WIDTH / 3);
	y = ceil(HEIGHT / 3);
	int frame = 0;
	float a = 1.0;
	while (frame++ <= framesToExit) {
		al_clear_to_color(al_map_rgb(0, 0, 0));
		uiColor = al_color_hsv(uiHue, 1.0, a);
		a -= 1.0 / 60.0;
		al_draw_text(uiFont, uiColor, x, y, NULL, items[1]);
		shipx += frame / 2.0;
		al_draw_rotated_bitmap(spaceshipSprite,
			16, 16, shipx - 32, y, PI / 2, NULL);
		al_flip_display();
		keepFPS();
	}
	menu_loop = false;
}

// keeps fps reasonable
// TODO currently retarded but kinda good enough
void Menu::keepFPS() {
	al_rest(1.0 / 35);
}
