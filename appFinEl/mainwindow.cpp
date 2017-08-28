#include <stdio.h>      /* NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <list>
using namespace std;
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "model/utilitarian.h"
#include "view/KeyPoint.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  t(5,3,25.5),
  kt(t),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  scene = new QGraphicsScene(this);
  ui->graphicsView->setScene(scene);

  kt.draw(*scene);

  this->show();
}

MainWindow::~MainWindow()
{
  clear();
  delete ui;
}

void MainWindow::drawLine(double x1, double y1, double x2, double y2)
{
  if(!scene)
  {
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
  }
  scene->addLine((qreal) x1, (qreal) y1, (qreal) x2, (qreal) y2);
  //m_painter->drawLine(x1, y1, x2, y2);
}

void MainWindow::highlightCell(unsigned int cellId)
{
  list<Cell> liste = t.getCellList();
  list<Cell>::iterator it=liste.begin();
  advance(it,cellId % t.getCellList().size());

  //KeyCell kc = KeyCell(*it);

  //kc.draw(*scene);

  for (unsigned int i=0; i<it->getMembraneNodeList().size() ; ++i)
  {
    KeyPoint kp = KeyPoint(it->iemeElement(i).getNode(1));
    kp.draw(*scene);
    kp = KeyPoint(it->iemeElement(i).getNode(0));
    kp.draw(*scene);
    kp = KeyPoint(it->iemeElement(i).getNode(2));
    kp.draw(*scene);
  }
}

void MainWindow::highlightCell(unsigned int cellId, unsigned int iemeElem)
{
  list<Cell> liste = t.getCellList();
  list<Cell>::iterator itc=liste.begin();
  advance(itc,cellId % t.getCellList().size());

  //list<Node>::iterator itn=itc->getMembraneNodeList().begin();
  //advance(itn,iemeElem % itc->getMembraneNodeList().size());

  KeyPoint kp = KeyPoint(itc->iemeElement(iemeElem%itc->getMembraneNodeList().size()).getNode(1));
  kp.draw(*scene);
  kp = KeyPoint(itc->iemeElement(iemeElem).getNode(0));
  kp.draw(*scene);
  //kp = KeyPoint(itc->iemeElement(iemeElem).getNode(2));
  kp.draw(*scene);
}

void MainWindow::clear()
{
  delete scene;
  scene=NULL;
  ui->graphicsView->setScene(scene);
}

QGraphicsScene& MainWindow::getscene()
{
  if(!scene)
  {
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
  }
  QGraphicsScene& sceneref(*scene);
  return sceneref;
}

void MainWindow::generate_tissue()
{
  int nbWidth = ui->spinBox_initialisation_width->value();
  int nbHeight = ui->spinBox_initialisation_height->value();

  t = Tissue(nbWidth,nbHeight,25.5);
  kt = KeyTissue(t);

  srand(time(NULL));

  clear();
  scene = new QGraphicsScene(this);
  ui->graphicsView->setScene(scene);

  kt.draw(*scene);
  this->show();
}

void MainWindow::next_MEF_without_area_constraint()
{
  int nsteps = ui->spinBox->value();
  for(int toto=0; toto < nsteps; ++toto)
  {
    // calcul :
    list<int> left, right; // cellules des conditions aux limites
    left = t.CLLeft();
    right = t.CLRight();

    Matrix K; // matrice de rigidité
    K = t.calculK();

    vector<double> F, U; // F : vecteur force ; U : vecteur déplacement
    F.resize(2 * t.getNodeNumber());
    U.resize(2 * t.getNodeNumber());

    // début conditions aux limites
    for (list<int>::const_iterator it = left.begin() ; it!=left.end() ; it++)
    {
      K(*it, *it) *= 1e20;
      F[*it] = -1 * K(*it, *it);
    }

    for (list<int>::const_iterator it = right.begin() ; it!=right.end() ; it++)
    {
      K(*it, *it) *= 1e20;
      F[*it] = 1 * K(*it, *it);
    }

    int d1 = left.front(); // bloque le degré de liberté en y de du premier noeud dans les CL de gauche
    int d2 = right.front(); // bloque le degré de liberté en y de du premier noeud dans les CL de droite
    for (int i = 0; i < 2 * t.getNodeNumber(); i++)
    {
      K(d1 + 1, i) = 0;
      K(i, d1 + 1) = 0;
      K(d2 + 1, i) = 0;
      K(i, d2 + 1) = 0;
    }
    K(d1 + 1, d1 + 1) = 1;
    K(d2 + 1, d2 + 1) = 1;
    F[d1 + 1] = 0;
    F[d2 + 1] = 0;

    // fin des CL

    U = t.GaussSolving(F.size(), K, F);

    t.mooveNodePosition(U);

    //kt = KeyTissue(t);

    // test intercalation :
    t.interleave();

    clear();
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QVector<double> q(F.size());
    for (int i = 0 ; i < F.size() ; i++)
      q[i] = F[i];

    kt.draw(*scene);
  }
}

