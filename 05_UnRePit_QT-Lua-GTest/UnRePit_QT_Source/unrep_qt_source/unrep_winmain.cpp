#include "unrep_winmain.h"
#include "ui_unrep_winmain.h"
#include "unrep_reformer.h"
#include "unrep_luafunc.h"
#include "unrep_tokensearch.h"
#include <QLineEdit>

unrep_winmain::unrep_winmain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::unrep_winmain){
    ui->setupUi(this);

    QPalette palette;
    palette.setColor(QPalette::Base, QColor(200, 200, 200));
    ui->OutputTxtWin->setPalette(palette);

    ///////////////////////////////////////////////////////


    int langNum = GetLuaInt("InterfaceLang");

    if (GetLuaBool("HideSettings")) {
        ui->spin_lang->setValue(langNum);

        ui->FRM_Settings->hide();
        return;
    }

    int SizeIn = 4; // from 1 to 9
    int FreqIn = 2; // from 2 to 9

    int LuaSize = GetLuaInt("MinTokenSize");
    if (LuaSize >= 1 && LuaSize <= 9) {
        SizeIn = LuaSize;
    }
    int LuaFreq = GetLuaInt("MinTokenFreqency");
    if (LuaFreq >= 2 && LuaFreq <= 9) {
        FreqIn = LuaFreq;
    }

    ui->spin_lang->setValue(langNum);
    ui->spin_size->setValue(SizeIn);
    ui->spin_freq->setValue(FreqIn);

    QLineEdit* lineEdit = ui->spin_lang->findChild<QLineEdit*>();
    lineEdit->setReadOnly(true);
    lineEdit->setFocusPolicy(Qt::NoFocus);
    connect(ui->spin_lang, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        ui->spin_lang, [&, lineEdit](){lineEdit->deselect();}, Qt::QueuedConnection);

    lineEdit = ui->spin_size->findChild<QLineEdit*>();
    connect(ui->spin_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        ui->spin_size, [&, lineEdit](){lineEdit->deselect();}, Qt::QueuedConnection);

    lineEdit = ui->spin_freq->findChild<QLineEdit*>();
    connect(ui->spin_freq, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        ui->spin_freq, [&, lineEdit](){lineEdit->deselect();}, Qt::QueuedConnection);

}

unrep_winmain::~unrep_winmain(){
    delete ui;
}

////////////TOKENIZER STAT CONTROL////////////


void unrep_winmain::on_spin_lang_valueChanged(int arg1){

    switch (arg1) {
    case 0:
        ui->spin_lang->setPrefix("English (");
        ui->spin_lang->setSuffix(")");
        ui->label_lang->setText("Interface Language");
        ui->label_size->setText("Min Size");
        ui->label_freq->setText("Min Frequency");
        ui->button_analyse->setText("ANALYZE");
        ui->button_reform->setText("REFORM");
        ui->button_readlua->setText("READ LUA");
        break;
    case 1:
        ui->spin_lang->setPrefix("Русский (");
        ui->spin_lang->setSuffix(")");
        ui->label_lang->setText("Язык Интерфейса");
        ui->label_size->setText("Мин. Длинна");
        ui->label_freq->setText("Мин. Частота");
        ui->button_analyse->setText("АНАЛИЗ");
        ui->button_reform->setText("РЕФОРМ");
        break;
    default:
        ui->spin_lang->setPrefix("none (");
        ui->spin_lang->setSuffix(")");
        break;
    }

}

void unrep_winmain::on_spin_size_valueChanged(int arg1){
    TokenSearcher::getInstance().setTokenMinSize(arg1);
}

void unrep_winmain::on_spin_freq_valueChanged(int arg1){
    TokenSearcher::getInstance().setTokenMinFrequency(arg1);
}

////////////TEXT WINDOWS CONTROL////////////

//Function to clears Input window from prompt

void unrep_winmain::on_InputTxtWin_selectionChanged(){

    if (IsJustOpened) {

        ui->InputTxtWin->setPlainText("");

        ui->OutputTxtWin->setPlainText("");

        IsJustOpened = false;
    }

}

////////////BUTTONS////////////


void unrep_winmain::on_button_reform_clicked(){

    IsJustOpened = false;

    if (GetLuaBool("UseLuaCode")) {
        std::string TextToDo = ui->InputTxtWin->toPlainText().toStdString();

        bool IsLuaError = CallLuaReformer(TextToDo);

        if (IsLuaError) {
            ui->OutputTxtWin->setPlainText(QString::fromStdString(TextToDo));
            return;
        }
        else {
            ui->InputTxtWin->setPlainText(QString::fromStdString(TextToDo));
            ui->OutputTxtWin->setPlainText (QString::fromStdString(""));
            return;
        }
    }
    else {
        std::wstring InputText = ui->InputTxtWin->toPlainText().toStdWString();

        std::wstring OutputText = TextReformer::ReformText(InputText);

        ui->InputTxtWin->setPlainText(QString::fromStdWString(OutputText));
    }
}


void unrep_winmain::on_button_readlua_clicked(){

    IsJustOpened = false;

    LuaInterpreter::FlushOutput();

    const std::string input = ui->InputTxtWin->toPlainText().toStdString();

    std::string output = LuaInterpreter::ReadLua(input.c_str());

    ui->OutputTxtWin->setPlainText(QString::fromStdString(output));

}


void unrep_winmain::on_button_analyse_clicked(){

    IsJustOpened = false;

    std::string TextToDo = ui->InputTxtWin->toPlainText().toStdString();

    std::string TokenList = TokenSearcher::getInstance().AnalyzeTokens(TextToDo);

    ui->OutputTxtWin->setPlainText(QString::fromStdString(TokenList));

}
