#include"graphical.h"
#include"circuit.h"
#include"button.h"
#include"draggable.h"
#include"component.h"
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

void Graphical::draw_textbox(int x, int y, std::vector<char> str, char prefix, component_type type){
  //first, find the width and height of the text string
  cairo_text_extents_t extents;
  std::string new_str(str.begin(),str.end());

  //add the prefix and type to end of new_str
  new_str.push_back(' ');
  if(prefix != 'o' && prefix != 'h' && prefix != 'f'){
    new_str.push_back(prefix);
  }
  switch(type){
    case RESISTOR:
      new_str += "ohm";
      break;
    case CAPACITOR:
      new_str += "farad";
      break;
    case INDUCTOR:
      new_str += "henry";
  }

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


void Graphical::draw_squigley(int x1,int y1,int x2,int y2,int ascent,double resistance,double theta){
  int deltax = x2-x1;
  int deltay = y2-y1;

  int distance = sqrt(deltax*deltax + deltay*deltay);
  double distance_over_twelve = distance/12.0;

  cairo_save(ctx);
  cairo_move_to(ctx,x1,y1);
  cairo_rotate(ctx,theta);
  cairo_rel_line_to(ctx,distance_over_twelve,ascent);
  cairo_rel_line_to(ctx,2*distance_over_twelve,-2*ascent);
  cairo_rel_line_to(ctx,2*distance_over_twelve,2*ascent);
  cairo_rel_line_to(ctx,2*distance_over_twelve,-2*ascent);
  cairo_rel_line_to(ctx,2*distance_over_twelve,2*ascent);
  cairo_rel_line_to(ctx,2*distance_over_twelve,-2*ascent);
  cairo_rel_line_to(ctx,distance_over_twelve,ascent);
  cairo_stroke(ctx);


  cairo_restore(ctx);

  //print the resistance value just above the resistor
  std::stringstream ss;
  if(resistance < .000000001){
    resistance *= 1000000000000;
    ss << resistance << " pohms";
  }
  else if(resistance < .000001){
    resistance *= 1000000000;
    ss << resistance << " nohms";
  }
  else if(resistance < .001){
    resistance *= 1000000;
    ss << resistance << " uohms";
  }
  else if(resistance < 1){
    resistance *= 1000;
    ss << resistance << " mohms";
  }
  else if(resistance < 1000){
    ss << resistance << " ohms";
  }
  else if(resistance < 1000000){
    resistance /= 1000;
    ss << resistance << " kohms";
  }
  else if(resistance < 1000000000){
    resistance /= 1000000;
    ss << resistance << " Mohms";
  }

  draw_text(x1+20*sin(theta),y1+20*cos(theta+M_PI),theta,ss.str());
}

void Graphical::draw_plates(int x1,int y1,int x2,int y2,int ascent,double capacitance,double theta){
  //find theta
  int deltax = x2 - x1;
  int deltay = y2 - y1;

  cairo_save(ctx);

  //calculate distance
  int distance = sqrt(deltax*deltax + deltay*deltay);
  int separation = 20;

  cairo_move_to(ctx,x1,y1);
  cairo_rotate(ctx,theta);
  cairo_rel_line_to(ctx,(distance-separation)/2,0);
  cairo_rel_move_to(ctx, 0, ascent);
  cairo_rel_line_to(ctx, 0, ascent * -2);

  cairo_restore(ctx);

  cairo_save(ctx);
  cairo_move_to(ctx,x2,y2);
  cairo_rotate(ctx,theta);
  cairo_rel_line_to(ctx,-1*(distance-separation)/2,0);
  cairo_rel_move_to(ctx, 0, ascent);
  cairo_rel_line_to(ctx, 0, ascent * -2);
  cairo_stroke(ctx);
  cairo_restore(ctx);


  //print the capacitance value just above the capacitor
  std::stringstream ss;
  if(capacitance < .000000001){
    capacitance *= 1000000000000;
    ss << capacitance << " pfarads";
  }
  else if(capacitance < .000001){
    capacitance *= 1000000000;
    ss << capacitance << " nfarads";
  }
  else if(capacitance < .001){
    capacitance *= 1000000;
    ss << capacitance << " ufarads";
  }
  else if(capacitance < 1){
    capacitance *= 1000;
    ss << capacitance << " mfarads";
  }
  else if(capacitance < 1000){
    ss << capacitance << " farads";
  }
  else if(capacitance < 1000000){
    capacitance /= 1000;
    ss << capacitance << " kfarads";
  }
  else if(capacitance < 1000000000){
    capacitance /= 1000000;
    ss << capacitance << " Mfarads";
  }

  draw_text(x1+20*sin(theta),y1+20*cos(theta+M_PI),theta,ss.str());
}

void Graphical::draw_loopy(int x1,int y1,int x2,int y2,int ascent,double inductance,double theta){
  //find deltas and distance
  int deltax = x2-x1;
  int deltay = y2-y1;
  double distance = sqrt(deltax*deltax + deltay*deltay);

  double circle_radius = distance / 8;

  cairo_save(ctx);

  cairo_translate(ctx,x1,y1);
  cairo_rotate(ctx,theta);

  cairo_arc(ctx,circle_radius,0,circle_radius,0,M_PI);
  cairo_stroke(ctx);
  cairo_arc(ctx,3*circle_radius,0,circle_radius,0,M_PI);
  cairo_stroke(ctx);
  cairo_arc(ctx,5*circle_radius,0,circle_radius,0,M_PI);
  cairo_stroke(ctx);
  cairo_arc(ctx,7*circle_radius,0,circle_radius,0,M_PI);
  cairo_stroke(ctx);

  cairo_restore(ctx);

  //print the inductance value just above the inductor
  std::stringstream ss;
  if(inductance < .000000001){
    inductance *= 1000000000000;
    ss << inductance << " phenrys";
  }
  else if(inductance < .000001){
    inductance *= 1000000000;
    ss << inductance << " nhenrys";
  }
  else if(inductance < .001){
    inductance *= 1000000;
    ss << inductance << " uhenrys";
  }
  else if(inductance < 1){
    inductance *= 1000;
    ss << inductance << " mhenrys";
  }
  else if(inductance < 1000){
    ss << inductance << " henrys";
  }
  else if(inductance < 1000000){
    inductance /= 1000;
    ss << inductance << " khenrys";
  }
  else if(inductance < 1000000000){
    inductance /= 1000000;
    ss << inductance << " Mhenrys";
  }

  draw_text(x1+20*sin(theta),y1+20*cos(theta+M_PI),theta,ss.str());
}

void Graphical::draw_components_parallel(std::vector<Component*> parallels){
  Component* c = parallels[0]; //used for finding nodes

  int x1,y1,x2,y2;
  x1 = c->a->x;
  y1 = c->a->y;
  x2 = c->b->x;
  y2 = c->b->y;
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

  double theta;
  if(deltax == 0)
    theta = M_PI / 2.0;
  else{
    theta = atan(((double)deltay)/deltax);
    if(deltax < 0)
      theta += M_PI;
  }

  int ascent = 20;
  double xcomp,ycomp;
  if(deltax == 0){
    xcomp = 1;
    ycomp = 0;
  }
  else{
    xcomp = sin(theta);
    ycomp = cos(theta);
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
    if(parallels[i]->type == RESISTOR)
      draw_squigley(xt1,yt1,xt2,yt2,ascent,parallels[i]->weight,theta);
    else if(parallels[i]->type == CAPACITOR)
      draw_plates(xt1,yt1,xt2,yt2,ascent, parallels[i]->weight,theta);
    else if(parallels[i]->type == INDUCTOR)
      draw_loopy(xt1,yt1,xt2,yt2,ascent, parallels[i]->weight,theta);
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
    int num_components = 0;
    std::vector<Component*> parallels;
    for(int k = 0; k < n->components.size(); k++){
      Component* r = n->components[k];
      if(  (r->a == n && r->b == m)
        || (r->b == n && r->a == m)){
        num_components++;
        parallels.push_back(r);
      }
    }
    if(num_components == 0)
      break; //draw nothing
    else
      draw_components_parallel(parallels);
  } //end foreach node pair
}

void Graphical::draw_background(){
  cairo_set_source_rgb(ctx,1,1,1);
  cairo_paint(ctx);
}
