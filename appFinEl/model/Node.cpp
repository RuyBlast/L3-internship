#include <list>
#include <cmath> // pour sqrt et pow, atan, sin et cos
#include "Node.h"
using namespace std;

#define _USE_MATH_DEFINES

  // attributs :: double x : abscisse ; double y : ordonnée
  // methodes

/*
list<Cell> Node::belongsToCells() const;

list<Element> Node::belongsToElements() const;
*/

Node::Node(Node node0, Node node1)
{
    // calcul des coordonnées relative du troisième noeud par rapport au node1 :
    double angle;
    // calcul de l'angle d entre l'axe des abscisses et [node1, node0) :
    double dx = node0.getx() - node1.getx();
    double dy = node0.gety() - node1.gety();

    angle = atan2(dy,dx);

    // l'angle qui nous intéresse est alpha == angle - PI/3;
    double alpha = angle - 2*M_PI/3;

    // calcul des coordonnées du node2 :
    x = node0.getx() + cos(alpha) * distance(node0, node1);
    y = node0.gety() + sin(alpha) * distance(node0, node1);
}

// méthodes statiques utilitaires concernant les noeuds :

double Node::distance (Node& a, Node& b)
{
  return sqrt( pow( a.getx() - b.getx() , 2) + pow( a.gety() - b.gety() , 2) ) ;
  // calculate the distance between node a and node b
}
/*
void Node::draw (Node a, Node b)
{
    drawL(a.getx(), a.gety(), b.getx(), b.gety());
}*/

std::pair<double,double> Node::barycentre(list<Node> nodeList)
{
  double xMoyen(0), yMoyen(0);
  for(list<Node>::iterator it = nodeList.begin(); it!=nodeList.end(); ++it)
    {
      xMoyen += it->getx();
      yMoyen += it->gety();
    }
  return std::pair<double,double>( xMoyen/static_cast<double>(sizeof(nodeList)),
       yMoyen/static_cast<double>(sizeof(nodeList)) ) ;
}

std::pair<double, double> Node::barycentre(Node a, Node b)
{
  list<Node> liste;
  liste.push_back(a);
  liste.push_back(b);
  return barycentre(liste);
}

bool Node::operator==(Node const& node) const
{
  return (x==node.getx() && y==node.gety());
}
