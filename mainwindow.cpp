#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(start()));

    connect(ui->lineEdit_rule, SIGNAL(textChanged(QString)), this, SLOT(readBinaryRuleset(QString)));
    connect(ui->spinBox_ruleDec, SIGNAL(valueChanged(int)), this, SLOT(readDecimalRuleset(int)));

    connect(ui->pushButton_zoomMinus, SIGNAL(clicked()), this, SLOT(zoomOut()));
    connect(ui->pushButton_zoomPlus, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(ui->pushButton_zoomReset, SIGNAL(clicked()), this, SLOT(zoomReset()));

    connect(ui->pushButton_saveImage, SIGNAL(clicked()), this, SLOT(saveImage()));

    connect(ui->pushButton_changeColorAlive, SIGNAL(clicked()), this, SLOT(changeColorAlive()));
    connect(ui->pushButton_changeColorDead, SIGNAL(clicked()), this, SLOT(changeColorDead()));
    connect(ui->pushButton_changeColorBackground, SIGNAL(clicked()), this, SLOT(changeColorBackground()));


    //default colors
    alive = QColor(Qt::white);
    dead = QColor(Qt::black);
    background = QColor (Qt::darkGray);

    //set color for buttons
    ui->pushButton_changeColorAlive->setStyleSheet(generateButtonCollorStylesheet(alive));
    ui->pushButton_changeColorDead->setStyleSheet(generateButtonCollorStylesheet(dead));
    ui->pushButton_changeColorBackground->setStyleSheet(generateButtonCollorStylesheet(background));

    //setup QGraphicsScene
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->clear();
    scene->setBackgroundBrush(QBrush(background));    
}

MainWindow::~MainWindow()
{
    delete ui;
    delete automaton;
}

void MainWindow::setInitLine()
{
    init.clear();
    gen_length = ui->spinBox_lineLength->value();

    if (ui->checkBox_initRandom->isChecked()){
        //make init line random
        for (int i = 0; i < gen_length; ++i){
            init.push_back((rand() % 100) < ui->spinBox_initLivingProcent->value());
        }
    }
    else {
        // default init 1 life cell in center
        this->init.reserve(gen_length);
        for (int i = 0; i < gen_length; ++i){
            init.push_back(false);
        }
        this->init[gen_length/2] = true;
    }
}

void MainWindow::setupAutomaton()
{
    std::vector<bool> rule;
    // parse rule
    QString rule_string = ui->lineEdit_rule->text();
    for (int i=0; i < 8; ++i){
        rule.push_back(rule_string.at(i) == '1');
    }

    setInitLine();
    if(automaton){
        delete automaton;
        automaton = nullptr;
    }
    automaton = new ECA(init, rule);
}

void MainWindow::runAutomaton(ECA *automaton, uint num_generations)
{
    ui->graphicsView->setBackgroundBrush(QBrush(background));

    automaton->computeMultipleGeneration((unsigned int)num_generations);
    drawState(automaton->getState(), 5); //TODO make ability to change cell_size on spinbox in ui
}

void MainWindow::drawState(std::vector<std::vector<bool>> state, uint cell_size)
{
    if (state.empty())
        return;

    int img_size_x = state.at(0).size() * cell_size;
    int img_size_y = state.size() * cell_size;

    if (image_buffer){
        delete image_buffer;
        image_buffer = nullptr;
    }
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
    image_was_generated = true;
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

//convert string of binary digits to decimal number
// TODO: change for better algoritm
int MainWindow::convertBinToDec(QString binary) {
    int decimal = 0;
    for(int i = 0; i < binary.length();  i++) {
        if(binary.at(i) == '1')
            decimal += (int)pow(2, binary.length() - i - 1); // change C style cast
    }
    return decimal;
}

//convert decimal number to QString of 0's and 1's
//the leading 0's are important for the ruleset
// TODO: change for better algoritm
QString MainWindow::convertDecToBin(int decimal) {
    QString binary;

    if(decimal == 0)
        return QString("00000000");

    while(binary.length() < 8) {
        if(decimal % 2 == 1)
            binary.insert(0, '1');
        else
            binary.insert(0, '0');

        decimal /= 2;
    }
    return binary;
}

//update decimal input field after binary ruleset was changed
void MainWindow::readBinaryRuleset(QString binary) {
    bool signals_blocked = ui->spinBox_ruleDec->blockSignals(true);
    ui->spinBox_ruleDec->setValue(convertBinToDec(binary));
    ui->spinBox_ruleDec->blockSignals(signals_blocked);
}

//update binary ruleset input field after decimal ruleset was changed
void MainWindow::readDecimalRuleset(int decimal) {
    bool signals_blocked = ui->lineEdit_rule->blockSignals(true);
    ui->lineEdit_rule->setText(convertDecToBin(decimal));
    ui->lineEdit_rule->blockSignals(signals_blocked);
}

void MainWindow::zoomIn() {
    ui->graphicsView->scale(1.2, 1.2);
}

void MainWindow::zoomOut() {
    ui->graphicsView->scale(0.8, 0.8);
}

void MainWindow::zoomReset()
{
    ui->graphicsView->resetTransform();
}

void MainWindow::saveImage()
{
    if (!image_was_generated){
        QMessageBox::information(this, "Image was not generated", "no image");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save as", QDir::currentPath(),
                                                    "Image Formats (*.png *.jpg *.jpeg *.tiff *.ppm *.bmp *.xpm");

    QFileInfo file(filename);
    if(!file.baseName().isEmpty() && file.suffix().isEmpty())
        filename += ".png";

    if(!image_buffer->save(filename))
        QMessageBox::information(this, "Error while saving image", "Image not saved!");
}

void MainWindow::changeColorAlive()
{
    alive = QColorDialog::getColor(alive, this, "Set Alive cell color");
    ui->pushButton_changeColorAlive->setStyleSheet(generateButtonCollorStylesheet(alive));
}

void MainWindow::changeColorDead()
{
    dead = QColorDialog::getColor(dead, this, "Set Dead cell color");
    ui->pushButton_changeColorDead->setStyleSheet(generateButtonCollorStylesheet(dead));
}

void MainWindow::changeColorBackground()
{
    background = QColorDialog::getColor(background, this, "Set Background color");
    ui->pushButton_changeColorBackground->setStyleSheet(generateButtonCollorStylesheet(background));
}

//give QString stylesheet and change text color to white/black for buttons
QString MainWindow::generateButtonCollorStylesheet(QColor color)
{
    QColor text_color;
    if(color.value() >= 140)
        text_color = QColor(Qt::black);
    else
        text_color = QColor(Qt::white);
    return QString("background-color: " + color.name() + "; color: " + text_color.name() + "; ");
}
