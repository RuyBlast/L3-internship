#ifndef TISSUE_H
#define TISSUE_H

#include <list>
#include <string> // pour passer le nom de la méthode de génération du tissue
#include <vector>
#include "Node.h"
#include "Cell.h"
#include "Element.h"
#include "Matrix.h"

class Tissue
{
  // attributs
  protected :
    std::list<Cell> CellList; // liste des cellules
    std::vector<Node> nodeList; // contient les noeuds
    int cellNumber; // nombre de cellules -1
    int nodeNumber; // nombre de noeuds -1
    double cellRadius;
    double vertexMin; // distance minimale admissible entre chaque noeud (fusion, intercalations)
    int timeUnit; // unité unitaire de temps

  // methodes
  public :

    // Constructors & destructors
    Tissue() : CellList(std::list<Cell>()), cellNumber(0), nodeNumber(0), cellRadius(10), vertexMin(0), timeUnit(0) {}
    Tissue(int length, int height, double cellRad);
    Tissue(const Tissue &t) : CellList(t.getCellList()), nodeList(t.getNodeList()), cellNumber(t.getCellNumber()), nodeNumber(t.getNodeNumber()),
                              cellRadius(t.getCellRadius()), vertexMin(t.getVertexMin()), timeUnit(0) {} // constructeur par copie
    ~Tissue() {}

    std::list<Cell> getCellList() const;
    std::vector<Node> getNodeList() const;

    void mooveNodePosition(std::vector<double> U);
    int getNodeNumber() const;
    int getCellNumber() const;
    double getCellRadius() const {return cellRadius;}
    const double getVertexMin() const {return vertexMin;}

    // opérateurs surchargés :
    Tissue& operator=(Tissue const& t);

    // getters
    Node getNode(int i) const { return nodeList[i]; }
    Cell getCell(int i) const { std::list<Cell>::const_iterator it = CellList.begin(); std::advance(it, i); return *it; }

    //setter
    void incrementTime() {timeUnit++;}

    // utilitaire pour constructeur :
    void creerPremiereRangeeDeCellules (int length, int height, double cellRad);
    void dupliquerLaRangeePrecedante(int length, double cellRad);
    void ajouterLaDerniereDemiRangee(int length, double cellRad);
    // rq : une rangée fait deux cellules d'épaisseur

    unsigned int addNode(Node n); // ajoute un noeud à la liste des noeuds s'il n'existe pas déjà et renvoie une référence vers ce noeud (nouvellement créé ou non)
    unsigned int addThirdNode(Node a, Node b);
    void setCoordNode(unsigned int nodeIndex, double x, double y); // change les coordonnées d'un noeud indexé par nodeIndex dans le tissu

    // Calcul
    Matrix calculK() const;
    Matrix calculKConstArea() const; // renvoie la matrice du systême sous contrainte d'Aire dA = 0
    std::list<int> CLRight() const; // conditions aux limites
    std::list<int> CLLeft() const;  // idem
    std::vector<double> GaussSolving(int taille_s_et_matrice, Matrix K, std::vector<double> U) const;

    // interleaving :
    bool isInterleavable(Element& elm) const; // renvoie true si l'arête extérieure de l'élément est entourée de 4 cellules ayant au moins 5 éléments, false sinon
    bool interleavingCondition(Element& elm) const; // renvoie true si isInterleavable renvoie true et que la condition d'intercalation est remplie
    std::vector<Cell*> findInterleavingCells(Element& elm); // renvoie un vector de pointeurs vers les trois autres cellules impliquées dans l'intercalation, dans l'ordre trigo
    std::vector<Element*> elemVector(Element& elm); // renvoie un vecteur de pointeurs vers les éléments dans "le bon ordre"
    unsigned int interleave(Element& elm); // réalise les intercalations et renvoie le nombre d'intercalations
    unsigned int interleave(); // réalise les intercalations possibles en parcourant les cellules dans l'ordre de la cellList, et renvoie le nombre d'intercalations réalisées
    void interleaveTest();

    // contraction :
    void deleteAreaConstraint(unsigned int cellId, Matrix& Karea); // annule la contrainte d'aire pour la cellule cellId dans la matrice du système
    void contractCell(unsigned int cellId, std::vector<double>& Farea); // contracte la cellule cellId dans le vecteur F du système
    void contractCell(unsigned int cellId, std::vector<double>& Farea, double coef); // coef négatif => contraction
    void contractCell(std::vector<double>& Farea, double proba); // contracte chaque cellule avec une probabilité proba
    void contractCell(std::vector<double>& Farea, double proba, double gradientPow); // prends en compte le gradient
};

#endif // TISSUE_H
