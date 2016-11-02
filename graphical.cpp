#include"graphical.h"
#include"circuit.h"
#include"button.h"
#include"draggable.h"
#include<string>
#include<X11/Xlib.h>
#include<cairo/cairo.h>
#include<cairo/cairo-xlib.h>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<cmath>

Graphical::Graphical(int width, int height){
  WIDTH = width;
  HEIGHT = height;

  display = XOpenDisplay(NULL);
  w = XCreateSimpleWindow(display,DefaultRootWindow(display),0,0,WIDTH,HEIGHT,0,0,0);
  screen = DefaultScreen(display);
  XSelectInput(display,w, StructureNotifyMask |
                          ButtonPress | 
                          ButtonReleaseMask |
                          Button1MotionMask |
                          KeyPressMask );
  XMapWindow(display,w);
  for(;;){
    XEvent e;
    XNextEvent(display,&e);
    if(e.type == MapNotify) break;
  }

  surface = cairo_xlib_surface_create(display,w,DefaultVisual(display,screen),WIDTH,HEIGHT);
  cairo_xlib_surface_set_size(surface,WIDTH,HEIGHT);
  ctx = cairo_create(surface);
}

Graphical::~Graphical(){
  cairo_destroy(ctx);
  cairo_surface_destroy(surface);
  XCloseDisplay(display);
}

