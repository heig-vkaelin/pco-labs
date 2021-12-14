/******************************************************************************
  \file mainwindow.h
  \author Yann Thoma
  \date 05.05.2011

  Fichier faisant partie du labo toboggan.
  ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QDockWidget>
#include "display.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(unsigned int nbConsoles, unsigned int nbStep, unsigned int nbKids,
               QWidget *parent = 0);
    ~MainWindow();

    std::vector<QDockWidget* > m_docks;
    std::vector<QTextEdit* > m_consoles;
    TobogganDisplay *m_display;

    void setConsoleTitle(unsigned int consoleId, const QString &title);

protected:
    unsigned int m_nbConsoles;

public slots:
    void consoleAppendText(unsigned int consoleId, const QString &text);
    void setKid(unsigned int step, unsigned int KidId);
    void travel(unsigned int KidId, unsigned int step2, unsigned int ms);
    void goToPlayground(unsigned int KidId, unsigned int ms);
};

#endif // MAINWINDOW_H
