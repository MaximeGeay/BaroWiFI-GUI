#include "graphe.h"
#include "ui_graphe.h"

Graphe::Graphe(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphe)
{
    ui->setupUi(this);

     m_Coul=Qt::blue;
     mLabel="Courbe";
     mTypeGraphe=Type::Pression;
     courbe = new QCPCurve(ui->GRF->xAxis, ui->GRF->yAxis);

     LabelInfo=new QCPItemText(ui->GRF);
     mIsInit=false;

}

Graphe::~Graphe()
{
    delete ui;
}

void Graphe::ajoutData(Graphe::stData data)
{
    double dHeure=data.dateHeure.toTime_t();

    if(mTypeGraphe==Type::Pression)
        m_TabData.append(data.pression);
    if(mTypeGraphe==Type::Temperature)
        m_TabData.append(data.temperature);
    if(mTypeGraphe==Type::Humidity)
        m_TabData.append(data.humidity);

    m_TabDate.append(dHeure);



    /*if(m_TabDate.first()<dHeure-m_DureeGraphe*60)
    {
        m_TabData.removeFirst();
        m_TabDate.removeFirst();
       // ui->grfCap->xAxis->setRange(m_TabData1X.first(),m_TabData1X.last());
    }
    else
    {
       // ui->grfCap->xAxis->setRange(m_TabData1X.last()-m_DureeGraphe*60,m_TabData1X.last());

    }*/

    courbe->setData(m_TabDate,m_TabData);


    ui->GRF->replot();
}

void Graphe::AfficheLabelInfo(QCPAbstractPlottable *curve, int i)
{


    double dData=0;
    double dHeure=0;
    double dPosY=ui->GRF->yAxis->range().upper;
    QString sSource;


        LabelInfo->setColor(m_Coul);
        sSource=mLabel;


    dData=curve->interface1D()->dataMainValue(i);
    dHeure=curve->interface1D()->dataMainKey(i);

    QDateTime dtHeure=QDateTime::fromTime_t(dHeure);
    dtHeure.setTimeZone(QTimeZone::utc());
    LabelInfo->position->setCoords(dHeure,dPosY);
    LabelInfo->setText(sSource+"\r\n"+QString::number(dData,'f',1).rightJustified(7,' ')+"\r\n"+dtHeure.toString("HH:mm:ss")+"\r\n"+QDateTime::fromTime_t(dHeure).toString("dd/MM/yyyy"));
    LabelInfo->setVisible(true);

}

void Graphe::gestionBorneZoom(QWheelEvent *event)
{
    QCPRange range=ui->GRF->yAxis->range();
    if(range.upper>mRangeMax)
        ui->GRF->yAxis->setRangeUpper(mRangeMax);
    if(range.lower<mRangeMin)
        ui->GRF->yAxis->setRangeLower(mRangeMin);

}

void Graphe::razEchelleY()
{
    if(mIsInit)
    {
        setY(mRangeMin,mRangeMax);
        this->replot();
    }
}

void Graphe::initGraphe(int nDuree,int nGRFType)
{
    m_DureeGraphe=nDuree;
    if(nGRFType==Type::Pression || nGRFType==Type::Temperature || nGRFType==Type::Humidity)
        mTypeGraphe=nGRFType;

    if(mTypeGraphe==Type::Pression)
    {
        m_Coul=Qt::blue;
        mRangeMin=950;
        mRangeMax=1050;
        mLabel="Pression (HPa)";
    }

    if(mTypeGraphe==Type::Temperature)
    {
        m_Coul=Qt::red;
        mRangeMin=0;
        mRangeMax=40;
        mLabel="Température (°C)";
    }

    if(mTypeGraphe==Type::Humidity)
    {
        m_Coul=Qt::darkGreen;
        mRangeMin=0;
        mRangeMax=100;
        mLabel="Humidité (%)";
    }


    this->setWindowTitle(mLabel);

    ui->GRF->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectPlottables);
    ui->GRF->setSelectionTolerance(20);



    ui->GRF->axisRect()->setRangeZoom(Qt::Vertical);

    //ui->GRF->axisRect()->setRangeZoomAxes(ui->GRF->xAxis,ui->GRF->yAxis);
     ui->GRF->axisRect()->setRangeDragAxes(ui->GRF->xAxis,ui->GRF->yAxis);

       setY(mRangeMin,mRangeMax);

       QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
       dateTicker->setDateTimeSpec(Qt::TimeSpec::UTC);
       dateTicker->setDateTimeFormat("HH:mm:ss");
       ui->GRF->xAxis->setTicker(dateTicker);

       ui->GRF->xAxis->setLabel("Heure UTC");
       ui->GRF->yAxis->setLabel(mLabel);


       QPen pen;
       pen.setWidth(2);
       pen.setColor(m_Coul);
       courbe->setPen(pen);


       courbe->setName(mLabel);



       ui->GRF->legend->setVisible(true);


       QDateTime dtHeureDebut=QDateTime::currentDateTimeUtc();
       QDateTime dtHeureFin=dtHeureDebut.addSecs(m_DureeGraphe*3600);
       setX(dtHeureDebut,dtHeureFin);

       LabelInfo->setPositionAlignment(Qt::AlignHCenter|Qt::AlignTop);
       LabelInfo->position->setType(QCPItemPosition::ptPlotCoords);
       LabelInfo->setVisible(false);

       QObject::connect(ui->GRF,SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)),this,SLOT(AfficheLabelInfo(QCPAbstractPlottable*,int)));

       QObject::connect(ui->btn_RazYAxis,&QPushButton::clicked,this,&Graphe::razEchelleY);
       QObject::connect(ui->GRF,&QCustomPlot::mouseWheel,this,&Graphe::gestionBorneZoom);

       mIsInit=true;

}

void Graphe::setY(int nMin, int nMax)
{
    ui->GRF->yAxis->setRange(nMin,nMax);


}

void Graphe::setX(QDateTime dtDebut, QDateTime dtFin)
{
    dtDebut.setTimeZone(QTimeZone::utc());
    dtFin.setTimeZone(QTimeZone::utc());
    double dDebut=dtDebut.toTime_t();

    double dFin=dtFin.toTime_t();
    ui->GRF->xAxis->setRange(dDebut,dFin);

}

void Graphe::replot()
{
    ui->GRF->replot();
}

QPair <QDateTime,double> Graphe::debutGraphe()
{
    double dHeure=0;
    double dData=0;
    if(!m_TabDate.isEmpty())
        dHeure=m_TabDate.first();
    QDateTime dtHeure=QDateTime::fromTime_t(dHeure);
    dtHeure=dtHeure.toUTC();

    if(!m_TabData.isEmpty())
        dData=m_TabData.first();

    return qMakePair(dtHeure,dData);
}

QPair<QDateTime, double> Graphe::finGraphe()
{
    double dHeure=0;
     double dData=0;
    if(!m_TabDate.isEmpty())
        dHeure=m_TabDate.last();
    QDateTime dtHeure=QDateTime::fromTime_t(dHeure);
   dtHeure=dtHeure.toUTC();

    if(!m_TabData.isEmpty())
        dData=m_TabData.first();

    return qMakePair(dtHeure,dData);

}




