/******************************************************************************
  \file display.cpp
  \author Yann Thoma
  \date 05.05.2011

  Fichier faisant partie du labo toboggan.
  ****************************************************************************/

#include <cmath>


#include <QPaintEvent>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QEventLoop>

#include <pcosynchro/pcomutex.h>

#include "display.h"


constexpr double RADIUS = 250.0;
constexpr double SCENEOFFSET = 0.0;
constexpr double STEPRADIUS = 25.0;
constexpr double KIDWIDTH = 30.0;
constexpr int NBKIDICONS = 30;

static TobogganDisplay* theDisplay;

KidItem::KidItem() = default;

KidItem *getKid(unsigned int id)
{
    return theDisplay->getKid(id);
}

TobogganDisplay::TobogganDisplay(unsigned int nbSteps, unsigned int nbKids, QWidget *parent):
    QGraphicsView(parent)
{
    theDisplay = this;
    m_nbPlaces = nbSteps + 1; // One bonus for the end of the slide
    m_stepPos=std::vector<QPointF>(m_nbPlaces);
    for(unsigned int i=0;i<nbSteps;i++)
    {
        m_stepPos[i]=
                QPointF(SCENEOFFSET+STEPRADIUS*i,
                        SCENEOFFSET-STEPRADIUS*(i+1)-10);
    }
    m_stepPos[nbSteps]=
            QPointF(SCENEOFFSET+STEPRADIUS*nbSteps*2,
                    SCENEOFFSET- STEPRADIUS - 10);
    m_scene=new QGraphicsScene(this);
    this->setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
    this->setMinimumHeight(2*SCENEOFFSET+2*RADIUS+10.0);
    this->setMinimumWidth(2*SCENEOFFSET+2*RADIUS+10.0);
    this->setScene(m_scene);
    m_nbStep=nbSteps;

    QPen pen;
    QBrush brush(QColor(100,255,100));
    QPolygonF path;
    path << QPointF(SCENEOFFSET,SCENEOFFSET);
    path << QPointF(SCENEOFFSET+STEPRADIUS,SCENEOFFSET);
    for(unsigned int i=1;i<nbSteps;i++) {
        path << QPointF(SCENEOFFSET + i*STEPRADIUS,SCENEOFFSET - i*STEPRADIUS);
        path << QPointF(SCENEOFFSET + (i+1)*STEPRADIUS,SCENEOFFSET - i*STEPRADIUS);
    }
    path << QPointF(SCENEOFFSET+STEPRADIUS*nbSteps*2,
                    SCENEOFFSET);
    m_scene->addPolygon(path,pen,brush);


    for (unsigned int i = 0; i < nbKids; i++)
    {
        QPixmap img(QString(":images/32x32/p%1.png").arg(m_kids.size() % NBKIDICONS));
        QPixmap kidPixmap;
        kidPixmap=img.scaledToWidth(KIDWIDTH);
        auto kid=new KidItem();
        kid->setPixmap(kidPixmap);
        m_scene->addItem(kid);
        m_kids.append(kid);
        kid->hide();
    }

}

TobogganDisplay::~TobogganDisplay()
{
    while (!m_kids.empty()) {
        KidItem *p = m_kids.at(0);
        p->sem.release();
        m_kids.removeFirst();
        delete p;
    }
}

void TobogganDisplay::setKid(unsigned int step, unsigned int kidId)
{
    KidItem *kid = getKid(kidId);
    QPointF curPos = m_stepPos[step];
    auto angle = static_cast<double>(rand());
    kid->setPos(curPos.x() + 40.0 * cos(angle), curPos.y() + 40.0 * sin(angle));
    kid->show();
}


KidItem *TobogganDisplay::getKid(unsigned int kidId)
{
    return m_kids.at(kidId);
}


void TobogganDisplay::travel(unsigned int kidId, unsigned int step2, unsigned int ms)
{
    static PcoMutex mutex;
    mutex.lock();


    auto group=new QParallelAnimationGroup(this);

    {
        KidItem *kid=getKid(kidId);
        kid->arrivalStep = step2;
        kid->show();
        auto animation=new QPropertyAnimation(kid, "pos");
        animation->setDuration(ms-10);
        animation->setStartValue(kid->pos());

        if (step2 >= m_stepPos.size()) {
            animation->setEndValue(m_stepPos[0]);
        }
        else {
            animation->setEndValue(m_stepPos[step2]);
        }
        if (kid->arrivalStep == m_nbStep)
            animation->setEasingCurve(QEasingCurve::InCubic);

        group->addAnimation(animation);
    }

    group->start();

    QObject::connect(group, SIGNAL(finished()), this,
                     SLOT(finishedAnimation()));

    mutex.unlock();
}

void TobogganDisplay::goToPlayground(unsigned int kidId, unsigned int ms)
{
    unsigned int step1 = m_nbStep;
    unsigned int step2 = 0;
    static PcoMutex mutex;
    mutex.lock();


    auto group=new QParallelAnimationGroup(this);

    {
        KidItem *kid=getKid(kidId);
        kid->show();
        kid->arrivalStep = 0;

        auto animation = new QPropertyAnimation(kid, "pos");
        animation->setDuration(static_cast<int>(ms)-10);
        if (step1 >= m_stepPos.size()) {
//            throw std::runtime_error("Internal error");
        }
        animation->setStartValue(m_stepPos[step1]);

        QPointF curPos = m_stepPos[step2]+ QPointF(KIDWIDTH/2,KIDWIDTH*1.2);
        auto angle = static_cast<double>(rand());
        curPos = QPointF(curPos.x() + 40.0 * cos(angle), curPos.y() + abs(40.0 * sin(angle)));


        animation->setEndValue(curPos);
        group->addAnimation(animation);
    }


    group->start();

    QObject::connect(group, SIGNAL(finished()), this,
                     SLOT(finishedAnimation()));

    mutex.unlock();
}

void TobogganDisplay::finishedAnimation()
{
    auto group=dynamic_cast<QAnimationGroup*>(sender());
    for(int i=0;i<group->animationCount();i++) {
        auto animation=dynamic_cast<QPropertyAnimation*>(group->animationAt(i));
        auto kid=dynamic_cast<KidItem*>(animation->targetObject());
        if (kid) {
            kid->sem.release();
        }
    }
    delete group;
}


