#include "KeyPoint.h"
#include "model/Node.h"

#define PIXPOINT 2 // rayon du cercle correspondant à un point sur l'affichage graphique

KeyPoint::KeyPoint(Node P) : node(P)
{}

KeyPoint::~KeyPoint()
{}

void KeyPoint::draw(QGraphicsScene& scene) const
{
  double x = node.getx();
  double y = node.gety();

  scene.addEllipse((qreal) x, (qreal) y, (qreal) PIXPOINT, (qreal) PIXPOINT);
}
