#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "form.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pForm = new Form(this);
    setCentralWidget(m_pForm);

    connect(ui->actionSettings, SIGNAL(triggered()), m_pForm, SLOT(editSettings()));
    connect(ui->actionAdd_place, SIGNAL(triggered()), m_pForm, SLOT(addPlace()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
