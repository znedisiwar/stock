#include <QDate>
#include "mainwindow.h"
#include "stockmanager.h"
#include "ui_mainwindow.h"
#include "stock.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QSqlError>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QPainter>
#include <QPrinter>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Set up the clickable label with rich text (for links)
    ui->stockwarning->setText("<a href=\"notification\">Click for notification</a>");

    // 2. Connect the linkActivated signal to a slot


    // 3. Optional: Make sure the label responds to mouse events
    ui->stockwarning->setAttribute(Qt::WA_Hover, true);  // Ensure it listens to hover events
    ui->stockwarning->setCursor(Qt::PointingHandCursor);  // Change cursor on hover to indicate clickability

    // Input validation
    QRegularExpression numberRegex("[0-9]{1,10}");
    QRegularExpressionValidator* numberValidator = new QRegularExpressionValidator(numberRegex, this);
    ui->code_ui->setValidator(numberValidator);
    ui->quantity_ui->setValidator(numberValidator);

    QRegularExpression letterRegex("[A-Za-z]{1,20}");
    QRegularExpressionValidator* letterValidator = new QRegularExpressionValidator(letterRegex, this);
    ui->name_ui->setValidator(letterValidator);
    ui->type_ui->setValidator(letterValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Define the link activation handler/
//void MainWindow::on_stockwarning_linkActivated(const QString &link)
//{
    // Handle the link activation here
  //  qDebug() << "Link activated: " << link;
    // Add your notification handling logic here
//}



void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (ui->stockwarning->rect().contains(event->pos())) {
        // This means the label was clicked
        on_stockwarning_clicked();  // Call your custom function
    }
}
void MainWindow::on_ajoute_clicked()
{
    QString codeText = ui->code_ui->text();
    QString quantityText = ui->quantity_ui->text();
    QString nom_stock = ui->name_ui->text();
    QString type_stock = ui->type_ui->text();
    QDate date_stock = ui->date_ui->date();  // Get date from QDateEdit widget
    QDate date_expiration = ui->date_ui->date();  // Get expiration date from QDateEdit widget

    // Ensure fields are filled
    if (codeText.isEmpty() || quantityText.isEmpty() || nom_stock.isEmpty() || type_stock.isEmpty() ||
        !date_stock.isValid() || !date_expiration.isValid()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled correctly.");
        return;
    }

    // C text to integers
    int code_stock = codeText.toInt();
    int quantity_stock = quantityText.toInt();

    // Create stock and add to database
    stock s(code_stock, quantity_stock, nom_stock, type_stock, date_stock, date_expiration);
    if (s.ajouter()) {
        QMessageBox::information(this, "Success", "Stock added successfully.");
        ui->tableView->setModel(s.afficher());

        // Log history for the added stock
        QSqlQuery query;
        query.prepare("INSERT INTO STOCK_HISTORY (ACTION, CODE, NOM, TYPE, QUANTITE, DATE_ACTION, USER) "
                      "VALUES ('Added', :code, :nom, :type, :quantite, :date, :user)");
        query.bindValue(":code", code_stock);
        query.bindValue(":nom", nom_stock);
        query.bindValue(":type", type_stock);
        query.bindValue(":quantite", quantity_stock);
        query.bindValue(":date", date_stock);

        if (!query.exec()) {
            qDebug() << "Failed to log history: " << query.lastError().text();
        }

    } else {
        QMessageBox::critical(this, "Error", "Failed to add stock.");
    }

    // Clear inputs after addition
    ui->code_ui->clear();
    ui->quantity_ui->clear();
    ui->name_ui->clear();
    ui->type_ui->clear();
    ui->date_ui->clear();
}

