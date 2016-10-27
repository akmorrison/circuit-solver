#include<iostream>
#include"circuit.h"
#include"graphical.h"
#include<cairo/cairo.h>
#include<X11/Xlib.h>
#include<cairo/cairo-xlib.h>
#include<unistd.h>


int main(){
  Circuit c(3);
  c.nodes[0]->x = 100;
  c.nodes[0]->y = 200;
  c.nodes[1]->x = 250;
  c.nodes[1]->y = 200;
  c.nodes[2]->x = 400;
  c.nodes[2]->y = 200;

  c.nodes[0]->selected = true;
  c.nodes[1]->selected = true;

  c.add_resistor(0,1,2.0);
  c.add_resistor(0,1,2.0);
  c.add_resistor(1,2,4.0);
  Graphical g(&c, 500,500);
  g.loop();
}
