#ifndef STOCKMANAGER_H
#define STOCKMANAGER_H

#include <QString>
#include <QSqlQuery>

class StockManager {
public:
    StockManager();

    void checkLowstockNotification();

    int lowStockThreshold = 10;  // Threshold for low stock notifications

    void updateStockWarning(const QString &message);

};

#endif
