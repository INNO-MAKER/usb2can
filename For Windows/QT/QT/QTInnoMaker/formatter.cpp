#include "formatter.h"

Formatter::Formatter()
{

}

QString Formatter::formatString(QString originString,QString charactersInString) {
    QString formatString = "";

    foreach (QChar c , originString)
    {
        if (charactersInString.contains(c))
        {
            formatString += c;
        }
    }

    return formatString;
}

QString Formatter::dealWithString(QString str) {
    QString dealStr = "";

    int i = 0;
    foreach (QChar c ,  str)
    {

        if (++i == 3)
        {
            dealStr += " ";
            i = 1;
        }
        dealStr += c;

    }
    return dealStr;
}

void Formatter::adjustTextFieldToHex(QLineEdit *textBox,int limitLength,bool padding) {
    QString originString = textBox->text();
    if (padding)
    {
        originString = originString.replace(" ", "");
    }

    QString str = formatString(originString, "0123456789ABCDEFabcdef");

    if (str.size() > limitLength)
    {
        str = str.left(limitLength);

    }

    if (padding)
    {
        str = dealWithString(str);
    }
    int pos = textBox->cursorPosition();
    textBox->setText(str);
    textBox->setCursorPosition(pos);
}

QString Formatter::formatStringToHex(QString originString, int limitLength, bool padding) {
    if (padding)
    {
        originString = originString.replace(" ", "");
    }

    QString str = formatString(originString, "0123456789ABCDEFabcdef");

    if (str.size() > limitLength)
    {
        str = str.left(limitLength);

    }

    if (str.size() == 0)
    {
        str = "00";
    }

    if (padding)
    {
        str = dealWithString(str);
    }

    return str;
}
