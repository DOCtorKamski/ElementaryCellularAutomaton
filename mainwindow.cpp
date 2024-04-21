#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGraphicsScene>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(start()));

    //default colors
    alive = QColor(250, 250, 250);
    dead = QColor(0, 0, 0);
    background = QColor (70, 70, 70);

    //setup QGraphicsScene
    QGraphicsScene *scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->clear();
    scene->setBackgroundBrush(QBrush(background));

    // init 1 life cell in center
    gen_length = 100;
    this->init.reserve(gen_length);
    for (int i = 0; i < gen_length; ++i){
        init.push_back(false);
    }
    this->init[50] = true;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupAutomaton()
{
    std::vector<bool> rule;
    // parse rule
    QString rule_string = ui->lineEdit_rule->text();
    for (int i=0; i < 8; ++i){
        rule.push_back(rule_string.at(i) == '1');
    }

    this->automaton = new ECA(init, rule);
}

void MainWindow::runAutomaton(ECA *automaton, uint num_generations)
{
    ui->graphicsView->setBackgroundBrush(QBrush(background));

    automaton->computeMultipleGeneration((unsigned int)num_generations);
    drawState(automaton->getState(), 5); //TODO change to spinbox in ui
}

void MainWindow::drawState(std::vector<std::vector<bool>> state, uint cell_size)
{
    if (state.empty())
        return;

    int img_size_x = state.at(0).size() * cell_size;
    int img_size_y = state.size() * cell_size;
    image_buffer = new QImage(img_size_x, img_size_y, QImage::Format_RGB32);

    for (uint y = 0; y < state.size(); ++y){
        for (uint x = 0; x < state.at(0).size(); ++x){
            // draw cell size subpixel
            for(uint sub_y = 0; sub_y < cell_size; ++sub_y){
                for(uint sub_x = 0; sub_x < cell_size; ++sub_x){
                    if (state.at(y).at(x)){
                        // cell alive, use live cell color
                        image_buffer->setPixel(x * cell_size + sub_x, y * cell_size + sub_y, alive.rgb());
                    }
                    else {
                        // cell dead, use dead cell color
                        image_buffer->setPixel(x * cell_size + sub_x, y * cell_size + sub_y, dead.rgb());
                    }
                }
            } //subpixel
        }
    }

    // add QImage to graphicsview
    addImageToGraphicsView(image_buffer);
}

void MainWindow::addImageToGraphicsView(QImage *image)
{
    QPixmap pixmap = QPixmap();
    pixmap.convertFromImage(*image);
    ui->graphicsView->scene()->addPixmap(pixmap);
}

void MainWindow::start()
{
    ui->graphicsView->scene()->clear();
    setupAutomaton();
    runAutomaton(automaton, ui->spinBox_numGens->value());
}