void MainWindow::on_delete_2_clicked()
{
    stock s;
    int code = ui->code_ui->text().toInt();

    bool test = s.supprimer(code);
    if (test) {
        ui->tableView->setModel(s.afficher());
        QMessageBox::information(nullptr, QObject::tr("Suppression avec succès"),
                                 QObject::tr("Suppression réussie.\nClick Cancel to exit."), QMessageBox::Cancel);
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Erreur de suppression"),
                              QObject::tr("Suppression échouée.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
}

void MainWindow::on_show_clicked() {
    stock s;
    ui->tableView->setModel(s.afficher());
}

void MainWindow::on_edit_clicked()
{
    QString codeText = ui->code_ui->text();
    if (codeText.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Code must not be empty.");
        return;
    }

    int code_stock = codeText.toInt();
    QString quantityText = ui->quantity_ui->text();
    int quantity_stock = quantityText.isEmpty() ? 0 : quantityText.toInt();  // Default to 0 if empty
    QString nom_stock = ui->name_ui->text();
    QString type_stock = ui->type_ui->text();
    QDate date_stock = ui->date_ui->date();
    QDate date_expiration = ui->date_ui->date();

    stock s(code_stock, quantity_stock, nom_stock, type_stock, date_stock, date_expiration);

    if (s.modifier()) {
        QMessageBox::information(this, "Success", "Stock modified successfully.");
        ui->tableView->setModel(s.afficher());
    } else {
        QMessageBox::critical(this, "Error", "Failed to modify stock. Ensure CODE exists.");
    }

    // Clear after modification
    ui->code_ui->clear();
    ui->quantity_ui->clear();
    ui->name_ui->clear();
    ui->type_ui->clear();
    ui->date_ui->clear();
}

void MainWindow::on_tri_clicked()
{
    QString searchTerm = ui->searchInput->text();
    QString searchColumn;

    if (ui->radio_code->isChecked()) {
        searchColumn = "CODE";
    } else if (ui->radio_nom->isChecked()) {
        searchColumn = "NOM";
    } else if (ui->radio_type->isChecked()) {
        searchColumn = "TYPE";
    }

    QString queryStr = QString("SELECT CODE, NOM, TYPE, QUANTITE, DATE_EXPIRATION FROM MEDICAMENTS WHERE LOWER(%1) LIKE LOWER(:searchTerm)").arg(searchColumn);
    qDebug() << "Executing query: " << queryStr;

    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":searchTerm", "%" + searchTerm + "%");

    if (query.exec()) {
        qDebug() << "Query executed successfully";

        QSqlQueryModel* model = new QSqlQueryModel;
        model->setQuery(std::move(query));
        ui->tableView_2->setModel(model);
    } else {
        qDebug() << "Query failed: " << query.lastError().text();
    }
}

void MainWindow::on_pushButton_28_clicked()
{
    int typeCount = 0, nameCount = 0, quantityCount = 0;

    QSqlQuery query;
    query.prepare("SELECT TYPE, NOM, CODE, QUANTITE FROM MEDICAMENTS");
    if (query.exec()) {
        while (query.next()) {
            QString type = query.value("TYPE").toString();
            QString name = query.value("NOM").toString();
            int quantity = query.value("QUANTITE").toInt();

            if (!type.isEmpty()) typeCount++;
            if (!name.isEmpty()) nameCount++;
            if (quantity > 0) quantityCount++;
        }
    }

    QString typeCountStr = QString::number(typeCount);
    QString nameCountStr = QString::number(nameCount);
    QString quantityCountStr = QString::number(quantityCount);

    auto *series = new QPieSeries();
    series->setHoleSize(0.35);
    series->append("Type: " + typeCountStr, typeCount);
    series->append("Nom: " + nameCountStr, nameCount);
    series->append("Quantité: " + quantityCountStr, quantityCount);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Stock Distribution");
    chart->legend()->setVisible(true);

    auto *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout_3->addWidget(chartView);
}

void MainWindow::on_PDF_clicked()
{
    QSqlQuery query;
    if (!query.exec("SELECT CODE, NOM, TYPE, DATE_STOCK, QUANTITE FROM MEDICAMENTS")) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to execute query: %1").arg(query.lastError().text()));
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save PDF"), "exported_stock_data.pdf", tr("PDF Files (*.pdf)"));
    if (filePath.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open PDF for writing."));
        return;
    }

    painter.setFont(QFont("Helvetica", 10));
    int y = 100;

    painter.setFont(QFont("Helvetica", 16, QFont::Bold));
    painter.drawText(100, y, "Stock Data Export");
    y += 50;

    painter.setFont(QFont("Helvetica", 12, QFont::Bold));
    painter.drawText(100, y, "Code");
    painter.drawText(200, y, "Name");
    painter.drawText(350, y, "Type");
    painter.drawText(500, y, "Date Stock");
    painter.drawText(650, y, "Quantity");
    y += 30;

    painter.setFont(QFont("Helvetica", 10));
    while (query.next()) {
        painter.drawText(100, y, query.value("CODE").toString());
        painter.drawText(200, y, query.value("NOM").toString());
        painter.drawText(350, y, query.value("TYPE").toString());
        painter.drawText(500, y, query.value("DATE_STOCK").toDate().toString("yyyy-MM-dd"));
        painter.drawText(650, y, query.value("QUANTITE").toString());
        y += 20;
    }

    painter.end();
    QMessageBox::information(this, tr("PDF Export"), tr("PDF has been successfully exported."));
}

void MainWindow::on_stockwarning_clicked()
{
    QString link = "notification";  // Added definition for link
    if (link == "notification") {
        ui->stockwarning->setText("Warning: Stocks are running low!");
    }
}

void MainWindow::on_Historique_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT ACTION, CODE, NOM, TYPE, QUANTITE, DATE_ACTION, USER FROM STOCK_HISTORY ORDER BY DATE_ACTION DESC");

    if (query.exec()) {
        QSqlQueryModel *model = new QSqlQueryModel();
        model->setQuery(query);

        // Check if the model has been populated with data
        if (model->rowCount() > 0) {
            ui->tableView_3->setModel(model); // Display history in tableView_3
        } else {
            QMessageBox::information(this, "No History", "No history of stock edits found.");
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to load history: " + query.lastError().text());
    }
}



