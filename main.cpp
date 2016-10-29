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

  c.loop();
}
