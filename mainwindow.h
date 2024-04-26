#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "eca.h"
#include <QMainWindow>
#include <QGraphicsScene>

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

private:
    void setInitLine();
    void setupAutomaton();
    void runAutomaton(ECA *automaton, uint num_generations);
    void drawState(std::vector<std::vector<bool>> state, uint pixelSize);
    void addImageToGraphicsView(QImage *image);
    int convertBinToDec(QString binary);
    QString convertDecToBin(int decimal);

    Ui::MainWindow *ui;
    int gen_length;
    std::vector<bool> init;
    ECA *automaton = nullptr;
    QImage *image_buffer = nullptr;
    QGraphicsScene *scene = nullptr;
    QColor alive;
    QColor dead;
    QColor background;

private slots:
    void start();
    void readBinaryRuleset(QString binary);
    void readDecimalRuleset(int decimal);
};
#endif // MAINWINDOW_H
