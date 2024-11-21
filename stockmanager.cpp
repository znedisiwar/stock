#include "stockmanager.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QApplication>
#include "mainwindow.h"

void StockManager::checkLowstockNotification() {
    QSqlQuery query;
    query.prepare("SELECT NOM, QUANTITE FROM MEDICAMENTS WHERE QUANTITE < :threshold");
    query.bindValue(":threshold", lowStockThreshold);

    QString lowStockItems;
    if (query.exec()) {
        while (query.next()) {
            QString itemName = query.value("NOM").toString();
            int quantity = query.value("QUANTITE").toInt();
            lowStockItems += itemName + " (Quantity: " + QString::number(quantity) + ")\n";
        }

        // Get the active MainWindow instance to update the stock warning
        MainWindow *mainWindow = qobject_cast<MainWindow*>(qApp->activeWindow());
        if (mainWindow) {
            // Check if there are low stock items and update the warning accordingly
            if (!lowStockItems.isEmpty()) {
               // mainWindow->updatestockWarning("THIS IS GOING TO BE OUT OF STOCK VERY SOON:\n" + lowStockItems);
            } else {
                //mainWindow->updatestockWarning("");  // Clear warning if stock is sufficient
            }
        }
    } else {
        qDebug() << "Error executing query for low stock check:" << query.lastError().text();
    }
}
