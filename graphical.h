#ifndef GRAPHICAL_H_
#define GRAPHICAL_H_
#include<X11/Xlib.h>
#include<cairo/cairo.h>
#include<cairo/cairo-xlib.h>
#include<string>
#include<vector>
#include"circuit.h"
#include"button.h"
#include"component.h"

class Circuit;
class Button;

class Graphical{
  public:

  Graphical(int width, int height);
  ~Graphical();

  int WIDTH,HEIGHT;

  Display* display;
  int screen;
  Window w;
  cairo_t *ctx;
  cairo_surface_t *surface;

  void draw_text(int x, int y, double theta, std::string resvalux);
  void draw_textbox(int x, int y, std::vector<char> str, char prefix, component_type type); //why is it a vector? 

  void draw_squigley(int x1, int y1, int x2, int y2, double resistance,double theta);
  void draw_plates(int x1, int y1, int x2, int y2, double capacitance,double theta);
  void draw_loopy(int x1, int y1, int x2, int y2, double inductance,double theta);
  void draw_components_parallel(std::vector<Component*> parallels);
  void draw_node(Node* n);
  void draw_circuit(const Circuit* c);
  void draw_background();
};


#endif
