#include "component.h"
#include "node.h"
#include <vector>

Component::~Component(){
  a->remove_component(this);
  b->remove_component(this);
}

bool Component::isParallel(Component* c){
  return( (c->a == a && c->b == b) ||
          (c->a == b && c->b == a));
}

Resistor::Resistor(double r){
  type = RESISTOR;
  weight = r;
  a = b = NULL;
}

Resistor::Resistor(double r, Node* n, Node* m){
  type = RESISTOR;
  weight = r;
  a = n;
  b = m;
  a->add_component(this);
  b->add_component(this);
}

Resistor::~Resistor(){
  a->remove_component(this);
  b->remove_component(this);
}

void Resistor::update_weight(std::vector<char> str, char prefix){
   //take a string in the form of a std::vector of chars
  //if it's a legal resistance value, make this resistor value that
  bool pre_decimal = true;
  double new_resistance = 0.0;
  int digits_after_decimal = 0;
  for(char c : str){
    if(c >= '0' && c <= '9'){
      new_resistance *= 10;
      new_resistance += c-'0';
      if(!pre_decimal)
        digits_after_decimal++;
    }
    else if(c == '.'){
      if(pre_decimal) pre_decimal = false;
      else return;
    }
    else return;
  }
  for(int i = 0; i < digits_after_decimal; i++)
    new_resistance /= 10.0;
  if(prefix == 'k')
    new_resistance *= 1000;
  else if(prefix == 'm')
    new_resistance *= 1000000;
  weight = new_resistance; //jesus that's some convoluted logic
}

bool Resistor::combine_series(Component* c){
  //route this across r, set c's resistance to 0
  if(c->type != RESISTOR){
    throw std::runtime_error("can't combine mismatched components");
  }

  //this and c share one noce. upstream and downstream should be the two
  //unshared nodes
  Node *upstream, *downstream;
  if(a == c->a || a == c->b)
    upstream = b;
  else
    upstream = a;
  if(a == c->a || b == c->a)
    downstream = c->b;
  else
    downstream = c->a;
  (a == upstream) ? b = downstream : a = downstream;
  c->weight = 0;
  return true;
}

bool Resistor::combine_parallel(Component* c){

  if(c->type != RESISTOR)
    throw std::runtime_error("can't combine mismatched components");

  //set this's resistance to (c->r*r/(c->r+r)), set c's resistance to inf
  weight = (c->weight * weight) /
                      (c->weight + weight);

  c->weight = std::numeric_limits<double>::infinity();
  return true;
}

Capacitor::Capacitor(double c){
  type = CAPACITOR;
  weight = c;
  a = b = NULL;
}

Capacitor::Capacitor(double c, Node* n, Node* m){
  type = CAPACITOR;
  weight = c;
  a = n;
  b = m;
  a->add_component(this);
  b->add_component(this);
}

Capacitor::~Capacitor(){
  a->remove_component(this);
  b->remove_component(this);
}
void Capacitor::update_weight(std::vector<char> str, char prefix){
   //take a string in the form of a std::vector of chars
  //if it's a legal inductance value, make this resistor value that
  bool pre_decimal = true;
  double new_capacitance = 0.0;
  int digits_after_decimal = 0;
  for(char c : str){
    if(c >= '0' && c <= '9'){
      new_capacitance *= 10;
      new_capacitance += c-'0';
      if(!pre_decimal)
        digits_after_decimal++;
    }
    else if(c == '.'){
      if(pre_decimal) pre_decimal = false;
      else return;
    }
    else return;
  }
  for(int i = 0; i < digits_after_decimal; i++)
    new_capacitance /= 10.0;

  switch(prefix){ //si prefixes that Have letters
    case 'p':
      new_capacitance /= 1000000000000; //picohenry's are small
      break;
    case 'n':
      new_capacitance /= 1000000000; //nanohenrys. Also small
      break;
    case 'u':
      new_capacitance /= 1000000; //microheny
      break;
    case 'm':
      new_capacitance /= 1000;
      break;
    case 'k':
      new_capacitance *= 1000;
      break;
    case 'M':
      new_capacitance *= 1000000; //megahenry
      break;
  }

  weight = new_capacitance; //jesus that's some convoluted logic
}

bool Capacitor::combine_series(Component* c){
  if(c->type != type)
    throw std::runtime_error("cannot combine mismatched components");

  //find the unshared nodes of this and c
  Node *upstream, *downstream;
  if(a == c->a || a == c->b)
    upstream = b;
  else
    upstream = a;
  if(c->a == a || c->a == b)
    downstream = c->b;
  else
    downstream = c->a;

  //bridge the node
  (a = upstream) ? b = downstream : a = downstream;

  //capacitances add inversely in seies
  weight = (c->weight * weight) /
                       (c->weight + weight);


  //don't touch c's capacitance
  return true;
}

bool Capacitor::combine_parallel(Component* c){
  if(c->type != type)
    throw std::runtime_error("cannot combine mismatched components");

  //add c's capacitance to this
  weight += c->weight;
  //set c's capacitance to 0
  c->weight = 0;

  return true;
}

Inductor::Inductor(double l){
  type = INDUCTOR;
  weight = l;
  a = b = NULL;
}

Inductor::Inductor(double l, Node* n, Node* m){
  type = INDUCTOR;
  weight = l;
  a = n;
  b = m;
  a->add_component(this);
  b->add_component(this);
}

Inductor::~Inductor(){
  a->remove_component(this);
  b->remove_component(this);
}

void Inductor::update_weight(std::vector<char> str, char prefix){
   //take a string in the form of a std::vector of chars
  //if it's a legal inductance value, make this resistor value that
  bool pre_decimal = true;
  double new_inductance = 0.0;
  int digits_after_decimal = 0;
  for(char c : str){
    if(c >= '0' && c <= '9'){
      new_inductance *= 10;
      new_inductance += c-'0';
      if(!pre_decimal)
        digits_after_decimal++;
    }
    else if(c == '.'){
      if(pre_decimal) pre_decimal = false;
      else return;
    }
    else return;
  }
  for(int i = 0; i < digits_after_decimal; i++)
    new_inductance /= 10.0;

  switch(prefix){ //si prefixes that Have letters
    case 'p':
      new_inductance /= 1000000000000; //picohenry's are small
      break;
    case 'n':
      new_inductance /= 1000000000; //nanohenrys. Also small
      break;
    case 'u':
      new_inductance /= 1000000; //microheny
      break;
    case 'm':
      new_inductance /= 1000;
      break;
    case 'k':
      new_inductance *= 1000;
      break;
    case 'M':
      new_inductance *= 1000000; //megahenry
      break;
  }

  weight = new_inductance; //jesus that's some convoluted logic

}

bool Inductor::combine_series(Component* c){
  if(type != c->type)
    throw std::runtime_error("cannot combine mismatched components");

  //set this across c, add c's inductance to this's
  Node *upstream, *downstream;
  if(a == c->a || a == c->b)
    upstream = b;
  else
    upstream = a;
  if(c->a == a || c->a == b)
    downstream = c->b;
  else
    downstream = c->a;
  (upstream == a) ? b = downstream : a = downstream;

  //update weight.inductance. Inductors in series sum.
  weight += c->weight;

  return true;
  //don't change c's inductance, because it works differently than resistors
}

bool Inductor::combine_parallel(Component* c){
  //works the same way as resistors
  if(c->type != INDUCTOR)
    throw std::runtime_error("cannot combine mismatched components");

  weight = (c->weight * weight) /
                      (c->weight + weight);
  //don't set c's inductance, because unlike resistors, it doesn't go to infinity
  return true;
}
