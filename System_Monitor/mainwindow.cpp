#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdatetime.h"
#include "QDateTime"
#include "QDebug"
#include "QThread"
#include "QDir"
#include "qstringlist.h"
#include "errno.h"
#include "qvector.h"
#include "findpro.h"
#include "newthread.h"
#include "QFileDialog"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <qdebug.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer=new QTimer;
    setupRealtimeDataDemo(ui->customPlot);
    setupRealtimeDataDemo(ui->customplot_mem);
    setupRealtimeDataDemo(ui->customplot_swap);
    int index=ui->tabWidget->currentIndex();
    show_Table_Info(index);
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_update()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
                     this, SLOT(changeInfo(int)));
    connect(timer,SIGNAL(timeout()),this,SLOT(info_update()));
    timer->start(1000);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot){
    customPlot->addGraph();
    int gi=1;
    QColor color(20+200/4.0*gi,70*(1.6-gi/4.0), 150, 150);
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->graph(0)->setName("temp");
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->graph(0)->setBrush(QBrush(Qt::yellow));
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
void MainWindow::timer_update()
{
    int index=ui->tabWidget->currentIndex();
    if(index==0||index==1||index==2){
        show_Table_Info(index);
    }
    return ;
}
void MainWindow::info_update(){
    QFile infoFile;
    QString tempstring;
    QDateTime dt;
    QTime time;
    QDate date;
    dt.setTime(time.currentTime());
    dt.setDate(date.currentDate());
    QString currentDate =dt.toString("yyyy:MM:dd hh:mm:ss");
    int run_time=2;
    int cpuinfo[2][7]={0};
    int idle[2]={0};
    int all[2]={0};
    while(run_time){
        infoFile.setFileName("/proc/stat");
        if(!infoFile.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,tr("warning"),tr("The stat file can not open!"),QMessageBox::Yes);
            return;
        }
        tempstring =infoFile.readLine();
        for(int i=0;i<7;i++){
            cpuinfo[2-run_time][i]=tempstring.section(" ",i+2,i+2).toInt();
            all[2-run_time]+=cpuinfo[2-run_time][i];
            if(i==3){
                idle[2-run_time]=cpuinfo[2-run_time][i];
            }
        }
        run_time--;
        infoFile.close();
        QThread::msleep(500);
    }
    int a=all[1]-all[0];
    int b=idle[1]-idle[0];
    if(a<0)
        a=(-a);
    if(b<0)
        b=(-b);
    double usge=100*double(a-b)/double(a);
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    ui->customPlot->graph(0)->addData(key, usge);
    ui->customPlot->xAxis->setTickLabels(false);
    ui->customPlot->yAxis->setRange(0,100);
    ui->customPlot->graph(0)->rescaleValueAxis(true);
    ui->customPlot->xAxis->setRange(key+0.25, 120, Qt::AlignRight);
    ui->customPlot->replot();
    ui->progressBar->setValue(usge);
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    infoFile.close();
    infoFile.setFileName("/proc/meminfo");
    if(!infoFile.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,tr("warning"),tr("The meminfo file can not open!"),QMessageBox::Yes);
        return ;
    }
    int flag=4;
    QString total_mem;
    QString free_mem;
    QString free_swap;
    QString total_swap;
    QString used_swap;
    double ntotal_mem,nfree_mem,nused_mem,nfree_swap,ntotal_swap,nused_swap;
    int pos;
    while(flag>0){
        tempstring=infoFile.readLine();
        pos=tempstring.indexOf("MemTotal");
        if(pos!=-1){
            total_mem=tempstring.mid(pos+17,tempstring.length()-15);
            total_mem=total_mem.trimmed();
            pos=total_mem.indexOf(" kB");
            total_mem=total_mem.mid(0,pos);
            ntotal_mem=total_mem.toDouble();
            flag--;
        }
        else if((pos=tempstring.indexOf("MemFree"))!=-1){
            free_mem=tempstring.mid(pos+18,tempstring.length()-9);
            free_mem=free_mem.trimmed();
            pos=free_mem.indexOf(" kB");
            free_mem=free_mem.mid(0,pos);
            nfree_mem=free_mem.toDouble();
            flag--;
        }
        else if((pos=tempstring.indexOf("SwapTotal"))!=-1){
            total_swap=tempstring.mid(pos+17,tempstring.length()-5);
            total_swap=total_swap.trimmed();
            pos=total_swap.indexOf(" kB");
            total_swap=total_swap.mid(0,pos);
            ntotal_swap=total_swap.toDouble();
            flag--;
        }
        else if((pos=tempstring.indexOf("SwapFree"))!=-1){
            free_swap=tempstring.mid(pos+17,tempstring.length()-5);
            free_swap=free_swap.trimmed();
            pos=free_swap.indexOf(" kB");
            free_swap=free_swap.mid(0,pos);
            nfree_swap=free_swap.toDouble();
            flag--;
        }

    }
    nused_mem=ntotal_mem-nfree_mem;
    ui->progressBar_2->setValue(nused_mem*100/ntotal_mem);
    ui->customplot_mem->graph(0)->addData(key, nused_mem*100/ntotal_mem);
    ui->customplot_mem->xAxis->setTickLabels(false);
    ui->customplot_mem->yAxis->setRange(0,100);
    ui->customplot_mem->graph(0)->rescaleValueAxis(true);
    ui->customplot_mem->xAxis->setRange(key+0.25, 120, Qt::AlignRight);
    ui->customplot_mem->replot();
    nused_swap=ntotal_swap-nfree_swap;
    ui->customplot_swap->graph(0)->addData(key, nused_swap*100/ntotal_swap);
    ui->customplot_swap->xAxis->setTickLabels(false);
    ui->customplot_swap->yAxis->setRange(0,100);
    ui->customplot_swap->graph(0)->rescaleValueAxis(true);
    ui->customplot_swap->xAxis->setRange(key+0.25, 120, Qt::AlignRight);
    ui->customplot_swap->replot();
}
void MainWindow::show_Table_Info(int index){
    QFile infoFile;
    QString tempstring;
    int pos;
    int flag=4;
    if(index==0){
        infoFile.setFileName("/proc/cpuinfo");
        if(!infoFile.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,tr("warning"),tr("The cpuinfo file can not open!"),QMessageBox::Yes);
            return ;
        }
        while(flag>0){
            tempstring=infoFile.readLine();
            pos=tempstring.indexOf("model name");
            if(pos!=-1){
                pos+=13;
                QString *cpuname=new QString(tempstring.mid(pos,tempstring.length()-14));
                ui->label_cpuname->setText(*cpuname);
                flag--;
            }
            else if((pos=tempstring.indexOf("vendor_id"))!=-1){
                pos+=12;
                QString *cputype=new QString(tempstring.mid(pos,tempstring.length()-13));
                ui->label_cputype->setText(*cputype);
                flag--;
            }
            else if((pos=tempstring.indexOf("cpu MHz"))!=-1){
                pos+=10;
                QString *cpuhz=new QString(tempstring.mid(pos,tempstring.length()-11));
                QString Cpuhz=(cpuhz->trimmed())+" MHZ";
                ui->label_cpuhz->setText(Cpuhz);
                qDebug()<<Cpuhz;
                flag--;
            }
            else if((pos=tempstring.indexOf("cache size"))!=-1){
                pos+=12;
                QString *cache=new QString(tempstring.mid(pos,tempstring.length()-13));
                ui->label_cache->setText(*cache);
                flag--;
            }
            else{
                ;
            }
        }
        infoFile.close();
        infoFile.setFileName("/proc/version");
        if(!infoFile.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,tr("warning"),tr("The version file can not open!"),QMessageBox::Yes);
            return ;
        }
        tempstring=infoFile.readLine();
        pos=tempstring.indexOf("version");
        QString *ostype=new QString(tempstring.mid(0,pos-1));
        ui->label_ostype->setText(*ostype);
        int pos1=tempstring.indexOf("(");
        QString *osver=new QString(tempstring.mid(pos,pos1-pos-1));
        ui->label_osver->setText(*osver);
        int pos2=tempstring.indexOf("gcc");
        QString *osgcc=new QString(tempstring.mid(pos2,pos2-18));
        ui->label_gcc->setText(*osgcc);

        infoFile.setFileName("/proc/sys/kernel/hostname");
        if(!infoFile.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,tr("warning"),tr("The host file can not open!"),QMessageBox::Yes);
            return ;
        }
        tempstring=infoFile.readLine();
        infoFile.close();
        infoFile.setFileName("/proc/uptime");
        if(!infoFile.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,tr("warning"),tr("The uptime file can not open!"),QMessageBox::Yes);
            return ;
        }
        tempstring=infoFile.readLine();
        pos=tempstring.indexOf(" ");
        QString *osstart=new QString(tempstring.mid(0,pos));
        double starttime=osstart->toDouble();
        int days=int(starttime)/(60*60*24);
        int hour=int(starttime-double(days*60*60*24))/(60*60);
        int minutes=int(starttime-double(days*60*60*24)-double(hour*60*60))/60;
        QString qdays=QString::number(days);
        QString qhours=QString::number(hour);
        QString qminutes=QString::number(minutes);
        QString qstartTime=qdays+" days "+qhours+" hours "+qminutes+" mins";
        QString *osstay=new QString(tempstring.mid(pos+1,tempstring.length()-1));
        double staytime=(osstay->toDouble())/4.0;
        double worktime=starttime-staytime;
        int workdays=int(worktime)/(60*60*24);
        int workhour=int(worktime-double(workdays*60*60*24))/(60*60);
        int workminutes=int(worktime-double(workdays*60*60*24)-double(workhour*60*60))/60;
        QString wdays=QString::number(workdays);
        QString whours=QString::number(workhour);
        QString wminutes=QString::number(workminutes);
        QString oswork=wdays+" days "+whours+" hours "+wminutes+" mins";
        infoFile.close();
    }
    else if(index==1){
        infoFile.setFileName("/proc/meminfo");
        if(!infoFile.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,tr("warning"),tr("The meminfo file can not open!"),QMessageBox::Yes);
            return ;
        }
        int flag=4;
        QString total_mem;
        QString free_mem;
        QString used_mem;
        QString free_swap;
        QString total_swap;
        QString used_swap;
        double ntotal_mem,nfree_mem,nused_mem,nfree_swap,ntotal_swap,nused_swap;
        int pos;
        while(flag>0){
            tempstring=infoFile.readLine();
            pos=tempstring.indexOf("MemTotal");
            if(pos!=-1){
                total_mem=tempstring.mid(pos+17,tempstring.length()-15);
                total_mem=total_mem.trimmed();
                pos=total_mem.indexOf(" kB");
                total_mem=total_mem.mid(0,pos);
                ntotal_mem=total_mem.toDouble();
                ntotal_mem=ntotal_mem/1024;
                flag--;
            }
            else if((pos=tempstring.indexOf("MemFree"))!=-1){
                free_mem=tempstring.mid(pos+18,tempstring.length()-9);
                free_mem=free_mem.trimmed();
                pos=free_mem.indexOf(" kB");
                free_mem=free_mem.mid(0,pos);
                nfree_mem=free_mem.toDouble();
                nfree_mem=nfree_mem/1024;
                flag--;
            }
            else if((pos=tempstring.indexOf("SwapTotal"))!=-1){
                total_swap=tempstring.mid(pos+17,tempstring.length()-5);
                total_swap=total_swap.trimmed();
                pos=total_swap.indexOf(" kB");
                total_swap=total_swap.mid(0,pos);
                ntotal_swap=total_swap.toDouble();
                ntotal_swap=ntotal_swap/1024;
                flag--;
            }
            else if((pos=tempstring.indexOf("SwapFree"))!=-1){
                free_swap=tempstring.mid(pos+17,tempstring.length()-5);
                free_swap=free_swap.trimmed();
                pos=free_swap.indexOf(" kB");
                free_swap=free_swap.mid(0,pos);
                nfree_swap=free_swap.toDouble();
                nfree_swap=nfree_swap/1024;
                flag--;
            }
        }
        nused_mem=ntotal_mem-nfree_mem;
        nused_swap=ntotal_swap-nfree_swap;
        used_mem=QString::number(nused_mem);
        used_swap=QString::number(nused_swap);
        total_mem=QString::number(ntotal_mem);
        total_swap=QString::number(ntotal_swap);
        free_mem=QString::number(nfree_mem);
        free_swap=QString::number(nfree_swap);

        total_mem=total_mem+" MB";
        total_swap=total_swap+" MB";
        free_mem=free_mem+" MB";
        free_swap=free_swap+" MB";
        used_mem=used_mem+" MB";
        used_swap=used_swap+" MB";
        ui->label_totalmem->setText(total_mem);
        ui->label_freemem->setText(free_mem);
        ui->label_totalswap->setText(total_swap);
        ui->label_freeswap->setText(free_swap);
        ui->label_usedmem->setText(used_mem);
        ui->label_usedswap->setText(used_swap);
        infoFile.close();
    }
    else if(index==2){
        QDir qd("/proc");
        QStringList prolist=qd.entryList();
        QString pro=prolist.join("\n");

        QString pro_id;
        bool ok;
        int find_start=3;
        int posa,posb;
        int npropid;
        int sleep_pro=0,zombie_pro=0,run_pro=0;
        int total_pro=0;
        QString nameofpro;
        QString stateofpro;
        QString priofpro;
        QString memofpro;
        QString ppidofpro;
        QStringList header;
        header<<"PID"<<"名称"<<"状态"<<"优先级"<<"PPID"<<"占用内存";
        ui->tableWidget->setRowCount(1000);
        ui->tableWidget->setColumnCount(6);
        ui->tableWidget->setHorizontalHeaderLabels(header);
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        int cur_row=0;
        while(1){
            posa=pro.indexOf("\n",find_start);
            posb=pro.indexOf("\n",posa+1);
            find_start=posb;
            pro_id=pro.mid(posa+1,posb-posa-1);
            total_pro++;
            npropid=pro_id.toInt(&ok,10);
            if(!ok) break;

            infoFile.setFileName("/proc/"+pro_id+"/stat");
            if(!infoFile.open(QIODevice::ReadOnly)){
                qDebug()<<pro_id;
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
            ppidofpro=tempstring.section(" ",3,3);
            switch (stateofpro.at(0).toLatin1()) {
            case 'S':
                sleep_pro++;
                break;
            case 'R':
                run_pro++;
                break;
            case 'Z':
                zombie_pro++;
                break;
            default:
                break;
            }
            ui->tableWidget->setItem(cur_row,0,new QTableWidgetItem(pro_id));
            ui->tableWidget->setItem(cur_row,1,new QTableWidgetItem(nameofpro));
            ui->tableWidget->setItem(cur_row,2,new QTableWidgetItem(stateofpro));
            ui->tableWidget->setItem(cur_row,3,new QTableWidgetItem(priofpro));
            ui->tableWidget->setItem(cur_row,4,new QTableWidgetItem(ppidofpro));
            ui->tableWidget->setItem(cur_row,5,new QTableWidgetItem(memofpro));
            cur_row++;
            infoFile.close();
        }
        ui->tableWidget->setRowCount(cur_row);
            QString temp;
            temp=QString::number(total_pro,10);
            ui->label_totalpro->setText(temp);
            temp=QString::number(run_pro,10);
            ui->label_runpro->setText(temp);
            temp=QString::number(sleep_pro,10);
            ui->label_sleepro->setText(temp);
            temp=QString::number(zombie_pro,10);
            ui->label_zombie->setText(temp);
            infoFile.close();
        }
    else if(index==3){
    }
    else{
    }
}
void MainWindow::changeInfo(int index){
    show_Table_Info(index);
    return ;
}
void MainWindow::on_pushButton_clicked()
{
    system("shutdown -h now");
}
void MainWindow::on_pushButton_2_clicked()
{
    system("reboot");
}
void MainWindow::on_pushButton_3_clicked()
{
    if(ui->tableWidget->currentItem()){
        QList<QTableWidgetItem*>items=ui->tableWidget->selectedItems();
        qDebug()<<"printf:"<<items;
        QTableWidgetItem *item=items.at(0);
        QString str=item->text();
        system("kill " + str.toLatin1());
        QMessageBox::warning(this, tr("kill"), QString::fromUtf8("kill successfully."), QMessageBox::Yes);
        show_Table_Info(2);
    }
    else{
        Findpro *findpro=new Findpro();
        findpro->show();
        show_Table_Info(2);
    }
}
void MainWindow::on_pushButton_4_clicked()
{
   Findpro *findpro=new Findpro();
   findpro->show();
   show_Table_Info(2);
   return ;
}
void MainWindow::on_pushButton_5_clicked()
{
    QString pro_id=ui->pro_text->text();
    QString id;
    QString name;
    qDebug()<<ui->pro_text->text();
    if(ui->tableWidget->currentItem()){
        for(int i=ui->tableWidget->currentRow()+1;i<ui->tableWidget->rowCount();i++){
            id=ui->tableWidget->item(i,0)->text();
            name=ui->tableWidget->item(i,1)->text();
            if(id.indexOf(pro_id)!=-1||name.indexOf(pro_id)!=-1)
            {
                ui->tableWidget->selectRow(i);
                return ;
            }
        }
    }
    else{
        for(int i=0;i<ui->tableWidget->rowCount();i++){
            id=ui->tableWidget->item(i,0)->text();
            name=ui->tableWidget->item(i,1)->text();
            if(id.indexOf(pro_id)!=-1||name.indexOf(pro_id)!=-1)
            {
                ui->tableWidget->selectRow(i);
                return ;
            }
        }
    }
}
void MainWindow::on_pushButton_6_clicked()
{
    QString file;
      qDebug()<<"find file1";
      file=ui->pro_text->text();

    qDebug()<<"find file2";
    if(!file.isNull()){

        openfile(file);
    }
    else{
        QMessageBox::warning(this, tr("kill"), QString::fromUtf8("The process is killed!"), QMessageBox::Yes);
    }
}
void MainWindow::openfile(QString filename){
    QByteArray fname=filename.toLatin1();
    const char *path=fname.data();
    system(path);
    return;
}
