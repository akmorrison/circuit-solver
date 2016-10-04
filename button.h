#ifndef BUTTON_H_
#define BUTTON_H_
#include"graphical.h"
#include"draggable.h"
#include<cairo/cairo.h>

class Graphical;

class Button : public Draggable{
  public:
    int x,y;
    int width, height;
    void (Graphical::* func)();

    Button();
    Button(int xval, int yval, int w, int h, void (Graphical::* f)()) :x(xval),y(yval),width(w),height(h),func(f) {}  ;


    bool is_inside(int button_x, int button_y);
    void draw_button(cairo_t* ctx);

    void call_function(Graphical* g);
    void drag_to(int new_x, int new_y);
};


#endif
