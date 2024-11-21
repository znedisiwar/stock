#include "mainwindow.h"
#include <QMessageBox>
#include "connection.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    // Set to use software rendering (optional, may help with rendering issues)
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QApplication a(argc, argv);

    // Set the default font
    QFont defaultFont("Arial", 10);
    QApplication::setFont(defaultFont);

    Connection c; // Une seule instance de la classe connection
    bool test = c.createconnect(); // Etablir la connexion
    MainWindow w;

    if (test) // si la connexion est établie
    {
        w.show();
        QMessageBox::information(nullptr, QObject::tr("database is open"),
                                 QObject::tr("connexion successful.\n"
                                             "click cancel to exit."), QMessageBox::Cancel);
    }
    else //si la connexion a échoué
    {
        QMessageBox::critical(nullptr, QObject::tr("database is not open"),
                              QObject::tr("connection failed.\n"
                                          "click Cancel to exit."), QMessageBox::Cancel);
    }

    return a.exec();
}
