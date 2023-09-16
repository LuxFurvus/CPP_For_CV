#include "unrep_ui_stringspinbox.h"

StringSpinBox::StringSpinBox(QWidget* parent)
    : QSpinBox(parent) {
    // Default values for minimum and maximum range
    setMinimum(0);
    setMaximum(0);
}

void StringSpinBox::setStringValues(const QStringList& values) {
    stringValues = values;
    setMinimum(0);
    setMaximum(values.size() - 1);
}

QStringList StringSpinBox::getStringValues() const {
    return stringValues;
}

QString StringSpinBox::textFromValue(int value) const {
    if (value >= 0 && value < stringValues.size())
        return stringValues[value];
    else
        return QString();
}

int StringSpinBox::valueFromText(const QString& text) const {
    return stringValues.indexOf(text);
}
