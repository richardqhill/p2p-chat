#ifndef P2PAPP_MAIN_HH
#define P2PAPP_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QUdpSocket>
#include <unistd.h>
#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>


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

private:
	QTextEdit *textview;
	QLineEdit *textline;

    QTimer *timer;
    quint16 myPort;
    QString myOrigin;
    quint32 mySeqNo;


    // chatLogs: <Origin, <SeqNo, Message>>
    QMap<QString, QMap<quint32, QString>> chatLogs;

    // statusMap: <Origin, LastSeqNo + 1>
    QVariantMap statusMap;



    void sendRumorMessage(QString origin, quint32 seqNo);
    void serializeMessage(QVariantMap &myMap);
    void deserializeMessage(QByteArray datagram);
    void receiveRumorMessage(QVariantMap inMap);
    void sendStatusMessage();
    void receiveStatusMessage(QVariantMap inMap);


};





#endif // P2PAPP_MAIN_HH
