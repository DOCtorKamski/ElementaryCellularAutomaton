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

    connect(ui->lineEdit_rule, SIGNAL(textChanged(QString)), this, SLOT(readBinaryRuleset(QString)));
    connect(ui->spinBox_ruleDec, SIGNAL(valueChanged(int)), this, SLOT(readDecimalRuleset(int)));


    //default colors
    alive = QColor(250, 250, 250);
    dead = QColor(0, 0, 0);
    background = QColor (70, 70, 70);

    //setup QGraphicsScene
    QGraphicsScene *scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->clear();
    scene->setBackgroundBrush(QBrush(background));    
}

MainWindow::~MainWindow()
{
    delete ui;
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

//convert string of binary digits to decimal number
int MainWindow::convertBinToDec(QString binary) {
    int decimal = 0;
    for(int i = 0; i < binary.length();  i++) {
        if(binary.at(i) == '1')
            decimal += (int)pow(2, binary.length() - i - 1);
    }
    return decimal;
}

//convert decimal number to QString of 0's and 1's
//the leading 0's are important for the ruleset
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
    bool signalsBlocked = ui->spinBox_ruleDec->blockSignals(true);
    ui->spinBox_ruleDec->setValue(convertBinToDec(binary));
    ui->spinBox_ruleDec->blockSignals(signalsBlocked);
}

//update binary ruleset input field after decimal ruleset was changed
void MainWindow::readDecimalRuleset(int decimal) {
    bool signalsBlocked = ui->lineEdit_rule->blockSignals(true);
    ui->lineEdit_rule->setText(convertDecToBin(decimal));
    ui->lineEdit_rule->blockSignals(signalsBlocked);
}
