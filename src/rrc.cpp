#include "rrc.h"
#include "./ui_rrc.h"

RRC::RRC(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RRC)
{
    ui->setupUi(this);

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsSvgItem *item = new QGraphicsSvgItem(":/dianzu_fenya.svg");
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
//    ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
    ui->graphicsView->show();

    qDebug()<<"ui->graphicsView->size():"<<ui->graphicsView->size();
    qDebug()<<"scene->sceneRect().size(): " << scene->sceneRect().size();
    read_csv(":/1.csv");
    read_csv(":/2.csv");
    read_csv(":/3.csv");
    read_csv(":/4.csv");
    read_csv(":/5.csv");
    read_csv(":/6.csv");
    read_csv(":/7.csv");
    qDebug()<<vec_resistances.count();
}

RRC::~RRC()
{
    delete ui;
}

void RRC::read_csv(QString path)
{
    QVector< QVector<float> > tmp_sec_vec;
    QFile file(path);
    bool bopen = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!bopen){
        qDebug()<<"Failed to open file.";
    } else {
        QTextStream *read = new QTextStream(&file);
        QStringList Data = read->readAll().split("\n");
        for (int i = 0; i < Data.count() - 1; i++){
            QStringList strLine = Data.at(i).split(",");
            QVector<float> vec_temp;
            for(const auto&temp_str: strLine){
                vec_temp.append(temp_str.toDouble());
            }
            tmp_sec_vec.append(vec_temp);
        }
        vec_resistances.append(tmp_sec_vec);
    }
}

