// pGatchi Pebble Virtual Pet
// Author: Calcaware

#include <pebble.h>

static Window *window;
static GBitmap *background;
static BitmapLayer *background_layer;
static GBitmap *floor;
static BitmapLayer *floor_layer;
static GBitmap *character;
static BitmapLayer *character_layer;
static TextLayer *text_layer;
static AppTimer *walk_timer;
//static char text[65];
static char time_text[65];
bool character_walking = false;
int character_coord_x = 0;
int character_coord_y = 104;
int character_walk_ani = 0;
int magic_level = 100;


static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M\n%m-%d" : "%I:%M\n%m-%d", tick_time);
}


void use_magic() {
	if (magic_level < 10) { text_layer_set_text(text_layer,"Not enough magic!"); return; }
	bitmap_layer_destroy(character_layer);
	character_layer = bitmap_layer_create(GRect(character_coord_x, character_coord_y, 32, 32));
	bitmap_layer_set_compositing_mode(character_layer, GCompOpSet);
	character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_USE_MAGIC);
	bitmap_layer_set_bitmap(character_layer, character);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(character_layer));
	magic_level -= 10;
}


void reset_stance() {
	character_walk_ani = 0;
	bitmap_layer_destroy(character_layer);
	character_layer = bitmap_layer_create(GRect(character_coord_x, character_coord_y, 32, 32));
	bitmap_layer_set_compositing_mode(character_layer, GCompOpSet);
	character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND);
	bitmap_layer_set_bitmap(character_layer, character);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(character_layer));
}


void jump() {
	bitmap_layer_destroy(character_layer);
	character_layer = bitmap_layer_create(GRect(character_coord_x, character_coord_y, 32, 32));
	bitmap_layer_set_compositing_mode(character_layer, GCompOpSet);
	if (character_walk_ani == 0) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND); }
	else if (character_walk_ani == 5) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_WALK_LEFT); }
	else if (character_walk_ani == 10) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND); }
	else if (character_walk_ani == 15) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_WALK_RIGHT); }
	bitmap_layer_set_bitmap(character_layer, character);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(character_layer));
}


void walk_animation() {
	if (character_coord_x < 144) { character_coord_x += 2; } else { character_coord_x = -16; }
	if (character_walk_ani > 20) { character_walk_ani = 0; } else { character_walk_ani++; }
	bitmap_layer_destroy(character_layer);
	character_layer = bitmap_layer_create(GRect(character_coord_x, character_coord_y, 32, 32));
	bitmap_layer_set_compositing_mode(character_layer, GCompOpSet);
	if (character_walk_ani == 0) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND); }
	else if (character_walk_ani == 5) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_WALK_LEFT); }
	else if (character_walk_ani == 10) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND); }
	else if (character_walk_ani == 15) { character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_WALK_RIGHT); }
	bitmap_layer_set_bitmap(character_layer, character);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(character_layer));
}


static void walk_timer_callback(void *data) {
	if (character_walking) {
		walk_animation();
		walk_timer = app_timer_register(10, walk_timer_callback, NULL);
	}
}


void time_change_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	if (magic_level < 100) { magic_level++; }
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {}
static void handle_battery(BatteryChargeState charge_state) {}
static void handle_bluetooth(bool connected) {}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) { // Jump
	
}
static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	vibes_short_pulse();
	
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) { // Interact
	
}
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	vibes_short_pulse();
	use_magic();
}
static void select_long_click_handler_release(ClickRecognizerRef recognizer, void *context) {
	reset_stance();
}

//static void down_click_handler(ClickRecognizerRef recognizer, void *context) { } // Ignore for Walking
static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) { // Keep Moving Forward
	if (!character_walking) { walk_timer = app_timer_register(10, walk_timer_callback, NULL); }
	character_walking = true;
}
static void down_long_click_handler_release(ClickRecognizerRef recognizer, void *context) { // Stop Moving Forward
	character_walking = false;
	reset_stance();
	app_timer_cancel(walk_timer);
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
	//window_set_background_color(window, GColorVividCerulean);
	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_DAY);
	bitmap_layer_set_compositing_mode(background_layer, GCompOpSet);
	bitmap_layer_set_bitmap(background_layer, background);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(background_layer));
	
	floor_layer = bitmap_layer_create(GRect(0, 136, 144, 32));
	floor = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLOOR_GRASS);
	bitmap_layer_set_compositing_mode(floor_layer, GCompOpSet);
	bitmap_layer_set_bitmap(floor_layer, floor);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(floor_layer));
	
	character_layer = bitmap_layer_create(GRect(0, 104, 32, 32));
	character = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAR_MAIN_STAND);
	bitmap_layer_set_compositing_mode(character_layer, GCompOpSet);
	bitmap_layer_set_bitmap(character_layer, character);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(character_layer));
}


static void window_unload(Window *window) {
	gbitmap_destroy(background);
	bitmap_layer_destroy(background_layer);
	text_layer_destroy(text_layer);
}


static void init(void) {
	tick_timer_service_subscribe(SECOND_UNIT, time_change_handler);
	battery_state_service_subscribe(handle_battery);
	handle_battery(battery_state_service_peek());
	accel_tap_service_subscribe(accel_tap_handler);
	connection_service_subscribe((ConnectionHandlers) { .pebble_app_connection_handler = handle_bluetooth });
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(window, false);
}


static void deinit(void) {
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	connection_service_unsubscribe();
	accel_tap_service_unsubscribe();
	window_destroy(window);
}


int main(void) {
	init();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
	app_event_loop();
	deinit();
}