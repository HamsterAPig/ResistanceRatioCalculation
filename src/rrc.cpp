#include "rrc.h"
#include "./ui_rrc.h"
#include "./about.h"
#include <QtMath>
#include <QElapsedTimer>
#include <QRegularExpression>
#include <QShortcut>

RRC::RRC(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RRC)
{
    ui->setupUi(this);

    // 右下角状态栏
    pragram_status = new QLabel(this);
    ui->statusbar->addPermanentWidget(pragram_status);
    pragram_status->setText(tr("空闲"));

    // 状态栏添加帮助信息
    QLabel* help_label = new QLabel(this);
    ui->statusbar->addPermanentWidget(help_label);
    help_label->setText(tr("按下F1查看帮助"));

    // 将svg图片显示出来
    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsSvgItem *item = new QGraphicsSvgItem(":/dianzu_fenya.svg");
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();

    qDebug()<<"ui->graphicsView->size():"<<ui->graphicsView->size();
    qDebug()<<"scene->sceneRect().size(): " << scene->sceneRect().size();

    // 读取csv文件到内存当中
    read_csv(":/1.csv");
    read_csv(":/2.csv");
    read_csv(":/3.csv");
    read_csv(":/4.csv");
    read_csv(":/5.csv");
    read_csv(":/6.csv");
    read_csv(":/7.csv");
    qDebug()<<vec_resistances.count();

    // 表格显示
    model = new QStandardItemModel();
    model->setHorizontalHeaderLabels({"R1","R2",tr("比值"),tr("误差")});
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setModel(model);
    ui->tableView->show();

    // 通过正则表达式限制输入框只能输入正浮点数
    QRegularExpression rx("^(([0-9]+\\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\\.[0-9]+)|([0-9]*[1-9][0-9]*))$");
    ui->vref_input->setValidator(new QRegularExpressionValidator(rx, this));
    ui->vout_input->setValidator(new QRegularExpressionValidator(rx, this));

    // 绑定键盘快捷键，即两个回车按键绑定到计算按钮上面
    ui->calc_btn->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->calc_btn->setShortcut(Qt::Key_Enter);
    ui->calc_btn->setShortcut(Qt::Key_Return);

    // 绑定快捷键，按下F1打开窗口获取帮助
    QShortcut* press_ps = new QShortcut(Qt::Key_F1, this, SLOT(press_f1_showHelp()));
}

RRC::~RRC()
{
    delete ui;
}

void RRC::read_csv(QString path)
{
    pragram_status->setText(tr("空闲"));
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
    pragram_status->setText(tr("忙碌"));
    QElapsedTimer mstimer;
    mstimer.start();
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
    do{
        qDebug()<<"Found left is:"<<vec_resistances[comb_index][index_left];
        tmp_found_vec.append(vec_resistances[comb_index][index_left]);
        tmp_found_vec[index].append(qFabs(point_number - vec_resistances[comb_index][index_left][2]));

        tmp_found_vec.append(vec_resistances[comb_index][index_right]);
        tmp_found_vec[++index].append(qFabs(point_number - vec_resistances[comb_index][index_right][2]));

        index_left--;
        index_right++;
        index++;
        if (index_left < 0 || index_right > vec_resistances[comb_index].count()) break;
    }while (qFabs(point_number - vec_resistances[comb_index][index_left][2]) <= 0.1 || qFabs(point_number - vec_resistances[comb_index][index_right][2]) <= 0.05);
    std::sort(tmp_found_vec.begin(), tmp_found_vec.end(), [](QVector<float>&a, QVector<float>&b)->bool{return a[3]<b[3];});
    qDebug()<<"tmp_found_vec:"<<tmp_found_vec;

    //表格显示
    model->clear();
    // 原本打算使用removerows的，结果这货没法清除好像
    // 所以就使用clear之后重建一下表头
    model->setHorizontalHeaderLabels({"R1","R2",tr("比值"),tr("误差")});
    for (int i = 0; i < tmp_found_vec.count(); i++){
        for(int j = 0; j < 4; j++){
            model->setItem(i,j,new QStandardItem(QString("%1").arg(tmp_found_vec[i][j])));
        }
        qDebug()<<"Model data:"<<model->rowCount();
    }
    pragram_status->setText(tr("空闲"));
    float time = (double)mstimer.nsecsElapsed()/(double)1000000;
    ui->statusbar->showMessage(tr("查询用时: %1").arg(time));
}

void RRC::press_f1_showHelp() {
    About* about_window = new About();
    about_window->show();
}