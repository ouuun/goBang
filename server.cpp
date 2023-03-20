#include "server.h"

Server ::Server(QObject *parent) : QObject(parent) {
    connect(&m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

void Server ::listen(quint16 port) {
    m_server.listen(QHostAddress::Any, port);
}

void Server ::onNewConnection() {
    if(!m_server.hasPendingConnections()) return;
//    QTcpSocket* socket = m_server.nextPendingConnection()->socketDescriptor();
//    m_socket.setSocketDescriptor(socket->socketDescriptor());
//    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
//    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
//    connect(socket, &QTcpSocket::errorOccurred,this, &Server::onError);

    m_socket.setSocketDescriptor(m_server.nextPendingConnection()->socketDescriptor());
    connect(&m_socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(&m_socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    connect(&m_socket, &QTcpSocket::errorOccurred,this, &Server::onError);
}

void Server ::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (!socket) return;
    while (socket->bytesAvailable() > 0) {
        QByteArray data = socket->readAll();

        if (m_receivedPacket.size()>0) {
            data = m_receivedPacket + data;
        }

        while (data.size() > 0) {
            qint32 packetSize = m_packetSize;
            if(packetSize == -1){
                if ((unsigned)data.size() < sizeof(qint32)) {
                    m_receivedPacket = data;
                    return;
                }
                QDataStream sizeStream(data.left(sizeof(qint32)));
                data.remove(0, sizeof(qint32));
                sizeStream >> packetSize;
                m_packetSize = packetSize;
            }
            if (data.size() <  packetSize) {
                m_receivedPacket = data;
                return;
            }

            QByteArray packetData = data.left(packetSize);
            data.remove(0, packetSize);
            m_packetSize = -1;
            m_receivedPacket = data;

            QDataStream* dataStream = new QDataStream(packetData);

            emit messageReceived(dataStream);
        }
    }
}

void Server ::onDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (!socket) return;

    qintptr socketDescriptor = socket->socketDescriptor();
    m_packetSize = -1;
    m_receivedPacket = QByteArray();
    socket->deleteLater();
    emit clientDisconnected(socketDescriptor);
}


void Server ::onError(QAbstractSocket::SocketError error) {
    qDebug() << "Socket error: " << error;
}
