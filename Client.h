#ifndef CLIENT_H
#define CLIENT_H


#include <QObject>
#include <QtNetwork/QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    //Client();
    Client(QObject *parent);

    void connectToHost(const QString& host, quint16 port);

    template<typename T> void sendMessage(QString type,T data);


signals:
    void connected();
    void disconnected();
    void messageReceived(QDataStream *message);

public slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);

private:
    QTcpSocket m_socket;
    QByteArray m_receivedPacket;
    qint32 m_packetSize = -1;

};

template<typename T>
void Client::sendMessage(QString type, T data)
{
    //T i = static_cast<T>(data);
    QByteArray packetData;
    QDataStream dataStream(&packetData, QIODevice::WriteOnly);
    dataStream << qint32(0);
    dataStream << type;
    dataStream << data;
    dataStream.device()->seek(0);
    dataStream << qint32(packetData.size() - sizeof(qint32));
    m_socket.write(packetData);
    qDebug()<<"client::sendMessage  type:"<<type;
}

#endif // CLIENT_H
