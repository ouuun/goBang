#ifndef PIECE_H
#define PIECE_H

#include <QDataStream>



class Piece {

public:
    int line;
    int column;
    static float originX;
    static float originY;
    static float interval;
public:
    Piece();
    Piece(int line,int column);
};

QDataStream& operator<<(QDataStream& out, const Piece& obj);
QDataStream& operator>>(QDataStream& in, Piece& obj);
QDebug operator<<(QDebug debug, const Piece& obj);
#endif // PIECE_H
