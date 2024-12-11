#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qRegisterMetaType<WorkDoneResult>("WorkDoneResult");
    //qRegisterMetaType<RecipeParam>("RecipeParam");

    setWindowTitle("Optical Proximity Correction Detect");
    setWindowIcon(QIcon("./Icon/OPC.png"));
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

