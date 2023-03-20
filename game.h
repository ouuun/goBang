#ifndef GAME_H
#define GAME_H

#include <QWidget>

namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT

public:
    /**
     * 游戏模式
     * Standalone：单人
     * Host：主机
     * Slave：从机
     */
    enum MODE {
        Standalone,
        Host,
        Slave
    };

public:
    explicit Game(QWidget *parent = nullptr);
    virtual ~Game();

    /**
     * 初始化相关配置
     */
    void init();

protected slots:
    /**
     * 点击显示步数按钮的相关处理
     */
    void onShowStepBtnClicked();
    /**
     * 点击显示当前的相关处理
     */
    void onShowCurrentBtnClicked();
    /**
     * 定时器触发timeout信号的处理
     */
    void timeout();
    /**
     * 某位玩家获胜的处理
     */
    void playerWin(int player);
    /**
     * 开始游戏
     */
    void start();

protected:
    void closeEvent(QCloseEvent *event) override;
    //void paintEvent(QPaintEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

public:
signals: void closed();


protected:

    /**
     * 该窗口对应的ui
     */
    Ui::Game *ui;
    /**
     * 是否显示步数
     */
    bool showStep = 1;
    /**
     * 是否显示鼠标下的棋子提示
     */
    bool showCurrent = 1;

    /**
     * 定时器对象
     */
    QTimer *timer;
    /**
     * 倒计时，用于显示在界面上
     */
    int time = 30;

    /**
     * 当前游戏的玩家棋子颜色
     */
    int player = 1;
    /**
     * 当前应该由哪个颜色的玩家下棋
     */
    int currentPlayer = 1;

    /**
     * 是否在运行中
     */
    bool isRunning = false;
    bool isBegin = false;

    /**
     * 游戏模式
     */
    MODE mode = Game::Standalone;
};

#endif // GAME_H
