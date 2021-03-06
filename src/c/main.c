// pGatchi Pebble Virtual Pet
// Author: Calcaware

#include <pebble.h>
#include "src/c/struct.h"

static Window *window;
static struct env_data environment;
static struct player_data player;

static void time_change_handler(struct tm *tick_time, TimeUnits units_changed) {
	strftime(environment.time, sizeof(environment.time), clock_is_24h_style() ? "%H:%M\n%m-%d" : "%I:%M\n%m-%d", tick_time);
}


static void environment_update() { // Show Random Rocks, Trees, Grass, Etc
	int index = 0;
	int x = rand() % 145;
	environment.decor_count = (rand() % environment.max_decor_count) + 1;
	while (index != environment.decor_count) {
		environment.decor[index].layer = bitmap_layer_create(GRect(x, 120, 16, 16));
		bitmap_layer_set_compositing_mode(environment.decor[index].layer, GCompOpSet);
		if (rand() % 2 == 0) {
			environment.decor[index].image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DECOR_ROCK_1);
		}
		else {
			environment.decor[index].image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DECOR_GRASS_1);
		}
		bitmap_layer_set_bitmap(environment.decor[index].layer, environment.decor[index].image);
		layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(environment.decor[index].layer));
		x = rand() % 145;
		++index;
	}
}


static void environment_scrub() {
	int index = 0;
	while (index != environment.decor_count) {
		layer_remove_from_parent(bitmap_layer_get_layer(environment.decor[index].layer));
		gbitmap_destroy(environment.decor[index].image);
		bitmap_layer_destroy(environment.decor[index].layer);
		++index;
	}
	environment.decor_count = 0;
}


static void player_update(uint32_t resource_id) { // Remove Player Image, Show New Player Image at Coordinates (Coordinates Set Elsewhere)
	layer_remove_from_parent(bitmap_layer_get_layer(player.layer));
	gbitmap_destroy(player.image);
	player.image = gbitmap_create_with_resource(resource_id);
	player.layer = bitmap_layer_create(GRect(player.coord.x, player.coord.y, player.size.w, player.size.h));
	bitmap_layer_set_compositing_mode(player.layer, GCompOpSet);
	bitmap_layer_set_bitmap(player.layer, player.image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(player.layer));
}


static void player_walk() {
	if (player.coord.x < environment.floor.size.w) { player.coord.x++; }
	else { player.coord.x = 0 - player.size.w; }
	if (player.coord.x == environment.floor.size.w) { environment_scrub(); environment_update(); }
	if (player.animation.sprite_iterator < 20) { player.animation.sprite_iterator++; } else { player.animation.sprite_iterator = 0; }
	if (player.animation.sprite_iterator < 5) { player_update(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND); }
	else if (player.animation.sprite_iterator < 10 && player.animation.sprite_iterator >= 6) { player_update(RESOURCE_ID_IMAGE_CHAR_MAIN_WALK_LEFT); }
	else if (player.animation.sprite_iterator < 15 && player.animation.sprite_iterator >= 11) { player_update(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND); }
	else if (player.animation.sprite_iterator < 20 && player.animation.sprite_iterator >= 16) { player_update(RESOURCE_ID_IMAGE_CHAR_MAIN_WALK_RIGHT); }
}


static void game_tick_callback(void *data) {
	if (player.animation.walking) { player_walk(); }
	environment.tick_timer = app_timer_register(environment.tick_speed, game_tick_callback, NULL);
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) { // Jump
	
}
static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	vibes_short_pulse();
	
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) { // Interact
	
}
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	vibes_short_pulse();
	
}
static void select_long_click_handler_release(ClickRecognizerRef recognizer, void *context) {
	
}


static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) { // Keep Moving Forward
	if (!player.animation.walking) { player.animation.walking = true; }
}
static void down_long_click_handler_release(ClickRecognizerRef recognizer, void *context) { // Stop Moving Forward
	player.animation.walking = false;
	player.animation.sprite_iterator = 0;
	player_update(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND);
}


static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
 window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
 //window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_long_click_subscribe(BUTTON_ID_UP, 500, up_long_click_handler, NULL);
 window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_click_handler, select_long_click_handler_release);
 window_long_click_subscribe(BUTTON_ID_DOWN, 1, down_long_click_handler, down_long_click_handler_release); // 0 Sets to 500ms Default
}


static void environment_initialize() {
	environment.tick_speed = 5;
	environment.max_decor_count = 5;
	environment.decor_count = 0; // Initialized to Ensure Scrubbing Works
	environment.background.coord.x = 0;
	environment.background.coord.y = 0;
	environment.background.size.w = 144;
	environment.background.size.h = 168;
	environment.background.layer = bitmap_layer_create(GRect(environment.background.coord.x, environment.background.coord.y, environment.background.size.w, environment.background.size.h));
	environment.background.image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_DAY);
	bitmap_layer_set_compositing_mode(environment.background.layer, GCompOpSet);
	bitmap_layer_set_bitmap(environment.background.layer, environment.background.image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(environment.background.layer));
	environment.floor.coord.x = 0;
	environment.floor.coord.y = 136;
	environment.floor.size.w = 144;
	environment.floor.size.h = 32;
	environment.floor.layer = bitmap_layer_create(GRect(environment.floor.coord.x, environment.floor.coord.y, environment.floor.size.w, environment.floor.size.h));
	environment.floor.image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLOOR_GRASS);
	bitmap_layer_set_compositing_mode(environment.floor.layer, GCompOpSet);
	bitmap_layer_set_bitmap(environment.floor.layer, environment.floor.image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(environment.floor.layer));
}


static void player_initialize() {
	player.coord.x = 0;
	player.coord.y = 104;
	player.size.w = 32;
	player.size.h = 32;
	player.layer = bitmap_layer_create(GRect(player.coord.x, player.coord.y, player.size.w, player.size.h));
	player.image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND);
	bitmap_layer_set_compositing_mode(player.layer, GCompOpSet);
	bitmap_layer_set_bitmap(player.layer, player.image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(player.layer));
	environment.tick_timer = app_timer_register(environment.tick_speed, game_tick_callback, NULL);
}


static void window_load(Window *window) {
	environment_initialize();
	player_initialize();
}


static void window_unload(Window *window) {
	environment_scrub();
	gbitmap_destroy(player.image);
	bitmap_layer_destroy(player.layer);
	gbitmap_destroy(environment.floor.image);
	bitmap_layer_destroy(environment.floor.layer);
	gbitmap_destroy(environment.background.image);
	bitmap_layer_destroy(environment.background.layer);
}


static void init(void) {
	srand((int32_t)time(NULL));
	tick_timer_service_subscribe(SECOND_UNIT, time_change_handler);
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) { .load = window_load, .unload = window_unload });
	window_stack_push(window, false);
}


static void deinit(void) {
	tick_timer_service_unsubscribe();
	window_destroy(window);
}


int main(void) {
	init();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
	app_event_loop();
	deinit();
}