#ifndef UNREP_UI_STRINGSPINBOX_H
#define UNREP_UI_STRINGSPINBOX_H

#include <QSpinBox>

class StringSpinBox : public QSpinBox{
    Q_OBJECT

private:
    QStringList stringValues;

public:
    StringSpinBox(QWidget* parent = nullptr);

    // Add methods to set and get string values
    void setStringValues(const QStringList& values);
    QStringList getStringValues() const;

    // Override textFromValue and valueFromText methods
    QString textFromValue(int value) const override;
    int valueFromText(const QString& text) const override;
};

#endif // UNREP_UI_STRINGSPINBOX_H
