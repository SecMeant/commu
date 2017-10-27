#ifndef INITWINDOW_H
#define INITWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QFont>
#include <QLineEdit>
#include <QtCore> // Qt::AlignRight
#include <QCloseEvent>

#include <QString>
#include <string>

using std::string;

class InitWindow : public QWidget
{
    Q_OBJECT
public:
    explicit InitWindow(string* nick, string* ip,bool* retVal = 0,QWidget *parent = 0);
		void closeEvent(QCloseEvent * event);

private:
    QPushButton* m_button;
    QLabel* nickName_label;
    QLabel* serverip_label;
    QLineEdit* nickName_text;
    QLineEdit* serverip_text;

    string* nickName;
    string* serverip;

    // returns 1 when data had been successfuly catched
    // and 0 otherwise
    bool* retValue;

public slots:
    void sendButtonClicked(bool checked);
};

#endif // INITWINDOW_H
