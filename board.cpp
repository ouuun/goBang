#include "board.h"
#include <QPainter>


float Piece::originX = 0;
float Piece::originY = 0;
float Piece::interval = 0;


Board::Board(QWidget *parent)
    : QWidget{parent},pieces()
{
    this->setMinimumHeight(600);
    this->setMinimumWidth(600);

    connect(this,&Board::win,this,[this]{
        pause();
    });

    setMouseTracking(true);

}

Board::~Board()
{

}

/**
 * 记录当前鼠标坐标，用于绘制棋子提示
 */
void Board::mouseMoveEvent(QMouseEvent *event){
    currentPiece = event->pos();
    update();
}

//void Board::mouseReleaseEvent(QMouseEvent *event){
//    event->ignore();
//}

/**
 * 根据当前窗口大小动态计算绘制棋盘的相关参数
 */
void Board::adjustConfig()
{
    //设置显示范围
    int widgetSide = qMin(this->width(),this->height());

    margin = widgetSide/15;
    float side = widgetSide-2*margin;
    float offsetWidth = (this->width()-2*margin-side)/2;
    float offsetheight = (this->height()-2*margin-side)/2;


    originX = margin+offsetWidth;
    originY = margin+offsetheight;
    interval = side/14;
    length = side;
    pieceRadius = interval/2;


    Piece::originX = originX;
    Piece::originY = originY;
    Piece::interval = interval;
}

/**
 * 绘制棋盘，棋子
 */
void Board::paintEvent(QPaintEvent *event){

    adjustConfig();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);  //抗锯齿

    paintBoard(painter);
    paintCurrent(painter);
    paintPices(painter);

    QWidget::paintEvent(event);
}

/**
 * 绘制棋盘
 */
void Board::paintBoard(QPainter &painter)
{
    painter.save();

    //绘制 board组件 边框
    painter.drawRect(0,0,this->width(),this->height());


    QPen pen,weightPen;
    weightPen.setWidth(2);
    pen.setWidth(1);

    //绘制棋盘
    for(int i = 0; i <= 14; i++){
        painter.setPen(pen);
        if(i==0||i==14) painter.setPen(weightPen);

        painter.drawLine(originX+i*interval,originY,
                         originX+i*interval,originY+length);
    }

    for(int i = 0; i <= 14; i++){
        painter.setPen(pen);
        if(i==0||i==14) painter.setPen(weightPen);
        painter.drawLine(originX,originY+i*interval,
                         originX+length,originY+i*interval);
    }

    //绘制行号列号
    int number[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    painter.setFont(QFont("FangSong", interval * 5 / 15, 2700));
    for(int i:number){
        QRect rectNumber(originX-margin*1.05,originY+(i-1)*interval-margin/2,interval,interval);
        painter.drawText(rectNumber,  QString::number(i), QTextOption(Qt::AlignCenter));

        QRect rectLetter(originX+(i-1)*interval-margin/2,originY-margin*1.05,interval,interval);
        painter.drawText(rectLetter,  QString(QChar(i+64)) , QTextOption(Qt::AlignCenter));
    }

    //绘制定位点
    int points[][2] = {{3,3},{11,3},{7,7},{3,11},{11,11}};

    for(int i=0;i<5;i++){
        float x = points[i][0]*interval+originX;
        float y = points[i][1]*interval+originY;
        painter.setBrush(Qt::black);
        painter.drawEllipse(QPointF(x,y),interval/5,interval/5);
    }

    painter.restore();
}

/**
 * 绘制棋子
 */
void Board::paintPices(QPainter &painter){
    if(pieces.empty()) return;
    int count =1;
    for(Piece piece: pieces){

        painter.setPen(Qt::black); // 设置画笔颜色

        painter.setBrush(Qt::white);
        if(count%2==1) painter.setBrush(Qt::black);// 设置画刷颜色
        float x = originX +  piece.line * interval;
        float y = originY +  piece.column * interval;
        QPointF paintPiece(x,y);
        painter.drawEllipse(paintPiece,pieceRadius,pieceRadius);  //绘制棋子

        if(showStep){

            painter.setPen(Qt::black);
            if(count%2==1) painter.setPen(Qt::white);

            painter.setFont(QFont("FangSong", interval * 5 / 12, 2700));
            QRect rect(QPoint(paintPiece.x()-pieceRadius/2,paintPiece.y()-pieceRadius/2),QSize(pieceRadius,pieceRadius));
            painter.drawText(rect, QString::number(count), QTextOption(Qt::AlignCenter));
        }
        count++;
    }
}

/**
 * 绘制鼠标下的棋子
 */
void Board::paintCurrent(QPainter &painter)
{
    if(!showCurrent) return;
    if(!isLawful(currentPiece)) return;
    if(mode!=Game::Standalone && (pieces.size()%2)+1 != player) return;
    if(!isRunning) return;

    painter.save();

    //根据pieces的长度判断当前执棋颜色
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0,0,0,230));
    if(pieces.size()%2==1) painter.setBrush(QColor(255,255,255,250));

    QPointF point = placementTransform(currentPiece);
    Piece piece = relativeCoordinates(point);
    if (piecesMark[piece.line][piece.column]!=1)
        painter.drawEllipse(point, interval/2, interval/2);


    painter.restore();
}


