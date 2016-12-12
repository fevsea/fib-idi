#include <QLineEdit>
#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

class MyLineEdit: public QLineEdit{
    Q_OBJECT
    public:
        MyLineEdit(QWidget *parent);
    public slots:
        void tractaReturn();
    signals:
        void enviaText(const QString &);
};

#endif // MYLINEEDIT_H
