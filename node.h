#ifndef NODE_H_
#define NODE_H_
#include"component.h"
#include"draggable.h"
#include<vector>

class Resistor;

class Node : public Draggable{
  public:
  std::vector<Component*> components;
  int x,y;
  bool selected;

  Node();
  Node(std::vector<Component*> c) : components(c) {};
  ~Node();

  bool collapse(); //returns true if the node can be collapsed, if it has two matching components
  bool combine(); //returns true if there are two matching components in parallel on this node
  void add_component(Component* c);
  void remove_component(Component* c);

  void drag_to(int new_x, int new_y);
};

#endif
