/******************************************************************************
  \file display.h
  \author Yann Thoma
  \date 05.05.2011

  Fichier faisant partie du labo toboggan.
  ****************************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <pcosynchro/pcosemaphore.h>


class KidItem :  public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    KidItem();

    unsigned int arrivalStep{0};
    PcoSemaphore sem;


};

KidItem *getKid(unsigned int kidId);


class TobogganDisplay : public QGraphicsView
{
    Q_OBJECT
public:
    TobogganDisplay(unsigned int nbStep, unsigned int nbKids, QWidget *parent=0);
    ~TobogganDisplay();
    unsigned int m_nbStep;
    unsigned int m_nbPlaces;
    std::vector<QPointF> m_stepPos;
    KidItem *getKid(unsigned int kidId);
private:
    QGraphicsScene *m_scene;
    QList<KidItem *> m_kids;


public slots:
    void setKid(unsigned int step, unsigned int kidId);
    void travel(unsigned int KidId, unsigned int step2, unsigned int ms);
    void goToPlayground(unsigned int KidId, unsigned int ms);
    void finishedAnimation();
};

#endif // DISPLAY_H
