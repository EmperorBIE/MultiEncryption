#include "multiencryption.h"
#include "ui_multiencryption.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QString>
#include <QMessageBox>
#include <QSplitter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
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

    // build option area
    QVBoxLayout *vbox3 = new QVBoxLayout;
    readFileChkBox = new QCheckBox("Read from file", this);
    writeFileChkBox = new QCheckBox("Write to file", this);

    vbox3->addWidget(readFileChkBox);
    vbox3->addWidget(writeFileChkBox);

    QGroupBox *optionGroupBox = new QGroupBox("Options", this);
    optionGroupBox->setLayout(vbox3);

    QHBoxLayout *optionHBox = new QHBoxLayout;
    optionHBox->addWidget(optionGroupBox);
    vbox2->addLayout(optionHBox);

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

    // build about area
    QLabel *authorLbl = new QLabel(
            "This program was developed by gaunthan.For more "
            "information about the author, please visit the "
            "following link:"
            "<a href = \"http://blog.leanote.com/gaunthan\">Home Page</a>",
                this);
    authorLbl->setTextFormat(Qt::RichText);
    authorLbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    authorLbl->setOpenExternalLinks(true);

    authorLbl->setGeometry(QRect(328, 240, 329, 27*4));
    authorLbl->setWordWrap(true);
    authorLbl->setAlignment(Qt::AlignTop);

    QVBoxLayout *vbox4 = new QVBoxLayout;
    vbox4->addWidget(authorLbl);

    QGroupBox *aboutGroupBox = new QGroupBox("About", this);
    aboutGroupBox->setLayout(vbox4);

    QHBoxLayout *aboutHBox = new QHBoxLayout;
    aboutHBox->addWidget(aboutGroupBox);

    vbox2->addLayout(aboutHBox);

    // connect signal to slot
    connect(encryptBtn, SIGNAL(clicked(bool)), this,
                    SLOT(onButtonClickEncrypt()));
    connect(decryptBtn, SIGNAL(clicked(bool)), this,
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

        keyLnEdit->setEnabled(true);
        encryptor->setKeyword(keyLnEdit->text().toStdString());
        dispLabel->setText(playfair.toString().c_str());
    }else if(sender() == hillBtn) {
        encryptor = &hill;

        keyLnEdit->clear();
        keyLnEdit->setEnabled(false);
        dispLabel->setText(hill.toString().c_str());
    }else {
        QMessageBox::about(nullptr, "Attention",
                           "Unsupported functionality!");
    }
}

namespace {
const QString ReadFromFile()
{
    const QString filename = QFileDialog::getOpenFileName(nullptr,
                                 "Choose file to read", ".");
    if(filename.isEmpty())
        return "";
    QFile readFile(filename);
    if(!readFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Warning",
                             "Cannot open file for reading");
        return "";
    }
    return readFile.readAll();
}

void WriteToFile(const QString& content)
{
    const QString filename = QFileDialog::getSaveFileName(nullptr,
                                 "Create file to save result", ".");
    if(filename.isNull())
        return;
    QFile writeFile(filename);
    if(!writeFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "Warning",
                             "Cannot open file for writing");
        return;
    }
    QTextStream out(&writeFile);
    out << content;
}

}

void MultiEncryption::onButtonClickEncrypt()
{
    QString plainText;
    if(readFileChkBox->checkState() == Qt::Checked) {
        plainText = ReadFromFile();
        plainTextEdit->setText(plainText);
    }else {
        plainText = plainTextEdit->toPlainText();
    }

    const QString key = keyLnEdit->text();

    qDebug() << "Input Key: " + key;
    qDebug() << "Input plain text: " + plainText;

    encryptor->setKeyword(key.toStdString());

    const QString cipherText = encryptor->encrypt(
                    plainText.toStdString()).c_str();

    qDebug() << "Cipher text: " + cipherText;

    cipherTextEdit->setText(cipherText);

    dispLabel->setText(encryptor->toString().c_str());

    if(writeFileChkBox->checkState() == Qt::Checked)
        WriteToFile(cipherText);
}

void MultiEncryption::onButtonClickDecrypt()
{
    QString cipherText;
    if(readFileChkBox->checkState() == Qt::Checked) {
        cipherText = ReadFromFile();
        cipherTextEdit->setText(cipherText);
    }else {
        cipherText = cipherTextEdit->toPlainText();
    }

    const QString key = keyLnEdit->text();

    qDebug() << "Input Key: " + key;
    qDebug() << "Input cipher text: " + cipherText;

    encryptor->setKeyword(key.toStdString());

    const QString plainText = encryptor->decrypt(
                    cipherText.toStdString()).c_str();

    qDebug() << "Plain text: " + plainText;

    plainTextEdit->setText(plainText);

    dispLabel->setText(encryptor->toString().c_str());

    if(writeFileChkBox->checkState() == Qt::Checked)
        WriteToFile(plainText);
}
