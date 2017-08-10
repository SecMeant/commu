#ifndef INITWINDOW_H
#define INITWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QFont>
#include <QLineEdit>
#include <QtCore> // Qt::AlignRight

#include <QString>
#include <string>

using std::string;

class InitWindow : public QWidget
{
    Q_OBJECT
public:
    explicit InitWindow(string* nick, string* ip,QWidget *parent = 0);

private:
    QPushButton* m_button;
    QLabel* nickName_label;
    QLabel* serverip_label;
    QLineEdit* nickName_text;
    QLineEdit* serverip_text;

    string* nickName;
    string* serverip;

signals:

public slots:
    void sendButtonClicked(bool checked);
};

#endif // INITWINDOW_H
