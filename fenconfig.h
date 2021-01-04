#ifndef FENCONFIG_H
#define FENCONFIG_H

#include <QWidget>
#include "sensordialog.h"


namespace Ui {
class fenConfig;
}


class fenConfig : public QWidget
{
    Q_OBJECT


public:
    explicit fenConfig(QWidget *parent = nullptr);
    ~fenConfig();



signals:

    void confChanged();
public slots:


    void showFen(bool bIsRunning);
    QList<SensorDialog*> getSensors();


private slots:
    void valider();

    void majInfo();
    void selRepArchi();
    void majConnecType();

private:
    Ui::fenConfig *ui;

    bool bConfChanged;

    SensorDialog* mSensor;
    SensorDialog* mDisplay;
    int mNbFiles;
    QString mRepArchi;

    bool mIsRunning;
    bool initRepArchi(QString sRep);
    void initFen();

    int mRatio;

};

#endif // FENCONFIG_H
