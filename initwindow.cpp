#include "initwindow.h"

using std::string;

InitWindow::InitWindow(string* nick, string* ip, QWidget *parent) :
    QWidget(parent),
    m_button(new QPushButton("",this)),
    nickName_label(new QLabel(this,0)),
    serverip_label(new QLabel(this,0)),
    nickName_text(new QLineEdit("",this)),
    serverip_text(new QLineEdit("",this)),
    nickName(nick),
    serverip(ip)
{
    // window settings
    this->QWidget::setFixedSize(300,80);

    // nickname label settings
    this->nickName_label->setText("Nick:");
    this->nickName_label->setFont(QFont("Calibri",9));
    this->nickName_label->setGeometry(10,12,50,30);
    this->nickName_label->setAlignment(Qt::AlignRight);

    // serverip label settings
    this->serverip_label->setText("Server IP:");
    this->serverip_label->setFont(QFont("Calibri",9));
    this->serverip_label->setGeometry(10,34,50,30);

    // nickname text settings
    this->nickName_text->setGeometry(60,10,100,20);

    // serverip text settings
    this->serverip_text->setGeometry(60,40,100,20);

    // Button settings
    this->m_button->setText("Connect");
    this->m_button->setGeometry(200,34,80,30);

    connect(m_button, SIGNAL(clicked(bool)),this,SLOT(sendButtonClicked(bool)));

}

void InitWindow::sendButtonClicked(bool checked)
{
    qDebug() << checked << '\n';
    QString nn = this->nickName_text->text();
    QString sip = this->serverip_text->text();

    if(nn.size() && sip.size())
    {
        this->nickName->operator=(nn.toLocal8Bit().constData());
        this->serverip->operator=(sip.toLocal8Bit().constData());
        this->close();
    }
}
