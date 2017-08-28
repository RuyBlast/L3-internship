#include "Cell.h"
#include "Tissue.h"
#include <cmath> // pour atan2, cos et sin
#include <cassert>
#include <list>
#include <QDebug>
using namespace std;

#define _USE_MATH_DEFINES

// methodes

Cell::Cell(Tissue& t, Node &centre, double rayon) : tissue(t), contractionBegin(0) //: centralNode(centre)
{
  elementList.clear();
  Node node1(centre.getx(), centre.gety() - rayon);
  elementList.push_back(Element(tissue, centre, node1));
  for (int i=0 ; i<5 ; i++) // construit les 4 autres éléments suivants et les ajoutes à la liste
  {
    elementList.push_back(Element(tissue, centre, elementList.back().getNode(2)));
  }
  // ajoute le dernier élément en faisant le raccord avec le premier :
  //elementList.push_back(Element(tissue, centre, elementList.back().getNode2(), elementList.front().getNode1()));
}

Cell::Cell(Tissue& t, Element voisin) : tissue(t), contractionBegin(0)
{
  // calcul du troisième noeud : le noeud central de la Cellule à construire

  Node node0(voisin.getNode(2)), node1(voisin.getNode(1)), centre;
  //node0 = voisin.getNode2();node1(voisin.getNode1), centre;
  // calcul des coordonnées relative du troisième noeud par rapport au node1 :
  double angle;
  // calcul de l'angle d entre l'axe des abscisses et [node1, node0) :
  double dx = node0.getx() - node1.getx();
  double dy = node0.gety() - node1.gety();

  angle = atan2(dy,dx);

  // l'angle qui nous intéresse est alpha == angle - PI/3;
  double alpha = angle - 2*M_PI/3;

  // calcul des coordonnées du node2 :
  double x = node0.getx() + cos(alpha) * Node::distance(node0, node1);
  double y = node0.gety() + sin(alpha) * Node::distance(node0, node1);

  centre = Node(x, y);

  // génération des 6 éléments de la cellule :

  elementList.clear();
  elementList.push_back(Element(tissue, centre, node0, node1));
  for (int i=0 ; i<5 ; i++) // construit les 4 autres éléments suivants et les ajoutes à la liste
  {
    elementList.push_back(Element(tissue, centre, elementList.back().getNode(2)));
  }
  // ajoute le dernier élément en faisant le raccord avec le premier :
  //elementList.push_back(Element(tissue, centre, elementList.back().getNode2(), elementList.front().getNode1()));

}

Cell::Cell(Tissue& t, Element voisin, int ivoisin) : Cell(t, voisin)
{
  for (int i=0 ; i<ivoisin+3 ; i++) // décale la liste de i
  {
    elementList.push_front(elementList.back());
    elementList.pop_back();
  }
  recalibrate(*this);
}

Cell::Cell(Tissue& t, Cell const& cell) : tissue(t), contractionBegin(0)
{
  for (list<Element>::const_iterator it=cell.elementList.begin(); it!=cell.elementList.end() ; ++it)
    elementList.push_back(Element(t,*it));
}

Cell& Cell::operator=(Cell const& c)
{
  tissue = c.tissue;
  elementList = c.elementList;
  contractionBegin = c.contractionBegin;
  return *this;
}

list<Node> Cell::getMembraneNodeList() const
{
  list<Node> liste;
  for(list<Element>::const_iterator it = elementList.begin(); it!=elementList.end(); ++it)
    liste.push_back(it->getNode(1));
  return liste;
}

list<unsigned int> Cell::getMembraneNodeIndexList() const
{
  list<unsigned int> liste;
  for(list<Element>::const_iterator it = elementList.begin(); it!=elementList.end(); ++it)
    liste.push_back(it->getNodeIndex(1));
  return liste;
}

// renvoie le ieme élément d'une cellule
Element Cell::iemeElement(int i) const
{
  //if (i >= elementList.size())
    //assert(false);
  i %= elementList.size();
  list<Element>::const_iterator it = elementList.begin();
  for(int j=0 ; j<i && it!=elementList.end() ; ++j)
    ++it;
  return *it;
}

double Cell::xcoeffIemeNodeForAreaConstraint(int i) const
{
  if (i >= elementList.size())
    assert(false);

  double yminus, yplus;

  list<Element>::const_iterator it = elementList.begin();
  advance(it, i);

  // calcul yminus
  if (it != elementList.begin())
  {
    yminus = (--it)->getNode(1).gety() ;
    ++it ;
  }
  else
    yminus = elementList.back().getNode(1).gety() ;

  // calcul yplus
  if (it == elementList.end())
  {
    it = elementList.begin();
    yplus = (++it)->getNode(1).gety();
  }
  else if (++it != elementList.end())
    yplus = (it)->getNode(1).gety();
  else
    yplus = elementList.front().getNode(1).gety() ;

  return (yplus - yminus)/2 ;
}

