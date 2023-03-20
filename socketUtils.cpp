#include "socketUtils.h"
#include <QIODevice>

QHash<QString, func> handleMap= {
    {"Piece", piece},
    {"Win", piece},
    {"RollBack", rollBack},
    {"RollBackResponse", rollBackResponse},
    {"Start", start},
    {"Change", change},
    {"ChangeResponse", changeResponse},
};


std::any piece(QDataStream* stream)
{
    Piece p;
    *stream >> p;
    return std::any(p);
}

std::any rollBack(QDataStream* stream)
{
    int player;
    *stream >> player;
    return std::any(player);
}

std::any rollBackResponse(QDataStream* stream)
{
    bool response;
    *stream >> response;
    return std::any(response);
}

std::any start(QDataStream* stream)
{
    int i;
    *stream >> i;
    return std::any(i);
}

std::any change(QDataStream* stream)
{
    int i;
    *stream >> i;
    return std::any(i);
}

std::any changeResponse(QDataStream* stream)
{
    bool response;
    *stream >> response;
    return std::any(response);
}
