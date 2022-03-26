#include "rrc.h"
#include "./ui_rrc.h"

RRC::RRC(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RRC)
{
    ui->setupUi(this);
}

RRC::~RRC()
{
    delete ui;
}