double Cell::ycoeffIemeNodeForAreaConstraint(int i) const
{
  if (i >= elementList.size())
    assert(false);

  double xminus, xplus;

  list<Element>::const_iterator it = elementList.begin();
  for(int j=0 ; j<i && it!=elementList.end() ; ++j)
    ++it;

  // calcul yminus
  if (it != elementList.begin())
  {
    xminus = (--it)->getNode(1).getx() ;
    //qDebug() << "it " << it->getNodeIndex(1);
    it++ ;
  }
  else
    xminus = elementList.back().getNode(1).getx() ;

  // calcul yplus
  if (it == elementList.end())
  {
    it = elementList.begin();
    xplus = (++it)->getNode(1).getx();
  }
  else if (++it != elementList.end())
    xplus = (it)->getNode(1).getx();
  else
    xplus = elementList.front().getNode(1).getx() ;

  return -(xplus - xminus)/2 ;
}

Element Cell::operator [] (int i) const
{
  return iemeElement(i);
}

Node & Cell::getCenter() const
{
  Node node = elementList.front().getNode(0);
  return node;
}

double Cell::getPerimeter() const
{

}

double Cell::getArea() const
{
  double x0, y0, x1, y1, area(0);
  list<Element>::const_iterator it0(elementList.begin()), it1(elementList.begin());
  ++it1;

  for( ; it1 != elementList.end() ; ++it0 , ++it1)
  {
    x0 = it0->getNode(1).getx();
    y0 = it0->getNode(1).gety();
    x1 = it1->getNode(1).getx();
    y1 = it1->getNode(1).gety();
    area += (x0*y1 - x1*y0)/2 ;
  }

  x0 = it0->getNode(1).getx();
  y0 = it0->getNode(1).gety();
  x1 = elementList.front().getNode(1).getx();
  y1 = elementList.front().getNode(1).gety();
  area += (x0*y1 - x1*y0)/2 ;

  return area ;
}

unsigned int Cell::getElementNumber() const
{
  unsigned int nb = 0;
  for(list<Element>::const_iterator it = elementList.begin(); it!=elementList.end(); ++it)
  {
    nb++ ;
  }
  return nb ;
}

Element & Cell::nextElement(Element const& elem) const
{
  for(list<Element>::const_iterator it = elementList.begin(); it!=elementList.end(); it++)
  {
    if (*it == elem)
    {
      if ((++it) != elementList.end())
        return const_cast<Element&>(*it);
      else
        return const_cast<Element&>(elementList.front());
    }
  }

  assert(false); // pb ici ... (quand il y a plein de carrés)
}

Element & Cell::previousElement(Element const& elem) const
{
  for(list<Element>::const_iterator it = elementList.begin(); it!=elementList.end(); ++it)
  {
    if (*it == elem)
    {
      if (it != elementList.begin())
        return const_cast<Element&>(*(--it));
      else
        return const_cast<Element&>(elementList.back());
    }
  }

  assert(false);
}

Element & Cell::findAdjoiningElement(Element const& elem) const
{
  for(list<Element>::const_iterator it = elementList.begin(); it!=elementList.end(); ++it)
    if (elem.getNodeIndex(1) == it->getNodeIndex(2) && elem.getNodeIndex(2) == it->getNodeIndex(1))
      return const_cast<Element&>(*it);

  assert(false);
}

Element * Cell::addElement(Element elem, Element & after)
{
  Element * ret = 0;
  list<Element>::iterator it = elementList.begin();
  for( ; !(*it==after); ++it) // plante si after n'est pas dans la liste (out of range)
  {}
  elementList.insert((++it), elem); // insert elem après after
  ret = &(*(--it));
  return ret;
}

void Cell::removeElement(Element & elem)
{
  elementList.remove(elem);
}

Element * Cell::giveElement(Element & elm, Cell & toCell, Element & after)
{
  Element * ret = toCell.addElement(elm, after);
  removeElement(elm);
  return ret;
}

void Cell::recalibrate(Cell & c)
{
  // on prend comme référence la première cellule de la membrane (extrémité haute dans la fenêtre d'affichage)
  double x1 = c.iemeElement(0).getNode(1).getx();
  double y1 = c.iemeElement(0).getNode(1).gety();
  // on construit le centre :
  double x0 = x1;
  double y0 = y1 + tissue.getCellRadius();
  tissue.setCoordNode(c.iemeElement(0).getNodeIndex(0), x0, y0); // centre

  double angle, alpha, dx, dy, x, y;
  for (int i=0; i<5; ++i) // recalibration des coordonnées des noeuds 1 à 5 de la cellule
  {
    // calcul de l'angle d entre l'axe des abscisses et [node1, node0) :
    double dx = x0 - x1;
    double dy = y0 - y1;

    angle = atan2(dy,dx);

    // l'angle qui nous intéresse est alpha == angle - PI/3;
    double alpha = angle - 2*M_PI/3;

    // calcul des coordonnées du node2 :
    x = x0 + cos(alpha) * tissue.getCellRadius();
    y = y0 + sin(alpha) * tissue.getCellRadius();

    // application du changement de coordonnées :
    tissue.setCoordNode(c.iemeElement(i).getNodeIndex(2), x, y);

    // changement des coordonnées de référence de x1 et y1 pour la prochaine boucle :
    x1 = x;
    y1 = y;
  }

}
