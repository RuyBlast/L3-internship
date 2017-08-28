#ifndef DEF_CELL
#define DEF_CELL

#include <list>
#include "Element.h"
#include "Node.h"

class Tissue; // nécessaire

class Cell
{
  protected :
    Tissue& tissue;
    std::list<Element> elementList;
    int contractionBegin; // date de début de contraction (en cours et précédante)
    void removeElement(Element & elem); // supprime l'élément elem de la cellule

    // methodes
  public :

    // Constructors & destructor
    Cell(Tissue& t) : tissue(t) {}
    Cell(Tissue& t, std::list<Element> elmList): tissue(t), elementList(elmList), contractionBegin(0) {}
    Cell(Tissue& t, Node & centre, double rayon);  // noeud du centre : pratique pour pavage hexagonal, le rayon est la distance du centre à un noeud
    Cell(Tissue& t, Element voisin); // constructeur par voisinage
    Cell(Tissue& t, Element voisin, int ivoisin); // constructeur par voisinage qui conserve l'orientation de l'indice i et recalibre les coordonnées
    Cell(Tissue& t, std::list<Node> nodeList); // liste des noeuds de la membrane
    Cell(Tissue& t, Cell const& cell); // constructeur par copie
    ~Cell() {}

    // opérateurs surchargés :
    Cell& operator=(Cell const& c);

    // Getters
    std::list<Node> getMembraneNodeList() const;
    std::list<unsigned int> getMembraneNodeIndexList() const;
    Element iemeElement(int i) const;
    double xcoeffIemeNodeForAreaConstraint(int i) const;
    double ycoeffIemeNodeForAreaConstraint(int i) const;
    Element operator [] (int i) const;
    Node & getCenter() const;
    double getPerimeter() const;
    double getArea() const;
    unsigned int getElementNumber() const;
    int getContractionBegin() const {return contractionBegin;}

    // Setter
    void initializeContraction(int t) {contractionBegin = t;}
    //void setTissue(Tissue& t) {tissue = t;}

    // opérations sur les éléments
    Element & nextElement(Element const& elem) const; // renvoie l'élément suivant (sens trigo, repère de déterminant positif)
    Element & previousElement(Element const& elem) const; // renvoie l'élément précédant (sens trigo, repère de déterminant positif)
    Element & findAdjoiningElement(Element const& elem) const; // renvoie l'élément adjacent à l'élément en paramètre
    Element *addElement(Element elem, Element &after); // ajoute l'élément elem après l'élément after
    Element *giveElement(Element & elmToGive, Cell & toCell, Element & after); // donne l'élément elm (de la cellule pointée par this) à la cellule cell

    // utilitaire pour génération tissu :
    void recalibrate(Cell & c); // repositionne les noeuds pour éliminer les erreurs de calculs dans la génération
};

#endif
