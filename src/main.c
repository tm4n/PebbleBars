// Simple Pebble Bar Watchface
// 2016, Tm4n

#include <pebble.h>

static Window *s_window;
static Layer *s_canvas;

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

  // Create layer to draw, with update function
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, s_canvas);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer and window on unload
  layer_destroy(s_canvas);
  window_destroy(s_window);
}

static void tick_handler(struct tm *time_now, TimeUnits units_changed) {
  s_hour = time_now->tm_hour;
  s_minute = time_now->tm_min;
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