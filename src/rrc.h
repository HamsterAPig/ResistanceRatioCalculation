#ifndef RRC_H
#define RRC_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QVector>
#include <QtSvg>
#include <QtSvgWidgets/QGraphicsSvgItem>

QT_BEGIN_NAMESPACE
namespace Ui { class RRC; }
QT_END_NAMESPACE

class RRC : public QMainWindow
{
    Q_OBJECT

public:
    RRC(QWidget *parent = nullptr);
    ~RRC();

private slots:
    void on_calc_btn_clicked();
    void press_f1_showHelp();

private:
    Ui::RRC *ui;

    QVector < QVector < QVector< float > > > vec_resistances;
    QStandardItemModel* model;
    QLabel* pragram_status;

    /**
     * 读取csv文件里面的数据到QVector中方便后续查找
     * @param path csv文件路径
     */
    void read_csv(QString path);
};
#endif // RRC_H
