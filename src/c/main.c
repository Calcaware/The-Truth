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


static void player_update(uint32_t resource_id) {
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
	player_update(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND);
}


static void game_tick_callback(void *data) {
	if (player.walking) { player_walk(); }
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
	if (!player.walking) { player.walking = true; }
}
static void down_long_click_handler_release(ClickRecognizerRef recognizer, void *context) { // Stop Moving Forward
	player.walking = false;
}


static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
 window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
 //window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_long_click_subscribe(BUTTON_ID_UP, 500, up_long_click_handler, NULL);
 window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_click_handler, select_long_click_handler_release);
 window_long_click_subscribe(BUTTON_ID_DOWN, 1, down_long_click_handler, down_long_click_handler_release); // 0 Sets to 500ms Default
}


static void window_load(Window *window) {
	environment.tick_speed = 5;
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


static void window_unload(Window *window) {
	gbitmap_destroy(player.image);
	bitmap_layer_destroy(player.layer);
	gbitmap_destroy(environment.floor.image);
	bitmap_layer_destroy(environment.floor.layer);
	gbitmap_destroy(environment.background.image);
	bitmap_layer_destroy(environment.background.layer);
}


static void init(void) {
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