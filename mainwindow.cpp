#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int ret = A.connect_arduino();
    switch (ret)
    {
    case 0:
        qDebug() << "arduino is available and connected to : " << A.getarduino_port_name();
        break;
    case 1:
        qDebug() << "arduino is available but not connected to :" << A.getarduino_port_name();
        break;
    case -1:
        qDebug() << "arduino is not available";
        break;
    default:
        qDebug() << "Unknown return value: " << ret;
        break;
    }

    QObject::connect(A.getserial(), SIGNAL(readyRead()), this, SLOT(update_label())); // Launch serial communication

    table();                                  
    style();                                  
    controle();                               
    charts(ui->comboBox_chart->currentText());
    profit();                                 
    graphics();                               
    
    ui->tabWidget->setGeometry(QRect(0, 0, 1600, 1000));
    
    QVector<MyButton*> buttons = {ui->pushButton_ajouter, ui->pushButton_clear, ui->pushButton_update, ui->pushButton_Search, ui->pushButton_delete};
    for (auto &button : buttons)
    {
        button->SetColor(QColor(255, 255, 255));
    }

    ui->label_moyen->setText(QString::number(voy.calculateAverageCost(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
    auto *timer = new QTimer();

    connect(timer, &QTimer::timeout, [=]()
            {
        QVector<QGroupBox*> groupBoxs = {ui->groupBox, ui->Information_Box, ui->groupBox_2, ui->groupBox_3};
        for (auto& groupBox : groupBoxs)
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
    connect(ui->lineEdit_rechercher, &QLineEdit::textChanged, [=](const QString &text)
            { table("search", text); });
    connect(ui->comboBox_sort, &QComboBox::currentTextChanged, [=](const QString &text)
            { table("sort", text); });
    connect(ui->comboBox_chart, &QComboBox::currentTextChanged, [=](const QString &text)
            { charts(text); });

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
        charts(ui->comboBox_chart->currentText());
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Ajout effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("Ajout non effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
}


void MainWindow::on_pushButton_delete_clicked()
{
    QString ref = ui->lineEdit_delete->text();
    bool test = voy.supprimer(ref);
    if (test && !ui->lineEdit_delete->text().isEmpty())
    {
        profit();
        table();
        charts(ui->comboBox_chart->currentText());
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Suppression effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("Suppression non effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
}


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
        charts(ui->comboBox_chart->currentText());
        table();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("update effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Not OK"), QObject::tr("update non effectué.\nClick Cancel to exit."), QMessageBox::Cancel);
}


void MainWindow::controle()
{
    QRegularExpression rx("\\b[a-zA-Z0-9]{1,8}\\b");
    ui->lineEdit_flightref->setValidator(new QRegularExpressionValidator(rx, this));
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
    if (voy.searchByFlightRef(ui->lineEdit_flightref->text()) && !ui->lineEdit_flightref->text().isEmpty())
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
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("voyage.pdf");
    QTextDocument doc;

    QList<Voyage> V = voy.getAllVoyagesSorted("FLIGHTREF");
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
    for (const auto& voyage : V)
    {
        h += "<tr>"
             "<td  align='center'>" +
             voyage.getFlightref() + "</td>"
                                    "<td  align='center'>" +
             voyage.getLieudep() + "</td>"
                                  "<td  align='center'>" +
             voyage.getLieuarr() + "</td>"
                                  "<td  align='center'>" +
             voyage.getDatearr().toString("dd/MM/yyyy") + "</td>"
                                                         "<td  align='center'>" +
             voyage.getDatedep().toString("dd/MM/yyyy") + "</td>"
                                                         "<td  align='center'>" +
             voyage.getAirline() + "</td>"
                                  "<td  align='center'>" +
             QString::number(voyage.getMontant()) + "</td>"
                                                   "<td  align='center'>" +
             QString::number(voyage.getNbper()) + "</td>"
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


void MainWindow::charts(QString type)
{
    qDeleteAll(ui->widget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    QList<QString> labels;
    auto seriesDep = new QPieSeries();
    QList<int> numbers = voy.getVoyageCount(type, labels);
    for (int i = 0; i < numbers.size(); i++)
        seriesDep->append(labels[i], numbers[i]);
    seriesDep->setLabelsVisible();
    seriesDep->setPieSize(300);

    auto chart = new QChart();
    chart->addSeries(seriesDep);
    chart->setTitle("Number of flights by country");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    auto chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(460, 230);

    auto layout = new QHBoxLayout();
    layout->addWidget(chartView);
    ui->widget->setLayout(layout);
    ui->widget->layout()->deleteLater();
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
    for (const auto& headerLabel : labels)
    {
        auto label = new QLabel(headerLabel);
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


void MainWindow::table(QString type, QString text)
{
    QList<Voyage> voyages = {};

    if (type == "" && text == "")
        voyages = voy.getAllVoyages();
    else if (type == "sort")
        voyages = voy.getAllVoyagesSorted(text);
    else if (type == "search")
        voyages = voy.searchVoyages(text);

    int rowHeight = 30;

    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    auto layout = new QGridLayout();
    QList<QLabel *> headerLabels = createHeadBar();

    int i = 0;
    for (auto headerLabel : headerLabels)
    {
        layout->addWidget(headerLabel, 0, i);
        i++;
    }

    for (const auto& voyage : voyages)
    {
        QStringList voyageData = {
            voyage.getFlightref(),
            voyage.getLieudep(),
            voyage.getLieuarr(),
            voyage.getDatedep().toString("dd/MM/yyyy"),
            voyage.getDatearr().toString("dd/MM/yyyy"),
            voyage.getAirline(),
            QString::number(voyage.getMontant()),
            QString::number(voyage.getNbper())};

        for (int j = 0; j < 8; j++)
        {
            auto lineEdit = new QLineEdit(voyageData[j]);
            lineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            lineEdit->setStyleSheet("QLineEdit {"
                                    "height: 30px;"
                                    "}");
            if (j != 0)
            {
                lineEdit->setReadOnly(true);
            }

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
    ui->label_moyen->setText(QString::number(voy.calculateAverageCost(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
}


void MainWindow::on_comboBox_lieu_To_currentTextChanged(const QString &arg1)
{
    ui->label_moyen->setText(QString::number(voy.calculateAverageCost(ui->comboBox_lieu_From->currentText(), ui->comboBox_lieu_To->currentText())));
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
        earn += query.value(0).toFloat() * (float) query.value(1).toInt();
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
    
    auto scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    
    scene->setBackgroundBrush(QBrush(QColor(5, 24, 150)));

    
    QGraphicsPixmapItem *plane = scene->addPixmap(QPixmap(":/plane.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation).transformed(QTransform().rotate(180)));
    plane->setPos(220, 5);
    plane->setOpacity(1);

    
    QVector<QGraphicsEllipseItem*> circles(50);
    int x = 10;
    for (int i = 0; i < 50; i++)
    {
        circles[i] = scene->addEllipse(0, 0, 10, 10, QPen(Qt::white), Qt::white);
        circles[i]->setPos(QPoint(-ui->graphicsView->geometry().width() / 2 - x, ui->graphicsView->geometry().height() / 2 - 50));
        x -= 20;
    }

    
    QGraphicsTextItem *text = scene->addText("Tayerni", QFont("Outfit", 18));
    text->setDefaultTextColor(QColor(255, 255, 255));
    text->setPos(QPoint(-ui->graphicsView->geometry().width() / 2, -20));

    
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]()
            {
        auto timeLine = new QTimeLine(5000);
        auto animation = new QGraphicsItemAnimation();
        animation->setItem(plane);
        animation->setTimeLine(timeLine);
        animation->setTranslationAt(0, 220, 0);
        
        timeLine->start(); });
    timer->start(5000);
}
