#include "Cell.h"
#include <list>
#include <string> // pour passer le nom de la méthode de génération du tissue
#include <vector>
#include <cmath> // pour sin, cos et pow
#include "Element.h"
#include "Node.h"
#include "Matrix.h"
#include "Tissue.h"
#include <stdio.h>      /* NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cassert>
#include <QDebug>
using namespace std;

#define _USE_MATH_DEFINES

Tissue::Tissue(int length, int height, double cellRad) : cellRadius(cellRad), vertexMin(cellRad/2), timeUnit(0)
{
//    if (length < 2 || height < 1)
//        assert(false);

    int parity = height % 2;

    creerPremiereRangeeDeCellules(length, height, cellRadius);
    for (int i=0 ; i < height/2 - 1 ; ++i)
        dupliquerLaRangeePrecedante(length, cellRadius);
    if (height > 2 && parity == 1)
        ajouterLaDerniereDemiRangee(length, cellRadius);

    cellNumber = CellList.size();
    nodeNumber = nodeList.size();
}

Tissue& Tissue::operator=(Tissue const& t)
{
  Tissue& r = *this;
  CellList.clear();
  for (list<Cell>::const_iterator it=t.CellList.begin(); it!=t.CellList.end() ; ++it)
    CellList.push_back(Cell(r,*it));

  nodeList = t.getNodeList();
  cellNumber = t.getCellNumber();
  nodeNumber = t.getNodeNumber();
  cellRadius = t.getCellRadius();
  vertexMin = t.getVertexMin();
  timeUnit = 0 ;

  return *this;
}

list<Cell> Tissue::getCellList() const
{
    return CellList;
}

vector<Node> Tissue::getNodeList() const
{
    return nodeList;
}

void Tissue::mooveNodePosition(vector<double> U)
{
  for (int i = 0 ; i < nodeNumber ; i++)
  {
    double x = nodeList[i].getx() ;
    nodeList[i].setx(x + U[2*i]) ;

    double y = nodeList[i].gety() ;
    nodeList[i].sety(y + U[2*i+1]) ;
  }
  incrementTime(); // on incrémente le temps chaque fois qu'on bouge un noeud !!
}

int Tissue::getNodeNumber() const
{
  return nodeNumber ;
}

int Tissue::getCellNumber() const
{
  return cellNumber ;
}

// utilitaire pour constructeur :
void Tissue::creerPremiereRangeeDeCellules(int length, int height, double cellRad)
{
    CellList.clear();
    // ajout de la première cellule :
    Node centre(2*cellRadius, 2*cellRadius); // warning : ceci n'est pas une ref !
    //Node& centre(centr);
    CellList.push_back(Cell(*this, centre, cellRadius));
    // création de la première rangée :
    for (int i=0 ; i<length-1 ; ++i)
        CellList.push_back(Cell(*this, CellList.back()[1],1));
    if (height > 1)
    {
        // ajout de la première cellule de la seconde rangée
        CellList.push_back(Cell(*this, CellList.back()[3],3));
        for (int i=0 ; i<length-2 ; ++i) // on complète la rangée
            CellList.push_back(Cell(*this, CellList.back()[4],4));
    }
}

void Tissue::dupliquerLaRangeePrecedante(int length, double cellRad)
{
    // ajout de la première cellule de cette nouvelle rangée :
    CellList.push_back(Cell(*this, CellList.back()[3],3));
    // création de la première demi-rangée :
    for (int i=0 ; i<length-1 ; ++i)
        CellList.push_back(Cell(*this, CellList.back()[1],1));

    // ajout de la première cellule de la seconde demi-rangée
    CellList.push_back(Cell(*this, CellList.back()[3],3));
    for (int i=0 ; i<length-2 ; ++i) // on complète la rangée
        CellList.push_back(Cell(*this, CellList.back()[4],4));
}

