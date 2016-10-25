#include "multiencryption.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultiEncryption w;
    w.setWindowTitle("MultiEncryption");
    w.resize(685, 425);
    w.setWindowIcon(QIcon("MultiEncryption.ico"));
    w.show();

    return a.exec();
}
