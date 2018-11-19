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
#include <QElapsedTimer>
#include <unistd.h>
#include <stdlib.h>
#include <limits>

#define QINT64MAX std::numeric_limits<qint64>::max()



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

#define EXPECTING_STATUS_MESSAGE 1


class ChatDialog : public QDialog
{
	Q_OBJECT

public:
    ChatDialog();

    NetSocket* mySocket;

public slots:
	void gotReturnPressed();
    void processPendingDatagrams();
    void antiEntropy();
    void resendRumor();


private:
	QTextEdit *textview;
	QLineEdit *textline;

    QTimer *resendTimer;
    QTimer *antiEntropyTimer;

    QElapsedTimer *n1Timer;
    QElapsedTimer *n2Timer;
    qint64 n1Time = QINT64MAX;
    qint64 n2Time = QINT64MAX;



    quint16 myPort;
    QString myOrigin;
    quint32 mySeqNo;

    // chatLogs: <Origin, <SeqNo, Message>>
    QMap<QString, QMap<quint32, QString>> chatLogs;

    // statusMap: <Origin, QVariant(LastSeqNo + 1)>
    QVariantMap statusMap;

    // last sent rumor message
    quint16 lastRumorPort;
    QString lastRumorOrigin;
    quint32 lastRumorSeqNo;

    quint16 pickClosestNeighbor();
    quint16 pickRandomNeighbor();
    void sendRumorMessage(QString origin, quint32 seqNo, quint16 destPort);
    void serializeMessage(QVariantMap &myMap, quint16 destPort);
    void deserializeMessage(QByteArray datagram);
    void receiveRumorMessage(QVariantMap inMap, quint16 sourcePort);
    void sendStatusMessage(quint16 destPort);
    void receiveStatusMessage(QVariantMap inMap, quint16 sourcePort);


};

#endif // P2PAPP_MAIN_HH
