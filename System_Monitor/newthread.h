#ifndef NEWTHREAD_H
#define NEWTHREAD_H
#include <QThread>
#include <QObject>

class Newthread: public QObject
{
	Q_OBJECT
public:
    Newthread(QString file);
    ~Newthread();
	void openfile();
private:
    QString filename;
};
#endif
