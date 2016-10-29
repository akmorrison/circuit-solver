#ifndef GRAPHICAL_H_
#define GRAPHICAL_H_
#include<X11/Xlib.h>
#include<cairo/cairo.h>
#include<cairo/cairo-xlib.h>
#include<string>
#include<vector>
#include"circuit.h"
#include"button.h"

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

  void draw_text(int x, int y, double theta, std::string resvalue);
  void draw_squigley(int x1, int y1, int x2, int y2, int ascent, double resistance);
  void draw_resistor(Resistor* r);
  void draw_resistors_parallel(std::vector<Resistor*> parallels);
  void draw_node(Node* n);
  void draw_circuit(const Circuit* c);
  void draw_background();
};


#endif
