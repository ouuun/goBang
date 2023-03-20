#ifndef ONLINEGAME_H
#define ONLINEGAME_H

#include "game.h"

#include <Client.h>
#include <Server.h>
#include <any>

class OnlineGame : public Game
{
    Q_OBJECT
public:
    OnlineGame();
    explicit OnlineGame(QWidget *parent = nullptr);
    virtual ~OnlineGame();
    /**
     * 设置在线游戏配置
     */
    void setOnLineConfig(Game::MODE mode,QString ip,int port);

    /**
     * 通过socket发送数据的模板方法
     */
    template<typename T> void sendMessage(QString type,T data);
public slots:
    /**
     * 处理收到的数据
     */
    void handleMessage(QDataStream* message);

protected:
    /**
     * 重写鼠标释放的事件，判断当前哪种棋子能够下棋
     */
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
//    QString ip;
//    int port;
    Server server;
    Client client;
};

template<typename T> void OnlineGame::sendMessage(QString type,T data)
{
    if(mode==Game::Host) server.sendMessage(type,data);
    if(mode==Game::Slave) client.sendMessage(type,data);
}
#endif // ONLINEGAME_H
