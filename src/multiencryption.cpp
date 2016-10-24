#include "multiencryption.h"
#include "ui_multiencryption.h"
#include "playfair.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QString>
#include <QMessageBox>
#include <QSplitter>
#include <string>
#include <QDebug>

using namespace std;

MultiEncryption::MultiEncryption(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiEncryption)
{
    ui->setupUi(this);

    // build functionality area
    QGroupBox *funcGroupBox = new QGroupBox("Functionality", this);

    playfairBtn = new QRadioButton("Playfair", this);
    hillBtn = new QRadioButton("Hill", this);

    playfairBtn->setChecked(true);

    encryptor = &playfair;

    QHBoxLayout *funcHBox = new QHBoxLayout;

    funcHBox->addWidget(playfairBtn);
    funcHBox->addWidget(hillBtn);

    funcGroupBox->setLayout(funcHBox);

    QVBoxLayout *vbox = new QVBoxLayout(this);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(funcGroupBox);

    vbox->addLayout(hbox);
    vbox->addSpacing(10);

    // build setting area
    keyLnEdit = new QLineEdit("monarchy", this);
    plainTextEdit = new QTextEdit(this);
    cipherTextEdit = new QTextEdit(this);

    cipherTextEdit->setReadOnly(true);

    QLabel *keyLbl = new QLabel("Key:", this);
    keyLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel *plainLbl = new QLabel("Plain text:", this);
    plainLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel *cipherLbl = new QLabel("Cipher text:", this);
    cipherLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QGridLayout *settGrid = new QGridLayout;

    settGrid->addWidget(keyLbl, 0, 0);
    settGrid->addWidget(keyLnEdit, 0, 1);
    settGrid->addWidget(plainLbl, 1, 0);
    settGrid->addWidget(plainTextEdit, 1, 1);
    settGrid->addWidget(cipherLbl, 2, 0);
    settGrid->addWidget(cipherTextEdit, 2, 1);

    QGroupBox *settGroupBox = new QGroupBox("Settings", this);
    settGroupBox->setLayout(settGrid);

    QHBoxLayout *settHBox = new QHBoxLayout;
    settHBox->addWidget(settGroupBox);

    QHBoxLayout *hbox2 = new QHBoxLayout;
    hbox2->addLayout(settHBox);

    QVBoxLayout *vbox2 = new QVBoxLayout;
    hbox2->addLayout(vbox2);

    // build button area
    QHBoxLayout *hbox3 = new QHBoxLayout;
    encryptBtn = new QPushButton("Encrtpt", this);
    decryptBtn = new QPushButton("Decrtpt", this);

    hbox3->addWidget(encryptBtn);
    hbox3->addWidget(decryptBtn);

    QGroupBox *btnGroupBox = new QGroupBox("Buttons", this);
    btnGroupBox->setLayout(hbox3);

    QHBoxLayout *btnHBox = new QHBoxLayout;
    btnHBox->addWidget(btnGroupBox);
    vbox2->addLayout(btnHBox);

    // build display area
    dispLayout = new QVBoxLayout;

    dispLabel = new QLabel(this);
    dispLabel->setFont(QFont("Purisa", 10));
    dispLabel->setText(playfair.toString().c_str());

    dispLayout->addWidget(dispLabel);

    QGroupBox *dispGroupBox = new QGroupBox("Display detail", this);
    dispGroupBox->setLayout(dispLayout);

    QHBoxLayout *dispHBox = new QHBoxLayout;
    dispHBox->addWidget(dispGroupBox);

    vbox2->addLayout(dispHBox);
    vbox2->addStretch(1);

    // connect signal to slot
    connect(encryptBtn, SIGNAL(clicked(bool)), this,
                    SLOT(onButtonClickEncrypt()));
    connect(encryptBtn, SIGNAL(clicked(bool)), this,
                    SLOT(onButtonClickDecrypt()));
    connect(playfairBtn, SIGNAL(clicked(bool)), this,
                    SLOT(onRadioClickFunc()));
    connect(hillBtn, SIGNAL(clicked(bool)), this,
                    SLOT(onRadioClickFunc()));

    vbox->addLayout(hbox2);
    setLayout(vbox);
}

MultiEncryption::~MultiEncryption()
{
    delete ui;
    delete playfairBtn;
    delete hillBtn;

    delete keyLnEdit;
    delete plainTextEdit;
    delete cipherTextEdit;

    delete encryptBtn;
    delete decryptBtn;
}

void MultiEncryption::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Escape) {
       qApp->quit();
   }
}

void MultiEncryption::onRadioClickFunc()
{
    if(sender() == playfairBtn) {
        encryptor = &playfair;

        dispLabel->setText(playfair.toString().c_str());
    }else if(sender() == hillBtn) {
        QMessageBox::about(nullptr, "Attention",
                           "Checked Hill");
    }else {
        QMessageBox::about(nullptr, "Attention",
                           "Unsupported");
    }

}


void MultiEncryption::onButtonClickEncrypt()
{
    const QString key = keyLnEdit->text();
    const QString plainText = plainTextEdit->toPlainText();

    qDebug() << "Input Key: " + key;
    qDebug() << "Input plain text: " + plainText;

    encryptor->setKeyword(key.toStdString());

    const QString cipherText = encryptor->encrypt(
                    plainText.toStdString()).c_str();

    qDebug() << "Cipher text: " + cipherText;

    cipherTextEdit->setText(cipherText);
}

void MultiEncryption::onButtonClickDecrypt()
{

}
