#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QMainWindow>
#include "stock.h"
#include <QMouseEvent>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void mouseReleaseEvent(QMouseEvent *event);

    void on_ajoute_clicked();
    void on_delete_2_clicked();
    void on_show_clicked();
    void on_edit_clicked();

    void on_tri_clicked();

    void on_pushButton_28_clicked();

    void on_tri_2_clicked();

    void on_Historique_clicked();

    void on_PDF_clicked();
    void updateStockWarning(const QString &warningMessage);
    void on_stockwarning_clicked();



public:

    void on_stockwarning_linkActivated(const QString &link);
    void updatestockWarning(const QString &message);



private:
    Ui::MainWindow *ui;
    stock s;
    QLabel *stockwarning;
};

#endif // MAINWINDOW_H
