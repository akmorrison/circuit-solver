#include"circuit.h"
#include<iostream>
#include<vector>
#include<algorithm>
#include"node.h"
#include"component.h"

Circuit::Circuit(){

}

Circuit::Circuit(int n){
  for(int i = 0; i < n; i++){
    Node* new_node = new Node();
    this->nodes.push_back(new_node);
  }
}

void Circuit::set_node_num(int n){
  for(auto n : nodes){
    delete n;
  }
  nodes.clear();
  for(int i = 0; i < n; i++)
    nodes.push_back(new Node);
}

void Circuit::resize_nodes(int n){
  //if n is less than num_nodes,
  //delete num_nodes-n nodes
  if(n < nodes.size()){
    for(int i = n; i < nodes.size(); i++){
      delete nodes[i];
    }
    nodes.resize(n);
  }
  //if n is greater than num_nodes,
  // add num_nodes-n nodes
  for(int i = nodes.size(); i < n; i++){
    nodes.push_back(new Node);
  }
}

void Circuit::add_resistor(int a, int b, double resistance){
  if(a >= nodes.size() || b >= nodes.size() || a == b){
    throw std::invalid_argument("node out of range");
  }
  Resistor* new_resistor = new Resistor(resistance,nodes[a],nodes[b]);
}

void Circuit::add_capacitor(int a, int b, double capacitance){
  if(a >= nodes.size() || b >= nodes.size() || a == b){
    throw std::invalid_argument("node out of range");
  }
  new Capacitor(capacitance, nodes[a], nodes[b]);
}

void Circuit::add_inductor(int a, int b, double inductance){
  if(a >= nodes.size() || b >= nodes.size() || a == b)
    throw std::invalid_argument("node out of range");
  new Inductor(inductance, nodes[a], nodes[b]);
}

int Circuit::index_of_node(Node* a){
  std::vector<Node*>::iterator i = std::find(nodes.begin(),
                                             nodes.end(),a);
  if(i != nodes.end())
    return i-nodes.begin();
  return -1;
}

void Circuit::remove_node(Node* a){
  std::vector<Node*>::iterator i = std::find(nodes.begin(),nodes.end(),a);
  if(i != nodes.end())
    nodes.erase(i);
  delete a;
}

void Circuit::add_node(){
  nodes.push_back(new Node);
}

bool Circuit::step(){
  //find the first components that are in series
  //this is true if a node has two components, both of the same type
  //and the two components aren't in parallel
  for(Node* n : nodes){
    if(n->components.size() == 2 && 
      (n->components[0]->type == n->components[1]->type) && 
      !(n->components[0]->isParallel(n->components[1]))){
      //two components of matching types are in series
      n->components[0]->combine_series(n->components[1]);
      delete n->components[1];
      //there's now 0 things connected to n. It can die
      remove_node(n);
      return true;
    }
  }


  //find the first components that are in series
  //for this, for each pair of nodes, if there are two components
  //of the same type between them, they are in l

  for(int i = 0; i < nodes.size(); i++)for(int j = i+1; j < nodes.size();j++){
    //foreach pair of nodes
    Node* n = nodes[i];
    Node* m = nodes[j];
    //make a vector of all components between n and m
    std::vector<Component*> parallels;
    for(Component* c : n->components){
      if(c->a == m || c->b == m)
        parallels.push_back(c);
    }
    //find out if there are two components in the same type
    for(int k = 0; k < parallels.size(); k++)
      for(int l = k+1; l < parallels.size(); l++)
        if(parallels[k]->type == parallels[l]->type){
          //combine those two as parallel
          parallels[k]->combine_parallel(parallels[l]);
          //destroy the abstract notion of parallels[l]
          delete parallels[l];
          return true;
        }
  }
  return false;
}


void Circuit::minimize(){
  while(step());
}


std::ostream& operator<<(std::ostream& out, const Circuit& c){
  out << c.nodes.size();
  return out;
}
