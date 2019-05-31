#ifndef ALPHAMESSAGEBOX_H
#define ALPHAMESSAGEBOX_H

#include<QMessageBox>

class AlphaMessageBox : public QMessageBox {

public :

protected:
    void showEvent(QShowEvent* event) {
        QMessageBox::showEvent(event);
        setFixedSize(640, 480);
    }
};
#endif // ALPHAMESSAGEBOX_H
