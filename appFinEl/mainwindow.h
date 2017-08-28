#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPainter>
#include "model/Tissue.h"
#include "view/KeyTissue.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void drawLine(double x1, double y1, double x2, double y2);
    void highlightCell(unsigned int cellId);
    void highlightCell(unsigned int cellId, unsigned int iemeElem);
    void clear();
    QGraphicsScene& getscene();
    //debug :
    void debugArea() const;
    void debugVector (const std::vector<double> &v) const;

  public slots:
    void generate_tissue();
    void next_MEF_without_area_constraint();
    void next_MEF();
    void next_static_MEF_with_contractions();
    //void next_MEF() { drawLine(0,0,200,200); }

  private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;
    QPainter *m_painter;
    Tissue t;
    KeyTissue kt;
};

#endif // MAINWINDOW_H