/**
 * 判断point是否合法
 * 1.是否在棋盘范围内
 * 2.当前位置是否已经有棋子
 */
bool Board::isLawful(QPointF &point){
    //range：棋盘边缘处理，至少小于 间隔/2
    if(point.isNull()) return false;

    float range = interval/3;
    if(point.x() < originX-range || point.x()> originX+length+range
            || point.y() < originY-range || point.y()> originY+length+range) {
        currentPiece = QPointF();
        return false;
    }
    Piece p = relativeCoordinates(point);
    if(piecesMark[p.line][p.column]>0) return false;

    return true;
}


/**
 * 添加一个棋子
 */
void Board::addPiece(Piece &piece)
{
    if(!isRunning) return;
    piecesMark[piece.line][piece.column] = pieces.length()%2==1?1:2;
    pieces.append(piece);
    update();
    emit canRollBack(pieces.length()%2==1?1:2,pieces.empty()?false:true);


    emit addPieceSuccess(pieces.length()%2==1?1:2,piece);
    checkWin();
}


/**
 * 添加一个棋子，但不发送addPieceSuccess信号，避免死循环
 */
void Board::socketAddPiece(Piece &piece)
{
    if(!isRunning) return;
    piecesMark[piece.line][piece.column] = pieces.length()%2==1?1:2;
    pieces.append(piece);
    update();
    emit canRollBack(pieces.length()%2==1?1:2,pieces.empty()?false:true);

    checkWin();
}

/**
 * 设置当前游戏模式，及用户棋子颜色
 */
void Board::setMode(Game::MODE mode, int player)
{
    this->mode = mode;
    this->player =player;
}

/**
 * 检测是否有玩家胜利
 */
void Board::checkWin(){
    if(pieces.length()<9) return;
    int player = pieces.length()%2==1 ?1:2;
    for(Piece piece:pieces){
        int x = piece.line;
        int y = piece.column;
        int color = piecesMark[x][y];
        int count = 1;

        //垂直方向
        for(int i=x+1;i<x+4;i++){
            if(piecesMark[i][y]!=color) break;
            count++;
        }
        for(int i=x-1;i>x-4;i--){
            if(piecesMark[i][y]!=color) break;
            count++;
        }
        if(count==5) {
            emit win(player);
            return;
        }

        //水平方向
        count = 1;
        for(int j=y+1;j<y+4;j++){
            if(piecesMark[x][j]!=color) break;
            count++;
        }
        for(int j=y-1;j>y-4;j--){
            if(piecesMark[x][j]!=color) break;
            count++;
        }
        if(count==5) {
            emit win(player);
            return;
        }

        //倾斜
        count = 1;
        for(int i=x+1,j=y+1;i<x+4&&j<y+4;i++,j++){
            if(piecesMark[i][j]!=color) break;
            count++;
        }
        for(int i=x-1,j=y-1;i>x-4&&j>y-4;i--,j--){
            if(piecesMark[i][j]!=color) break;
            count++;
        }
        if(count==5) {
            emit win(player);
            return;
        }

        //倾斜
        count = 1;
        for(int i=x-1,j=y+1;i>x-4&&j<y+4;i--,j++){
            if(piecesMark[i][j]!=color) break;
            count++;
        }
        for(int i=x+1,j=y-1;i<x+4&&j>y-4;i++,j--){
            if(piecesMark[i][j]!=color) break;
            count++;
        }
        if(count==5) {
            emit win(player);
            return;
        }
    }
}


/**
 * 将棋盘上任意一点转换为最近的棋子标准落点
 */
QPointF Board::placementTransform(QPointF &point){
    float indexX = qRound((point.x()-originX)/interval);
    float indexY = qRound((point.y()-originY)/interval);

    float x = originX +  indexX * interval;
    float y = originY +  indexY * interval;

    return QPointF(x,y);
}


/**
 * 清空 pieceList，pieceMark 数组
 */
void Board::clear()
{
    pieces.clear();
    memset(piecesMark, 0, sizeof(piecesMark));
}


/**
 * 悔棋，pieceList弹出末尾元素，将pieceMark数组中的标记清除
 */
void Board::rollBack()
{
    if(pieces.empty()) return;
    Piece p = pieces.last();
    piecesMark[p.line][p.column] = 0;
    pieces.pop_back();
    update();

    emit canRollBack(pieces.length()%2==1?1:2,pieces.empty()?false:true);
}

/**
 * 开始游戏
 */
void Board::begin()
{
    clear();
    isRunning = true;
    update();
}

/**
 * 暂停
 */
void Board::pause()
{
    isRunning = false;
}

/**
 * 继续
 */
void Board::resume()
{
    isRunning = true;
}

/**
 * 改变 showStep的取值
 */
void Board::changeShowStep()
{
    showStep = !showStep;
    update();
}

/**
 * 改变 showCurrent的取值
 */
void Board::changeShowCurrent()
{
    showCurrent = !showCurrent;
}

/**
 * 将point转换为行列坐标
 */
Piece Board::relativeCoordinates(QPointF &point){
    float indexX = qRound((point.x()-originX)/interval);
    float indexY = qRound((point.y()-originY)/interval);
    Piece p(indexX,indexY);
    return p;
}





