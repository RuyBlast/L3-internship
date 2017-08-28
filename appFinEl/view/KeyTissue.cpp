#include "view/KeyTissue.h"
#include <list>

#include <vector>
#include "view/KeyPoint.h"
using namespace std;

void KeyTissue::draw(QGraphicsScene& scene) const
{
  list<Cell> cellList(tissue.getCellList());
  for(list<Cell>::iterator it = cellList.begin(); it!=cellList.end(); ++it)
  {
    KeyCell(*it).draw(scene);
  }
}

void KeyTissue::drawbug(QGraphicsScene& scene) const
{
  vector<Node> nodeVector(tissue.getNodeList());
  for(vector<Node>::iterator it = nodeVector.begin(); it!=nodeVector.end(); ++it)
  {
    KeyPoint(*it).draw(scene);
  }
}
