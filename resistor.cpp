#include"resistor.h"
#include<vector>
#include<limits>

Resistor::Resistor(double r, Node* n, Node* m){
  resistance = r;
  a = n;
  b = m;
  a->addResistor(this);
  b->addResistor(this);
}

Resistor::~Resistor(){
  a->remove_resistor(this);
  b->remove_resistor(this);
}

bool Resistor::combine_series(Resistor* r){ //route's this across r
                                            //set's r's resistance to 0
  Node *upstream, *downstream;
  if(this->a == r->a || this->a == r->b)
    upstream = this->b;
  else
    upstream = this->a;
  if(r->a == this->a || r->a == this->b)
    downstream = r->b;
  else
    downstream = r->a;
  this->resistance += r->resistance;
  (this->a == upstream) ? this->b = downstream : this->a = downstream;
  r->resistance = 0;
  return true;
}

bool Resistor::combine_parallel(Resistor* r){ //make's r's resistance inf
  double new_resistance = (this->resistance*r->resistance)/(this->resistance+r->resistance);
  this->resistance = new_resistance;
  r->resistance = std::numeric_limits<double>::infinity();
  return true;
}

bool Resistor::isParallel(Resistor* r){
  return (this->a == r->a && this->b == r->b) || (this->a == r->b && this->b == r->a);
}
