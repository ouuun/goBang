#include "mainmenu.h"
#include "ui_mainmenu.h"

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainMenu)
    , game(new Game)
    , onlineGame(new OnlineGame(parent))
    , onlineConfig(new OnlineConfiguration(parent))
{
    ui->setupUi(this);

    //处理两个button的点击事件
    connect(ui->singleGameBtn,&QPushButton::clicked,this,&MainMenu::singleGameBegin);
    connect(ui->netGameBtn,&QPushButton::clicked,this,&MainMenu::netGameBegin);

    //当游戏窗口关闭时，显示当前窗口
    connect(this->game,&Game::closed,this,[&]{
        this->show();
    });

    //用户选择主从机后，打开游戏窗口，并设置相关参数
    connect(this->onlineConfig,&OnlineConfiguration::gameModeSelected,this,[&](Game::MODE mode,QString ip,QString port){
        this->onlineConfig->hide();
        this->onlineGame->show();
        this->onlineGame->setOnLineConfig(mode,ip,port.toInt());
    });
}

MainMenu::~MainMenu()
{
    delete ui;
    delete game;
    delete onlineConfig;
}

//打开游戏窗口，隐藏当前窗口
void MainMenu::singleGameBegin()
{
    this->hide();
    this->game->show();
}

//打开联机设置窗口，隐藏当前窗口
void MainMenu::netGameBegin()
{
    this->hide();
    this->onlineConfig->show();
}
