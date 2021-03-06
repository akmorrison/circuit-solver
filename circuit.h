#ifndef CIRCUIT_H_
#define CIRCUIT_H_
#include<vector>
#include"node.h"

class Node;

class Circuit{
  public:
  std::vector<Node*> nodes;

  Circuit();
  Circuit(int n);

  int index_of_node(Node* a);
  void remove_node(Node* a);
  void add_node();

  void set_node_num(int n);
  void resize_nodes(int n);

  void add_resistor(int a, int b, double resistance);
  void add_capacitor(int a, int b, double capacitance);
  void add_inductor(int a, int b, double inductance);

  bool step();
  void minimize(); //calls step until it doesn't return true
};

std::ostream& operator<<(std::ostream& out, const Circuit& c);

#endif
