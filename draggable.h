#ifndef DRAGGABLE_H_
#define DRAGGABLE_H_

class Draggable{
  public:
    int x, y;
    virtual void drag_to(int new_x, int new_y) = 0;
};

#endif
