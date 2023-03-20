#ifndef SOCKETUTILS_H
#define SOCKETUTILS_H

#include <QDataStream>
#include <QHash>
#include <any>
#include "piece.h"
typedef std::any(* func)(QDataStream*);

extern QHash<QString, func> handleMap;

std::any piece(QDataStream *stream);
std::any win(QDataStream *stream);
std::any rollBack(QDataStream *stream);
std::any rollBackResponse(QDataStream* stream);
std::any start(QDataStream* stream);
std::any change(QDataStream* stream);
std::any changeResponse(QDataStream* stream);

#endif // SOCKETUTILS_H
