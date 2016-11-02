#include<iostream>
#include"controller.h"
#include"circuit.h"
#include"graphical.h"
#include<cairo/cairo.h>
#include<X11/Xlib.h>
#include<cairo/cairo-xlib.h>
#include<unistd.h>


int main(){
  Controller c;

  c.add_node(); c.add_node();
  c.set_node_pos(0,100,200);
  c.set_node_pos(1,300,200);
  c.add_resistor(0,1,20);
  c.add_resistor(0,1,40);

  c.loop();
}
