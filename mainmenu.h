#ifndef MAINMENU_H
#define MAINMENU_H

#include "game.h"
#include "onlineconfiguration.h"
#include "onlinegame.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenu; }
QT_END_NAMESPACE

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

public slots:
    void singleGameBegin();
    void netGameBegin();

private:
    Ui::MainMenu *ui;
    Game* game;
    OnlineGame *onlineGame;
    OnlineConfiguration* onlineConfig;
};
#endif // MAINMENU_H
