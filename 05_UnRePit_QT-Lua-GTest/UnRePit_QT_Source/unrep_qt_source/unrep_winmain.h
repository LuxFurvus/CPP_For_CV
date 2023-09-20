#ifndef UNREP_WINMAIN_H
#define UNREP_WINMAIN_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPalette>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui { class unrep_winmain; }
QT_END_NAMESPACE

class unrep_winmain : public QMainWindow{
    Q_OBJECT

private: Ui::unrep_winmain *ui;
private: bool IsJustOpened = true;

public:
    unrep_winmain(QWidget *parent = nullptr);
    ~unrep_winmain();

private slots:
    void on_button_reform_clicked();
    void on_button_readlua_clicked();
    void on_button_analyse_clicked();
    void on_spin_size_valueChanged(int arg1);
    void on_InputTxtWin_selectionChanged();
    void on_spin_freq_valueChanged(int arg1);
    void on_spin_lang_valueChanged(int arg1);
};

#endif // UNREP_WINMAIN_H
