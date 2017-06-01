#include <chrono>
#include <cstdint>
#include <random>

#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>

#include "iologic.hpp"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <iostream>
int randomSeed()
{
    auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(time);
    uint64_t timeSeed = static_cast<uint64_t>(micros.count());

    std::mt19937_64 prng(timeSeed);
    prng.discard(10000);
    std::uniform_int_distribution<int> dis(0, 2147483647);

    return dis(prng);
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), dotrIso{nullptr}
{
    ui->setupUi(this);
    ui->seedBox->setValidator(new QIntValidator(0, 2147483647));
    ui->seedBox->setText(QString::number(randomSeed()));

    setStatusBar(nullptr);
    setGeometry(200, 200, 400, 175);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dotrIso;
}

void MainWindow::on_loadBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file",
                                                    QDir::currentPath(),
                                                    "PS2 isos (*.iso)");
    if (fileName.isEmpty())
        return;

    QFile* newIso = new QFile(fileName);
    if (!newIso->exists()) {
        delete newIso;
        return;
    }

    newIso->open(QIODevice::ReadWrite);
    if (!isFileValid(newIso)) {
        delete newIso;
        QMessageBox::information(this, "DotR Randomiser",
                                 "Not a valid Yu-Gi-Oh! The "
                                 "Duelists of the Roses (US NTSC) iso.");
    }
    else {
        delete dotrIso;
        dotrIso = newIso;
    }
}

void MainWindow::on_genSeedBtn_clicked()
{
    ui->seedBox->setText(QString::number(randomSeed()));
}

void MainWindow::on_randomiseBtn_clicked()
{
    if (dotrIso == nullptr) {
        QMessageBox::information(this, "DotR Randomiser", "No DotR iso loaded.");
        return;
    }

    if (ui->starterCheckbox->isChecked()) {
        std::vector<int> dcs = readDeckCosts(dotrIso);
        DeckFactory factory = createDefaultFactory(dcs);
        createStarterDecks(dotrIso, factory, dcs, getSeed());
    }
    writeDmkAi(dotrIso, ui->aiCheckbox->isChecked());
    toggleDcCheck(dotrIso, !(ui->dcCheckbox->isChecked()));

    QMessageBox::information(this, "DotR Randomiser", "Randomisation complete.");
}

int MainWindow::getSeed()
{
    return ui->seedBox->text().toInt();
}
