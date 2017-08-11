#include "chatwindow.h"
#include <QDebug>

using namespace std;

ChatWindow::ChatWindow(SOCKET &cs,QWidget *parent)
    : QWidget(parent),
      send_button(new QPushButton("",this)),
      chat_main(new QScrollArea(this)),
      chat_label(new QLabel(chat_main,0)),
      msg_text(new QLineEdit(this)),
      connectSocket(cs)
{
    // window settings
    this->QWidget::setFixedSize(800,800);
    this->setStyleSheet("QLabel { background-color: #383838; color:#ffa500; }");

    // chat scroll area settings
    this->chat_main->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    this->chat_main->setWidgetResizable( true );
    this->chat_main->setGeometry( 10, 10, 770, 740 );
    this->chat_main->setWidget(chat_label);

    // chat label settings
    this->chat_label->setText("");
    this->chat_label->setFont(QFont("Calibri",12));
    this->chat_label->setGeometry(5,5,770,740);
    this->chat_label->setAlignment(Qt::AlignLeft);

    // msg text settings
    this->msg_text->setGeometry(10,760,700,30);

    // Button settings
    this->send_button->setText("Send");
    this->send_button->setGeometry(710,760,80,30);

    connect(send_button, SIGNAL(released()),this,SLOT(sendButtonClicked()));
    connect(this, SIGNAL(adjustLabelSignal()),this,SLOT(adjustLabelSlot()));
    connect(msg_text,SIGNAL(returnPressed()),this,SLOT(sendMSG()));
}

ChatWindow& ChatWindow::operator<<(const string msg)
{
    // preparing to append message from socket to chat label
    QString receivedMessages = this->chat_label->text();
    QString qmsg(msg.c_str());

    // getting pointer to main chat scroll
    QScrollBar* scrollBar = this->chat_main->verticalScrollBar();

    // will scroll automagicaly if scroll is already at lowest position
    // if someone reads older messages won't break he's 'session'
    if(scrollBar->maximum()-30 <= scrollBar->value())
    {
        this->chat_label->setText(receivedMessages + qmsg);
        scrollBar->setValue(scrollBar->maximum());
    }
    else{
        this->chat_label->setText(receivedMessages + qmsg);
    }

    // adjusting size of scroll area to child label
    // so if child size starts overgrowing parent adds scroll
    emit adjustLabelSignal();

    return *this;
}

ChatWindow& ChatWindow::operator<<(const char* msg)
{
    // preparing to append message from socket to chat label
    QString receivedMessages = this->chat_label->text();
    QString qmsg(msg);

    // getting pointer to main chat scroll
    QScrollBar* scrollBar = this->chat_main->verticalScrollBar();

    // will scroll automagicaly if scroll is already at lowest position
    // if someone reads older messages won't break he's 'session'
    if(scrollBar->maximum()-30 <= scrollBar->value())
    {
        this->chat_label->setText(receivedMessages + qmsg);
        scrollBar->setValue(scrollBar->maximum());
    }
    else
    {
        this->chat_label->setText(receivedMessages + qmsg);
    }

    // adjusting size of scroll area to child label
    // so if child size starts overgrowing parent adds scroll
    emit adjustLabelSignal();

    return *this;
}

SOCKET ChatWindow::getSock()
{
    return this->connectSocket;
}

void ChatWindow::sendButtonClicked()
{
    QString msg = this->msg_text->text();
    this->msg_text->setText("");

    mlProto mlp;

    mlp.fillFrame('1',"",msg.toLocal8Bit().constData());
    string sendbuff = mlp.packFrame();
    send(this->getSock(), sendbuff.c_str(), sendbuff.size()+1,0);
}

void ChatWindow::adjustLabelSlot()
{
    this->chat_label->adjustSize();
    return;
}

void ChatWindow::sendMSG()
{
    QString msg = this->msg_text->text();
    this->msg_text->setText("");

    mlProto mlp;

    mlp.fillFrame('1',"",msg.toLocal8Bit().constData());
    string sendbuff = mlp.packFrame();
    send(this->getSock(), sendbuff.c_str(), sendbuff.size()+1,0);
}
