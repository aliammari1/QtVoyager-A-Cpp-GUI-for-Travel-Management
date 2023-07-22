#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mybutton.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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
    for (auto &button : std::array<MyButton *, 5>{ui->pushButton_ajouter, ui->pushButton_clear, ui->pushButton_update, ui->pushButton_Search, ui->pushButton_delete})
    {
        button->SetColor(QColor(255, 255, 255));
    }
    ui->label_moyen->setText(QString::number(voy.calculerCoutMoyen(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
    QTimer *timer = new QTimer();

    connect(timer, &QTimer::timeout, [=]()
            {
        for (auto& groupBox : std::array<QGroupBox*,4>{ui->groupBox, ui->Information_Box, ui->groupBox_2, ui->groupBox_3})
        {
            groupBox->setStyleSheet("QGroupBox {"      "background-color: qlineargradient(spread:pad, x1:0.349, y1:0.336364, x2:"+QString::number(lineargradx)+", y2:"+QString::number(lineargrady)+", stop:0 rgba(5, 24, 122, 200), stop:1 rgba(5, 24, 255, 125));"      "border-radius:20px;"      "color:white;"      "}");
        };

        lineargradx-=0.001;
        lineargrady-=0.001;

        if(lineargradx <= 0.8 && lineargrady <= 0.8)
        {
            lineargradx = 1;
            lineargrady = 1;
        } });

    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ajouter_clicked()
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
    bool test = v.ajouter();
    if (test)
    {
        profit();
        table();
        charts();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Ajout effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("Ajout non effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
}
//-----------------------------delete button--------------------------------
void MainWindow::on_pushButton_delete_clicked()
{
    QString ref = ui->lineEdit_delete->text();
    bool test = voy.supprimer(ref);
    if (test && !ui->lineEdit_delete->text().isEmpty())
    {
        profit();
        table();
        charts();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Suppression effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("Suppression non effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
}
//-------------------------------update button---------------------------------
void MainWindow::on_pushButton_update_clicked()
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
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("update effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("update non effectué.\n"
                                                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
}

void MainWindow::controle()
{
    QRegularExpression rx("\\b[a-zA-Z0-9]{1,8}\\b");
    ui->lineEdit_flightref->setValidator(new QRegularExpressionValidator(rx, this));
    // ui->lineEdit_flightref->setInputMask("NNNNNNNN");
    ui->lineEdit_nbper->setValidator(new QIntValidator(0, 9999, this));
    ui->lineEdit_montant->setValidator(new QDoubleValidator(0, 99999999, 2, this));
}

void MainWindow::on_dateEdit_datedep_userDateChanged(const QDate &date)
{
    ui->dateEdit_datearr->setDate(date);
    ui->dateEdit_datearr->setMinimumDate(date);
}

void MainWindow::on_pushButton_Search_clicked()
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
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("recherche effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("recherche non effectué.\n"
                                                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
}

void MainWindow::on_pushButton_3_clicked()
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

void MainWindow::charts()
{

    qDeleteAll(ui->widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));

    QPieSeries *series = new QPieSeries();
    series->append("Tunisia", voy.getNeededDatabaseValue("LIEUDEP", "Tunisia"));
    series->append("France", voy.getNeededDatabaseValue("LIEUDEP", "France"));
    series->append("United Kingdom", voy.getNeededDatabaseValue("LIEUDEP", "United Kingdom"));
    series->setLabelsVisible();
    series->setPieSize(300);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Number of flights by country");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(460, 230);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(chartView);
    ui->widget->setLayout(layout);
}

void MainWindow::style()
{
    QString styleSheet = "QWidget#tab { background-color: #1F408E; }"
                         "QGroupBox { background-color: qlineargradient(spread: pad, x1: 0.349, y1: 0.336364, x2: 1, y2: 1, stop: 0 rgba(5, 24, 122, 200), stop: 1 rgba(5, 24, 150, 125)); border-radius: 20px; color: white; }"
                         "QLineEdit { border: none; border-radius: 10px; color: white; background-color: #0C1B64; }"
                         "QLabel { color: white; }"
                         "QScrollArea #scrollAreaWidgetContents { background-color: #05187A; }";
    ui->centralwidget->setStyleSheet(styleSheet);
}

void MainWindow::on_pushButton_clear_clicked()
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

QList<QLabel *> MainWindow::createHeadBar()
{
    QStringList labels = {"flightref", "lieudep", "lieuarr", "datedep", "datearr", "airline", "montant", "nbper"};
    QList<QLabel *> headerLabels;
    for (int i = 0; i < labels.size(); i++)
    {
        QLabel *label = new QLabel(labels[i]);
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        label->setStyleSheet("QLabel {"
                             "height: 30px;"
                             "border: none;"
                             "background-color: rgb(57, 15, 194);"
                             "}"
                             "QLabel:first-child {"
                             "border-bottom-left-radius:10px;"
                             "border-top-left-radius:10px;"
                             "}"
                             "QLabel:last-child {"
                             "border-bottom-right-radius:10px;"
                             "border-top-right-radius:10px;"
                             "}");
        headerLabels.append(label);
    }
    return headerLabels;
}

void MainWindow::table()
{
    QList<Voyage> voyages = voy.getDatabaseValues();
    int rowHeight = 30;
    int columnWidth = 120;

    QGridLayout *layout = new QGridLayout();
    QList<QLabel *> headerLabels = createHeadBar();

    for (int i = 0; i < headerLabels.size(); i++)
    {
        layout->addWidget(headerLabels[i], 0, i);
    }

    for (int i = 0; i < voyages.size(); i++)
    {
        QStringList voyageData = {
            voyages[i].getFlightref(),
            voyages[i].getLieudep(),
            voyages[i].getLieuarr(),
            voyages[i].getDatedep().toString("dd/MM/yyyy"),
            voyages[i].getDatearr().toString("dd/MM/yyyy"),
            voyages[i].getAirline(),
            QString::number(voyages[i].getMontant()),
            QString::number(voyages[i].getNbper())};

        for (int j = 0; j < 8; j++)
        {
            QLineEdit *lineEdit = new QLineEdit(voyageData[j]);
            lineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lineEdit->setStyleSheet("QLineEdit {"
                                    "height: 30px;"
                                    "}");
            lineEdit->setReadOnly(true);

            layout->addWidget(lineEdit, i + 1, j);
        }
    }

    layout->setRowMinimumHeight(0, rowHeight);
    layout->setHorizontalSpacing(0);
    layout->setRowStretch(voyages.size() + 1, 1);
    layout->setVerticalSpacing(10);

    ui->scrollAreaWidgetContents->setLayout(layout);
    ui->scrollAreaWidgetContents->layout()->deleteLater();
}

void MainWindow::on_lineEdit_rechercher_textChanged(const QString &arg1)
{
    int size = 0;
    QList<Voyage> voyages = voy.getDatabaseValues_recherche(arg1, &size);
    int rowHeight = 30;
    int columnWidth = 120;

    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));

    QGridLayout *layout = new QGridLayout();
    QList<QLabel *> headerLabels = createHeadBar();

    for (int i = 0; i < headerLabels.size(); i++)
    {
        layout->addWidget(headerLabels[i], 0, i);
    }

    for (int i = 0; i < voyages.size(); i++)
    {
        QStringList voyageData = {
            voyages[i].getFlightref(),
            voyages[i].getLieudep(),
            voyages[i].getLieuarr(),
            voyages[i].getDatedep().toString("dd/MM/yyyy"),
            voyages[i].getDatearr().toString("dd/MM/yyyy"),
            voyages[i].getAirline(),
            QString::number(voyages[i].getMontant()),
            QString::number(voyages[i].getNbper())};

        for (int j = 0; j < 8; j++)
        {
            QLineEdit *lineEdit = new QLineEdit(voyageData[j]);
            lineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lineEdit->setStyleSheet("QLineEdit {"
                                    "height: 30px;"
                                    "}");
            lineEdit->setReadOnly(true);

            layout->addWidget(lineEdit, i + 1, j);
        }
    }

    layout->setRowMinimumHeight(0, rowHeight);
    layout->setHorizontalSpacing(0);
    layout->setRowStretch(voyages.size() + 1, 1);
    layout->setVerticalSpacing(10);

    ui->scrollAreaWidgetContents->setLayout(layout);
    ui->scrollAreaWidgetContents->layout()->deleteLater();
}

void MainWindow::on_comboBox_sort_currentTextChanged(const QString &arg1)
{
    QList<Voyage> voyages = voy.getDatabaseValues_tri(arg1);
    int rowHeight = 30;
    int columnWidth = 120;

    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));

    QGridLayout *layout = new QGridLayout();
    QList<QLabel *> headerLabels = createHeadBar();

    for (int i = 0; i < headerLabels.size(); i++)
    {
        layout->addWidget(headerLabels[i], 0, i);
    }

    for (int i = 0; i < voyages.size(); i++)
    {
        QStringList voyageData = {
            voyages[i].getFlightref(),
            voyages[i].getLieudep(),
            voyages[i].getLieuarr(),
            voyages[i].getDatedep().toString("dd/MM/yyyy"),
            voyages[i].getDatearr().toString("dd/MM/yyyy"),
            voyages[i].getAirline(),
            QString::number(voyages[i].getMontant()),
            QString::number(voyages[i].getNbper())};

        for (int j = 0; j < 8; j++)
        {
            QLineEdit *lineEdit = new QLineEdit(voyageData[j]);
            lineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lineEdit->setStyleSheet("QLineEdit {"
                                    "height: 30px;"
                                    "}");
            lineEdit->setReadOnly(true);

            layout->addWidget(lineEdit, i + 1, j);
        }
    }

    layout->setRowMinimumHeight(0, rowHeight);
    layout->setHorizontalSpacing(0);
    layout->setRowStretch(voyages.size() + 1, 1);
    layout->setVerticalSpacing(10);

    ui->scrollAreaWidgetContents->setLayout(layout);
    ui->scrollAreaWidgetContents->layout()->deleteLater();
}

void MainWindow::on_comboBox_lieu_From_currentTextChanged(const QString &arg1)
{
    ui->label_moyen->setText(QString::number(voy.calculerCoutMoyen(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
}

void MainWindow::on_comboBox_lieu_To_currentTextChanged(const QString &arg1)
{
    ui->label_moyen->setText(QString::number(voy.calculerCoutMoyen(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
}

void MainWindow::profit()
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

void MainWindow::update_label()
{
    data = A.read_from_arduino();
    data.remove(0, data.length() - 11);
    qDebug() << data;
    if (data != "" && data.length() > 10)
    {
        QSqlQuery query;
        query.exec("SELECT * FROM VOYAGEURS WHERE RFID_CARD = '" + data + "'");
        query.first();
        qDebug() << query.value(1).toByteArray();
        A.write_to_arduino(query.value(1).toByteArray());
        ui->lineEdit_rfid->setText(data);
    }
}

void MainWindow::graphics()
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
