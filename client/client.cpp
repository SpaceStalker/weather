#include "client.h"
#include "ui_client.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

client::client(QWidget *parent): QMainWindow(parent), ui(new Ui::client), blockSize(0)
{
    ui->setupUi(this);
    ui->disconnect->setEnabled(false);
    ui->findButton->setEnabled(false);
    tcpSocket = new QTcpSocket(this);
    trueConnect = false;
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(on_ReadyRead()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(on_Connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(on_Disconnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(on_DisplayError(QAbstractSocket::SocketError)));
}

void client::on_connect_clicked()
{
    tcpSocket->connectToHost(ui->hostLineEdit->text(), ui->portLineEdit->text().toInt());
    ui->textBrowser->append("Попытка соединения");
}

void client::on_disconnect_clicked()
{
    ui->textBrowser->append("Попытка отключения");
    tcpSocket->disconnectFromHost();
}

void client::on_Connected()
{
    ui->textBrowser->append("Установлена связь с сервером");
    on_sendData(ui->nameEdit->text() + "_"+ ui->passwordEdit->text());
    ui->connect->setEnabled(false);
    ui->disconnect->setEnabled(true);
}

void client::on_DisplayError(QAbstractSocket::SocketError)
{
    ui->textBrowser->append("Ошибка");
}
void client::on_Disconnected()
{
    trueConnect = false;
    ui->textBrowser->append("Отключено");
    ui->findButton->setEnabled(false);
    ui->connect->setEnabled(true);
    ui->disconnect->setEnabled(false);
}

void client::on_ReadyRead()
{
    QDataStream in(tcpSocket);
    if(!blockSize) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;;
        in >> blockSize;
    }
    if (tcpSocket->bytesAvailable() < blockSize)
        return;
    QByteArray text;
    in >> text;
    if (trueConnect) {
        parse(text);
    } else {   
        switch (text.toInt()) {
            case 0:
                ui->textBrowser->append("Вы авторизованы");
                trueConnect = true;
                ui->findButton->setEnabled(true);
                break;
            case 99:
                ui->textBrowser->append("Неправильные данные");
                on_disconnect_clicked();
                break;
        }
    }
    blockSize = 0;
}

void client::on_sendData(QString str) {
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void client::on_findButton_clicked()
{
    on_sendData(ui->cityLineEdit->text());
}

void client::parse(QByteArray ba) {
    ui->weatherBrowse->setText("");
    QJsonDocument document = QJsonDocument::fromJson(ba);
    QJsonObject root = document.object();
    if (root.value("cod") == "404") {
        ui->weatherBrowse->append("Неправильно введен город");
    } else {
        QJsonArray ja = root.value("weather").toArray();
        QJsonObject subtree = ja.at(0).toObject();
        ui->weatherBrowse->append("Город: " + root.value("name").toString());
        ui->weatherBrowse->append("Описание: " + subtree.value("description").toString());
        QJsonObject main = root.value("main").toObject();
        ui->weatherBrowse->append("Температура: " + QString::number(main.value("temp").toInt()) + " C");
        ui->weatherBrowse->append("Давление: " + QString::number(main.value("pressure").toInt()) + " мм рт. ст.");
        ui->weatherBrowse->append("Влажность: " + QString::number(main.value("humidity").toInt()) + "%");
        QJsonObject wind =root.value("wind").toObject();
        ui->weatherBrowse->append("Скорость ветра: " + QString::number(wind.value("speed").toInt()) + " м/c");
        QJsonObject clouds = root.value("clouds").toObject();
        ui->weatherBrowse->append("Облачность: " + QString::number(clouds.value("all").toInt()) + "%");
    }
}

client::~client()
{
    delete ui;
}
