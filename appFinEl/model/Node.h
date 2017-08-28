#ifndef NODE_H
#define NODE_H

#include <list>

class Cell;
class Element;

class Node
{
    // attributs
  protected :
    double x; // abscisse
    double y; // ordonnée

    // methodes
  public :

    Node() : x(0), y(0) {}
    Node(double xn, double yn)  : x(xn), y(yn) {}
    Node(const Node &n) : x(n.getx()), y(n.gety()) {}  // constructeur par copie
    Node(Node a, Node b); // construit le troisième noeud à partir de a et b
    ~Node() { /*x = 0; y = 0;*/ } // ???

    double getx() const { return x; }
    double gety() const { return y; }
    void setx(double newx) { x=newx; }
    void sety(double newy) { y=newy; }

    std::list<Cell> belongsToCells() const;
    std::list<Element> belongsToElements() const;
    static double distance(Node& a, Node& b); // calcule la distance entre 2 noeuds // warning !!
    //static void draw(Node a, Node b);
    static std::pair<double, double> barycentre(std::list<Node> nodeList);
    static std::pair<double, double> barycentre(Node a, Node b);
    bool operator==(Node const& node) const;
};

#endif // NODE_H
