#include"circuit.h"
#include<iostream>
#include<vector>
#include<algorithm>
#include"node.h"
#include"resistor.h"

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
    std::cout << "node out of range\n";
     return; //if you're out of range, just stop
  }
  Resistor* new_resistor = new Resistor(resistance,nodes[a],nodes[b]);
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

bool Circuit::step(){
  //search for all series resistors
  for(int i = 0; i < nodes.size(); i++){
    Node* n = nodes[i];
    if(n->resistors.size() == 2 && !(n->resistors[0]->isParallel(n->resistors[1]))){
      //make pointers to the two resistors. Add Resistances
      Node *a,*b;
      a = ((n->resistors[0]->a != n)
           ? n->resistors[0]->a
           : n->resistors[0]->b);
      b = ((n->resistors[1]->a != n)
           ? n->resistors[1]->a
           : n->resistors[1]->b);
      double new_resistance = n->resistors[0]->resistance + n->resistors[1]->resistance;
      add_resistor(index_of_node(a),index_of_node(b),new_resistance);
      remove_node(n);
      return true;
    }
  }
  //search for all parallel resistors
  for(int i = 0; i < nodes.size(); i++){
    Node* n = nodes[i]; //node we're currently looking at
    //search every pair of resistors, check if they're parallel
    for(int i = 0; i < n->resistors.size()-1; i++){
      for(int j = i+1; j < n->resistors.size(); j++){ //resistors i and j
        if(n->resistors[i]->isParallel(n->resistors[j])){
          Resistor *r = n->resistors[i], *s = n->resistors[j];
          Node* m = (r->a == n) ? r->b : r->a;
          //r and s are in parallel between n and m
          double new_resistance = (r->resistance*s->resistance)/(r->resistance+s->resistance);
          new Resistor(new_resistance,n,m);
          delete r;
          delete s;

          return true;
        }
      }
    }
  }
  return false;
}

void Circuit::minimize(){
  while(step());
}

double Circuit::equiv_resistance(){
  Node* n = nodes[0];
  Resistor* r = n->resistors[0];
  double res = r->resistance;
  return res;
}

std::ostream& operator<<(std::ostream& out, const Circuit& c){
  out << c.nodes.size();
  return out;
}
