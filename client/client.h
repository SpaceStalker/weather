#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class client; }
QT_END_NAMESPACE

class client : public QMainWindow
{
    Q_OBJECT

public:
    client(QWidget *parent = nullptr);
    ~client();

private slots:
    void on_Connected();
    void on_Disconnected();
    void on_ReadyRead();
    void on_DisplayError(QAbstractSocket::SocketError);
    void on_connect_clicked();
    void on_disconnect_clicked();
    void on_sendData(QString str);
    void on_findButton_clicked();
    void parse(QByteArray str);
private:
    Ui::client *ui;
    bool trueConnect;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
};
#endif // CLIENT_H
