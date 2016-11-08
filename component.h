#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "node.h"
#include <vector>

class Node;

typedef enum{
  RESISTOR,
  CAPACITOR,
  INDUCTOR
} component_type;

class Component{
  public:
  Node *a, *b;
  component_type type;

  double weight; //resistance, capacitance, or inductance

  virtual ~Component() = 0;

  virtual void update_weight(std::vector<char> str, char prefix) = 0;

  virtual bool combine_series(Component* c) = 0;
  virtual bool combine_parallel(Component* c) = 0;
  bool isParallel(Component* c);

};

class Resistor : public Component{
  public:

  Resistor(double r);
  Resistor(double r, Node* n, Node* m);
  ~Resistor();

  void update_weight(std::vector<char> str, char prefix);
  bool combine_series(Component* c);
  bool combine_parallel(Component* c);
};

class Capacitor : public Component{
  public:

  Capacitor(double c);
  Capacitor(double c, Node* n, Node* m);
  ~Capacitor();

  void update_weight(std::vector<char> str, char prefix);
  bool combine_series(Component* c);
  bool combine_parallel(Component* c);

};

class Inductor : public Component{
  public:

  Inductor(double l);
  Inductor(double l, Node* n, Node* m);
  ~Inductor();

  void update_weight(std::vector<char> str, char prefix);
  bool combine_series(Component* c);
  bool combine_parallel(Component* c);

};

#endif