void Tissue::ajouterLaDerniereDemiRangee(int length, double cellRad)
{
    // ajout de la première cellule de cette nouvelle rangée :
    CellList.push_back(Cell(*this, CellList.back()[3],3));
    // suite et fin de la dernière demi-rangée :
    for (int i=0 ; i<length-1 ; ++i)
        CellList.push_back(Cell(*this, CellList.back()[1],1));
}

unsigned int Tissue::addNode(Node n)
{
    unsigned int i = 0;
    while(i<nodeList.size() && Node::distance(n, nodeList[i]) >= vertexMin)
      ++i;

    if(i==nodeList.size())
      nodeList.push_back(n);

    return i;
}

unsigned int Tissue::addThirdNode(Node a, Node b)
{
    Node tmp(a,b);
    return addNode(tmp);
}

void Tissue::setCoordNode(unsigned int nodeIndex, double x, double y)
{
  nodeList[nodeIndex].setx(x);
  nodeList[nodeIndex].sety(y);
}

// calcul :

Matrix Tissue::calculK() const
{
  int num_cell=0 ;
  Matrix K (nodeNumber*2, nodeNumber*2);
  for (list<Cell>::const_iterator it = CellList.begin(); it!=CellList.end() ; it++) // parcours des cellules
  {
    for (int num_elem = 0 ; num_elem < it->getElementNumber() ; num_elem++) // parcours des élements
    {
      Matrix Ke = it->iemeElement(num_elem).calculKe() ; // calcul de la matrice de rigidité de l'élément courant
      for (int ik = 0; ik < 3; ik++) // double parcours --
        for (int jk = 0; jk < 3; jk++) // -- des éléments
          for (int i = 0; i < 2; i++) //
            for (int j = 0; j < 2; j++) //
            {
              // unsigned int num_current_node = it->iemeElement(num_elem).getNodeIndex(ik) ;
              K(2 * it->iemeElement(num_elem).getNodeIndex(ik) + i,
                2 * it->iemeElement(num_elem).getNodeIndex(jk) + j) += Ke(2 * ik + i, 2 * jk + j);
              //qDebug() << "K" << K(2 * it->iemeElement(num_elem).getNodeIndex(ik) + i, 2 * it->iemeElement(num_elem).getNodeIndex(jk) + j);
            }
    }
    num_cell++;
  }
  return K;
}

Matrix Tissue::calculKConstArea() const
{
  int num_cell=0 ;
  Matrix Ka (nodeNumber*2 + cellNumber, nodeNumber*2 + cellNumber);
  for (list<Cell>::const_iterator it = CellList.begin(); it!=CellList.end() ; it++) // parcours des cellules
  {
    for (int num_elem = 0 ; num_elem < it->getElementNumber() ; num_elem++) // parcours des élements
    {
      Matrix Ke = it->iemeElement(num_elem).calculKe() ; // calcul de la matrice de rigidité de l'élément courant
      for (int ik = 0; ik < 3; ik++) // double parcours --
        for (int jk = 0; jk < 3; jk++) // -- des éléments
          for (int i = 0; i < 2; i++) //
            for (int j = 0; j < 2; j++) //
            {
              Ka(2 * it->iemeElement(num_elem).getNodeIndex(ik) + i,
                2 * it->iemeElement(num_elem).getNodeIndex(jk) + j) += Ke(2 * ik + i, 2 * jk + j);
            }

      // contraintes sur les noeuds de la membrane :
      Ka (2 * it->iemeElement(num_elem).getNodeIndex(1)   , 2*nodeNumber + num_cell) += it->xcoeffIemeNodeForAreaConstraint(num_elem);
      Ka (2 * it->iemeElement(num_elem).getNodeIndex(1) +1, 2*nodeNumber + num_cell) += it->ycoeffIemeNodeForAreaConstraint(num_elem);

      Ka (2*nodeNumber + num_cell, 2 * it->iemeElement(num_elem).getNodeIndex(1)) += it->xcoeffIemeNodeForAreaConstraint(num_elem);
      Ka (2*nodeNumber + num_cell, 2 * it->iemeElement(num_elem).getNodeIndex(1) +1) += it->ycoeffIemeNodeForAreaConstraint(num_elem);

    }
    num_cell++;
  }
  return Ka;
}

