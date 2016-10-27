#ifndef NODE_H_
#define NODE_H_
#include"resistor.h"
#include"draggable.h"
#include<vector>

class Resistor;

class Node : public Draggable{
  public:
  std::vector<Resistor*> resistors;
  int x,y;
  bool selected;

  Node();
  Node(std::vector<Resistor*> r) : resistors(r) {};
  ~Node();

  bool collapse(); //returns true if the node can be collapsed, if it has two resistors
  bool combine(); //returns true if there are two resistors in parallel on this node
  void addResistor(Resistor* r);
  void remove_resistor(Resistor* r);

  void drag_to(int new_x, int new_y);
};

#endif
