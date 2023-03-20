#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    void listen(quint16 port);
    template<typename T> void sendMessage(QString type,T data);

signals:
    void clientConnected(qintptr socketDescriptor);
    void clientDisconnected(qintptr socketDescriptor);
    void messageReceived(QDataStream* message);

public
    slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

   private:
    QTcpServer m_server;
    QTcpSocket m_socket;
    QByteArray m_receivedPacket;
    qint32 m_packetSize = -1;

};

template<typename T>
void Server::sendMessage(QString type, T data)
{

    QByteArray packetData;
    QDataStream dataStream(&packetData, QIODevice::WriteOnly);
    dataStream << qint32(0);
    dataStream << type;
    dataStream << data;
    dataStream.device()->seek(0);
    dataStream << qint32(packetData.size() - sizeof(qint32));
    m_socket.write(packetData);
    qDebug()<<"server::sendMessage  type:"<<type;
}

#endif // SERVER_H
