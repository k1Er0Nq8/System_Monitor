#ifndef PROINFO_H
#define PROINFO_H

#include <QDialog>

namespace Ui {
class ProInfo;
}

class ProInfo : public QDialog
{
    Q_OBJECT

public:
    //explicit ProInfo(QWidget *parent = 0);
    explicit ProInfo(QString pid,QString pname,QString pstate,QString ppri,QString pmem,QWidget *parent = 0);
    ~ProInfo();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ProInfo *ui;
};

#endif // PROINFO_H
