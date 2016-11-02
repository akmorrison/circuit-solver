#include<iostream>
#include"controller.h"
#include"circuit.h"
#include"graphical.h"
#include"button.h"
#include"draggable.h"
#include<cmath>

Controller::Controller(){
  const int WIDTH = 400;
  const int HEIGHT = 400;  //should move these to a constants header
  c = new Circuit();
  g = new Graphical(WIDTH,HEIGHT);
  current_drag = NULL;

  focus = no_focus;
  continue_flag = true;
  editing_resistance = NULL;
  textbox_x = textbox_y = 0;
  prefix = 'o';

  //initialize buttons
  buttons.reserve(4);

  buttons.push_back(new Button(WIDTH-60,HEIGHT-60,40,40,
                               &Controller::button_quit,
                                 "images/quit_button.png"));
  buttons.push_back(new Button(WIDTH-110,HEIGHT-60,40,40,
                               &Controller::button_step,
                                 "images/step_button.png"));
  buttons.push_back(new Button(WIDTH-160,HEIGHT-60,40,40,
                               &Controller::button_add_node,
                                 "images/node_button.png"));
  buttons.push_back(new Button(WIDTH-210,HEIGHT-60,40,40,
                               &Controller::button_add_resistor,
                                 "images/resi_button.png"));
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
      focus = dragging;
      return true;
    }
  }
  return false;
}

Resistor* Controller::checkall_resistors(int mouse_x, int mouse_y){
/*
this logic may get somewhat convoluted. So here's my plan
for each pair of nodes. Count the resistors between them.
if there is only one resistor between them, see if x,y
is within 20 pixels of the midpoint between them.


to test for which of parallel resistors is clicked,
get a competent programmer to do this shit
*/
  for(int i=0;i<c->nodes.size();i++)for(int j=i+1;j<c->nodes.size();j++){
  //when you're trying to fit two for statements in half a terminal
  //window, you do some crazy things
    Node* n = c->nodes[i];
    Node* m = c->nodes[j];

    //count resistors between n and m
    std::vector<Resistor*> parallels;
    for(Resistor *r : n->resistors)
      if(r->a == m || r->b == m)
        parallels.push_back(r);

    //if there's only the one resistor between them
    //calculate distance between midpoint of nm and (x,y)
    if(parallels.size() == 1){
      int mid_x = (n->x + m->x)/2;
      int mid_y = (n->y + m->y)/2;
      double distance = (mouse_x-mid_x)*(mouse_x-mid_x) + 
                        (mouse_y-mid_y)*(mouse_y-mid_y);
      if(distance < 20*20)
        return parallels[0];
    }
    else if(parallels.size() > 1){
      int mid_x = (n->x+m->x)/2;
      int mid_y = (n->y+m->y)/2;
      //vector pointing to the first resistor from midpoint is -1/(y-y)/(x-x)
      double ypart,xpart;
      if(m->y == n->y){ //nodes are stacked horizontally
        xpart = 0.0;
        ypart = 1.0;
      }
      else if(m->x == n->x){ //nodes are stacked vertically
        xpart = 1.0;
        ypart = 0.0;
      }
      else{
        xpart =  1.0;
        ypart = (n->x-m->x)/(m->y-n->y);
        //create unit vector of [xpart,ypart]. Yes i know there's a better way
        double magnitude = sqrt(xpart*xpart+ypart*ypart);
        xpart /= magnitude;
        ypart /= magnitude;
      }
      int distance_between_resistors = 60;
      int x1 = mid_x - xpart*(parallels.size()-1)/2.0*distance_between_resistors;
      int y1 = mid_y - ypart*(parallels.size()-1)/2.0*distance_between_resistors;
      for(int i = 0; i < parallels.size(); i++){
        //x,y of parallels[i] is x1,y1
        double square_d = (mouse_x-x1)*(mouse_x-x1) +
                          (mouse_y-y1)*(mouse_y-y1);
        if(square_d < 900)
          return parallels[i];
        x1 += xpart*distance_between_resistors;
        y1 += ypart*distance_between_resistors;
      }
        
    }
  }
  return NULL;
}

