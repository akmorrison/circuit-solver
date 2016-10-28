#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include"circuit.h"
#include"graphical.h"
#include"button.h"


class Controller{
  Circuit *c;
  Graphical *g;
  std::vector<Button*> buttons;

  Controller();
  ~Controller();

  bool continue_flag; //if this is false, loop() doesn't run
  void loop();

  void button_quit();
  void button_step();
  void button_add_node();
  void button_add_resistor();

  void add_node();
  void set_node_pos(int node_num, int x, int y);
  void add_resistor(int node_a, int node_b, int weight);
};


#endif
