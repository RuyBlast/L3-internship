#include <vector>
#include <cassert>
using namespace std;
#include "Matrix.h"
#include <QDebug>
#include <algorithm> // pour min

/*
// attributs
private :
  int nbLines; // i
  int nbColumns; // j
  std::vector< std::vector<double> > matrix;
*/
// methodes

double Matrix::getCoef(int i, int j) const
{ return matrix [i] [j]; }

void Matrix::setCoef(int i, int j, double coef)
{ matrix [i] [j] = coef; }

double& Matrix::operator () (int i, int j) { return matrix[i][j]; }

void Matrix::debug() const
{
  QVector<double> q(nbColumns);
  for (int i = 0 ; i < nbLines ; i++)
  {
    for (int j = 0 ; j < nbColumns ; j++)
      q[j] = getCoef(i,j);
    qDebug() << q ;
  }
}

bool Matrix::hasNonZeroDiagonal() const
{
  int max = min(nbLines, nbColumns);
  for (int i = 0 ; i < max ; i++)
    if (getCoef(i,i) == 0)
      return false ;
  return true ;
}

unsigned int Matrix::getSize()
{
  if (nbLines!=nbColumns)
    assert(false);
  return nbLines;
}
