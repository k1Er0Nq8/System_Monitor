#ifndef FINDPRO_H
#define FINDPRO_H

#include <QDialog>

namespace Ui {
class Findpro;
}

class Findpro : public QDialog
{
    Q_OBJECT

public:
    explicit Findpro(QWidget *parent = 0);
    ~Findpro();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Findpro *ui;
};

#endif // FINDPRO_H
