#ifndef P2PAPP_MAIN_HH
#define P2PAPP_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QUdpSocket>


class NetSocket : public QUdpSocket
{
    Q_OBJECT

public:

    NetSocket();

    // Bind this socket to a P2Papp-specific default port.
    bool bind();

    int getPort();
    int myPortMin, myPortMax, port;

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

    quint16 myPort;
    quint16 mySeqNo;

    QMap <QString, QString> outgoing_messages;


    void serializeMessage(QVariantMap &myMap);
    void deserializeMessage(QByteArray datagram);


};





#endif // P2PAPP_MAIN_HH
