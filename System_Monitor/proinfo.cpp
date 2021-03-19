#include "proinfo.h"
#include "ui_proinfo.h"
#include "QMessageBox"
ProInfo::ProInfo(QString pid, QString pname, QString pstate, QString ppri, QString pmem, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProInfo)
{
    ui->setupUi(this);
    ui->label_pid->setText(pid);
    ui->label_pname->setText(pname);
    ui->label_pstate->setText(pstate);
    ui->label_ppri->setText(ppri);
    ui->label_pmem->setText(pmem);
}

ProInfo::~ProInfo()
{
    delete ui;
}

void ProInfo::on_pushButton_clicked()
{
    QString pid=ui->label_pid->text();
    system("kill " + pid.toLatin1());
    QMessageBox::warning(this, tr("kill"), QString::fromUtf8("The process is killed!"), QMessageBox::Yes);
    this->close();
}
