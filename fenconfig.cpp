#include "fenconfig.h"
#include "ui_fenconfig.h"


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

    mSensor=new SensorDialog;
    mDisplay=new SensorDialog;

    QObject::connect(this->ui->btnOk,SIGNAL(clicked()),this,SLOT(valider()));
    QObject::connect(this->ui->btn_Cancel,SIGNAL(clicked()),this,SLOT(close()));

    QObject::connect(ui->btn_Parcourir,&QToolButton::clicked,this,&fenConfig::selRepArchi);

    ui->lay_Sensor->addWidget(mSensor);
    ui->lay_Display->addWidget(mDisplay);
    initFen();




}

fenConfig::~fenConfig()
{
    delete ui;
}

void fenConfig::majInfo()
{


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
    QSettings settings;


    mNbFiles=settings.value("NbFiles",1).toInt();

    QString sDefPath=QStandardPaths::locate(QStandardPaths::HomeLocation,QString(),QStandardPaths::LocateDirectory)+"Barographe_Datas";
    mRepArchi=settings.value("RepArchi",sDefPath).toString();
    mRatio=settings.value("Ratio",1).toInt();

    ui->sp_NBJours->setValue(mNbFiles);


    mSensor->initSensor("Sensor");
    mDisplay->initSensor("Display");

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



    initRepArchi(mRepArchi);

}




void fenConfig::showFen(bool bIsRunning)
{
    mIsRunning=bIsRunning;
    initFen();


   // mSensor->majInfo();
  //  mDisplay->majInfo();
    this->show();
}

QList<SensorDialog *> fenConfig::getSensors()
{
    QList<SensorDialog*> list;
    list.append(mSensor);
    list.append(mDisplay);
    return list;
}




void fenConfig::valider()
{
    QSettings settings;

    if(mSensor->saveSensor("Sensor"))
    {
        bConfChanged=true;
    }

    if(mDisplay->saveSensor("Display"))
    {
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

