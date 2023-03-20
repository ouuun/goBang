#ifndef BOARD_H
#define BOARD_H

#include "game.h"
#include "piece.h"

#include <QMouseEvent>
#include <QWidget>

//class Piece;

class Board : public QWidget
{
    Q_OBJECT

public:
    explicit Board(QWidget *parent = nullptr);
    ~Board();


    QPointF placementTransform(QPointF &point);
    Piece relativeCoordinates(QPointF &point);
    bool isLawful(QPointF &point);

    void changeShowStep();
    void changeShowCurrent();
    void addPiece(Piece &piece);
    void socketAddPiece(Piece &piece);
    void setMode(Game::MODE mode,int player);
signals:
    //player胜利
    void win(int player);
    //棋子添加成功
    void addPieceSuccess(int player,Piece piece);
    //player 是否能悔棋
    void canRollBack(int player,bool can);

    //void playerRound(int player);

public slots:
    //复原棋盘
    void clear();
    //悔棋
    void rollBack();
    //开始游戏
    void begin();
    //暂停
    void pause();
    //恢复
    void resume();


protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    //void mouseReleaseEvent(QMouseEvent *event) override;
private:
    void adjustConfig();
    void paintBoard(QPainter &painter);
    void paintPices(QPainter &painter);
    void paintCurrent(QPainter &painter);


    void checkWin();




    /**
    *   棋盘相关参数
    *       originX:棋盘左上角X坐标
    *       originY：棋盘左上角Y坐标
    *       interval：棋盘两条线之间的间隔
    *       length：棋盘线条长度
    *       pieceRadius:棋子半径 (用于绘制棋子)
    **/
    float originX;
    float originY;
    float interval;
    float length;
    float pieceRadius;
    float margin;


    /**
     * 已经下了的棋子
     */
    QPointF currentPiece;

    /**
     * 已经下了的棋子
     */
    QList<Piece> pieces;

    /**
     *  棋子标识
     */
    int piecesMark[16][16]={};

    /**
     * 是否显示当前步数
     */
    bool showStep=1;


    /**
     *  是否显示模拟棋子
     */
    bool showCurrent=1;


    /**
     * 是否开始
     */
    bool isRunning=0;

    /**
     * 本局游戏玩家的棋子颜色，只在联机模式下使用
     */
    int player = 1;

    /**
     * 游戏模式，默认参数为单机
     */
    Game::MODE mode = Game::Standalone;
};

#endif // BOARD_H
