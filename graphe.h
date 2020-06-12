#ifndef GRAPHE_H
#define GRAPHE_H

#include <QWidget>
#include <QDateTime>

#include "qcustomplot.h"

namespace Ui {
class Graphe;


}

class Graphe : public QWidget
{
    Q_OBJECT

public:
    explicit Graphe(QWidget *parent = nullptr);
    ~Graphe();

    struct stData
    {
        double temperature=0;
        double pression=0;
        double humidity=0;
        QDateTime dateHeure;

    };

    enum Type{
        Pression=0x01,Temperature=0x02,Humidity=0x03
    };

signals:

public slots:
    void ajoutData(Graphe::stData data);
     void initGraphe(int nDuree,int nGRFType);
     void setY(int nMin,int nMax);
     void setX(QDateTime dtDebut,QDateTime dtFin);
     void replot();
   QPair <QDateTime,double> debutGraphe();
    QPair <QDateTime,double> finGraphe();



private slots:
    void AfficheLabelInfo(QCPAbstractPlottable*curve, int i);
    void gestionBorneZoom (QWheelEvent* event);


    void razEchelleY();
private:
    Ui::Graphe *ui;


    QColor m_Coul;
    QString mLabel;

    int mTypeGraphe;
    int m_DureeGraphe;
    int mRangeMin;
    int mRangeMax;
    QVector<double> m_TabData;

    QVector<double> m_TabDate;

    QCPCurve *courbe;

    QCPItemText *LabelInfo;
    bool mIsInit;
};

#endif // GRAPHE_H



/*
#ifndef GRAPHE_H
#define GRAPHE_H

#include <QWidget>
#include <QDateTime>

#include "qcustomplot.h"

namespace Ui {
class graphe;
}

class graphe : public QWidget
{
    Q_OBJECT
public:
    explicit graphe(QWidget *parent = nullptr);


    struct stData
    {
        double temperature=0;
        double pression=0;
        QDateTime dtData;

    };

signals:

public slots:
    void ajoutData(graphe::stData data);

private slots:
    void AfficheLabelInfo(QCPAbstractPlottable*curve, int i);

private:

    void initGraphe(int nDuree);
    QColor m_CoulCap;
    QColor m_CoulPitch;
    QColor m_CoulRoll;

    int m_DureeGraphe;
    QVector<double> m_TabCap;
    QVector<double> m_TabPitch;
    QVector<double> m_TabRoll;
    QVector<double> m_TabDate;

    QCPCurve *courbeCap;
    QCPCurve *courbePitch;
    QCPCurve *courbeRoll;
    QCPItemText *LabelInfo;
};

#endif // GRAPHE_H*/
