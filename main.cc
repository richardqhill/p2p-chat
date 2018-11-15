
#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDataStream>

#include "main.hh"


int myPort, portMin, portMax;

ChatDialog::ChatDialog(){
	setWindowTitle("P2Papp");

	// Read-only text box where we display messages from everyone.
	// This widget expands both horizontally and vertically.
	textview = new QTextEdit(this);
	textview->setReadOnly(true);

	// Small text-entry box the user can enter messages.
	// This widget normally expands only horizontally,
	// leaving extra vertical space for the textview widget.
	//
	// You might change this into a read/write QTextEdit,
	// so that the user can easily enter multi-line messages.
	textline = new QLineEdit(this);

	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(textview);
	layout->addWidget(textline);
	setLayout(layout);

	// Register a callback when another p2papp sends us a message over
	// UDP
	//connect(&QUdpSocket, SIGNAL(readyRead()),
	//		this, SLOT(processPendingDatagrams()));

	// Register a callback on the textline's returnPressed signal
	// so that we can send the message entered by the user.
	connect(textline, SIGNAL(returnPressed()),
		this, SLOT(gotReturnPressed()));
}

void ChatDialog::processPendingDatagrams(){

	;
}

void ChatDialog::gotReturnPressed(){

    QString msg = textline->text();
    textview->append(textline->text());

    // Clear the text line to get ready for the next input message.
    textline->clear();

    QMap<QString, QString> outMap, inMap;
    QByteArray mapData;

    outMap.insert("ChatText", msg);

    QDataStream outStream(&mapData, QIODevice::WriteOnly);
    outStream << outMap;
    qDebug() << outMap;

    QDataStream inStream(&mapData, QIODevice::ReadOnly);
    inStream >> inMap;
    qDebug() << inMap;



    qDebug() << "FIX: send message to other peers: " << inMap["ChatText"];

    qDebug() << "My port is: " << myPort;

    // To do: do not send to yourself and to the person the message came from
	for(quint16 port=portMin; port<= portMax && port!=myPort; port++)
	{
		; //QUdpSocket::writeDatagram(&mapData, QHostAddress::LocalHost, port);

	}
    // send to every port b/w 32768 to 49151

    //QUdpSocket::writeDatagram()


}

NetSocket::NetSocket(){
	// Pick a range of four UDP ports to try to allocate by default,
	// computed based on my Unix user ID.
	// This makes it trivial for up to four P2Papp instances per user
	// to find each other on the same host,
	// barring UDP port conflicts with other applications
	// (which are quite possible).
	// We use the range from 32768 to 49151 for this purpose.
	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;

	portMin = myPortMin;
	portMax = myPortMax;
}

bool NetSocket::bind(){
	// Try to bind to each of the range myPortMin..myPortMax in turn.
	for (int p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			qDebug() << "bound to UDP port " << p;
			myPort = p;

			return true;
		}
	}

	qDebug() << "Oops, no ports in my default range " << myPortMin
		<< "-" << myPortMax << " available";
	return false;
}

int main(int argc, char **argv){
	// Initialize Qt toolkit
	QApplication app(argc,argv);

	// Create an initial chat dialog window
	ChatDialog dialog;
	dialog.show();

	// Create a UDP network socket
	NetSocket sock;
	if (!sock.bind())
		exit(1);

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

