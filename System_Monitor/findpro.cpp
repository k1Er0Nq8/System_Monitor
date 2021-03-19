#include "findpro.h"
#include "ui_findpro.h"
#include "QDir"
#include "proinfo.h"
#include "QMessageBox"
Findpro::Findpro(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Findpro)
{
    ui->setupUi(this);
}

Findpro::~Findpro()
{
    delete ui;
}

void Findpro::on_pushButton_clicked()
{
    QString input=ui->lineEdit->text();
    QFile infoFile;
    QString tempstring;
    QDir qd("/proc");
    QStringList prolist=qd.entryList();
    QString pro=prolist.join("\n");
    QString pro_id;
    bool ok;
    int find_start=3;
    int posa,posb;
    int npropid;
    int total_pro=0;
    int flag=0;
    QString nameofpro;
    QString stateofpro;
    QString priofpro;
    QString memofpro;
    while(!flag){
        posa=pro.indexOf("\n",find_start);
        posb=pro.indexOf("\n",posa+1);
        find_start=posb;
        pro_id=pro.mid(posa+1,posb-posa-1);
        total_pro++;
        npropid=pro_id.toInt(&ok,10);
        if(!ok) break;
        if(QString::compare(input,pro_id)==0){
            flag=1;
            infoFile.setFileName("/proc/"+pro_id+"/stat");
            if(!infoFile.open(QIODevice::ReadOnly)){
                QMessageBox::warning(this,tr("warning"),tr("The pid stat file can not open!"),QMessageBox::Yes);
                return;
            }
            tempstring=infoFile.readLine();
            if(tempstring.length()==0)  break;
            posa=tempstring.indexOf("(");
            posb=tempstring.indexOf(")");
            nameofpro=tempstring.mid(posa+1,posb-posa-1);
            nameofpro=nameofpro.trimmed();
            stateofpro=tempstring.section(" ",2,2);
            priofpro=tempstring.section(" ",17,17);
            memofpro=tempstring.section(" ",22,22);
            ProInfo *proinfo=new ProInfo(pro_id,nameofpro,stateofpro,priofpro,memofpro);
            proinfo->show();
            break;
        }
        else{
            infoFile.setFileName("/proc/"+pro_id+"/stat");
            if(!infoFile.open(QIODevice::ReadOnly)){
                QMessageBox::warning(this,tr("warning"),tr("The pid stat file can not open!"),QMessageBox::Yes);
                return;
            }
            tempstring=infoFile.readLine();
            if(tempstring.length()==0)  break;
            posa=tempstring.indexOf("(");
            posb=tempstring.indexOf(")");
            nameofpro=tempstring.mid(posa+1,posb-posa-1);
            nameofpro=nameofpro.trimmed();
            stateofpro=tempstring.section(" ",2,2);
            priofpro=tempstring.section(" ",17,17);
            memofpro=tempstring.section(" ",22,22);
            if(QString::compare(input,nameofpro)==0){
                //output information
                flag=1;
                ProInfo *proinfo=new ProInfo(pro_id,nameofpro,stateofpro,priofpro,memofpro);
                proinfo->show();
                break;
            }
        }
    }
    if(flag==0){
       QMessageBox::information(this,tr("Sorry"),tr("The process can not find!"));
    }
    infoFile.close();
}