void MainWindow::next_MEF() // with area constraint
{
  int nsteps = ui->spinBox->value();
  for(int toto=0; toto < nsteps; ++toto)
  {
    // calcul :
    list<int> left, right; // cellules des conditions aux limites
    left = t.CLLeft();
    right = t.CLRight();

    Matrix K; // matrice de rigidité
    //K = t.calculK();
    K = t.calculKConstArea();

    vector<double> F, U; // F : vecteur force ; U : vecteur déplacement
    //F.resize(2 * t.getNodeNumber());
    //U.resize(2 * t.getNodeNumber());
    F.resize(2 * t.getNodeNumber() + t.getCellNumber());
    U.resize(2 * t.getNodeNumber() + t.getCellNumber());

    // début conditions aux limites
    for (list<int>::const_iterator it = left.begin() ; it!=left.end() ; it++)
    {
      K(*it, *it) *= 1e20;
      F[*it] = -1 * K(*it, *it);
    }

    for (list<int>::const_iterator it = right.begin() ; it!=right.end() ; it++)
    {
      K(*it, *it) *= 1e20;
      F[*it] = 1 * K(*it, *it);
    }

    int d1 = left.front(); // bloque le degré de liberté en y de du premier noeud dans les CL de gauche
    int d2 = right.front(); // bloque le degré de liberté en y de du premier noeud dans les CL de droite
    for (int i = 0; i < 2 * t.getNodeNumber() + t.getCellNumber(); i++)
    {
      K(d1 + 1, i) = 0;
      K(i, d1 + 1) = 0;
      K(d2 + 1, i) = 0;
      K(i, d2 + 1) = 0;
    }
    K(d1 + 1, d1 + 1) = 1;
    K(d2 + 1, d2 + 1) = 1;
    F[d1 + 1] = 0;
    F[d2 + 1] = 0;

    // fin des CL

    //t.deleteAreaConstraint(3,K);
    //t.deleteAreaConstraint(7,K);
    //t.deleteAreaConstraint(12,K);

    //K.debug();

    U = t.GaussSolving(F.size(), K, F);

    t.mooveNodePosition(U);

    //kt = KeyTissue(t);

    // test intercalation :
    t.interleave();
    //t.contractCell(7, F);
    clear();
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QVector<double> q(F.size());
    for (int i = 0 ; i < F.size() ; i++)
      q[i] = F[i];

    kt.draw(*scene);
  }
}

void MainWindow::next_static_MEF_with_contractions()
{
  int nsteps = ui->spinBox_contraction_nb_iterations->value();
  for(int toto=0; toto < nsteps; ++toto)
  {
    // calcul :
    list<int> left, right; // cellules des conditions aux limites
    left = t.CLLeft();
    right = t.CLRight();

    Matrix K; // matrice de rigidité
    K = t.calculKConstArea();

    vector<double> F, U; // F : vecteur force ; U : vecteur déplacement
    F.resize(2 * t.getNodeNumber() + t.getCellNumber());
    U.resize(2 * t.getNodeNumber() + t.getCellNumber());

    // début conditions aux limites
    for (list<int>::const_iterator it = left.begin() ; it!=left.end() ; it++)
    {
      K(*it, *it) *= 1e20;
      F[*it] = 0; // impose un déplacement nul aux bord (pas d'étirement à gauche)
    }
// a supprimer (radio button) pour permettre un allongement
    for (list<int>::const_iterator it = right.begin() ; it!=right.end() ; it++)
    {
      K(*it, *it) *= 1e20;
      F[*it] = 0; // impose un déplacement nul aux bord (pas d'étirement à droite)
    }

    int d1 = left.front(); // bloque le degré de liberté en y de du premier noeud dans les CL de gauche
    int d2 = right.front(); // bloque le degré de liberté en y de du premier noeud dans les CL de droite
    for (int i = 0; i < 2 * t.getNodeNumber() + t.getCellNumber(); i++)
    {
      K(d1 + 1, i) = 0;
      K(i, d1 + 1) = 0;
      K(d2 + 1, i) = 0;
      K(i, d2 + 1) = 0;
    }
    K(d1 + 1, d1 + 1) = 1;
    K(d2 + 1, d2 + 1) = 1;
    F[d1 + 1] = 0;
    F[d2 + 1] = 0;

    // fin des CL
/*
    t.contractCell(0, F);
    t.contractCell(4, F);
    t.contractCell(5, F);
    t.contractCell(8, F);
    t.contractCell(9, F);
    t.contractCell(13, F);*/
    double proba = static_cast<double>(ui->horizontalSlider_proba->value())/100.;
    double exposant_gradient = ui->doubleSpinBox_gradient->value();
    t.contractCell(F, proba, exposant_gradient);


    U = t.GaussSolving(F.size(), K, F);

    t.mooveNodePosition(U);

    //kt = KeyTissue(t);

    // test intercalation :
    t.interleave(); //pb here !!!!!!!!!!!!!!!!!!!!!!!!!

    clear();
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QVector<double> q(F.size());
    for (int i = 0 ; i < F.size() ; i++)
      q[i] = F[i];

/*
    KeyPoint kp0(t.getNode(12));
    KeyPoint kp1(t.getNode(16));
    KeyPoint kp2(t.getNode(33));
    //KeyPoint kp0(t.getNode(45));
    //KeyPoint kp1(t.getNode(11));
    //KeyPoint kp2(t.getNode(35));
    kp0.draw(*scene);
    kp1.draw(*scene);
    kp2.draw(*scene);

    kp0= t.getNode(48);
    kp1= t.getNode(16);
    kp2= t.getNode(32);
    kp0.draw(*scene);
    kp1.draw(*scene);
    kp2.draw(*scene);*/

/*
    int i = 9;
    highlightCell(i);
    highlightCell(2+i);
    highlightCell(4+i);*/
    //this->debugArea();

    kt.draw(*scene);
  }
}

// fonction de debugging :

void MainWindow::debugArea() const
{
  list<Cell> liste = t.getCellList();
  int i = 0;

  for (list<Cell>::const_iterator it = liste.begin() ; it!=liste.end() ; it++, i++)
    qDebug() << "cell area" << i << it->getArea();
  qDebug() << "";
}

void MainWindow::debugVector(vector<double> const& v) const
{
  QVector<double> qv(v.size());
  for (int j = 0 ; j < v.size() ; j++)
    qv[j] = v[j];
  qDebug() << qv ;
}

