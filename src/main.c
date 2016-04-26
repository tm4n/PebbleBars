// Simple Pebble Bar Watchface
// 2016, Tm4n

#include <pebble.h>

static Window *s_window;
static Layer *s_canvas;
static TextLayer *s_text_hour;
static TextLayer *s_text_minute;

static int s_minute;
static int s_hour;

static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  int hourbar_max = (double)(bounds.size.h-10);
  int minbar_max = (double)(bounds.size.h-10);
  //////////////////////////////////////
  // draw calls here
  graphics_context_set_fill_color(ctx, GColorOrange);
  graphics_context_set_stroke_color(ctx, GColorOrange);
  graphics_context_set_stroke_width(ctx, 20);
  
  // 12 hours only, with a minimum size
  s_hour -= (s_hour > 12) ? 12 : 0;
  
  // draw dots for hours
  for (int i = 0; i < 13; i++)
  {
    graphics_draw_pixel(ctx, GPoint(18, 5.+(double)i*((double)hourbar_max/12.)));
    graphics_draw_pixel(ctx, GPoint(17, 5.+(double)i*((double)hourbar_max/12.)));
    graphics_draw_pixel(ctx, GPoint(16, 5.+(double)i*((double)hourbar_max/12.)));
  }
  
  // draw bar for hours
  int hourbar_length = ((double)s_hour/12.) * (double)hourbar_max +1;
  graphics_fill_rect(ctx, GRect(20, bounds.size.h - hourbar_length - 5, 40, hourbar_length + 1), 0, 0);
  
  
  // draw limited amounts of dots for minutes
  for (int i = 0; i < 5; i++)
  {
    graphics_draw_pixel(ctx, GPoint(92, 5.+(double)i*((double)minbar_max/4.))); 
    graphics_draw_pixel(ctx, GPoint(91, 5.+(double)i*((double)minbar_max/4.)));
    graphics_draw_pixel(ctx, GPoint(90, 5.+(double)i*((double)minbar_max/4.)));
  }
  
  // draw bar for minutes
  int minbar_length = ((double)s_minute/60.) * (double)minbar_max +1;
  graphics_fill_rect(ctx, GRect(94, bounds.size.h - minbar_length - 5, 40, minbar_length + 1), 0, 0);
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
  s_text_hour = text_layer_create(GRect(10, 59, 60, 50));
  s_text_minute = text_layer_create(GRect(84, 59, 60, 50));

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

static void tick_handler(struct tm *time_now, TimeUnits units_changed) {
  s_hour = time_now->tm_hour;
  s_minute = time_now->tm_min;
  
  // update text layers
  // Write the current hours and minutes into a buffer
  static char s_buffer_h[4];
  static char s_buffer_m[4];
  strftime(s_buffer_h, sizeof(s_buffer_h), clock_is_24h_style() ? "%H" : "%I", time_now);
  strftime(s_buffer_m, sizeof(s_buffer_m), "%M", time_now);

  // Display this time on the TextLayer
  text_layer_set_text(s_text_hour, s_buffer_h);
  text_layer_set_text(s_text_minute, s_buffer_m);
  
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