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

Graphical::Graphical(Circuit *c, int width, int height){
  circuit = c;
  WIDTH = width;
  HEIGHT = height;

  display = XOpenDisplay(NULL);
  w = XCreateSimpleWindow(display,DefaultRootWindow(display),0,0,WIDTH,HEIGHT,0,0,0);
  screen = DefaultScreen(display);
  XSelectInput(display,w, StructureNotifyMask |
                          ButtonPress | 
                          ButtonReleaseMask |
                          Button1MotionMask);
  XMapWindow(display,w);
  for(;;){
    XEvent e;
    XNextEvent(display,&e);
    if(e.type == MapNotify) break;
  }

  surface = cairo_xlib_surface_create(display,w,DefaultVisual(display,screen),WIDTH,HEIGHT);
  cairo_xlib_surface_set_size(surface,WIDTH,HEIGHT);
  ctx = cairo_create(surface);

  //button code
  continue_flag = true;
  buttons.push_back(new Button(WIDTH-60, HEIGHT-60,40,40,
    &Graphical::quit,
    cairo_image_surface_create_from_png("images/quit_button.png")));
  buttons.push_back(new Button(WIDTH-100,HEIGHT-60,40,40,
    &Graphical::step,
    cairo_image_surface_create_from_png("images/step_button.png")));
  buttons.push_back(new Button(WIDTH-140,HEIGHT-60,40,40,
    &Graphical::addNode,
    cairo_image_surface_create_from_png("images/node_button.png")));
  buttons.push_back(new Button(WIDTH-180,HEIGHT-60,40,40,
    &Graphical::addResistor,
    cairo_image_surface_create_from_png("images/node_button.png")));

  current_drag = NULL;
}

Graphical::~Graphical(){
  cairo_destroy(ctx);
  cairo_surface_destroy(surface);
  XCloseDisplay(display);

  for(int i = 0; i < buttons.size(); i++)
    delete buttons[i];
}

void Graphical::step(){
  circuit->step();
}

void Graphical::quit(){
  continue_flag = false;
}

void Graphical::addNode(){
  Node* new_node = new Node();
  new_node->drag_to(WIDTH/2,HEIGHT/2); //put the new node in the middle of the screen

  circuit->nodes.push_back(new Node()); // add to nodes
}

void Graphical::addResistor(){
  //count selected nodes
  int count = 0;
  std::vector<Node*> nodes;
  for(auto n : circuit->nodes)
    if(n->selected){
      count++;
      nodes.push_back(n);
    }
  if(count != 2)
    throw std::runtime_error("add resistor requires only 2 selected nodes");
  new Resistor(100,nodes[0],nodes[1]);
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
  ss << std::fixed << std::setprecision(2) << resistance << " ohms";
 
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

void Graphical::draw_circuit(){

//draw each node
  for(int i = 0; i < circuit->nodes.size(); i++){
    if(circuit->nodes[i]->selected)
      cairo_set_source_rgb(ctx,0,1,0);
    else
      cairo_set_source_rgb(ctx,0,0,1);
    draw_node(circuit->nodes[i]);
  }

  cairo_set_source_rgb(ctx,0,0,0);
  for(int i = 0; i < circuit->nodes.size(); i++){
    for(int j = i+1; j < circuit->nodes.size(); j++){
      Node* n = circuit->nodes[i];
      Node* m = circuit->nodes[j];
      //count all the resistors between n and m
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
        break;
      if(num_resistors == 1)
        draw_resistor(parallels[0]);
      else
        draw_resistors_parallel(parallels);
    }
  }

}

void Graphical::draw(){
  cairo_set_source_rgb(ctx,1,1,1);
  cairo_paint(ctx);
  cairo_set_source_rgb(ctx,0,0,0);

  draw_circuit();
  for(int i = 0; i < buttons.size(); i++)
    buttons[i]->draw_button(ctx);
  cairo_stroke(ctx);
  cairo_surface_flush(surface);
}

void Graphical::loop(){
//this is a great example of functionality that belongs in the controller
  XEvent event;
  int x,y;
  while(1){ //main loop
    draw();


    //handle next event
    XNextEvent(display,&event);
    switch(event.type){
      case ButtonPress:
      x = event.xbutton.x;
      y = event.xbutton.y;
      for(int i = 0; i < buttons.size(); i++) //iterate over each button
        if(buttons[i]->is_inside(x,y)){
          current_drag = buttons[i];
          buttons[i]->call_function(this);
          if(!continue_flag) return; //this is for the quit function
        }
      for(int i = 0; i < circuit->nodes.size(); i++){ //iterate over every node so we can drag
        Node* n = circuit->nodes[i];
        if((x-n->x)*(x-n->x)+(y-n->y)*(y-n->y) < 100){ //if we clicked the node
//TODO
          if(current_drag != n)
            //toggle selected on that node
            n->selected ^= 1;
          current_drag = n;
        } 
      }
      break;

      case ButtonRelease:
        current_drag = NULL;
        break;

      case MotionNotify:
        if(current_drag != NULL)
          current_drag->drag_to(event.xmotion.x,event.xmotion.y);
        break;
    }
  }
}
