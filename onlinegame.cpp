#include "onlinegame.h"

#include <QMessageBox>
#include <any>
#include <ui_game.h>
#include "socketUtils.h"

OnlineGame::OnlineGame():server(this),client(this){

}

OnlineGame::OnlineGame(QWidget *parent):Game(parent),server(parent),client(parent)
{
    currentPlayer = 1;

    //本地添加棋子成功时，向另一方发送信息
    connect(ui->board,&Board::addPieceSuccess,this,[&](int player,Piece piece){
        sendMessage("Piece",piece);
    });

    //本地开始游戏时，发送信息通知对方开始游戏
    connect(ui->startBtn,&QPushButton::clicked,this,[&]{
        sendMessage("Start",true);
    });

    //断开父类建立的链接
    disconnect(ui->rollBackBtn,&QPushButton::clicked,ui->board,&Board::rollBack);
    //向对方发送通知，想要悔棋
    connect(ui->rollBackBtn,&QPushButton::clicked,this,[&]{
        //当己方已经落子，且对方未落子时可以悔棋
        if(currentPlayer==player) return;
        sendMessage("RollBack",1);
    });
    //向对方发送通知，想要交换棋子颜色
    connect(ui->changeBtn,&QPushButton::clicked,this,[&]{
        sendMessage("Change",1);
    });

    //处理收到的信息
    connect(&server,&Server::messageReceived,this,&OnlineGame::handleMessage);
    connect(&client,&Client::messageReceived,this,&OnlineGame::handleMessage);
}

OnlineGame::~OnlineGame()
{
    Game::~Game();
}

//设置当前模式，当前棋子颜色，根据参数连接主机/监听端口
//todo：考虑后续统一由服务器处理，不区分主从机
void OnlineGame::setOnLineConfig(Game::MODE mode, QString ip, int port)
{
    this->mode = mode;
    if(mode == Game::Host){
        this->server.listen(port);
        this->player = 1;
        this->ui->board->setMode(mode,1);
    }
    if(mode == Game::Slave){
        this->client.connectToHost(ip,port);
        this->player = 2;
        this->ui->board->setMode(mode,2);
    }
}

//处理接收到的信息，socket包格式，包长度|消息类型|消息
void OnlineGame::handleMessage(QDataStream *message)
{
    QString str;
    (*message) >> str;

    std::any res = (*handleMap[str])(message);

    if(str=="Piece"){
        Piece piece= std::any_cast<Piece>(res);
        ui->board->socketAddPiece(piece);
        currentPlayer = currentPlayer ==1 ?2:1;
        time = 31;
    }
    if(str=="Start") {
        this->start();
    }
    //根据弹窗选择发送反馈信息是否同意对方悔棋
    if(str=="RollBack"){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "提示", "对方想要悔棋，是否同意", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            sendMessage("RollBackResponse",true);
            ui->board->rollBack();
            currentPlayer = currentPlayer ==1 ?2:1;
        } else {
            sendMessage("RollBackResponse",false);
        }
    }
    //处理悔棋请求的回应，如果同意则可以悔棋
    if(str=="RollBackResponse"){
        bool canRollBack= std::any_cast<bool>(res);
        QString content = canRollBack ?"对方同意悔棋":"对方拒绝悔棋";
        QMessageBox::question(nullptr, "悔棋", content, QMessageBox::Yes);
        if (!canRollBack) return;
        ui->board->rollBack();
        currentPlayer = currentPlayer ==1 ?2:1;
    }

    //处理请求交换执棋颜色，并发送相应的回应
    if(str=="Change"){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "提示", "对方想要交换执棋颜色，是否同意", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            sendMessage("ChangeResponse",true);
            player = player ==1 ?2:1;
            ui->board->setMode(mode,player);
            start();
        } else {
            sendMessage("ChangeResponse",false);
        }
    }
    //处理交换颜色请求的回应，如果对方同意则进行处理
    if(str=="ChangeResponse"){
        bool canChange= std::any_cast<bool>(res);
        QString content = canChange ?"对方同意交换":"对方拒绝交换";
        QMessageBox::question(nullptr, "交换执棋", content, QMessageBox::Yes);
        if (!canChange) return;
        player = player == 1 ?2:1;
        ui->board->setMode(mode,player);
        start();
    }

    delete message;
}

void OnlineGame::mouseReleaseEvent(QMouseEvent *event)
{
    if(mode != Game::Standalone && player !=currentPlayer) return;
    QPointF point = event->pos();
    QPointF subPoint = ui->board->mapFromParent(point);
    if(!ui->board->isLawful(subPoint)) return;
    Piece piece = ui->board->relativeCoordinates(subPoint);
    ui->board->addPiece(piece);
}
