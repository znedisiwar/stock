#ifndef STOCK_H
#define STOCK_H
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>

class stock
{
private:
    int code_stock, quantity_stock;
    QString nom_stock, type_stock;
    QDate date_stock, date_expiration; // Added date_expiration as a private member

public:
    stock();
    stock(int code_stock, int quantity_stock, QString nom_stock, QString type_stock, QDate date_stock, QDate date_expiration);

    int codestock();
    int quantitystock();
    QString nom();
    QString type();
    QDate date(); // Getter for date_stock
    QDate expirationDate(); // Getter for date_expiration

    void setcode(int);
    void setquantity(int);
    void setnom(QString);
    void settype(QString);
    void setdate(QDate); // Setter for date_stock
    void setExpirationDate(QDate); // Setter for date_expiration
    void on_tri_clicked();
    void on_PDF_clicked();
    bool ajouter();
    void checkLowStockNotification();
    QSqlQueryModel* afficher();
    bool modifier();
    bool supprimer(int);
    QSqlQueryModel * tristock(QString attribut);
    QSqlQuery searchMedicament(const QString &searchColumn, const QString &searchTerm);


    QSqlQueryModel* historique();
};


#endif // STOCK_H