char Controller::keycode_to_char(unsigned int keycode){
  //jesus christ xlib handles this in a complicated way.
  //this is highly machine dependant, so this is pretty much hardcoded
  //if you're looking to make this program portable, this needs fixing

  //my machine maps [26-29] to ['1'-'4']
  //30 is '6', 31 is '5'
  //it maps 33 to '9', 34 to '7'
  //36 to '8', 37 to '0'

  switch(keycode){ //there is probably a smart way to do this
    case 37:       //what with it being 12:30am, and I have
      return '0';  //absolutely no idea what I'm doing, I'm
    case 26:       //gonna hardcode a switch statement for all
      return '1';  //the keyvalues I care about
    case 27:
      return '2';
    case 28:
      return '3';
    case 29:
      return '4';
    case 31:
      return '5';
    case 30:
      return '6';
    case 34:
      return '7';
    case 36:
      return '8';
    case 33:
      return '9';
    case 55:
      return '.';
    default:
      return 'Q'; //no reason this is the default 
  }
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
    //if we're in text entry mode, draw a textbox
    if(focus == enter_text)
      g->draw_textbox(textbox_x, textbox_y, current_string, prefix);
    //draw the buttons
    buttons[0]->draw_button(g->ctx);
    buttons[1]->draw_button(g->ctx);
    buttons[2]->draw_button(g->ctx);
    buttons[3]->draw_button(g->ctx);
    buttons[3]->draw_button(g->ctx);

//    for(int i = buttons.size(); i > 0;)
  //   buttons[--i]->draw_button(g->ctx);

  //event handling
    //get Xevent
    XNextEvent(g->display,&event); //this waits until next event

    //case by case possibilities
    switch(event.type){
      case ButtonPress:
      //if the user clicks a button or a node, enter dragging focus
      //if the user clicks a resistor, enter enter_text focus
      //if the user clicks empty space, enter no_focus focus

        //check if user clicked button. if it was pressed, call its function
        x = event.xbutton.x;
        y = event.xbutton.y;
        if(checkall_buttons(x,y))
          break;
      //else check for node. If yes, select it, and make it current drag
        if(checkall_nodes(x,y)){
          
          break;
        }
      //else check for resistor. If yes, start enter_text focus
        Resistor* clicked_resistor;
        if( (clicked_resistor = checkall_resistors(x,y)) != NULL){
          focus = enter_text;
          editing_resistance = clicked_resistor;
          textbox_x = x;
          textbox_y = y;
          //clicked_resistor->get_textbox_xy(textbox_x,textbox_y);
          break;
        }
        //at this point, we assume the user clicked whitespace
          //deselect all nodes
          for(Node* n : c->nodes)
            n->selected = false;
          //if we were in enter_text mode, update resistor value
          if(focus == enter_text){
            editing_resistance->update_resistance(current_string,prefix);
            //delete everything from current_string
            current_string.clear();
            //set editing resistance to null
            editing_resistance = NULL;
            textbox_x = textbox_y = -1;
          }
          //enter no_focus note
          focus = no_focus;
        //end of case ButtonPress
        break;
      case ButtonRelease:
      //if you're in dragging focus, enter no_focus focus
        if(focus == dragging){
          current_drag = NULL;
          focus = no_focus;
        }
        break; 
      case MotionNotify:
        x = event.xbutton.x;
        y = event.xbutton.y;
        //if something is being dragged, move it to mouse xy
        if(focus == dragging)
          current_drag->drag_to(x,y); 
        break;
      case KeyPress:
        //user typed a key. if we're in enter_text focus, push key on string
        unsigned int key = event.xkey.keycode;
        if(focus == enter_text){
          switch(key){
            case 44: case 61: //44 is return key, 61 is escape
              editing_resistance->update_resistance(current_string, prefix);
              //delete everything from current_string
              current_string.clear();
              //set editing resistance to null
              editing_resistance = NULL;
              textbox_x = textbox_y = -1;
              focus = no_focus;
              break;
          case 26:case 27:case 28:case 29:case 30:case 31:
          case 33:case 34:case 36:case 37:case 55:
            //keycodes for numbers and decimals
            current_string.push_back(keycode_to_char(key));
            break;
          case 48: //k key
            prefix = 'k';
            break;
          case 54: //m key
            prefix = 'm';
            break;
          case 39: // o key
            prefix = 'o';
            break;
          case 59: //backspace key
            if(current_string.size() > 0){
              current_string.pop_back();
              break;
            }
          default: //user enterred invalid key
            //delete everything. burn the world. Leave nothing alive
            current_string.clear();
            editing_resistance = NULL;
            textbox_x = textbox_y=-1;
            focus = no_focus;
        }
      }
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
  //is it obvious that I'm writing this part at like 1:30am?
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
