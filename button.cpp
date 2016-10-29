#include "button.h"
#include "graphical.h"
#include "controller.h"
#include<iostream>
#include<cairo/cairo.h>

#define M_PI 3.141592

Button::Button(){
  x = y = 0;
  width = height = 50; //because why not
}

Button::~Button(){
  cairo_surface_destroy(disp_image);
}

bool Button::is_inside(int button_x, int button_y){
  return (button_x > x && button_x < x+width)
       &&(button_y > y && button_y < y+height);
}

void Button::draw_button(cairo_t* ctx){
  cairo_save(ctx);
  cairo_set_source_surface(ctx,disp_image,x,y);
  cairo_paint(ctx);
  cairo_restore(ctx);

  int radius = 10; //radius of the rounded corners

  cairo_new_sub_path(ctx);
  cairo_arc(ctx, x+width-radius, y+radius, radius, M_PI/-2,0); //tright
  cairo_arc(ctx, x+width-radius, y+height-radius, radius, 0,M_PI/2); //bright
  cairo_arc(ctx, x+radius, y+height-radius, radius, M_PI/2,M_PI); //bleft
  cairo_arc(ctx, x+radius, y+radius, radius, M_PI,M_PI/-2); //tleft
  cairo_close_path(ctx);

  cairo_set_source_rgb(ctx,0,0,0);
  cairo_stroke(ctx);
}


void Button::call_function(Controller* c){
  (c->*func)();
}

void Button::drag_to(int new_x, int new_y){
  x = new_x;
  y = new_y;
}
