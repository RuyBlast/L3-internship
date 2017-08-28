#include "view/KeyLine.h"

KeyLine::KeyLine(Node n1, Node n2) : node1(n1), node2(n2)
{}

KeyLine::KeyLine(Element El) : node1(El.getNode(1)), node2(El.getNode(2))
{}

KeyLine::~KeyLine()
{}

void KeyLine::draw(QGraphicsScene& scene) const
{
  int x1, y1, x2, y2;
  x1 = node1.getx();
  y1 = node1.gety();
  x2 = node2.getx();
  y2 = node2.gety();

  scene.addLine((qreal) x1, (qreal) y1, (qreal) x2, (qreal) y2);
}
