#include"node.h"
#include "component.h"
#include<vector>
#include<algorithm>
#include<iostream>

Node::Node(){
  x = y = 0;
  selected = false;
}

Node::~Node(){
//  x = y = 0;
  while(components.size()){
    delete components[0];
  }
}

bool Node::collapse(){
  if(components.size() == 2 && (components[0]->type == components[1]->type)){
    //TODO, make sure the components aren't in parallel. That would
    //screw some stuff up.

    components[0]->combine_series(components[1]);
    return true;

  }
  return false;
}

bool Node::combine(){
  bool return_val = false;
  for(int i = 0; i < components.size()-1; i++){
    for(int j = i+1; j < components.size(); j++){
      if(components[i]->isParallel(components[j])){
        components[i]->combine_parallel(components[j]);
        components.erase(components.begin()+j--);
        return_val = false;
      }
    }
  }
  return return_val;
}

void Node::add_component(Component* r){
  components.push_back(r);
}

void Node::remove_component(Component* c){
  std::vector<Component*>::iterator position = std::find(components.begin(), components.end(), c);
  if(position != components.end()){
    components.erase(position);
  }
}

void Node::drag_to(int new_x, int new_y){
  x = new_x;
  y = new_y;
}
