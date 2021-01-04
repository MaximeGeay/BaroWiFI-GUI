#ifndef SENSORDIALOG_H
#define SENSORDIALOG_H

#include <QWidget>
#include <QUdpSocket>
#include <QSerialPort>


namespace Ui {
class SensorDialog;
}


class SensorDialog :public QWidget
{
    Q_OBJECT
public:
    enum ConnexionType{
        Serie=0x00,UDP=0x01
    };

    struct Parameters{
        ConnexionType typeConnexion;
        QString PortSerie,Baudrate,ipAddress,Name;
        int PortUDP=0;
    };

    explicit SensorDialog(QWidget *parent = nullptr);
    ~SensorDialog();
    void setSensorType(ConnexionType bType);
    ConnexionType getSensorType();

    void setParameters(Parameters param);
    Parameters getParameters();

    bool setConnected();
    void setDisconnected();
    bool isConnected();
    bool sendMessage(QString sMessage);
    bool broadcastMessage(QString sMessage);

    void majInfo();
    void initSensor(QString sName);
    bool saveSensor(QString sName);

    bool majConnecType();


signals:
    void dataReceived(QString,QString);
    void errorString(QString);

private slots:
     void readData();

private:

    Ui::SensorDialog *ui;
    QSerialPort* mSerialPort;
    QUdpSocket* mUdpSocket;

    Parameters mParam;
    QString mTrameEnCours;
    QString mTrameEntiere;




};

#endif // SENSORDIALOG_H
