#ifndef SENSORDIALOG_H
#define SENSORDIALOG_H


#include <QUdpSocket>
#include <QSerialPort>

class SensorDialog :public QObject
{
    Q_OBJECT
public:
    enum ConnexionType{
        Serie=0x00,UDP=0x01
    };

    struct Parameters{
        ConnexionType typeConnexion;
        QString PortSerie,Baudrate,ipAddress;
        int PortUDP=0;
    };

    SensorDialog();
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


signals:
    void dataReceived(QString);
    void errorString(QString);

private slots:
     void readData();
private:

    QSerialPort* mSeriaPort;
    QUdpSocket* mUdpSocket;

    Parameters mParam;
    QString mTrameEnCours;
    QString mTrameEntiere;

    void init();


};

#endif // SENSORDIALOG_H
