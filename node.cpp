#include"node.h"
#include<vector>
#include<algorithm>
#include<iostream>

Node::Node(){
  x = y = 0;
}

Node::~Node(){
  x = y = 0;
  while(resistors.size()){
    delete resistors[0];
  }
}

bool Node::collapse(){
  if(resistors.size() == 2){
    //TODO, make sure the resistors aren't in parallel. That would
    //screw some stuff up.

    resistors[0]->combine_series(resistors[1]);
    return true;

  }
  return false;
}

bool Node::combine(){
  bool return_val = false;
  for(int i = 0; i < resistors.size()-1; i++){
    for(int j = i+1; j < resistors.size(); j++){
      if(resistors[i]->isParallel(resistors[j])){
        resistors[i]->combine_parallel(resistors[j]);
        resistors.erase(resistors.begin()+j--);
        return_val = false;
      }
    }
  }
  return return_val;
}

void Node::addResistor(Resistor* r){
  resistors.push_back(r);
}

void Node::remove_resistor(Resistor* r){
  std::vector<Resistor*>::iterator position = std::find(resistors.begin(), resistors.end(), r);
  if(position != resistors.end()){
    resistors.erase(position);
  }
}

void Node::drag_to(int new_x, int new_y){
  x = new_x;
  y = new_y;
}
