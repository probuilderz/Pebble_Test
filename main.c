/*
 * main.c
 * Sets up a Window and a TextLayer, then subscribes to future
 * Bluetooth state updates.
 */

#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_output_layer;

//static Window *s_main_window;
static TextLayer *s_uptime_layer;

static int s_uptime = 0;
//static int s_time = 3;
void vibes_double_pulse(void);

// Bluetooth handler

static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    text_layer_set_text(s_output_layer, "Watch is Connected");
  } else {
    text_layer_set_text(s_output_layer, "Watch is Disconnected");
    vibes_double_pulse();
  }
   
}//end bt_handler

//Time count handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Use a long-lived buffer
  static char s_uptime_buffer[32];

  // Get time since launch
  int seconds = s_uptime % 60;
  int minutes = (s_uptime % 3600) / 60;
  int hours = s_uptime / 3600;

  // Update the TextLayer
  snprintf(s_uptime_buffer, sizeof(s_uptime_buffer), "Uptime: %dh %dm %ds", hours, minutes, seconds);
  text_layer_set_text(s_uptime_layer, s_uptime_buffer);

  // Increment s_uptime
  s_uptime++;
  //Reminding to register
  if(s_uptime ==10){
    vibes_double_pulse();
    text_layer_set_text(s_output_layer, "Please register.");
  }
    else if(s_uptime ==20)
      vibes_double_pulse();
      else if(s_uptime ==30)
        vibes_double_pulse();
  

}//end tick handler

//Create a tap handler
static void tap_handler(AccelAxisType axis, int32_t direction) {

}


// create an handler to notify user for registration--not done yet
void reg_reminder(void){
  // Vibe pattern: ON for 200ms, OFF for 100ms, ON for 400ms:
  uint32_t const segments[] = { 200, 100, 400 };
VibePattern pat ={
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
};
vibes_enqueue_custom_pattern(pat);

}

//main window
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
 // GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer
//   s_output_layer = text_layer_create(GRect(1, 1, window_bounds.size.w, window_bounds.size.h));
//   s_uptime_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  s_output_layer = text_layer_create(GRect(0, 43, 100,30));
  s_uptime_layer = text_layer_create(GRect(0, 0, 110,35));
  text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_uptime_layer, GTextAlignmentCenter);
  text_layer_set_text(s_uptime_layer, "Uptime: 0h 0m 0s");
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_uptime_layer));

  // Show current connection state
  bt_handler(bluetooth_connection_service_peek());
}//End main window load

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
  // Destroy output TextLayer
  text_layer_destroy(s_uptime_layer);

}

static void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  // Subscribe to Bluetooth updates
  bluetooth_connection_service_subscribe(bt_handler);
  // Subscribe to TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  //register the tap handler
  accel_tap_service_subscribe(tap_handler);


}

static void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
