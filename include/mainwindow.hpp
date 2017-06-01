#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QFile>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
private slots:
    void on_loadBtn_clicked();
    void on_genSeedBtn_clicked();
    void on_randomiseBtn_clicked();
private:
    Ui::MainWindow* ui;
    QFile* dotrIso;
    int getSeed();
};

#endif
