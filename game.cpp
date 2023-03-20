#include "game.h"
#include "ui_game.h"

#include <QMessageBox>
#include <QTimer>

Game::Game(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Game),
    timer(new QTimer)
{
    ui->setupUi(this);
    init();
}

Game::~Game()
{
    delete ui;
    delete timer;
}

void Game::init()
{

    //初始化界面
    ui->timer->display("00:30");
    ui->timer->setStyleSheet("color: black"); // 设置样式
    ui->timer->setSegmentStyle(QLCDNumber::Flat); // 设置数字样式

    ui->showCurrentBtn->setDisabled(true);
    ui->showStepBtn->setDisabled(true);
    ui->rollBackBtn->setDisabled(true);

    if(mode == Game::Standalone) ui->changeBtn->hide();
    if(mode!= Game::Standalone) ui->pauseBtn->hide();




    connect(ui->showStepBtn,&QPushButton::clicked,this,&Game::onShowStepBtnClicked);
    connect(ui->showCurrentBtn,&QPushButton::clicked,this,&Game::onShowCurrentBtnClicked);
    connect(ui->rollBackBtn,&QPushButton::clicked,ui->board,&Board::rollBack);
    connect(ui->board,&Board::win,this,&Game::playerWin);

    connect(ui->startBtn,&QPushButton::clicked,this,&Game::start);

    //点击暂停按钮时的相关处理
    connect(ui->pauseBtn,&QPushButton::clicked,this,[&]{
        if(!isRunning){
            ui->pauseBtn->setText(QString("暂停"));
            ui->board->resume();
            this->timer->start(1000);
        }else{
            ui->pauseBtn->setText(QString("继续"));
            ui->board->pause();
            this->timer->stop();
        }
    });

    //处理 canRollBack 信号
    connect(ui->board,&Board::canRollBack,this,[&](int player,bool can){
        if (player==this->player)
            ui->rollBackBtn->setDisabled(!can);
    });

    //重置计时器
    connect(ui->board,&Board::addPieceSuccess,this,[&](int player){
        time = 31;
        QString text =  ((time < 10) ? QString("00:0") : QString("00:")) + QString::number(time);
        currentPlayer = currentPlayer==1?2:1;
    });

    //绑定计时器的timeout信号处理
    connect(timer,&QTimer::timeout,this,&Game::timeout);

}

void Game::onShowStepBtnClicked()
{
    showStep = !showStep;
    ui->showStepBtn->setText((showStep ? QString("隐藏") : QString("显示")) + QString("步数"));
    ui->board->changeShowStep();
}

void Game::onShowCurrentBtnClicked()
{
    showCurrent = !showCurrent;
    ui->showCurrentBtn->setText((showCurrent ? QString("隐藏") : QString("显示")) + QString("当前"));
    ui->board->changeShowCurrent();
}

void Game::timeout()
{
    time--;
    QString text =  ((time < 10) ? QString("00:0") : QString("00:")) + QString::number(time);
    ui->timer->display(text);

    //超时判负
    if(time!=0) return;
    playerWin(currentPlayer==1?2:1);
}

void Game::playerWin(int player)
{

    timer->stop();
    time = 30;
    QMessageBox messageBox;

    messageBox.setBaseSize(200,300);
    messageBox.setWindowTitle("比赛结束");
    messageBox.setText(QString(player==1?"黑":"白")+"方胜利!");
    messageBox.setStandardButtons(QMessageBox::Ok);

    messageBox.exec();

    ui->board->pause();
}

void Game::start()
{
    if(!isBegin){
        ui->showCurrentBtn->setDisabled(false);
        ui->showStepBtn->setDisabled(false);
        ui->pauseBtn->setDisabled(false);
        isRunning = true;
        this->timer->start(1000);
        ui->board->begin();
        isBegin = true;

        ui->startBtn->setText(QString("重新开始"));
    }else{
        time = 30;
        ui->board->begin();
    }
}

void Game::closeEvent(QCloseEvent *event)
{
    emit closed();
    QWidget::closeEvent(event);
}

//void Game::paintEvent(QPaintEvent *event){
//    QWidget::paintEvent(event);
//}

void Game::mouseReleaseEvent(QMouseEvent *event){
    //获取当前点击坐标，转换为棋盘坐标系下的坐标
    QPointF point = event->pos();
    QPointF subPoint = ui->board->mapFromParent(point);
    //判断是否合法
    if(!ui->board->isLawful(subPoint)) return;
    //转换为棋盘相对坐标（行，列）
    Piece piece = ui->board->relativeCoordinates(subPoint);
    //添加棋子
    ui->board->addPiece(piece);
}
