#include "client.h"

Client::Client(QObject* parent):QObject(parent)
{
    connect(&m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(&m_socket, &QTcpSocket::errorOccurred,this, &Client::onError);
}

void Client::connectToHost(const QString& host, quint16 port) {
    m_socket.connectToHost(host, port);
}

void Client::onConnected() {
    emit connected();
    qDebug() << "client connected ";
}

void Client::onDisconnected() {
    emit disconnected();
    m_packetSize = -1;
    m_receivedPacket = QByteArray();
}

void Client::onReadyRead() {
    while (m_socket.bytesAvailable() > 0) {
        QByteArray data = m_socket.readAll();

        if (m_receivedPacket.size() > 0) {
            data = m_receivedPacket + data;
        }

        while (data.size() > 0) {
            qint32 packetSize = m_packetSize;
            if (packetSize == -1) {
                if ((unsigned)data.size() < sizeof(qint32)) {
                    m_receivedPacket = data;
                    return;
                }
                QDataStream sizeStream(data.left(sizeof(qint32)));
                sizeStream >> packetSize;
                data.remove(0, sizeof(qint32));
                m_packetSize = packetSize;
            }

            if (data.size() < packetSize) {
                m_receivedPacket = data;
                return;
            }

            QByteArray packetData = data.left(packetSize);
            data.remove(0, packetSize);
            m_packetSize = -1;
            m_receivedPacket = data;
            QDataStream *stream = new QDataStream(packetData);

            emit messageReceived(stream);
        }

    }
}

void Client::onError(QAbstractSocket::SocketError error) {
    qDebug() << "Socket error: " << error;
}
