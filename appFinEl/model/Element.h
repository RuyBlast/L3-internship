#ifndef ELEMENT_H
#define ELEMENT_H

#include <list>
#include <vector>
#include "Node.h"
#include "Matrix.h"

class Cell;
class Tissue;

class Element
{
    // attributs
  protected :
    unsigned int node_index[3]; // tableau d'index des noeuds dans l'ordre du tissue
    Tissue& tissue;

    // methodes
  public :

    // Constructors & destructor
    Element(Tissue& t);

    Element(Tissue& t, Node node0, Node node1); // va calculer le node2

    Element(Tissue& t, Node node0, Node node1, Node node2);

    Element(const Element &elt) : tissue(elt.getTissue()) { node_index[0] = elt.getNodeIndex(0);
                                                            node_index[1] = elt.getNodeIndex(1);
                                                            node_index[2] = elt.getNodeIndex(2); }  // constructeur par copie

    Element(Tissue& t, const Element &elt) : tissue(t) { node_index[0] = elt.getNodeIndex(0);
                                                         node_index[1] = elt.getNodeIndex(1);
                                                         node_index[2] = elt.getNodeIndex(2); }  // constructeur par copie

    ~Element() {}

    // Getters
    Node getNode(int i) const;
    unsigned int getNodeIndex(int i) const { return node_index[i%3]; }
    Tissue & getTissue() const { return tissue; }
    double getLength() const; // renvoie la longueur de la portion de membrane de l'élément

    // Others
    Cell belongsToCell() const;
    Matrix calculKe() const;
    bool operator==(Element const& elem) const;
    Element& operator=(Element const& e);
    void setNodeIndex(unsigned int i, unsigned int newNodeIndex);
    // std::vector< std::vector<double> > calculK() const;
    // ou void calculK(double* *)
    //const double computeThirdNodex(Node a, Node b);
};

#endif // ELEMENT_H
