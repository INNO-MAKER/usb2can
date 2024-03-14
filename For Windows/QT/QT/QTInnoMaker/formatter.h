#ifndef FORMATTER_H
#define FORMATTER_H
#include <QString>
#include <QLineEdit>
class Formatter
{
public:
    Formatter();
    void adjustTextFieldToHex(QLineEdit* textBox,int limitLength,bool padding);
    QString formatStringToHex(QString originString, int limitLength, bool padding);
private:
    QString formatString(QString originString,QString charactersInString);
    QString dealWithString(QString str);

};

#endif // FORMATTER_H
