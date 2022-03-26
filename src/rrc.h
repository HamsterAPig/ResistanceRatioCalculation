#ifndef RRC_H
#define RRC_H

#include <QMainWindow>

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
};
#endif // RRC_H