list<int> Tissue::CLRight() const
{
  Cell c = CellList.front(); // première cellule
  double xright = c.iemeElement(1).getNode(1).getx(); // x du noeud le plus à droite
  //double xmax = c.iemeElement(1).getNode(1).getx();
  for (list<Cell>::const_iterator it=CellList.begin(); it!=CellList.end() && it->iemeElement(1).getNode(1).getx() >= xright ; ++it)// pour chaque cellule
    xright = it->iemeElement(1).getNode(1).getx() ; // mise à jour (incrémentale) de xright

  list<int> liste;

  for (list<Cell>::const_iterator it=CellList.begin() ; it!=CellList.end() ; ++it) // pour chaque cellule
    if (double x = it->iemeElement(1).getNode(1).getx() > xright - cellRadius) //pour chaque élément de gauche
    {
      liste.push_back(it->iemeElement(1).getNodeIndex(1) *2);
      liste.push_back(it->iemeElement(1).getNodeIndex(2) *2);
    }

  return liste ; // debug comment : liste == CLRight == [14,15,16,18,33,34] à 8 cellules
}

list<int> Tissue::CLLeft() const
{
  list<int> liste;
  Cell c = CellList.front();
  double xleft = c.iemeElement(4).getNode(1).getx(); // x du noeud le plus à gauche

  for (list<Cell>::const_iterator it=CellList.begin() ; it!=CellList.end() ; ++it) // pour chaque cellule
  {
    int variableDajustement = it->getElementNumber() - 6; // représente le nombre d'éléments gagnés par la cellule
    if (double x = it->iemeElement(4 + variableDajustement).getNode(2).getx() < xleft + cellRadius) //pour chaque élément de gauche
    {
      liste.push_back(it->iemeElement(4).getNodeIndex(1) *2);
      liste.push_back(it->iemeElement(4).getNodeIndex(2) *2);
    }
  }

  return liste ; // debug comment : liste == CLRight == [5,6,23,4,27,28] à 8 cellules

}

