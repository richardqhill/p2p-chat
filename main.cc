#include <unistd.h>
#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <string>
#include "main.hh"


ChatDialog::ChatDialog(){

    mySocket = new NetSocket();
    if(!  mySocket->bind()) {
		exit(1);
	}
    else {
		myPort = mySocket->getPort();
	}

	mySeqNo = 0;

    setWindowTitle("P2Papp");

	textview = new QTextEdit(this);
	textview->setReadOnly(true);
	textline = new QLineEdit(this);
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(textview);
	layout->addWidget(textline);
	setLayout(layout);

	// Register a callback on the textline's returnPressed signal
	connect(textline, SIGNAL(returnPressed()),
		this, SLOT(gotReturnPressed()));

    // Register a callback when another p2papp sends us a message over UDP
    connect(mySocket, SIGNAL(readyRead()),
    		this, SLOT(processPendingDatagrams()));
}

void ChatDialog::gotReturnPressed(){

	textview->append(QString(myPort)); // prints out chinese character, need to cast?
	textview->append(textline->text());

	QString msg = textline->text();
	outgoing_messages.insert("ChatText", msg);






    //serialize
    QByteArray outData;
    QDataStream outStream(&outData, QIODevice::WriteOnly); //Constructs a data stream that operates on a byte array
    outStream << outgoing_messages;

    for(int i = mySocket->myPortMin; i<= mySocket->myPortMax; i++){
        mySocket->writeDatagram(outData.data(), outData.size(), QHostAddress::LocalHost, i);
    }


	//createRumorMessage(QString(myPort), quint16(mySeqNo));


	mySeqNo += 1;

	// Clear the text line to get ready for the next input message.
	textline->clear();
}


void ChatDialog::processPendingDatagrams(){

	while(mySocket->hasPendingDatagrams()){
		QByteArray datagram;
		datagram.resize(mySocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

		if(mySocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort) != -1)
			deserializeMessage(datagram);
		else
			return;
	}
}

void ChatDialog::deserializeMessage(QByteArray datagram) {

	QDataStream inStream(&datagram, QIODevice::ReadOnly);
    QMap<QString, QString> inMap;
	inStream >> inMap;

    qDebug() << inMap;

}








/* Pick a range of four UDP ports to try to allocate by default, computed based on my Unix user ID.*/
NetSocket::NetSocket(){
	// This makes it trivial for up to four P2Papp instances per user to find each other on the same host,
	// barring UDP port conflicts with other applications (which are quite possible). We use the range from
	// 32768 to 49151 for this purpose.
	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;

}

/* Bind one of ports between myPortMin and myPortMax */
bool NetSocket::bind(){
	for (int p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			qDebug() << "bound to UDP port " << p;
			port = p;
			return true;
		}
	}
	qDebug() << "Oops, no ports in my default range " << myPortMin << "-" << myPortMax << " available";
	return false;
}

int NetSocket::getPort(){
    return port;
}

int main(int argc, char **argv){
	// Initialize Qt toolkit
	QApplication app(argc,argv);

	// Create an initial chat dialog window
	ChatDialog dialog;
	dialog.show();

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

