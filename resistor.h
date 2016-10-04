#ifndef RESISTOR_H_
#define RESISTOR_H_
#include"node.h"

class Node;

class Resistor{
  public:
  Node* a;
  Node* b;
  double resistance;

  Resistor();
  Resistor(double r) : resistance(r) {};
  Resistor(double r, Node* n, Node* m);
  ~Resistor();

  bool combine_series(Resistor* r);
  bool combine_parallel(Resistor* r);
  bool isParallel(Resistor* r);
};

#endif
