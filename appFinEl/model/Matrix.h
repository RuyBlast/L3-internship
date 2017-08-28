#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

class Matrix
{
  // attributs
  protected :
    int nbLines; // i (-1)
    int nbColumns; // j (-1)
    //double matrix[nbLines * nbColumns]; // doit etre connu à la compilation
    std::vector< std::vector<double> > matrix; // stockage des lignes

  // methodes
  public :

    Matrix() {}
    Matrix(int i, int j) : nbLines(i), nbColumns(j)// i : nb de lignes ; j : nb de colonnes !!!
    {
      // mise à 0 de la matrice
      std::vector<double> v(j);
      for (int ki=0 ; ki<i ; ki++)
      {
        for (int kj=0 ; kj<j ; kj++)
          v[kj] = 0;
        matrix.push_back(v);
      }
    }
    ~Matrix() {}
    double getCoef(int i, int j) const;
    void setCoef(int i, int j, double coef);
    double& operator () (int i, int j);
    unsigned int getSize(); // renvoie la taille d'une matrice carrée (échoue si la matrice n'est pas carrée)

    // debugging fonctions :
    void debug() const;
    bool hasNonZeroDiagonal() const;

};


#endif // MATRIX_H
