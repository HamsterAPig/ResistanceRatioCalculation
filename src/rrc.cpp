#include "rrc.h"
#include "./ui_rrc.h"
#include <QtMath>

RRC::RRC(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RRC)
{
    ui->setupUi(this);

    // 将svg图片显示出来
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
    // 定义一个二维的QVector暂时缓存一整份csv文件
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
                vec_temp.append(temp_str.toFloat());
            }
            tmp_sec_vec.append(vec_temp);
        }
        // 将暂时缓存的一整份csv文件添加到全局总的三维vec_resistances中
        vec_resistances.append(tmp_sec_vec);
    }
}


void RRC::on_calc_btn_clicked()
{
    float vout = ui->vout_input->text().toFloat();
    float vref = ui->vref_input->text().toFloat();
    int comb_index = ui->comboBox->currentIndex();

    float point_number = vref / vout;

    // 三分查找最接近的值
    int left = 1;
    int right = vec_resistances[comb_index].count();
    while(left < right){
        int midl = left + (right - left)/3;
        int midr = right - (right - left)/3;

        float err_l = point_number - vec_resistances[comb_index][midl][2];
        float err_r = point_number - vec_resistances[comb_index][midr][2];
        if(qFabs(err_l) > qFabs(err_r)){
            left  = midl + 1;
        } else {
            right = midr - 1;
        }
        qDebug()<<"loop for left :"<<left;
    }

    // 定义一个二维的QVector暂时缓存一下找到的元素
    QVector< QVector<float> > tmp_found_vec;
    int index_left = left ,index_right = left + 1, index = 0;
    while (qFabs(point_number - vec_resistances[comb_index][index_left][2]) <= 0.01 || qFabs(point_number - vec_resistances[comb_index][index_right][2]) <= 0.01){
        qDebug()<<"Found left is:"<<vec_resistances[comb_index][index_left];
        tmp_found_vec.append(vec_resistances[comb_index][index_left]);
        tmp_found_vec[index].append(qFabs(point_number - vec_resistances[comb_index][index_left][2]));

        tmp_found_vec.append(vec_resistances[comb_index][index_right]);
        tmp_found_vec[++index].append(qFabs(point_number - vec_resistances[comb_index][index_right][2]));

        index_left--;
        index_right++;
        index++;
        if (index_left < 0 || index_right > vec_resistances[comb_index].count()) break;
    }
    std::sort(tmp_found_vec.begin(), tmp_found_vec.end(), [](QVector<float>&a, QVector<float>&b)->bool{return a[3]<b[3];});
    qDebug()<<"tmp_found_vec:"<<tmp_found_vec;
}
