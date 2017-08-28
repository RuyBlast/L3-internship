#ifndef KEYCELL_H
#define KEYCELL_H

#include "model/Cell.h"
#include <QGraphicsScene>

class KeyCell
{
  private :
    Cell cell;

  public :
    KeyCell(Cell c);
    ~KeyCell();
    void draw(QGraphicsScene& scene) const;

};

#endif // KEYCELL_H
