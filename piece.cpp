#include "piece.h"
#include "qdebug.h"

//Piece类方法
Piece::Piece(){}


Piece::Piece(int line, int column)
    :line(line),column(column)
{

}

QDataStream& operator<<(QDataStream& out, const Piece& obj)
{
    out << obj.line;
    out << obj.column;
    return out;
}

QDataStream& operator>>(QDataStream& in, Piece& obj){
    in >> obj.line;
    in >> obj.column;
    return in;
}

QDebug operator<<(QDebug debug, const Piece& obj){
    QDebugStateSaver saver(debug);
    debug.nospace() << "Piece(" << obj.line <<","<< obj.column<< ")";
    return debug;
}
