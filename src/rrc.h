#ifndef RRC_H
#define RRC_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class RRC; }
QT_END_NAMESPACE

class RRC : public QMainWindow
{
    Q_OBJECT

public:
    RRC(QWidget *parent = nullptr);
    ~RRC();

private:
    Ui::RRC *ui;

    QVector < QVector < QVector< float > > > vec_resistances;
    void read_csv(QString path);
};
#endif // RRC_H
