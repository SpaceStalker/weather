#include "server.h"
#include "ui_server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkReply>

Server::Server(QWidget *parent): QMainWindow(parent), ui(new Ui::Server), blockSize(0)
{
    ui->setupUi(this);
    tcpServer = NULL;
    regClients.append("Иван_4513");
    regClients.append("Магомедова_583");
    ui->stopServer->setEnabled(false);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(replyFinished(QNetworkReply*)));
}

void Server::on_stopServer_clicked()
{
    ui->textBrowser->append("Попытка остановки");
    tcpServer->close();
    delete tcpServer;
    ui->textBrowser->append("Сервер остановлен");
    ui->stopServer->setEnabled(false);
    ui->startServer->setEnabled(true);
}

void Server::on_startServer_clicked()
{ 
    ui->textBrowser->append("Попытка запуска");
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
    if(!tcpServer->listen(QHostAddress(ui->lineHostEdit->text()), ui->linePortEdit->text().toInt())){
        ui->textBrowser->append("Ошибка");
        tcpServer->close();
    } else {
        ui->textBrowser->append("Сервер запущен");
        ui->startServer->setEnabled(false);
        ui->stopServer->setEnabled(true);
    }
}

/*virtual*/ void Server::slotNewConnection()
{
    ui->textBrowser->append("Новое соединение");
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()),clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(readyRead()), this,SLOT(slotReadClient()));

}

void Server::slotReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    if (!blockSize) {
        if (clientSocket->bytesAvailable() < (int)sizeof(quint16)) {
            return;;
        }
        in >> blockSize;
        if (clientSocket->bytesAvailable() < blockSize) {
            return;;
        }
        QString str;
        in >> str;
        if(authClients.contains(clientSocket)) {
            manager->setProperty("socket", authClients.indexOf(clientSocket));
            manager->get(QNetworkRequest(QUrl("http://api.openweathermap.org/data/2.5/weather?q=" + str + "&lang=ru&units=metric&appid=0b9054d39eff4c73e7bbfcd75b2f9181")));
        } else if (regClients.contains(str)) {
            authClients.append(clientSocket);
            ui->textBrowser->append("Клиент авторизован - " + str);
            sendToClient(clientSocket, "0");
        } else {
            ui->textBrowser->append("Попытка подключения - " + str);
            sendToClient(clientSocket, "99");
        }
        blockSize = 0;
    }
}

void Server::sendToClient(QTcpSocket* pSocket, QByteArray bts)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << bts;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

void Server::replyFinished(QNetworkReply *reply){

    QByteArray bts = reply->readAll();
    sendToClient(authClients[sender()->property("socket").toInt()], bts);
}

Server::~Server()
{
    delete ui;
}






