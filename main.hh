#ifndef P2PAPP_MAIN_HH
#define P2PAPP_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QTimer>
#include <QTime>
#include <unistd.h>
#include <stdlib.h>

class NetSocket : public QUdpSocket
{
    Q_OBJECT

public:

    NetSocket();

    // Bind this socket to a P2Papp-specific default port.
    bool bind();

    quint16 myPortMin, myPortMax, port;

private:

};



class ChatDialog : public QDialog
{
	Q_OBJECT

public:
    ChatDialog();

    NetSocket* mySocket;

public slots:
	void gotReturnPressed();
    void processPendingDatagrams();
    void clearCurrentPeer();
    void reinvokeRumorMongering();


private:
	QTextEdit *textview;
	QLineEdit *textline;

    QTimer *reinvokeTimer;
    QTimer *newPeerTimer;

    quint16 myPort;
    QString myOrigin;
    quint32 mySeqNo;

    QHostAddress* peerAddress = nullptr;
    quint16* peerPort = nullptr;


    // chatLogs: <Origin, <SeqNo, Message>>
    QMap<QString, QMap<quint32, QString>> chatLogs;

    // statusMap: <Origin, QVariant(LastSeqNo + 1)>
    QVariantMap statusMap;

    void sendRumorMessage(QString origin, quint32 seqNo);
    void serializeMessage(QVariantMap &myMap);
    void deserializeMessage(QByteArray datagram);
    void receiveRumorMessage(QVariantMap inMap);
    void sendStatusMessage();
    void receiveStatusMessage(QVariantMap inMap);


};

#endif // P2PAPP_MAIN_HH
