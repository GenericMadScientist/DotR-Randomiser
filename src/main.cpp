#include "decks.hpp"
#include "iologic.hpp"
#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{/*
        createStarterDecks(file, factory, dcs, 798021);
    }*/

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
