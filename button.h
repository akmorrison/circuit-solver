#ifndef BUTTON_H_
#define BUTTON_H_
#include"controller.h"
#include"draggable.h"
#include<cairo/cairo.h>

class Controller;

class Button : public Draggable{
  public:
    int x,y;
    int width, height;
    void (Controller::* func)();
    std::string image_path;
    cairo_surface_t* disp_image;

    Button();
    ~Button();
    Button(int xval, int yval, int w, int h, void 
          (Controller::* f)(), std::string image) :
             x(xval),y(yval),width(w),height(h),func(f),image_path(image) {} ;


    bool is_inside(int button_x, int button_y);
    void draw_button(cairo_t* ctx);

    void call_function(Controller* c);
    void drag_to(int new_x, int new_y);
};


#endif
