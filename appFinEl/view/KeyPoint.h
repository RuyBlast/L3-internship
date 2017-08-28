#ifndef KEYPOINT_H
#define KEYPOINT_H

#include "model/Node.h"
#include <QGraphicsScene>

class KeyPoint
{
  private :
    Node node;

  public :
    KeyPoint(Node P);
    ~KeyPoint();
    void draw(QGraphicsScene& scene) const;

};

#endif // KEYPOINT_H
