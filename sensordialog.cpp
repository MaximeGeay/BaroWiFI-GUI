#include<QSettings>
#include<QDebug>
#include <QSerialPortInfo>

#include "sensordialog.h"
#include "ui_sensordialog.h"

SensorDialog::SensorDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorDialog)
{
    ui->setupUi(this);

    mSerialPort=new QSerialPort;
    mUdpSocket=new QUdpSocket;


    QObject::connect(mSerialPort,&QSerialPort::readyRead,this,&SensorDialog::readData);
    QObject::connect(mUdpSocket,&QUdpSocket::readyRead,this,&SensorDialog::readData);
    QObject::connect(ui->rad_Udp,&QRadioButton::toggled,this,&SensorDialog::majConnecType);
    QObject::connect(ui->rad_Serie,&QRadioButton::toggled,this,&SensorDialog::majConnecType);
}

SensorDialog::~SensorDialog()
{
    delete mSerialPort;
    delete mUdpSocket;
    delete ui;
}

void SensorDialog::setSensorType(SensorDialog::ConnexionType bType)
{
    mParam.typeConnexion=bType;

}

SensorDialog::ConnexionType SensorDialog::getSensorType()
{
    return mParam.typeConnexion;
}

void SensorDialog::setParameters(SensorDialog::Parameters param)
{

    qDebug()<<"setParameters"<<param.Name;
    if(!param.Name.isEmpty())
    {
        mParam=param;
        bool bType=false;
        if(mParam.typeConnexion==Serie)
            bType=false;
        if(mParam.typeConnexion==UDP)
            bType=true;

         qDebug()<<"InitSensor"<<mParam.Name<<mParam.PortSerie<<mParam.typeConnexion;
        ui->rad_Serie->setChecked(!bType);
        ui->rad_Udp->setChecked(bType);
        majConnecType();

        ui->cb_Baudrate->setCurrentIndex(ui->cb_Baudrate->findText(mParam.Baudrate));
        ui->sp_UDPPort->setValue(mParam.PortUDP);
        ui->le_IpCapteur->setText(mParam.ipAddress);
        saveSensor(mParam.Name);
    }



}

SensorDialog::Parameters SensorDialog::getParameters()
{
    return mParam;
}



bool SensorDialog::setConnected()
{
    //init();

    bool bRes=false;
    if(mParam.typeConnexion==Serie)
     {
        mSerialPort->setPortName(mParam.PortSerie);
        if(mParam.Baudrate=="4800")
            mSerialPort->setBaudRate(QSerialPort::Baud4800);
        if(mParam.Baudrate=="9600")
            mSerialPort->setBaudRate(QSerialPort::Baud9600);
        if(mParam.Baudrate=="19200")
            mSerialPort->setBaudRate(QSerialPort::Baud19200);
        if(mParam.Baudrate=="38400")
            mSerialPort->setBaudRate(QSerialPort::Baud38400);
        if(mParam.Baudrate=="115200")
            mSerialPort->setBaudRate(QSerialPort::Baud115200);

        mSerialPort->setParity(QSerialPort::NoParity);
        mSerialPort->setDataBits(QSerialPort::Data8);
        bRes=mSerialPort->open(QIODevice::ReadWrite);
        if(bRes)
        {

            emit errorString(QString("Connecté à %1").arg(mParam.PortSerie));
            mTrameEnCours="";
        }
        else
        {

            if(mSerialPort->error()==QSerialPort::DeviceNotFoundError)
                emit errorString("Port série introuvable");
            else {
                if(mSerialPort->error()==QSerialPort::PermissionError)
                    emit errorString("Problème de droits sur le port série");
                else {
                    emit errorString("Echec d'ouverture du port série");
                }
            }
        }
    }

    if(mParam.typeConnexion==UDP)
    {
        qDebug()<<"Connexion"<<mParam.Name<<mParam.PortUDP;
        if(mUdpSocket->bind(QHostAddress::Any,mParam.PortUDP))
        {
             emit errorString(QString("Connecté au port UDP %1").arg(mParam.PortUDP));
            bRes=true;
        }
        else
        {
            emit errorString(mUdpSocket->errorString());
            bRes=false;
        }
    }



    return bRes;
}

