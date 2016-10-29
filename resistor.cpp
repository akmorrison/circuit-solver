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

void Resistor::get_textbox_xy(int& x, int& y){
  //just return the x,y midway between the two nodes
  //we can math this better later. Not right now
  x = (a->x + b->x)/2;
  y = (a->y + b->y)/2;
}

void Resistor::update_resistance(std::vector<char> str){
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
  resistance = new_resistance; //jesus that's some convoluted logic
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
