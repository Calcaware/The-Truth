// Environment and Player Structures

#include <pebble.h>

struct coordinate_data {
	int x;
	int y;
};


struct size_data {
	int w;
	int h;
};


struct image_data {
	GBitmap *image;
	BitmapLayer *layer;
	struct coordinate_data coord;
	struct size_data size;
};


struct env_data {
	struct image_data background;
	struct image_data floor;
	AppTimer *tick_timer;
	int tick_speed;
	char time[32];
};


struct player_data {
	GBitmap *image;
	BitmapLayer *layer;
	bool walking;
	struct coordinate_data coord;
	struct size_data size;
	int health;
	int food;
	int level;
};
