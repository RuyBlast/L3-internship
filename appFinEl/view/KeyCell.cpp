#include "view/KeyCell.h"
#include <list>
#include <QPolygonF>
#include <QPointF>
#include <cmath>
using namespace std;

#include <QDebug>

KeyCell::KeyCell(Cell c) : cell(c)
{}

KeyCell::~KeyCell()
{}

void KeyCell::draw(QGraphicsScene& scene) const
{
    // début debug
    QVector<QPointF> qvector; // conteneur des points (coordonnées) de la cellule
    //list<Node>liste(cell.getMembraneNodeList()); // liste des points de la cellule

    for(int i=0 ; i < cell.getElementNumber() ; ++i) // pour chaque noeud de la membrane
    {
      //qDebug() << i << ", cell.iemeElement(i).getNode(1).getx() : " << cell.iemeElement(i).getNode(1).getx() ;
      qreal qx = cell.iemeElement(i).getNode(1).getx();
      qreal qy = cell.iemeElement(i).getNode(1).gety();
      qvector.append( QPointF(qx, qy) );
    }
//qDebug()<<qvector;
    scene.addPolygon(qvector); // bug here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//qDebug()<<"end";

    // fin debug
/*
  QVector<QPointF> qvector; // conteneur des points (coordonnées) de la cellule
  list<Node>liste(cell.getMembraneNodeList()); // liste des points de la cellule

  int i = 0;
  for(list<Node>::iterator it = liste.begin(); it!=liste.end(); ++it) // pour chaque noeud de la membrane
  {
    ++i;
    qDebug() << i-1 << ", it->getx() : " << it->getx() ;
    qreal qx = isnan(it->getx())?i*10:it->getx();
    qreal qy = isnan(it->gety())?i*10:it->gety();
    qvector.append( QPointF(qx, qy) );
  }

  scene.addPolygon(qvector);*/
}
