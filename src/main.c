// Simple Pebble Bar Watchface
// 2016, Tm4n

#include <pebble.h>

#define cfg_nodots

static Window *s_window;
static Layer *s_canvas;
static TextLayer *s_text_hour;
static TextLayer *s_text_minute;
static GRect text_hour_frame = {.origin = {.x = 12, .y = 59}, .size = {.w = 60, .h = 30}};
static GRect text_minute_frame = {.origin = {.x = 74, .y = 59}, .size = {.w = 60, .h = 30}};

static int s_minute;
static int s_hour;

static int pos_hourbar_y = 22;
static int width_hourbar = 40;
static int pos_minutebar_y = 84;
static int width_minutebar = 40;
static int hourbar_length = 0;
static int minutebar_length = 0;
static int hourbar_max = 168-10;
static int minutebar_max = 168-10;

// Update hourbars
static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  
  //////////////////////////////////////
  // draw calls here
  graphics_context_set_fill_color(ctx, GColorOrange);
  graphics_context_set_stroke_color(ctx, GColorOrange);
  graphics_context_set_stroke_width(ctx, 20);
  
  #ifndef cfg_nodots
  // draw dots for hours
  for (int i = 0; i < 13; i++)
  {
    graphics_draw_pixel(ctx, GPoint(pos_hourbar_y-2, 5.+(double)i*((double)hourbar_max/12.)));
    graphics_draw_pixel(ctx, GPoint(pos_hourbar_y-3, 5.+(double)i*((double)hourbar_max/12.)));
    graphics_draw_pixel(ctx, GPoint(pos_hourbar_y-4, 5.+(double)i*((double)hourbar_max/12.)));
  }
  #endif
  
  // draw bar for hours

  graphics_fill_rect(ctx, GRect(pos_hourbar_y, bounds.size.h - hourbar_length - 5, width_hourbar, hourbar_length + 1), 0, 0);
  
  #ifndef cfg_nodots
  // draw limited amounts of dots for minutes
  for (int i = 0; i < 5; i++)
  {
    graphics_draw_pixel(ctx, GPoint(pos_minutebar_y-2, 5.+(double)i*((double)minbar_max/4.))); 
    graphics_draw_pixel(ctx, GPoint(pos_minutebar_y-3, 5.+(double)i*((double)minbar_max/4.)));
    graphics_draw_pixel(ctx, GPoint(pos_minutebar_y-4, 5.+(double)i*((double)minbar_max/4.)));
  }
  #endif
  
  // draw bar for minutes
  graphics_fill_rect(ctx, GRect(pos_minutebar_y, bounds.size.h - minutebar_length - 5, width_minutebar, minutebar_length + 1), 0, 0);
}


static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  /////////////////////////////////////////////////////// canvas
  // Create layer to draw, with update function
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, s_canvas);
  
  /////////////////////////////////////////////////////// text
  // Create the TextLayer with specific bounds
  s_text_hour = text_layer_create(text_hour_frame);
  s_text_minute = text_layer_create(text_minute_frame);

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_text_hour, GColorClear);
  text_layer_set_text_color(s_text_hour, GColorWhite);
  text_layer_set_text(s_text_hour, "");
  text_layer_set_font(s_text_hour, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_text_hour, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_text_minute, GColorClear);
  text_layer_set_text_color(s_text_minute, GColorWhite);
  text_layer_set_text(s_text_minute, "");
  text_layer_set_font(s_text_minute, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_text_minute, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_text_hour));
  layer_add_child(window_layer, text_layer_get_layer(s_text_minute));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer and window on unload
  layer_destroy(s_canvas);
  text_layer_destroy(s_text_hour);
  text_layer_destroy(s_text_minute);
  window_destroy(s_window);
}

// update texts and defer hourbar update
static void tick_handler(struct tm *time_now, TimeUnits units_changed) {
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_hour = time_now->tm_hour;
  s_hour -= (s_hour > 12) ? 12 : 0;
  s_minute = time_now->tm_min;
  hourbar_length = ((double)s_hour/12.) * (double)hourbar_max +1;
  minutebar_length = ((double)s_minute/60.) * (double)minutebar_max +1;
  
  // update text layers
  // Write the current hours and minutes into a buffer
  static char s_buffer_h[4];
  static char s_buffer_m[4];
  strftime(s_buffer_h, sizeof(s_buffer_h), clock_is_24h_style() ? "%H" : "%I", time_now);
  strftime(s_buffer_m, sizeof(s_buffer_m), "%M", time_now);

  // Display this time on the TextLayer
  text_layer_set_text(s_text_hour, s_buffer_h);
  text_layer_set_text(s_text_minute, s_buffer_m);
  
  // move textlayers, mark for redraw
  text_hour_frame.origin.y = bounds.size.h - hourbar_length - 45;
  text_hour_frame.origin.y = text_hour_frame.origin.y < 0 ? 0 : text_hour_frame.origin.y;
  Layer *hour_layer = text_layer_get_layer(s_text_hour);
  layer_set_frame(hour_layer, text_hour_frame);
  layer_mark_dirty(hour_layer);
  
  text_minute_frame.origin.y = bounds.size.h - minutebar_length - 45;
  text_minute_frame.origin.y = text_minute_frame.origin.y < 0 ? 0 : text_minute_frame.origin.y;
  Layer *minute_layer = text_layer_get_layer(s_text_minute);
  layer_set_frame(minute_layer, text_minute_frame);
  layer_mark_dirty(minute_layer);
  
  // Initiate redraw, so layer_update_proc gets called
  layer_mark_dirty(s_canvas); 
}


static void init() {
  // Create main Window element and assign to pointer
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}