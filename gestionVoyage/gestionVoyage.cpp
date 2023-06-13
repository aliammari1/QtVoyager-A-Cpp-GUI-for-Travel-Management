#include "gestionVoyage.h"
#include "ui_gestionVoyage.h"
#include "voyage.h"
#include <QTextDocument>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QPieSeries>
#include <QPieSlice>
#include "mybutton.h"

gestionVoyage::gestionVoyage(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::gestionVoyage)
{
    ui->setupUi(this);
    int ret = A.connect_arduino(); // lancer la connexion à arduino
    switch (ret)
    {
    case (0):
        qDebug() << "arduino is available and connected to : " << A.getarduino_port_name();
        break;
    case (1):
        qDebug() << "arduino is available but not connected to :" << A.getarduino_port_name();
        break;
    case (-1):
        qDebug() << "arduino is not available";
    }
    QObject::connect(A.getserial(), SIGNAL(readyRead()), this, SLOT(update_label())); // permet de lancer

    table();
    style();
    controle();
    charts();
    profit();
    graphics();
    //---------------initialisation de place de TabWidget------------------------------
    ui->tabWidget->setGeometry(QRect(0, 0, 1600, 1000));
    //---------------setting PushButton initialization color------------------------------
    ui->pushButton_ajouter->SetColor(QColor(255, 255, 255));
    ui->pushButton_clear->SetColor(QColor(255, 255, 255));
    ui->pushButton_update->SetColor(QColor(255, 255, 255));
    ui->pushButton_Search->SetColor(QColor(255, 255, 255));
    ui->pushButton_delete->SetColor(QColor(255, 255, 255));
    ui->label_moyen->setText(QString::number(voy.calculerCoutMoyen(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
    QTimer *timer = new QTimer();

    connect(timer, &QTimer::timeout, [=]()
            {
        ui->groupBox->setStyleSheet("QGroupBox {"
                                    "background-color: qlineargradient(spread:pad, x1:0.349, y1:0.336364, x2:"+QString::number(lineargradx)+", y2:"+QString::number(lineargrady)+", stop:0 rgba(5, 24, 122, 200), stop:1 rgba(5, 24, 255, 125));"
                                    "border-radius:20px;"
                                    "color:white;"
                                    "}");
        lineargradx-=0.001;
        lineargrady-=0.001;

        if(lineargradx <= 0.8 && lineargrady <= 0.8)
          {
            lineargradx = 1;
            lineargrady = 1;
        } });
    connect(timer, &QTimer::timeout, [=]()
            {
        ui->Information_Box->setStyleSheet("QGroupBox {"
                                    "background-color: qlineargradient(spread:pad, x1:0.349, y1:0.336364, x2:"+QString::number(lineargradx)+", y2:"+QString::number(lineargrady)+", stop:0 rgba(5, 24, 122, 200), stop:1 rgba(5, 24, 255, 125));"
                                    "border-radius:20px;"
                                    "color:white;"
                                    "}");

        lineargradx-=0.001;
        lineargrady-=0.001;

        if(lineargradx <= 0.8 && lineargrady <= 0.8)
        {
            lineargradx = 1;
            lineargrady = 1;
        } });
    connect(timer, &QTimer::timeout, [=]()
            {
        ui->groupBox_2->setStyleSheet("QGroupBox {"
                                    "background-color: qlineargradient(spread:pad, x1:0.349, y1:0.336364, x2:"+QString::number(lineargradx)+", y2:"+QString::number(lineargrady)+", stop:0 rgba(5, 24, 122, 200), stop:1 rgba(5, 24, 255, 125));"
                                    "border-radius:20px;"
                                    "color:white;"
                                    "}");

        lineargradx-=0.001;
        lineargrady-=0.001;

        if(lineargradx <= 0.8 && lineargrady <= 0.8)
        {
            lineargradx = 1;
            lineargrady = 1;
        } });
    connect(timer, &QTimer::timeout, [=]()
            {
        ui->groupBox_3->setStyleSheet("QGroupBox {"
                                    "background-color: qlineargradient(spread:pad, x1:0.349, y1:0.336364, x2:"+QString::number(lineargradx)+", y2:"+QString::number(lineargrady)+", stop:0 rgba(5, 24, 122, 200), stop:1 rgba(5, 24, 255, 125));"
                                    "border-radius:20px;"
                                    "color:white;"
                                    "}");

        lineargradx-=0.001;
        lineargrady-=0.001;

        if(lineargradx <= 0.8 && lineargrady <= 0.8)
        {
            lineargradx = 1;
            lineargrady = 1;
        } });
    timer->start(1000);
}

gestionVoyage::~gestionVoyage()
{
    delete ui;
}

void gestionVoyage::on_pushButton_ajouter_clicked()
{
    QString flightref = ui->lineEdit_flightref->text();
    QString lieudep = ui->comboBox_lieudep->currentText();
    QString lieuarr = ui->comboBox_lieuarr->currentText();
    QString airline = ui->comboBox_Airline->currentText();
    QDate datedep = ui->dateEdit_datedep->date();
    QDate datearr = ui->dateEdit_datearr->date();
    float montant = ui->lineEdit_montant->text().replace(",", ".", Qt::CaseSensitive).toFloat();
    int nbper = ui->lineEdit_nbper->text().toInt();
    Voyage v(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper);
    bool test = false;
    if (v.search(flightref))
        test = v.ajouter();
    if (test)
    {
        profit();
        table();
        charts();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Ajout effectué.\n"
                                                                         "Click Cancel to exit."),
                                 QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("Ajout non effectué.\n"
                                                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
}
//-----------------------------delete button--------------------------------
void gestionVoyage::on_pushButton_delete_clicked()
{
    QString ref = ui->lineEdit_delete->text();
    bool test = voy.supprimer(ref);
    if (test && !ui->lineEdit_delete->text().isEmpty())
    {
        profit();
        table();
        charts();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Suppression effectué.\n"
                                                                         "Click Cancel to exit."),
                                 QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("Suppression non effectué.\n"
                                                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
}
//-------------------------------update button---------------------------------
void gestionVoyage::on_pushButton_update_clicked()
{
    QString ref = ui->lineEdit_flightref->text();
    QString flightref = ui->lineEdit_flightref->text();
    QString lieudep = ui->comboBox_lieudep->currentText();
    QString lieuarr = ui->comboBox_lieuarr->currentText();
    QString airline = ui->comboBox_Airline->currentText();
    QDate datedep = ui->dateEdit_datedep->date();
    QDate datearr = ui->dateEdit_datearr->date();
    float montant = ui->lineEdit_montant->text().replace(",", ".", Qt::CaseSensitive).toFloat();
    int nbper = ui->lineEdit_nbper->text().toInt();
    Voyage v(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper);
    bool test = v.update(ref);
    if (test)
    {
        profit();
        charts();
        table();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("update effectué.\n"
                                                                         "Click Cancel to exit."),
                                 QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("update non effectué.\n"
                                                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
}

void gestionVoyage::controle()
{
    QRegularExpression rx("\\b[a-zA-Z0-9]{1,8}\\b");
    ui->lineEdit_flightref->setValidator(new QRegularExpressionValidator(rx, this));
    // ui->lineEdit_flightref->setInputMask("NNNNNNNN");
    ui->lineEdit_nbper->setValidator(new QIntValidator(0, 9999, this));
    ui->lineEdit_montant->setValidator(new QDoubleValidator(0, 99999999, 2, this));
}

void gestionVoyage::on_dateEdit_datedep_userDateChanged(const QDate &date)
{
    ui->dateEdit_datearr->setDate(date);
    ui->dateEdit_datearr->setMinimumDate(date);
}

void gestionVoyage::on_pushButton_Search_clicked()
{
    if (voy.search(ui->lineEdit_flightref->text()) && !ui->lineEdit_flightref->text().isEmpty())
    {
        ui->comboBox_lieudep->setCurrentText(voy.getLieudep());
        ui->comboBox_lieuarr->setCurrentText(voy.getLieuarr());
        ui->comboBox_Airline->setCurrentText(voy.getAirline());
        ui->dateEdit_datedep->setDate(voy.getDatedep());
        ui->dateEdit_datearr->setDate(voy.getDatearr());
        ui->lineEdit_montant->setText(QString::number(voy.getMontant()));
        ui->lineEdit_nbper->setText(QString::number(voy.getNbper()));
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("recherche effectué.\n"
                                                                         "Click Cancel to exit."),
                                 QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("recherche non effectué.\n"
                                                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
}

void gestionVoyage::on_pushButton_3_clicked()
{
    /*
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("fichier.pdf");
    QPainter painter;
    painter.begin(&printer);
    ui->tableView->render(&painter,QPoint(30,30));
    painter.end();
    */
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("voyage.pdf");
    QTextDocument doc;

    QList<Voyage> V = voy.getDatabaseValues_tri("FLIGHTREF");
    QString h("");
    h = "<html>"
        "<body>"
        "<table border='1' cellspacing='5' width='100%' cellpadding='10'>"
        "<tr>"
        "<th>flightref</th>"
        "<th>lieudep</th>"
        "<th>lieuarr</th>"
        "<th>datedep</th>"
        "<th>datearr</th>"
        "<th>airline</th>"
        "<th>montant</th>"
        "<th>nbper</th>"
        "</tr>";
    for (int it = 0; it < V.size(); it++)
    {
        h += "<tr>"
             "<td  align='center'>" +
             V[it].getFlightref() + "</td>"
                                    "<td  align='center'>" +
             V[it].getLieudep() + "</td>"
                                  "<td  align='center'>" +
             V[it].getLieuarr() + "</td>"
                                  "<td  align='center'>" +
             V[it].getDatearr().toString("dd/MM/yyyy") + "</td>"
                                                         "<td  align='center'>" +
             V[it].getDatedep().toString("dd/MM/yyyy") + "</td>"
                                                         "<td  align='center'>" +
             V[it].getAirline() + "</td>"
                                  "<td  align='center'>" +
             QString::number(V[it].getMontant()) + "</td>"
                                                   "<td  align='center'>" +
             QString::number(V[it].getNbper()) + "</td>"
                                                 "</tr>";
    }

    h += "</table>"
         "</body>"
         "</html>";
    doc.setHtml(h);
    doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
    doc.print(&printer);
    QDesktopServices::openUrl(QUrl::fromLocalFile("voyage.pdf"));
}

void gestionVoyage::charts()
{
    QHBoxLayout *layout = new QHBoxLayout();

    qDeleteAll(ui->widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    QPieSeries *series = new QPieSeries();
    int nb_tunisia = voy.getNeededDatabaseValue("LIEUDEP", "Tunisia");
    int nb_france = voy.getNeededDatabaseValue("LIEUDEP", "France");
    int nb_uk = voy.getNeededDatabaseValue("LIEUDEP", "United Kingdom");
    QPieSlice *s1 = new QPieSlice();
    QPieSlice *s2 = new QPieSlice();
    QPieSlice *s3 = new QPieSlice();

    s1->setValue(nb_tunisia);
    s1->setLabel("Tunisia");
    s2->setValue(nb_france);
    s2->setLabel("France");
    s3->setValue(nb_uk);
    s3->setLabel("UnitedKingdom");
    series->append({s1, s2, s3});
    /*
    series->append(s1);
    series->append(s2);
    series->append(s3);
    */
    series->setLabelsVisible();
    series->setPieSize(nb_uk + nb_france + nb_tunisia);

    QChart *chart = new QChart();

    chart->removeAllSeries();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->repaint();
    chartView->update();
    chartView->resize(460, 230);
    layout->addWidget(chartView);
    ui->widget->setLayout(layout);
    ui->widget->layout()->deleteLater();
}

void gestionVoyage::style()
{
    ui->centralwidget->setStyleSheet("QWidget#tab {"
                                     "background-color:#1F408E;"
                                     "}"
                                     "QGroupBox {"
                                     "background-color: qlineargradient(spread:pad, x1:0.349, y1:0.336364, x2:1, y2:1, stop:0 rgba(5, 24, 122, 200), stop:1 rgba(5, 24, 150, 125));"
                                     "border-radius:20px;"
                                     "color:white;"
                                     "}"
                                     "QLineEdit {"
                                     "border: none;"
                                     "border-radius: 10px;"
                                     "color:white;"
                                     "background-color:#0C1B64;"
                                     "}"
                                     "QLabel {"
                                     "color:white;"
                                     "}"
                                     "QScrollArea #scrollAreaWidgetContents {"
                                     "background-color:#05187A;"
                                     "}");
}

QList<QLabel *> gestionVoyage::HeadBar()
{
    QList<QLabel *> la = {};
    QStringList s = {
        "flightref", "lieudep", "lieuarr", "datedep", "datearr", "airline", "montant", "nbper"};

    int x = 120;
    int y = 0;
    for (int j = 0; j < 8; j++)
    {
        QLabel *l = new QLabel();

        l->setObjectName("la_" + QString::number(j));
        l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        l->setText(s[j]);
        l->setGeometry(QRect(10 + y, 40, 90, 30));
        l->setStyleSheet("QLabel {"
                         "height: 30px;"
                         "border: none;"
                         "background-color: rgb(57, 15, 194);"
                         "}"
                         "QLabel#la_0 {"
                         "border-bottom-left-radius:10px;"
                         "border-top-left-radius:10px;"
                         "}"
                         "QLabel#la_7 {"
                         "border-bottom-right-radius:10px;"
                         "border-top-right-radius:10px;"
                         "}");
        y += x;
        la.append(l);
    }
    return la;
}

void gestionVoyage::table()
{
    QGridLayout *layout = new QGridLayout();
    QList<Voyage> V = voy.getDatabaseValues();
    int x = 120;
    int y = 0;
    QList<QLabel *> la = HeadBar();
    for (int j = 0; j < 8; j++)
    {
        layout->addWidget(la[j], 0, j);
    }
    int size = voy.row_number();
    QLineEdit *l[size][8];
    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            l[i][j] = new QLineEdit();
            l[i][j]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            controleur_grid(i, j, l[i][j], V);
            l[i][j]->setGeometry(QRect(10 + y, 40, 90, 30));
            l[i][j]->setStyleSheet("QLineEdit {"
                                   "height: 30px;"
                                   "}");
            l[i][j]->setReadOnly(true);
            layout->addWidget(l[i][j], i + 1, j);
            y += x;
        }
    }
    layout->setRowMinimumHeight(0, 30);
    layout->setHorizontalSpacing(0);
    layout->setRowStretch(size + 1, 1);
    layout->setVerticalSpacing(10);
    ui->scrollAreaWidgetContents->setLayout(layout);
    ui->scrollAreaWidgetContents->layout()->deleteLater();
}

void gestionVoyage::controleur_grid(int i, int j, QLineEdit *l, QList<Voyage> V)
{

    l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if (j == 0)
        l->setText(V[i].getFlightref());
    else if (j == 1)
        l->setText(V[i].getLieudep());
    else if (j == 2)
        l->setText(V[i].getLieuarr());
    else if (j == 3)
        l->setText(V[i].getDatedep().toString("dd/MM/yyyy"));
    else if (j == 4)
        l->setText(V[i].getDatearr().toString("dd/MM/yyyy"));
    else if (j == 5)
        l->setText(V[i].getAirline());
    else if (j == 6)
        l->setText(QString::number(V[i].getMontant()));
    else
        l->setText(QString::number(V[i].getNbper()));
}

void gestionVoyage::on_pushButton_clear_clicked()
{
    ui->lineEdit_flightref->clear();
    ui->comboBox_lieudep->setCurrentIndex(0);
    ui->comboBox_lieuarr->setCurrentIndex(0);
    ui->dateEdit_datedep->setDate(QDate(2000, 1, 1));
    ui->dateEdit_datearr->setDate(QDate(2000, 1, 1));
    ui->comboBox_Airline->setCurrentIndex(0);
    ui->lineEdit_montant->clear();
    ui->lineEdit_nbper->clear();
}

void gestionVoyage::on_lineEdit_rechercher_textChanged(const QString &arg1)
{
    QList<Voyage> V = voy.getDatabaseValues_recherche(arg1);

    QGridLayout *layout = new QGridLayout();
    int x = 120;
    int y = 0;
    QList<QLabel *> la = HeadBar();
    for (int j = 0; j < 8; j++)
    {
        layout->addWidget(la[j], 0, j);
    }

    QSqlQuery query;
    int size = 0;
    query.exec("SELECT * FROM VOYAGES WHERE (FLIGHTREF LIKE '" + arg1 + "%')"
                                                                        "OR(LIEUDEP LIKE '" +
               arg1 + "%')"
                      "OR(LIEUARR LIKE '" +
               arg1 + "%')"
                      "OR(DATEARR LIKE '" +
               arg1 + "%')"
                      "OR(DATEDEP LIKE '" +
               arg1 + "%')"
                      "OR(AIRLINE LIKE '" +
               arg1 + "%')"
                      "OR(MONTANT LIKE '" +
               arg1 + "%')"
                      "OR(NBPER LIKE '" +
               arg1 + "%')");
    while (query.next())
        size++;
    QLineEdit *l[size][8];
    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            l[i][j] = new QLineEdit();
            l[i][j]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            controleur_grid(i, j, l[i][j], V);
            l[i][j]->setGeometry(QRect(10 + y, 40, 90, 30));
            l[i][j]->setStyleSheet("QLineEdit {"
                                   "height: 30px;"
                                   "}");
            l[i][j]->setReadOnly(true);
            layout->addWidget(l[i][j], i + 1, j);
            y += x;
        }
    }
    layout->setRowMinimumHeight(0, 30);
    layout->setHorizontalSpacing(0);
    layout->setRowStretch(size + 1, 1);
    layout->setVerticalSpacing(10);
    ui->scrollAreaWidgetContents->setLayout(layout);
    ui->scrollAreaWidgetContents->layout()->deleteLater();
}

void gestionVoyage::on_comboBox_sort_currentTextChanged(const QString &arg1)
{
    QGridLayout *layout = new QGridLayout();
    QList<Voyage> V = voy.getDatabaseValues_tri(arg1);
    qDebug() << ui->comboBox_sort->currentText();
    int x = 120;
    int y = 0;
    QList<QLabel *> la = HeadBar();
    for (int j = 0; j < 8; j++)
    {
        layout->addWidget(la[j], 0, j);
    }
    int size = voy.row_number();
    QLineEdit *l[size][8];
    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            l[i][j] = new QLineEdit();
            l[i][j]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            controleur_grid(i, j, l[i][j], V);
            l[i][j]->setGeometry(QRect(10 + y, 40, 90, 30));
            l[i][j]->setStyleSheet("QLineEdit {"
                                   "height: 30px;"
                                   "}");
            l[i][j]->setReadOnly(true);
            layout->addWidget(l[i][j], i + 1, j);
            y += x;
        }
    }
    layout->setRowMinimumHeight(0, 30);
    layout->setHorizontalSpacing(0);
    layout->setRowStretch(size + 1, 1);
    layout->setVerticalSpacing(10);
    ui->scrollAreaWidgetContents->setLayout(layout);
    ui->scrollAreaWidgetContents->layout()->deleteLater();
}

void gestionVoyage::on_comboBox_lieu_From_currentTextChanged(const QString &arg1)
{
    ui->label_moyen->setText(QString::number(voy.calculerCoutMoyen(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
}

void gestionVoyage::on_comboBox_lieu_To_currentTextChanged(const QString &arg1)
{
    ui->label_moyen->setText(QString::number(voy.calculerCoutMoyen(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
}

void gestionVoyage::profit()
{
    QSqlQuery query;
    query.exec("SELECT MONTANT,NBPER FROM VOYAGES");
    int size = 0;
    while (query.next())
        size++;
    query.first();
    int it = 0;
    float earn = 0;
    while (it < size)
    {
        earn += (float)query.value(0).toFloat() * query.value(1).toInt();
        query.next();
        it++;
    }
    ui->label_profit->setText(QString::number(earn));
}

void gestionVoyage::update_label()
{
    data = A.read_from_arduino();
    data.remove(0, data.length() - 11);
    qDebug() << data;
    if (data != "" && data.length() > 10)
    {
        QSqlQuery query;
        query.exec("SELECT * FROM VOYAGEURS WHERE RFID_CARD='" + data + "'");
        query.first();
        qDebug() << query.value(1).toByteArray();
        A.write_to_arduino(query.value(1).toByteArray());
        ui->lineEdit_rfid->setText(data);
    }
}

void gestionVoyage::graphics()
{
    QGraphicsScene *scene;
    QGraphicsTextItem *text;
    QGraphicsPixmapItem *pixmap;
    QGraphicsEllipseItem *circle[50];
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->setBackgroundBrush(QBrush(QColor(5, 24, 150)));
    pixmap = scene->addPixmap(QPixmap("C:\\Users\\aliam\\OneDrive\\Images\\f1.png"));
    pixmap->setPos(100, 0);
    int x = 10;
    for (int i = 0; i < 50; i++)
    {
        circle[i] = scene->addEllipse(-520, 0, 10, 10, QPen(Qt::white), Qt::white);
        circle[i]->setPos(QPoint(-10 - x, ui->graphicsView->geometry().height() / 2 - 6));
        x -= 20;
    }
    text = scene->addText("Tayerni", QFont("Outfit", 18));
    text->setDefaultTextColor(QColor(255, 255, 255));
    text->setPos(QPoint(-ui->graphicsView->geometry().width() / 2, 0));

    QTimer *t = new QTimer();
    connect(t, &QTimer::timeout, [=]()
            {
        QTimeLine *timer = new QTimeLine(5000);
        QGraphicsItemAnimation *animation_plane = new QGraphicsItemAnimation();
            animation_plane->setItem(pixmap);
            animation_plane->setTimeLine(timer);
            animation_plane->setTranslationAt(0,100,0);
            animation_plane->setTranslationAt(0.5,-800,00);
            animation_plane->setTranslationAt(1,100,0);

            timer->start(); });
    t->start(5000);
}
