#include "mainwindow.h"
#include <QApplication>
#include "view/KeyPoint.h"
#include "view/KeyLine.h"
#include "view/KeyCell.h"
#include "model/Tissue.h"
#include "view/KeyTissue.h"

#include <unistd.h> // for sleep()

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  MainWindow w;

  QGraphicsScene& G(w.getscene());

  return a.exec();
}
