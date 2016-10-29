#ifndef RESISTOR_H_
#define RESISTOR_H_
#include"node.h"
#include<vector>

class Node;

class Resistor{
  public:
  Node* a;
  Node* b;
  double resistance;

  Resistor(double r) : resistance(r) {};
  Resistor(double r, Node* n, Node* m);
  ~Resistor();

  void get_textbox_xy(int& x, int& y);
  void update_resistance(std::vector<char> str);

  bool combine_series(Resistor* r);
  bool combine_parallel(Resistor* r);
  bool isParallel(Resistor* r);
};

#endif
