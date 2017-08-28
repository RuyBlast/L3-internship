#ifndef KEYLINE_H
#define KEYLINE_H

#include "model/Element.h"
#include <QGraphicsScene>

class KeyLine
{
  private :
    Node node1;
    Node node2;

  public :
    KeyLine(Node n1, Node n2);
    KeyLine(Element el);
    ~KeyLine();
    void draw(QGraphicsScene& scene) const;

};

#endif // KEYLINE_H
