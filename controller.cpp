#include<iostream>
#include"controller.h"
#include"circuit.h"
#include"graphical.h"
#include"button.h"
#include"draggable.h"

Controller::Controller(){
  const int WIDTH = 400;
  const int HEIGHT = 400;  //should move these to a constants header
  c = new Circuit();
  g = new Graphical(WIDTH,HEIGHT);
  current_drag = NULL;

  continue_flag = true;

  //initialize buttons
  buttons.reserve(4);

  buttons.push_back(new Button(WIDTH-60,HEIGHT-60,40,40,
                               &Controller::button_quit,
                               cairo_image_surface_create_from_png(
                                 "images/quit_button.png")));
  buttons.push_back(new Button(WIDTH-110,HEIGHT-60,40,40,
                               &Controller::button_step,
                               cairo_image_surface_create_from_png(
                                 "images/step_button.png")));
  buttons.push_back(new Button(WIDTH-160,HEIGHT-60,40,40,
                               &Controller::button_add_node,
                               cairo_image_surface_create_from_png(
                                 "images/node_button.png")));
  buttons.push_back(new Button(WIDTH-210,HEIGHT-60,40,40,
                               &Controller::button_add_resistor,
                               cairo_image_surface_create_from_png(
                                 "images/resi_button.png")));
}

Controller::~Controller(){
  for(Button* b : buttons)
    delete b;
  delete c;
  delete g;
}

bool Controller::checkall_buttons(int x, int y){
  for(Button* b : buttons){
    if(b->is_inside(x,y)){
      current_drag = b;
      b->call_function(this);
      return true;
    }
  }
  return false;
}

bool Controller::checkall_nodes(int x, int y){ //handle mouse clicks
  for(Node* n : c->nodes){
    //calculate distance from click to node via pythagoras
    double squared_distance = (x-n->x)*(x-n->x) + (y-n->y)*(y-n->y);
    if(squared_distance < 100){ //we clicked within 10 pixels of the node
      n->selected ^= 1; //toggle n's selected-ness
      current_drag = n; //we're now dragging n
      return true;
    }
  }
  return false;
}

void Controller::loop(){
  //setup event handling
  XEvent event;
  int x,y; //for mouse handling
  //main program loop. Draw circuit, wait for event,handle, repeat
  while(continue_flag){
  //draw things
    //draw background as like... white
    g->draw_background();
    //draw circuit
    g->draw_circuit(c);
    //draw buttons
    for(Button* b : buttons)
      b->draw_button(g->ctx);

  //event handling
    //get Xevent
    XNextEvent(g->display,&event); //this waits until next event
    //case by case possibilities
    switch(event.type){
      case ButtonPress:
        //check for each button. if it was pressed, call its function
        x = event.xbutton.x;
        y = event.xbutton.y;
        if(checkall_buttons(x,y))
          break;
      //else check for each node. If yes, select it, and make it current drag
        if(checkall_nodes(x,y))
          break;

        //end of case ButtonPress
        break;
      case ButtonRelease:
        //nothing is being dragged. Release dragged
        current_drag = NULL;
        break; 
      case MotionNotify:
        x = event.xbutton.x;
        y = event.xbutton.y;
        //if something is being dragged, move it to mouse xy
        if(current_drag != NULL)
          current_drag->drag_to(x,y); 
    }
  }
}

//button code
//by setting this flag to false, loop no longer loops
void Controller::button_quit(){
  continue_flag = false;
}


void Controller::button_step(){
  //yay, error handling!
  try{
    c->step();
  }
  catch(const std::exception& e){
    std::cerr << "could not minimize, step failed with message: "
              << e.what() << std::endl;
  }
}

void Controller::button_add_node(){
  //add a new node at 20,20.
  //why 20,20? you ask too many questions
  add_node();
  set_node_pos(c->nodes.size()-1,20,20);
}

void Controller::button_add_resistor(){
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