void SensorDialog::setDisconnected()
{
    if(mParam.typeConnexion==Serie)
        mSerialPort->close();
    if(mParam.typeConnexion==UDP)
        mUdpSocket->close();
}

bool SensorDialog::isConnected()
{
    if(mParam.typeConnexion==Serie)
        return mSerialPort->isOpen();

    if(mParam.typeConnexion==UDP)
    {

       if(mUdpSocket->state()==QUdpSocket::BoundState)
        return  true;
    }


    return false;
}

bool SensorDialog::sendMessage(QString sMessage)
{
    bool bRes=false;

    if(!isConnected())
    {
        emit errorString(QString ("Capteur non connecté"));
        return false;
    }
    if(mParam.typeConnexion==Serie)
    {
        qint64 result=mSerialPort->write(sMessage.toLocal8Bit());
        if(result==-1)
        {
            emit errorString(QString("Message non envoyé : %1").arg(sMessage));
            emit errorString(mSerialPort->errorString());
            bRes=false;
        }
        else
        {
            emit errorString(QString("Message envoyé sur %1 : %2").arg(mParam.PortSerie).arg(sMessage));
            bRes=true;
        }


    }

    if(mParam.typeConnexion==UDP)
    {


        QByteArray datagram = sMessage.toLocal8Bit();

       qint64 result=mUdpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress(mParam.ipAddress),mParam.PortUDP);
       qDebug()<<mParam.ipAddress<<mParam.PortUDP;

       if (result==-1)
       {
           bRes=false;
           emit errorString(QString("Message non envoyé : %1").arg(sMessage));
           emit errorString(mUdpSocket->errorString());

       }
       else
       {
           emit errorString(QString("Message envoyé sur le port %1 : %2").arg(mParam.PortUDP).arg(sMessage));
           qDebug()<<QString("Message envoyé sur le port %1 : %2").arg(mParam.PortUDP).arg(sMessage);
           bRes=true;

       }

    }

    return bRes;
}

bool SensorDialog::broadcastMessage(QString sMessage)
{
    bool bRes=false;

    if(!isConnected())
    {
        emit errorString(QString ("Capteur non connecté"));
        return false;
    }

    if(mParam.typeConnexion==Serie)
    {
        qint64 result=mSerialPort->write(sMessage.toLocal8Bit());
        if(result==-1)
        {
            emit errorString(QString("Message non envoyé : %1").arg(sMessage));
            emit errorString(mSerialPort->errorString());
            bRes=false;
        }
        else
        {
            emit errorString(QString("Message envoyé sur %1 : %2").arg(mParam.PortSerie).arg(sMessage));
            bRes=true;
        }


    }

    if(mParam.typeConnexion==UDP)
    {

        QByteArray datagram = sMessage.toLocal8Bit();

       qint64 result=mUdpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress("255.255.255.255"),mParam.PortUDP);

       if (result==-1)
       {
           bRes=false;
           emit errorString(QString("Message non envoyé : %1").arg(sMessage));
           emit errorString(mUdpSocket->errorString());

       }
       else
       {
           emit errorString(QString("Message envoyé sur le port %1 : %2").arg(mParam.PortUDP).arg(sMessage));
           bRes=true;

       }

    }

    return bRes;
}

void SensorDialog::majInfo()
{
    QStringList portList;
    ui->cb_Serial->clear();

    const auto infos = QSerialPortInfo::availablePorts();

        for (const QSerialPortInfo &info : infos)
        {
            portList.append(info.portName());
            this->ui->cb_Serial->addItem(info.portName());
        }

        QStringListIterator it (portList);
        QString sUneLigne;
        while(it.hasNext())
        {
            sUneLigne=it.next();
            if(mParam.PortSerie==sUneLigne)
                ui->cb_Serial->setCurrentIndex(ui->cb_Serial->findText(mParam.PortSerie));

        }


}

