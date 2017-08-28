#include <list>
#include <vector>
#include <cmath> // pour atan2, cos et sin
#include <cassert>
using namespace std;
#include "Node.h"
#include "Matrix.h"
#include "Element.h"
#include "Cell.h"
#include "Tissue.h"

#include <QDebug>

#define _USE_MATH_DEFINES

// methodes

// Constructors
Element::Element(Tissue& t) : tissue(t)
{
  node_index[0] = t.addNode(Node(0,0));
  node_index[1] = t.addNode(Node(0,0));
  node_index[2] = t.addNode(Node(0,0));
}

Element::Element(Tissue& t, Node node0, Node node1) : tissue(t)
{
  node_index[0] = t.addNode(node0);
  node_index[1] = t.addNode(node1);
  node_index[2] = t.addThirdNode(node0, node1);
} // va calculer le node2

Element::Element(Tissue& t, Node node0, Node node1, Node node2) : tissue(t)
{
  node_index[0] = t.addNode(node0);
  node_index[1] = t.addNode(node1);
  node_index[2] = t.addNode(node2);
}

Node Element::getNode(int i) const { return tissue.getNode(node_index[i%3]); }

double Element::getLength() const
{
  Node n1 = getNode(1);
  Node n2 = getNode(2);
  return Node::distance(n1, n2);
}

Cell Element::belongsToCell() const
{}

Matrix Element::calculKe() const
{
  Matrix Ke (6,6);
  //vector<Node> node = tissue.getNodeList();

  double x21 = getNode(1).getx() - getNode(0).getx();
  double x31 = getNode(2).getx() - getNode(0).getx();
  double x32 = getNode(2).getx() - getNode(1).getx();
  double y21 = getNode(1).gety() - getNode(0).gety();
  double y31 = getNode(2).gety() - getNode(0).gety();
  double y32 = getNode(2).gety() - getNode(1).gety();

  double A = 0.5 *(x21 * y31 - x31 * y21); // aire du triangle
  double B [3*6] = // au facteur 1/2A près
  {
    -y32,   0, y31,   0,-y21,   0 , // epsilonx
       0, x32,   0,-x31,   0, x21 , // epsilony
     x32,-y32,-x31, y31, x21,-y21   // gammaxy
  };



  double E = 1, nu = 0.;
  double G [3*3] =  // matrice de comportement, au facteur E/[(1+nu)(1-2nu)] près
  {
    1-nu,   nu,    0       ,
      nu, 1-nu,    0       ,
       0,    0, (1-2*nu)/2
  };

  double GB [3*6] = {0,0,0,0,0,0,
                     0,0,0,0,0,0,
                     0,0,0,0,0,0};
  int i, j, k;
  double s = 0;
  for (i = 0; i < 3; i++) // pour chaque ligne i de [G]
    for (j = 0; j < 6; j++) // pour chaque colone j de [B]
      {
        s = 0;
        for (k = 0; k < 3; k++) // pour chaque ligne k
          s += G[i*3 + k] * B[k*6 + j]; // on parcourt la ligne i de G et la colonne j de B
        GB[i*6 + j] = s;
      }

  // Ke = [B]t [G] [B]
  for (i = 0; i < 6; i++) // pour chaque ligne i de [B]t
    for (j = 0; j < 6; j++) // pour chaque colonne j de [GB]
    {
      s = 0;
      for (k = 0; k < 3; k++) // pour chaque ligne k de [GB] (ok pour sens de parcours)
        s += B[k*6 + i] * GB[k*6 + j]; // parcours dans le même sens car [B]t
      Ke(i, j) = s * A * E / (2 * (1 + nu)); // coef ? pour A ?
      //qDebug() << "Ke(" << i << "," << j << ") : " << Ke(i,j) ;
    }
    //qDebug() << Ke.hasNonZeroDiagonal() ;
    //Ke.debug();
   return Ke;

}

bool Element::operator ==(Element const& elem) const
{
    if (node_index[0] == elem.node_index[0] &&
        node_index[1] == elem.node_index[1] &&
        node_index[2] == elem.node_index[2])
        return true;
    else
        return false;
}

Element& Element::operator=(Element const& e)
{
  tissue = e.tissue;
  node_index[0] = e.node_index[0];
  node_index[1] = e.node_index[1];
  node_index[2] = e.node_index[2];
  return *this;
}

void Element::setNodeIndex(unsigned int i, unsigned int newNodeIndex)
{
  if (newNodeIndex >= tissue.getNodeNumber()) // si newNodeIndex référence un noeud qui n'existe pas dans le tissu
    assert(false);
  node_index[i%3] = newNodeIndex;
}
