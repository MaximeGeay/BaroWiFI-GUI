#include "fenconfig.h"
#include "ui_fenconfig.h"

#include <QSerialPortInfo>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>


fenConfig::fenConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fenConfig)
{
    ui->setupUi(this);


    QObject::connect(this->ui->btnOk,SIGNAL(clicked()),this,SLOT(valider()));
    QObject::connect(this->ui->btn_Cancel,SIGNAL(clicked()),this,SLOT(close()));

    QObject::connect(ui->btn_Parcourir,&QToolButton::clicked,this,&fenConfig::selRepArchi);
    QObject::connect(ui->rad_Udp,&QRadioButton::toggled,this,&fenConfig::majConnecType);
    QObject::connect(ui->rad_Serie,&QRadioButton::toggled,this,&fenConfig::majConnecType);
}

fenConfig::~fenConfig()
{
    delete ui;
}

void fenConfig::majInfo()
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
        bool bFound=it.findNext(mPortName);
        if(bFound)
        {
            ui->cb_Serial->setCurrentIndex(ui->cb_Serial->findText(mPortName));
        }



}

void fenConfig::selRepArchi()
{
    QString sRep=QFileDialog::getExistingDirectory(this,QString("Sélectionner le répertoire d'archivage"),mRepArchi,QFileDialog::ShowDirsOnly);

    if(!sRep.isEmpty())
     {
        if(!initRepArchi(sRep))
         ui->le_RepArchi->setText(sRep);
    }

}

void fenConfig::majConnecType()
{
    if(ui->rad_Serie->isChecked())
        selConnecType(SensorDialog::Serie);
    if(ui->rad_Udp->isChecked())
        selConnecType(SensorDialog::UDP);
}

bool fenConfig::initRepArchi(QString sRep)
{
 QDir dir;
 if(!dir.mkpath(sRep))
 {
     QMessageBox::warning(this,"Répertoire d'archivage",QString("Création du répertoire d'archivage impossible : %1").arg(sRep));
     return false;
 }
 else {
     ui->le_RepArchi->setText(sRep);
     return true;
 }

}

void fenConfig::initFen()
{
    QSettings settings ("Barographe","BaroConfig");
    mConnec=settings.value("ConnecType",0).toBool();
    mPortName=settings.value("PortName","").toString();
    mBaudrate=settings.value("BaudRate","4800").toString();
    mNbFiles=settings.value("NbFiles",1).toInt();
    mIpSensor=settings.value("IpSensor","").toString();
    mPortUDP=settings.value("PortUDP",50000).toInt();


    QString sDefPath=QStandardPaths::locate(QStandardPaths::HomeLocation,QString(),QStandardPaths::LocateDirectory)+"Barographe_Datas";
    mRepArchi=settings.value("RepArchi",sDefPath).toString();
    mRatio=settings.value("Ratio",1).toInt();

    ui->sp_NBJours->setValue(mNbFiles);

    if(mConnec==SensorDialog::Serie)
        ui->rad_Serie->setChecked(true);

    if(mConnec==SensorDialog::UDP)
        ui->rad_Udp->setChecked(true);


    bConfChanged=false;

    switch (mRatio)
    {
    case 0:
           ui->comboRatio->setCurrentIndex(ui->comboRatio->findText("Aucune"));
        break;

        case 1:
        ui->comboRatio->setCurrentIndex(ui->comboRatio->findText("Toutes"));
        break;

    default:
        ui->comboRatio->setCurrentIndex(ui->comboRatio->findText(QString("1 sur %1").arg(mRatio)));
        break;
    }

    ui->cb_Baudrate->setCurrentIndex(ui->cb_Baudrate->findText(mBaudrate));

    ui->sp_UDPPort->setValue(mPortUDP);
    ui->le_IpCapteur->setText(mIpSensor);

    initRepArchi(mRepArchi);

}

void fenConfig::selConnecType(SensorDialog::ConnexionType bType)
{
    ui->cb_Serial->setEnabled(!bType);
    ui->cb_Baudrate->setEnabled(!bType);
    ui->le_IpCapteur->setEnabled(bType);
    ui->sp_UDPPort->setEnabled(bType);

    mConnec=bType;

}


void fenConfig::showFen(bool bIsRunning)
{
    mIsRunning=bIsRunning;
    initFen();


    this->majInfo();
    this->show();
}



void fenConfig::valider()
{
    QSettings settings ("Barographe","BaroConfig");


    if(mConnec!=settings.value("ConnecType").toBool())
    {
        settings.setValue("ConnecType",mConnec);
        bConfChanged=true;
    }

    if(ui->cb_Serial->currentText()!=mPortName)
    {
        mPortName=ui->cb_Serial->currentText();
        settings.setValue("PortName",mPortName);
        bConfChanged=true;

    }

    if(ui->cb_Baudrate->currentText()!=mBaudrate)
    {
        mBaudrate=ui->cb_Baudrate->currentText();
        settings.setValue("BaudRate",mBaudrate);
        bConfChanged=true;
    }

    if(ui->le_IpCapteur->text()!=mIpSensor)
    {
        mIpSensor=ui->le_IpCapteur->text();
        settings.setValue("IpSensor",mIpSensor);
        bConfChanged=true;
    }

    if(ui->sp_UDPPort->value()!=mPortUDP)
    {
        mPortUDP=ui->sp_UDPPort->value();
        settings.setValue("PortUDP",mPortUDP);
        bConfChanged=true;
    }





    if(ui->le_RepArchi->text()!=mRepArchi)
    {
        mRepArchi=ui->le_RepArchi->text();
        bConfChanged=true;
        settings.setValue("RepArchi",mRepArchi);
    }

    int nCurrentRatio=0;
    if(ui->comboRatio->currentText()=="Aucune")
        nCurrentRatio=0;
    if(ui->comboRatio->currentText()=="Toutes")
        nCurrentRatio=1;
    if(ui->comboRatio->currentText().section(" ",0,0).toInt()==1)
        nCurrentRatio=ui->comboRatio->currentText().section(" ",2).toInt();

    if(nCurrentRatio!=mRatio)
    {
        mRatio=nCurrentRatio;
        bConfChanged=true;
        settings.setValue("Ratio",mRatio);
    }

    if(ui->sp_NBJours->value()!=mNbFiles)
    {
        mNbFiles=ui->sp_NBJours->value();
        bConfChanged=true;
        settings.setValue("NbFiles",mNbFiles);
    }

    if(bConfChanged)
    {
        if(mIsRunning)
        {
       int nRep=QMessageBox::question(this,"Deconnexion nécessaire","La configuration a été modifiée, l'acquisition sera relancée.\r\nSouhaitez-vous continuer?",QMessageBox::Ok,QMessageBox::Cancel);
       if(nRep==QMessageBox::Ok)
       {
           emit confChanged();
            this->close();
       }
        }
        else {
            this->close();
        }
    }
    else {
        this->close();
    }




}