void SensorDialog::initSensor(QString sName)
{
    QSettings settings;
    mParam.PortSerie=settings.value(QString("%1/PortName").arg(sName),"").toString();
    mParam.Baudrate=settings.value(QString("%1/BaudRate").arg(sName),4800).toString();
    mParam.typeConnexion=static_cast<SensorDialog::ConnexionType>(settings.value(QString("%1/ConnecType").arg(sName),0).toBool());

    mParam.ipAddress=settings.value(QString("%1/IpSensor").arg(sName)).toString();
    mParam.PortUDP=settings.value(QString("%1/PortUDP").arg(sName),50000).toInt();
    mParam.Name=sName;

    qDebug()<<"InitSensor"<<mParam.Name<<mParam.PortSerie;

    bool bType=false;
    if(mParam.typeConnexion==Serie)
        bType=false;
    if(mParam.typeConnexion==UDP)
        bType=true;

     qDebug()<<"InitSensor"<<mParam.Name<<mParam.PortSerie<<mParam.typeConnexion;
    ui->rad_Serie->setChecked(!bType);
    ui->rad_Udp->setChecked(bType);
    majConnecType();

    ui->cb_Baudrate->setCurrentIndex(ui->cb_Baudrate->findText(mParam.Baudrate));
    ui->sp_UDPPort->setValue(mParam.PortUDP);
    ui->le_IpCapteur->setText(mParam.ipAddress);
    majInfo();


}

bool SensorDialog::saveSensor(QString sName)
{

    QSettings settings;
    bool bChanged=false;

    if(ui->cb_Serial->currentText()!=mParam.PortSerie)
    {
        bChanged=true;
    }
    if(ui->cb_Baudrate->currentText()!=mParam.Baudrate)
        bChanged=true;
    if(ui->le_IpCapteur->text()!=mParam.ipAddress)
        bChanged=true;
    if(ui->sp_UDPPort->value()!=mParam.PortUDP)
        bChanged=true;
    ConnexionType bType=static_cast<ConnexionType>(majConnecType());
    if(bType!=mParam.typeConnexion)
        bChanged=true;

    mParam.Name=sName;
    mParam.PortSerie=ui->cb_Serial->currentText();
    mParam.Baudrate=ui->cb_Baudrate->currentText();
    mParam.typeConnexion=bType;
    mParam.ipAddress=ui->le_IpCapteur->text();
    mParam.PortUDP=ui->sp_UDPPort->value();


    settings.setValue(QString("%1/PortName").arg(sName),mParam.PortSerie);
    settings.setValue(QString("%1/Baudrate").arg(sName),mParam.Baudrate);
    settings.setValue(QString("%1/ConnecType").arg(sName),mParam.typeConnexion);
    settings.setValue(QString("%1/IpSensor").arg(sName),mParam.ipAddress);
    settings.setValue(QString("%1/PortUDP").arg(sName),mParam.PortUDP);

    qDebug()<<"SaveSensor"<<mParam.Name<<mParam.PortSerie;


    return bChanged;
}

bool SensorDialog::majConnecType()
{
    bool bType =false;
    if(ui->rad_Serie->isChecked())
    {
        bType=false;
        mParam.typeConnexion=Serie;
    }
    if(ui->rad_Udp->isChecked())
    {
            bType=true;
            mParam.typeConnexion=UDP;
    }


    ui->cb_Serial->setEnabled(!bType);
    ui->cb_Baudrate->setEnabled(!bType);
    ui->le_IpCapteur->setEnabled(bType);
    ui->sp_UDPPort->setEnabled(bType);

    return bType;

}

void SensorDialog::readData()
{
    if(mParam.typeConnexion==Serie)
    {
        QByteArray cBuf;

        for(int i=0;i<mSerialPort->bytesAvailable();i++)
        {
            cBuf=mSerialPort->read(1);

            if(cBuf=="$")
            {
                mTrameEnCours=cBuf;
            }
            else if (cBuf=="\n") {
                QString sTrame=mTrameEnCours+"\n";
                emit dataReceived(mParam.Name,sTrame);

                mTrameEnCours="";

            }
            else
            {
                mTrameEnCours=mTrameEnCours+cBuf;
            }
        }
    }

    if(mParam.typeConnexion==UDP)
    {
        while (mUdpSocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(mUdpSocket->pendingDatagramSize());
            mUdpSocket-> readDatagram(datagram.data(),datagram.size()); // datagram.data est un char
            QString sTrame=datagram.data();  // convertion qbytearray en string
            emit dataReceived(mParam.Name,sTrame); // transmission du signal avec la trame recu



        }
    }
}


