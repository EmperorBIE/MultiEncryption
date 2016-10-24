#ifndef MULTIENCRYPTION_H
#define MULTIENCRYPTION_H

#include <QWidget>
#include <QKeyEvent>
#include <QRadioButton>
#include <QGroupBox>
#include <QString>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include "encryptable.h"
#include "playfair.h"
#include "hill.h"

namespace Ui {
class MultiEncryption;
}

class MultiEncryption : public QWidget
{
    Q_OBJECT

public:
    explicit MultiEncryption(QWidget *parent = 0);
    ~MultiEncryption();

protected:
    void keyPressEvent(QKeyEvent * e);

private:
    Ui::MultiEncryption *ui;

    QRadioButton *playfairBtn;
    QRadioButton *hillBtn;

    QLayout *dispLayout;
    QLabel *dispLabel;

    QLineEdit *keyLnEdit;
    QTextEdit *plainTextEdit;
    QTextEdit *cipherTextEdit;

    QPushButton *encryptBtn;
    QPushButton *decryptBtn;

    Encryptable *encryptor;
    Playfair playfair;
    Hill hill;

private slots:
    void onRadioClickFunc();
    void onButtonClickEncrypt();
    void onButtonClickDecrypt();
};

#endif // MULTIENCRYPTION_H