vector<double> Tissue::GaussSolving(int n, Matrix K, vector<double> s) const // pb assign
{
  double temp;
  int i,j, k;

  Matrix a(n, n + 1);
  vector<double> x;
  x.resize(n);

  for ( i = 0; i < n; i++)
  {
      for ( j = 0; j < n; j++)
          a(i, j) = K(i, j);
      if(i<s.size()) a(i, n) = s[i];
  }

  for (i = 0; i < n; i++)                    //Pivotisation
      for (k = i + 1; k < n; k++)
          if ( a(i,i) < a(k,i) )
              for (j = 0; j <= n; j++)
              {
                  temp = a(i,j);
                  a(i,j) = a(k,j);
                  a(k,j) = temp;
              }


  for (i = 0; i < n - 1; i++)            //loop to perform the gauss elimination
      for (k = i + 1; k < n; k++)
      {
        //qDebug() << "a(" <<i<<","<<i<< ") : " << a(i,i);
        //double t = a(k,i) / a(i,i);
        double t = a(k,i) / ((a(i,i)!=0)?a(i,i):1);
        for (j = 0; j <= n; j++)
          a(k,j) = a(k,j) - t * a(i,j);    //make the elements below the pivot elements equal to zero or eliminate the variables
      }


  for (i = n - 1; i >= 0; i--)                //back-substitution
  {                        //x is an array whose values correspond to the values of x,y,z..
      x[i] = a(i,n);                //make the variable to be calculated equal to the rhs of the last equation
      //qDebug() << "x(" << i << "):" << x[i];
      for (j = 0; j < n; j++)
          if (j != i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
              x[i] = x[i] - a(i,j) * x[j];
      //x[i] = x[i] / a(i,i);            //now finally divide the rhs by the coefficient of the variable to be calculated
      x[i] = x[i] / ((a(i,i)!=0)?a(i,i):1);
  }


  return x;
}


// interleaving :

bool Tissue::isInterleavable(Element& elm) const
{
  unsigned int index1 = elm.getNodeIndex(1);
  unsigned int i1=0;
  unsigned int index2 = elm.getNodeIndex(2);
  unsigned int i2=0;
  list<unsigned int> liste;

  for (list<Cell>::const_iterator it=CellList.begin() ; it!=CellList.end() ; ++it)// pour chaque cell
  {
    liste = it->getMembraneNodeIndexList();
    /*if (liste.size()<5) //si la cellule contient trop peu d'arêtes
      return false;*/ // pas assez satisfaisant...
    for(list<unsigned int>::const_iterator it=liste.begin() ; it!=liste.end() ; ++it)
    {
      if (*it == index1) // compte le nombre de cellules qui contiennent le noeud1 de l'arête
        ++i1;
      else if (*it == index2) // idem pour le noeud2
        ++i2;
    }
  }

  return (i1 == 3 && i2 == 3);
}

bool Tissue::interleavingCondition(Element& elm) const
{
  return (elm.getLength() < vertexMin && isInterleavable(elm));
}

vector<Cell*> Tissue::findInterleavingCells(Element& elm)
{
  vector<Cell*> v;
  v.resize(4);
  unsigned int index0 = elm.getNodeIndex(0);
  bool i0 = false;
  unsigned int index1 = elm.getNodeIndex(1);
  bool i1 = false;
  unsigned int index2 = elm.getNodeIndex(2);
  bool i2 = false;
  list<unsigned int> liste;

  for (list<Cell>::iterator it=CellList.begin() ; it!=CellList.end() ; ++it)
  {

    liste = it->getMembraneNodeIndexList();
    for(list<unsigned int>::const_iterator itNode=liste.begin() ; itNode!=liste.end() ; ++itNode)
    {
      if (*itNode == index1)
        i1 = true;
      else if (*itNode == index2)
        i2 = true;

      if (it->iemeElement(0).getNodeIndex(0) == index0)
        i0=true;
    }
    if (i1 || i2) //si la cellule contient l'un des deux points de l'arête au moins
    {
      if (i1 && i2) //si elle contient les deux
      {
        if (i0) //si elle contient l'élément entier
          v[0]=&*it;
        else // si elle contient l'arête mais pas l'élément
          v[2]=&*it;
      }
      else if (i1)
        v[1]=&*it;
      else
        v[3]=&*it;
    }

    i1=false;
    i2=false;
    i0=false;
  }

  return v;
}

vector<Element*> Tissue::elemVector(Element& elm)
{
  vector<Element*> elem;

  vector<Cell*> cell = findInterleavingCells(elm);

  elem.push_back(&(cell[0]->nextElement(elm))); // ajout elem0
  elem.push_back(&elm); // ajout elem1
  elem.push_back(&(cell[0]->previousElement(elm))); // ajout elem2

  elem.push_back( & (cell[1]->findAdjoiningElement(*(elem.back()))) ); // ajout elem3
  elem.push_back(&(cell[1]->previousElement(*(elem.back())))); //ajout elem4

  elem.push_back( & (cell[2]->findAdjoiningElement(*(elem.back()))) ); // ajout elem5
  elem.push_back(&(cell[2]->previousElement(*(elem.back())))); //ajout elem6
  elem.push_back(&(cell[2]->previousElement(*(elem.back())))); //ajout elem7

  elem.push_back( & (cell[3]->findAdjoiningElement(*(elem.back()))) ); // ajout elem8
  elem.push_back(&(cell[3]->previousElement(*(elem.back())))); //ajout elem9

  return elem;
}

unsigned int Tissue::interleave(Element& elm)
{

  if (!interleavingCondition(elm))
      return 0;

  vector<Cell*> cell = findInterleavingCells(elm); // contient [cell0, cell1, cell2, cell3]
  vector<Element*> elem = elemVector(elm); // contient [elem0..elem9]

  elem[1] = cell[0]->giveElement(*elem[1], *cell[1], *elem[4]); // cell0 donne elem1 à la cell1, entre elem3 et elem4
  elem[6] = cell[2]->giveElement(*elem[6], *cell[3], *elem[9]); // cell2 donne elem6 à la cell3, entre elem8 et elem9

  elem[1]->setNodeIndex(0, elem[4]->getNodeIndex(0)); // mets le noeud central de elem1 au centre de cell1
  elem[6]->setNodeIndex(0, elem[9]->getNodeIndex(0)); // mets le noeud central de elem6 au centre de cell3

  // un peu de géométrie pour faire tourner l'arête centrale de Pi/2 et lui donner la bonne longueur l :
  double l = 1.1 * vertexMin; // longueur (magique) de l'arête centrale après intercalation
  double d = elem[1]->getLength(); // longueur de l'arête centrale avant intercalation
  double ratio = l/d;
  double x1, y1, x2, y2; // anciennes coordonnées des node1 et node2 de elm
  x1 = elem[1]->getNode(1).getx();
  y1 = elem[1]->getNode(1).gety();
  x2 = elem[1]->getNode(2).getx();
  y2 = elem[1]->getNode(2).gety();
  double X1, Y1, X2, Y2; // coordonnées des node1 et node2 de elm à calculer
  double xc, yc; // coordonnées de l'arête centrale
  xc = (x1+x2)/2;
  yc = (y1+y2)/2;

  // 1) effectuer la rotation de Pi/2 autour du centre :
  double theta = M_PI/2 ;
  X1 = cos(theta)*(x1-xc) - sin(theta)*(y1-yc) + xc;
  Y1 = sin(theta)*(x1-xc) + cos(theta)*(y1-yc) + yc;
  X2 = cos(theta)*(x2-xc) - sin(theta)*(y2-yc) + xc;
  Y2 = sin(theta)*(x2-xc) + cos(theta)*(y2-yc) + yc;

  // 2) mettre le segment à la bonne longueur :
  X1 = xc + (ratio)*(X1-xc);
  Y1 = yc + (ratio)*(Y1-yc);
  X2 = xc + (ratio)*(X2-xc);
  Y2 = yc + (ratio)*(Y2-yc);

  // appliquer les modifications de coordonnées :
  nodeList[elem[1]->getNodeIndex(1)].setx(X1);
  nodeList[elem[1]->getNodeIndex(1)].sety(Y1);
  nodeList[elem[1]->getNodeIndex(2)].setx(X2);
  nodeList[elem[1]->getNodeIndex(2)].sety(Y2);

  // maintenant, on effectue les raccords des éléments séparés durant le déplacement avec les bons noeuds (voisins) :
  elem[2]->setNodeIndex(2, elem[0]->getNodeIndex(1));
  elem[3]->setNodeIndex(1, elem[0]->getNodeIndex(1));
  elem[7]->setNodeIndex(2, elem[5]->getNodeIndex(1));
  elem[8]->setNodeIndex(1, elem[5]->getNodeIndex(1));

  return 1;
}

unsigned int Tissue::interleave()
{
  unsigned int ret(0);
  for (list<Cell>::iterator it=CellList.begin() ; it!=CellList.end() ; ++it)
    for (unsigned int i=0 ; i<it->getElementNumber() ; ++i)
    {
      Element elm = it->iemeElement(i);
      ret += interleave(elm);
    }

  return ret;
}

void Tissue::interleaveTest()
{
  int i(0), num(cellNumber/2);
  list<Cell>::iterator it=CellList.begin();
  for ( ; it!=CellList.end() && i<num ; ++i, ++it)
  {}
  Element elm = it->iemeElement(1);
  if (interleavingCondition(elm))
    interleave(elm);
}

// contraction :
void Tissue::deleteAreaConstraint(unsigned int cellId, Matrix& Karea) // effet de bord sur Karea // deprecated
{
  if (cellId > cellNumber)
    assert(false);

  for (unsigned int i=0 ; i<Karea.getSize() ; ++i)
  {
    Karea(i, 2*nodeNumber + cellId) = 0;
    Karea(2*nodeNumber + cellId, i) = 0;
  }
  Karea(2*nodeNumber + cellId, 2*nodeNumber + cellId) = 1; // pour ne pas faire planter le pivot de Gauss
}

void Tissue::contractCell(unsigned int cellId, vector<double>& Farea, double coef) // effet de bord sur Farea
{
  list<Cell>::iterator it=CellList.begin();
  advance(it,cellId);
  qDebug() << it->getArea();
  Farea[2*nodeNumber + cellId] = it->getArea()*coef;
}

void Tissue::contractCell(unsigned int cellId, vector<double>& Farea) // effet de bord sur Farea
{
  int nb_step = 10;
  double proba = 0.2;
  list<Cell>::iterator it=CellList.begin();
  advance(it,cellId);
  double coef = 1./100;
  if ((it->getContractionBegin())==0)
  {
    double random = (static_cast<double>(rand() % 1000))/1000.;
    if (random < proba)
      it->initializeContraction(timeUnit + 1);
  }
  else if (timeUnit - it->getContractionBegin() < nb_step) // beware
    Farea[2*nodeNumber + cellId] = it->getArea()*-coef; // contract cell
  else
    Farea[2*nodeNumber + cellId] = it->getArea()*coef; // decontract cell

  if (timeUnit - it->getContractionBegin() >= 2*nb_step-1)
    it->initializeContraction(0);
}

void Tissue::contractCell(vector<double>& Farea, double proba) // effet de bord sur Farea
{
  int nb_step = 10;
  double coef = 1./100;
  int cellId = 0;
  for (list<Cell>::iterator it=CellList.begin(); it!=CellList.end() ; cellId++, ++it)
  {
    if ((it->getContractionBegin())==0)
    {
      double random = (static_cast<double>(rand() % 1000))/1000.;
      if (random < proba)
        it->initializeContraction(timeUnit + 1);
    }
    else if (timeUnit - it->getContractionBegin() < nb_step)
      Farea[2*nodeNumber + cellId] = it->getArea()*-coef; // contract cell
    else
      Farea[2*nodeNumber + cellId] = it->getArea()*coef; // decontract cell

    if (timeUnit - it->getContractionBegin() >= 2*nb_step-1)
      it->initializeContraction(0);
  }
}

void Tissue::contractCell(vector<double>& Farea, double proba, double gradientPow) // effet de bord sur Farea
{
  int nb_step = 10;
  double coef = 1./100*5;
  int cellId = 0;
  double p; // stock la probabilité pour calcul

  // recherche x des bordures gauche et droite :
  double xmin = nodeList[0].getx();
  double xmax = nodeList[0].getx();
  for (int i=0 ; i<nodeNumber ; ++i)
  {
    if (xmin > nodeList[i].getx())
      xmin = nodeList[i].getx();
    if (xmax < nodeList[i].getx())
      xmax = nodeList[i].getx();
  }

  for (list<Cell>::iterator it=CellList.begin(); it!=CellList.end() ; cellId++, ++it)
  {
    if ((it->getContractionBegin())==0)
    {
      double random = (static_cast<double>(rand() % 1000))/1000.;
      //qDebug() << proba;
      p = proba * (pow(cellRadius / (it->iemeElement(0).getNode(0).getx() - xmin) , gradientPow)
                  +pow(cellRadius / (xmax - it->iemeElement(0).getNode(0).getx()) , gradientPow)); // pb avec cette partie là :-/
      //qDebug() << p;
      if (random < p)
        it->initializeContraction(timeUnit + 1);
    }
    else if (timeUnit - it->getContractionBegin() < nb_step)
      Farea[2*nodeNumber + cellId] = it->getArea()*-coef; // contract cell
    else
      Farea[2*nodeNumber + cellId] = it->getArea()*coef; // decontract cell

    if (timeUnit - it->getContractionBegin() >= 2*nb_step-1)
      it->initializeContraction(0);
  }
}
