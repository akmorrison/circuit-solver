#include"controller.h"
#include"circuit.h"
#include"graphical.h"
#include"button.h"

Controller::Controller(){
  c = new Circuit();
  g = new Graphical();

  continue_flag = true;

  //initialize buttons
  buttons.reserve(4);

  buttons.push_back(new Button(WIDTH-60,HEIGHT-60,40,40,
                               &Controller::quit();
                               cairo_image_surface_create_from_png(
                                 "images/quit_button.png")));
  buttons.push_back(new Button(WIDTH-110,HEIGHT-60,40,40,
                               &Controller::quit();
                               cairo_image_surface_create_from_png(
                                 "images/step_button.png")));
  buttons.push_back(new Button(WIDTH-160,HEIGHT-60,40,40,
                               &Controller::quit();
                               cairo_image_surface_create_from_png(
                                 "images/node_button.png")));
  buttons.push_back(new Button(WIDTH-210,HEIGHT-60,40,40,
                               &Controller::quit();
                               cairo_image_surface_create_from_png(
                                 "images/resi_button.png")));
}

Controller::~Controller(){
  for(Button* b : buttons)
    delete b;
  delete c;
  delete g;
}

void loop(){
  //setup event handling
  XEvent event;
  int x,y; //for mouse handling
  //main program loop. Draw circuit, wait for event,handle, repeat
  while(continue_flag){
    //draw things

    //event handling
    
  }
}

//button code
void Controller::button_quit(){
  continue_flag = false;
}

void Controller::button_step(){
  c->step();
}

void Controller::add_node(){
  //add a new node at 20,20.
  //why 20,20? you ask too many questions
  add_node();
  set_node_pos(c->nodes.size()-1,20,20);
}

void Controller::add_resistor(){
  //count selected nodes
  std::vector<Node*> selected_nodes;
  for(Node* n : c->nodes)
    if(n->selected)
      selected_nodes.push_back(n);

  //error handling
  if(selected_nodes.size() != 2)
    throw std::runtime_error("adding resistor requires 2 selected nodes");

  //add a new 100 ohm resistor
  //why 100 ohm? because brown-black-brown looks like a sideways hamburger
  //is it obvious that I'm writing this part at like 12:30am?
  add_resistor(c->index_of_node(selected_nodes[0]),
               c->index_of_node(selected_nodes[1]),
               100.0);
}

//required for creating the starting circuit
void Controller::add_node(){
  c->add_node();
}

void Controller::set_node_pos(int node_num, int x, int y){
  if(c->nodes.size() <= node_num || node_num < 0)
    throw std::runtime_error("node index out of range");
  c->nodes[node_num]->x = x;
  c->nodes[node_num]->y = y;
}

void Controller::add_resistor(int node_a, int node_b, int weight){
  if(c->nodes.size() <= node_a || node_a < 0)
    throw std::runtime_error("node_a index out of range");
  if(c->nodes.size() <= node_b || node_b < 0)
    throw std::runtime_error("node_b index out of range");
  if(node_a == node_b)
    throw std::runtime_error("node_a = node_b.... what do you want?");

  new Resistor(weight,c->nodes[node_a],c->nodes[node_b]);

}
