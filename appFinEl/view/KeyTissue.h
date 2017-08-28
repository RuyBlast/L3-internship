#ifndef KEYTISSUE_H
#define KEYTISSUE_H

#include "model/Tissue.h"
#include <QGraphicsScene>
#include "view/KeyCell.h"

class KeyTissue
{
  private :
    Tissue & tissue;

  public :
    KeyTissue(Tissue & t) : tissue(t) {}
    KeyTissue(const KeyTissue & kt) : tissue(kt.getTissue()) {} // constructeur par copie
    ~KeyTissue() {}

    // opérateurs surchargés :
    KeyTissue operator=(KeyTissue const& kt) const {return KeyTissue(kt);}

    Tissue& getTissue() const {return tissue;}

    void draw(QGraphicsScene& scene) const;

    void drawbug(QGraphicsScene& scene) const;

};

#endif // KEYTISSUE_H
