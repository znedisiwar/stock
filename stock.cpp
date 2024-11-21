#include <QDate>
#include "stock.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>


stock::stock()
{
    code_stock = 0;
    quantity_stock = 0;
    nom_stock = "";
    type_stock = "";
    date_stock = QDate();
    date_expiration = QDate();
}

stock::stock(int code_stock, int quantity_stock, QString nom_stock, QString type_stock, QDate date_stock, QDate date_expiration)
{
    this->code_stock = code_stock;
    this->quantity_stock = quantity_stock;
    this->nom_stock = nom_stock;
    this->type_stock = type_stock;
    this->date_stock = date_stock;
    this->date_expiration = date_expiration;
}

int stock::codestock() { return code_stock; }
int stock::quantitystock() { return quantity_stock; }
QString stock::nom() { return nom_stock; }
QString stock::type() { return type_stock; }
QDate stock::date() { return date_stock; }
QDate stock::expirationDate() { return date_expiration; }

void stock::setcode(int code_stock) { this->code_stock = code_stock; }
void stock::setquantity(int quantity_stock) { this->quantity_stock = quantity_stock; }
void stock::setnom(QString nom_stock) { this->nom_stock = nom_stock; }
void stock::settype(QString type_stock) { this->type_stock = type_stock; }
void stock::setdate(QDate date_stock) { this->date_stock = date_stock; }
void stock::setExpirationDate(QDate date_expiration) { this->date_expiration = date_expiration; }

bool stock::ajouter()
{
    if (code_stock <= 0 || quantity_stock <= 0 || nom_stock.isEmpty() || type_stock.isEmpty() ||
        !date_stock.isValid() || !date_expiration.isValid()) {
        qDebug() << "Error: All fields must be properly filled.";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO MEDICAMENTS (CODE, NOM, TYPE, QUANTITE, DATE_EXPIRATION) "
                  "VALUES (:CODE, :NOM, :TYPE, :QUANTITE, :DATE_EXPIRATION)");

    query.bindValue(":CODE", code_stock);
    query.bindValue(":NOM", nom_stock);
    query.bindValue(":TYPE", type_stock);
    query.bindValue(":QUANTITE", quantity_stock);
    query.bindValue(":DATE_EXPIRATION", date_expiration);

    return query.exec();
}

QSqlQueryModel* stock::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM MEDICAMENTS");
    return model;
}

bool stock::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE MEDICAMENTS SET NOM = :NOM, TYPE = :TYPE, QUANTITE = :QUANTITE, DATE_EXPIRATION = :DATE_EXPIRATION "
                  "WHERE CODE = :CODE");

    query.bindValue(":CODE", code_stock);
    query.bindValue(":NOM", nom_stock);
    query.bindValue(":TYPE", type_stock);
    query.bindValue(":QUANTITE", quantity_stock);
    query.bindValue(":DATE_EXPIRATION", date_expiration);

    return query.exec();
}

bool stock::supprimer(int code_stock)
{
    QSqlQuery query;
    query.prepare("DELETE FROM MEDICAMENTS WHERE CODE = :CODE");
    query.bindValue(":CODE", code_stock);
    return query.exec();
}

QSqlQueryModel * stock::tristock(QString attribut)
{
    QSqlQueryModel* model =new QSqlQueryModel() ;
    if(attribut == "CODE") model->setQuery("select * from MEDICAMENTS order by CODE");
    else if( attribut == "NOM") model->setQuery("select * from MEDICAMENTS order by NOM");
    else  model->setQuery("select * from MEDICAMENTS order by TYPE");
    return  model ;
}
QSqlQuery stock::searchMedicament(const QString &searchColumn, const QString &searchTerm) {
    QSqlQuery query;

    // Prepare the search query with placeholders
    QString queryStr = QString("SELECT CODE, NOM, TYPE, QUANTITE, DATE_STOCK, DATE_EXPIRATION "
                               "FROM MEDICAMENTS WHERE LOWER(%1) LIKE LOWER(:searchTerm)").arg(searchColumn);
    qDebug() << "Executing query: " << queryStr; // Debugging the query to ensure it's correct

    // Prepare the query and bind search term
    query.prepare(queryStr);
    query.bindValue(":searchTerm", "%" + searchTerm + "%");

    if (!query.exec()) {
        qDebug() << "Query failed: " << query.lastError().text();
    }

    return query;
}

QSqlQueryModel* stock::historique()
{
    // Create a new query model
    QSqlQueryModel* model = new QSqlQueryModel();

    // Set the query to select all medications ordered by `date_stock`
    model->setQuery("SELECT CODE, NOM, TYPE, QUANTITE, DATE_STOCK, DATE_EXPIRATION "
                    "FROM MEDICAMENTS "
                    "ORDER BY DATE_STOCK ASC");

    // Check for errors in query execution
    if (model->lastError().isValid()) {
        qDebug() << "Failed to fetch historic data: " << model->lastError().text();
    } else {
        qDebug() << "Historic data fetched successfully.";
    }

    return model; // Return the model containing the historic data
}