void Graphical::draw_text(int x, int y, double theta, std::string resvalue){

  cairo_select_font_face(ctx,"sans-serif",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(ctx,18);

  cairo_save(ctx);
  cairo_move_to(ctx,x,y);
  cairo_rotate(ctx,theta);

  cairo_show_text(ctx,resvalue.c_str());
  cairo_stroke(ctx);
  cairo_restore(ctx);
}

void Graphical::draw_textbox(int x, int y, std::vector<char> str, char prefix){
  //first, find the width and height of the text string
  cairo_text_extents_t extents;
  std::string new_str(str.begin(),str.end());
  if(prefix == 'o')
    new_str += " ohms";
  else if(prefix == 'k')
    new_str += " Kohms";
  else if(prefix == 'm')
    new_str += " Mohms";

  int text_width, text_height;
  cairo_text_extents(ctx,new_str.c_str(),&extents);

  int text_vertical_pad = 5;
  int text_horizontal_pad = 5;

  text_width = extents.width + 2*text_horizontal_pad;
  text_height = extents.height + 2*text_vertical_pad;

  if(text_width < 100)
    text_width = 100;
  if(text_height < 20)
    text_height = 20;

  //white rectangle
  cairo_rectangle(ctx,x,y,text_width,text_height);
  //fill in in white
  cairo_set_source_rgba(ctx,1,1,1,.7);
  cairo_fill(ctx);


  //draw the border in black
  cairo_set_source_rgb(ctx,0,0,0);
  cairo_set_line_width(ctx,2);

  cairo_rectangle(ctx,x,y,text_width,text_height);
  cairo_stroke(ctx);

  draw_text(x+text_horizontal_pad,y+text_height-text_vertical_pad,
            0.0,new_str);
}

void Graphical::draw_squigley(int x1, int y1, int x2, int y2, int ascent, double resistance){
  //draws a 6 point squigley line because that's what resistors look like.
  //Each vertex is (ascent) distance from the line (x1,y1)->(x2,y2). 
  //This function also writes out the resistance above the squgley,
  //using the draw_text funciton

  //start at (x1,y1)
  cairo_move_to(ctx,x1,y1);

  int deltax = x2-x1;
  int deltay = y2-y1;
  
  //xcomp is the x-part of the ascent vector, which has magnitude
  //1 and the negative reciprocal slope of (x1,y1)->(x2,y2)
  double xcomp,ycomp;
  if(deltax == 0){
    xcomp = 1;
    ycomp = 0;
  }
  else{
    xcomp = sin(atan(((double)deltay)/deltax));
    ycomp = cos(atan(((double)deltay)/deltax));
  }

  //a 6 point squigley has 12 lines
  for(int i = 1; i < 12; i += 2){
    int j = (i-1)/2;
    int xval = x1+i*deltax/12 + pow(-1,j)*ascent*xcomp;
    int yval = y1+i*deltay/12 - pow(-1,j)*ascent*ycomp;;
    cairo_line_to(ctx,xval,yval);
  }
  cairo_line_to(ctx,x2,y2);

 
  std::stringstream ss;
  if(resistance > 9999999)
    ss << (int) (resistance / 1000000) << " Mohms";
  if(resistance > 9999)
    ss << (int) resistance / 1000 << " Kohms";
  else if((resistance-(int)resistance) < 0.01)
    ss << std::fixed << std::setprecision(0) << resistance << " ohms";
  else
    ss << std::fixed << std::setprecision(2) << resistance << " ohms";
 
  if(ycomp < 0){
    ycomp *= -1;
    xcomp *= -1;
  }  
  std::string resvalue = ss.str();
  int text_x = x1 + fabs(xcomp)*ascent;
  int text_y = y1 - fabs(ycomp)*ascent;

  draw_text(text_x,text_y,atan(xcomp/ycomp), resvalue);
}

void Graphical::draw_resistor(Resistor* r){
  int x1,y1,x2,y2;
  x1 = r->a->x;
  y1 = r->a->y;
  x2 = r->b->x;
  y2 = r->b->y;

  int distance = (int) sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

  if(distance > 100){
    double temp = 100.0/distance;
    int new_x1, new_x2, new_y1,new_y2,dx,dy;
    dx = x2-x1;
    dy = y2-y1;
    new_x1 = x1+(dx/2-temp*dx/2);
    new_x2 = x2-(dx/2-temp*dx/2);
    new_y1 = y1+(dy/2-temp*dy/2);
    new_y2 = y2-(dy/2-temp*dy/2);

    //draw the first line
    cairo_move_to(ctx,x1,y1);
    cairo_line_to(ctx,new_x1,new_y1);
    //draw the second line
    cairo_move_to(ctx,x2,y2);
    cairo_line_to(ctx,new_x2,new_y2);

    x1 = new_x1;
    x2 = new_x2;
    y1 = new_y1;
    y2 = new_y2;

  }

  int ascent = 20;
  draw_squigley(x1,y1,x2,y2,ascent,r->resistance);
  cairo_stroke(ctx);
}

void Graphical::draw_resistors_parallel(std::vector<Resistor*> parallels){
  Resistor* r = parallels[0]; //used for finding nodes
  int x1,y1,x2,y2;
  x1 = r->a->x;
  y1 = r->a->y;
  x2 = r->b->x;
  y2 = r->b->y;
  int distance = (int) sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

  //the total distance of the resistor should be less than
  // 100px. If it's more than that, draw some traces to make it that
  if(distance > 100){
    double temp = 100.0/distance;
    int new_x1, new_x2, new_y1,new_y2,dx,dy;
    dx = x2-x1;
    dy = y2-y1;

    //find two points on the line (x1,y1)->(x2,y2)
    //which are equidistant from the midpoint and
    //100px apart
    new_x1 = x1+(dx/2-temp*dx/2);
    new_x2 = x2-(dx/2-temp*dx/2);
    new_y1 = y1+(dy/2-temp*dy/2);
    new_y2 = y2-(dy/2-temp*dy/2);

    //draw the first parallel line
    cairo_move_to(ctx,x1,y1);
    cairo_line_to(ctx,new_x1,new_y1);
    //draw the second parallel line
    cairo_move_to(ctx,x2,y2);
    cairo_line_to(ctx,new_x2,new_y2);

    x1 = new_x1;
    x2 = new_x2;
    y1 = new_y1;
    y2 = new_y2;
  }

  int deltax = x2-x1;
  int deltay = y2-y1;

  int ascent = 20;
  double xcomp,ycomp;
  if(deltax == 0){
    xcomp = 1;
    ycomp = 0;
  }
  else{
    xcomp = sin(atan(((double)deltay)/deltax));
    ycomp = cos(atan(((double)deltay)/deltax));
  }

  //find the slope of the orthogonal
  double xpart, ypart; //x and y components of a unit vector orthogonal to slope
  //edge cases to avoid division by 0
  if(deltay == 0){
    xpart = 0;
    ypart = 1;
  }
  else if(deltax == 0){
    xpart = 1;
    ypart = 0;
  }
  //general case, not even remotely optimized
  else{
    double slope = ((double)deltay)/deltax;
    double orthog_slope = -1/slope;
    xpart = 1;
    ypart = orthog_slope*xpart;
    double reduction_factor = sqrt(xpart*xpart + ypart*ypart);
    xpart /= reduction_factor;
    ypart /= reduction_factor;
  }

  //draw the orthogonal bars
  int pad = 20; //pixels between two parallel resistors
  int orthog_length = (2*ascent+pad)*(parallels.size()-1);
  //starting positions for the bars
  int xstart1 = x1-xpart*orthog_length/2;
  int ystart1 = y1-ypart*orthog_length/2;
  int xstart2 = x2-xpart*orthog_length/2;
  int ystart2 = y2-ypart*orthog_length/2;
  //draw bar 1
  cairo_move_to(ctx,xstart1,ystart1);
  cairo_line_to(ctx,xstart1+xpart*orthog_length,ystart1+ypart*orthog_length);
  //draw bar 2
  cairo_move_to(ctx,xstart2,ystart2);
  cairo_line_to(ctx,xstart2+xpart*orthog_length,ystart2+ypart*orthog_length);




  //draw each parallel resistor
  for(int i = 0; i < parallels.size(); i++){
    int xt1, yt1, xt2, yt2;
    xt1 = xstart1+(2*ascent+pad)*xpart*i;
    yt1 = ystart1+(2*ascent+pad)*ypart*i;
    xt2 = xstart2+(2*ascent+pad)*xpart*i;
    yt2 = ystart2+(2*ascent+pad)*ypart*i;
    draw_squigley(xt1,yt1,xt2,yt2,ascent,parallels[i]->resistance);
  }

}

void Graphical::draw_node(Node* n){
  double radius = 10;
  cairo_arc(ctx,n->x,n->y,radius,0,2*M_PI);
  cairo_stroke(ctx);
}


void Graphical::draw_circuit(const Circuit* c){
  //draw each node
  for(Node* n : c->nodes){
    if(n->selected) //draw it in green
      cairo_set_source_rgb(ctx,0,1,0);
    else //draw it in blue
      cairo_set_source_rgb(ctx,0,0,1);
    //call the draw node
    draw_node(n);
  }

  //draw each resistor
  cairo_set_source_rgb(ctx,0,0,0); //resistors are back in black

  //be trying to fit line into half a terminal window on 13" like
  for(int i=0;i<c->nodes.size();i++)for(int j=i+1;j<c->nodes.size();j++){
  //for all combinations of nodes. This isn't efficient, but it works
    Node* n = c->nodes[i];
    Node* m = c->nodes[j]; //to save characters.
    //count all resistors between n and m
    int num_resistors = 0;
    std::vector<Resistor*> parallels;
    for(int k = 0; k < n->resistors.size(); k++){
      Resistor* r = n->resistors[k];
      if(  (r->a == n && r->b == m)
        || (r->b == n && r->a == m)){
        num_resistors++;
        parallels.push_back(r);
      }
    }
    if(num_resistors == 0)
      break; //draw nothing
    else if(num_resistors == 1)
      draw_resistor(parallels[0]); //draw a single resistor
    else
      draw_resistors_parallel(parallels);
  } //end foreach node pair
}

void Graphical::draw_background(){
  cairo_set_source_rgb(ctx,1,1,1);
  cairo_paint(ctx);
}
