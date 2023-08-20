#pragma once
#ifndef MASTERWIN_H
#define MASTERWIN_H

#include <msclr/marshal_cppstd.h>
#include "unrep_tokensearch.h"
#include "unrep_reformer.h"
#include "unrep_luafunc.h"

namespace UNREPEATER {
	/// Summary for MasterWin
	public ref class MasterWin : public System::Windows::Forms::Form {
	public:
		MasterWin(void) {
			InitializeComponent();
		}

	protected:
		/// Clean up any resources being used.
		~MasterWin() {
			if (components) {
				delete components;
			}
		}

		//////////////VARIABLES//////////////
	private: bool IsJustOpened = true;

	private: System::ComponentModel::Container^ components;

	private: System::Windows::Forms::Panel^ TextPanel;
	private: System::Windows::Forms::RichTextBox^ InputTextBox;
	private: System::Windows::Forms::TextBox^ OutputWin;

	private: System::Windows::Forms::Button^ AnalyzeButton;
	private: System::Windows::Forms::Button^ ReformButton;
	private: System::Windows::Forms::Button^ ButtonLUAread;

	private: System::Windows::Forms::GroupBox^ LangGB;
	private: System::Windows::Forms::GroupBox^ SizeGB;
	private: System::Windows::Forms::GroupBox^ FrequGB;
	private: System::Windows::Forms::GroupBox^ MainPanel;

	private: System::Windows::Forms::DomainUpDown^ LangDom;
	private: System::Windows::Forms::DomainUpDown^ SizeDom;
	private: System::Windows::Forms::DomainUpDown^ FrequDom;
	private: System::Windows::Forms::FlowLayoutPanel^ SettingsPanel;

		   //////////////////////////////////////////

#pragma region Windows Form Designer generated code
		   /// <summary>
		   /// Required method for Designer support - do not modify
		   /// the contents of this method with the code editor.
		   /// </summary>
		   void InitializeComponent(void)
		   {
			   System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MasterWin::typeid));
			   this->OutputWin = (gcnew System::Windows::Forms::TextBox());
			   this->TextPanel = (gcnew System::Windows::Forms::Panel());
			   this->InputTextBox = (gcnew System::Windows::Forms::RichTextBox());
			   this->MainPanel = (gcnew System::Windows::Forms::GroupBox());
			   this->ButtonLUAread = (gcnew System::Windows::Forms::Button());
			   this->ReformButton = (gcnew System::Windows::Forms::Button());
			   this->SettingsPanel = (gcnew System::Windows::Forms::FlowLayoutPanel());
			   this->FrequGB = (gcnew System::Windows::Forms::GroupBox());
			   this->FrequDom = (gcnew System::Windows::Forms::DomainUpDown());
			   this->SizeGB = (gcnew System::Windows::Forms::GroupBox());
			   this->SizeDom = (gcnew System::Windows::Forms::DomainUpDown());
			   this->LangGB = (gcnew System::Windows::Forms::GroupBox());
			   this->LangDom = (gcnew System::Windows::Forms::DomainUpDown());
			   this->AnalyzeButton = (gcnew System::Windows::Forms::Button());
			   this->TextPanel->SuspendLayout();
			   this->MainPanel->SuspendLayout();
			   this->SettingsPanel->SuspendLayout();
			   this->FrequGB->SuspendLayout();
			   this->SizeGB->SuspendLayout();
			   this->LangGB->SuspendLayout();
			   this->SuspendLayout();
			   //
			   // OutputWin
			   //
			   this->OutputWin->Dock = System::Windows::Forms::DockStyle::Right;
			   this->OutputWin->Font = (gcnew System::Drawing::Font(L"Clear Sans Medium", 15.75F, System::Drawing::FontStyle::Bold));
			   this->OutputWin->Location = System::Drawing::Point(428, 0);
			   this->OutputWin->Multiline = true;
			   this->OutputWin->Name = L"OutputWin";
			   this->OutputWin->ReadOnly = true;
			   this->OutputWin->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			   this->OutputWin->Size = System::Drawing::Size(255, 565);
			   this->OutputWin->TabIndex = 1;
			   this->OutputWin->Text = L"word(3\r\nfreqency(4\r\nhere(5";
			   //
			   // TextPanel
			   //
			   this->TextPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Left)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->TextPanel->Controls->Add(this->InputTextBox);
			   this->TextPanel->Controls->Add(this->OutputWin);
			   this->TextPanel->Location = System::Drawing::Point(0, 97);
			   this->TextPanel->Name = L"TextPanel";
			   this->TextPanel->Size = System::Drawing::Size(683, 565);
			   this->TextPanel->TabIndex = 6;
			   //
			   // InputTextBox
			   //
			   this->InputTextBox->AcceptsTab = true;
			   this->InputTextBox->DetectUrls = false;
			   this->InputTextBox->Dock = System::Windows::Forms::DockStyle::Left;
			   this->InputTextBox->Font = (gcnew System::Drawing::Font(L"Clear Sans", 15.75F));
			   this->InputTextBox->HideSelection = false;
			   this->InputTextBox->Location = System::Drawing::Point(0, 0);
			   this->InputTextBox->Margin = System::Windows::Forms::Padding(0);
			   this->InputTextBox->Name = L"InputTextBox";
			   this->InputTextBox->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::ForcedVertical;
			   this->InputTextBox->Size = System::Drawing::Size(426, 565);
			   this->InputTextBox->TabIndex = 0;
			   this->InputTextBox->Text = L"Insert text here.\nClick:\n\t\"Analyze\" to find repeated words.\n\t\"Reform\" to break th"
				   L"e text into paragraphs, one per sentences.\n\t\"Read LUA\" to interpret text as a Lu"
				   L"a code.";
			   this->InputTextBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MasterWin::InputTextBox_MouseDown);
			   //
			   // MainPanel
			   //
			   this->MainPanel->Controls->Add(this->ButtonLUAread);
			   this->MainPanel->Controls->Add(this->ReformButton);
			   this->MainPanel->Controls->Add(this->SettingsPanel);
			   this->MainPanel->Controls->Add(this->AnalyzeButton);
			   this->MainPanel->Dock = System::Windows::Forms::DockStyle::Top;
			   this->MainPanel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->MainPanel->Font = (gcnew System::Drawing::Font(L"Clear Sans", 11.25F));
			   this->MainPanel->Location = System::Drawing::Point(0, 0);
			   this->MainPanel->Margin = System::Windows::Forms::Padding(0);
			   this->MainPanel->Name = L"MainPanel";
			   this->MainPanel->Padding = System::Windows::Forms::Padding(0);
			   this->MainPanel->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			   this->MainPanel->Size = System::Drawing::Size(684, 101);
			   this->MainPanel->TabIndex = 5;
			   this->MainPanel->TabStop = false;
			   //
			   // ButtonLUAread
			   //
			   this->ButtonLUAread->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			   this->ButtonLUAread->Font = (gcnew System::Drawing::Font(L"Clear Sans", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ButtonLUAread->Location = System::Drawing::Point(619, 17);
			   this->ButtonLUAread->Margin = System::Windows::Forms::Padding(0);
			   this->ButtonLUAread->Name = L"ButtonLUAread";
			   this->ButtonLUAread->Size = System::Drawing::Size(61, 80);
			   this->ButtonLUAread->TabIndex = 9;
			   this->ButtonLUAread->Text = L"READ LUA";
			   this->ButtonLUAread->UseVisualStyleBackColor = true;
			   this->ButtonLUAread->Click += gcnew System::EventHandler(this, &MasterWin::ButtonLUAread_Click);
			   //
			   // ReformButton
			   //
			   this->ReformButton->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			   this->ReformButton->Font = (gcnew System::Drawing::Font(L"Clear Sans", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ReformButton->Location = System::Drawing::Point(526, 17);
			   this->ReformButton->Margin = System::Windows::Forms::Padding(0);
			   this->ReformButton->Name = L"ReformButton";
			   this->ReformButton->Size = System::Drawing::Size(90, 80);
			   this->ReformButton->TabIndex = 8;
			   this->ReformButton->Text = L"REFORM";
			   this->ReformButton->UseVisualStyleBackColor = true;
			   this->ReformButton->Click += gcnew System::EventHandler(this, &MasterWin::ReformButton_Click);
			   //
			   // SettingsPanel
			   //
			   this->SettingsPanel->Controls->Add(this->FrequGB);
			   this->SettingsPanel->Controls->Add(this->SizeGB);
			   this->SettingsPanel->Controls->Add(this->LangGB);
			   this->SettingsPanel->Location = System::Drawing::Point(5, 12);
			   this->SettingsPanel->Name = L"SettingsPanel";
			   this->SettingsPanel->Size = System::Drawing::Size(421, 84);
			   this->SettingsPanel->TabIndex = 7;
			   //
			   // FrequGB
			   //
			   this->FrequGB->Controls->Add(this->FrequDom);
			   this->FrequGB->Location = System::Drawing::Point(289, 3);
			   this->FrequGB->Name = L"FrequGB";
			   this->FrequGB->Size = System::Drawing::Size(129, 79);
			   this->FrequGB->TabIndex = 3;
			   this->FrequGB->TabStop = false;
			   this->FrequGB->Text = L"Min Frequency";
			   //
			   // FrequDom
			   //
			   this->FrequDom->Font = (gcnew System::Drawing::Font(L"Clear Sans", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->FrequDom->Items->Add(9);
			   this->FrequDom->Items->Add(8);
			   this->FrequDom->Items->Add(7);
			   this->FrequDom->Items->Add(6);
			   this->FrequDom->Items->Add(5);
			   this->FrequDom->Items->Add(4);
			   this->FrequDom->Items->Add(3);
			   this->FrequDom->Items->Add(2);
			   this->FrequDom->Location = System::Drawing::Point(28, 26);
			   this->FrequDom->Margin = System::Windows::Forms::Padding(0);
			   this->FrequDom->Name = L"FrequDom";
			   this->FrequDom->ReadOnly = true;
			   this->FrequDom->RightToLeft = System::Windows::Forms::RightToLeft::No;
			   this->FrequDom->Size = System::Drawing::Size(75, 33);
			   this->FrequDom->TabIndex = 36;
			   this->FrequDom->Text = L"3";
			   this->FrequDom->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->FrequDom->Wrap = true;
			   this->FrequDom->SelectedItemChanged += gcnew System::EventHandler(this, &MasterWin::FrequDom_SelectedItemChanged);
			   //
			   // SizeGB
			   //
			   this->SizeGB->Controls->Add(this->SizeDom);
			   this->SizeGB->Location = System::Drawing::Point(150, 3);
			   this->SizeGB->Name = L"SizeGB";
			   this->SizeGB->Size = System::Drawing::Size(133, 79);
			   this->SizeGB->TabIndex = 4;
			   this->SizeGB->TabStop = false;
			   this->SizeGB->Text = L"Min Size";
			   //
			   // SizeDom
			   //
			   this->SizeDom->Font = (gcnew System::Drawing::Font(L"Clear Sans", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->SizeDom->Items->Add(9);
			   this->SizeDom->Items->Add(8);
			   this->SizeDom->Items->Add(7);
			   this->SizeDom->Items->Add(6);
			   this->SizeDom->Items->Add(5);
			   this->SizeDom->Items->Add(4);
			   this->SizeDom->Items->Add(3);
			   this->SizeDom->Items->Add(2);
			   this->SizeDom->Items->Add(1);
			   this->SizeDom->Location = System::Drawing::Point(33, 26);
			   this->SizeDom->Margin = System::Windows::Forms::Padding(0);
			   this->SizeDom->Name = L"SizeDom";
			   this->SizeDom->ReadOnly = true;
			   this->SizeDom->RightToLeft = System::Windows::Forms::RightToLeft::No;
			   this->SizeDom->SelectedIndex = 5;
			   this->SizeDom->Size = System::Drawing::Size(75, 33);
			   this->SizeDom->TabIndex = 2;
			   this->SizeDom->Text = L"4";
			   this->SizeDom->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->SizeDom->Wrap = true;
			   this->SizeDom->SelectedItemChanged += gcnew System::EventHandler(this, &MasterWin::SizeDom_SelectedItemChanged);
			   //
			   // LangGB
			   //
			   this->LangGB->Controls->Add(this->LangDom);
			   this->LangGB->Location = System::Drawing::Point(9, 3);
			   this->LangGB->Name = L"LangGB";
			   this->LangGB->Size = System::Drawing::Size(135, 79);
			   this->LangGB->TabIndex = 5;
			   this->LangGB->TabStop = false;
			   this->LangGB->Text = L"Interface";
			   //
			   // LangDom
			   //
			   this->LangDom->AutoSize = true;
			   this->LangDom->Font = (gcnew System::Drawing::Font(L"Clear Sans Medium", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->LangDom->Items->Add(L"English");
			   this->LangDom->Items->Add(L"Русский");
			   this->LangDom->Location = System::Drawing::Point(9, 29);
			   this->LangDom->Name = L"LangDom";
			   this->LangDom->ReadOnly = true;
			   this->LangDom->RightToLeft = System::Windows::Forms::RightToLeft::No;
			   this->LangDom->SelectedIndex = 0;
			   this->LangDom->Size = System::Drawing::Size(120, 29);
			   this->LangDom->Sorted = true;
			   this->LangDom->TabIndex = 3;
			   this->LangDom->Text = L"English";
			   this->LangDom->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->LangDom->Wrap = true;
			   this->LangDom->SelectedItemChanged += gcnew System::EventHandler(this, &MasterWin::LangDom_SelectedItemChanged);
			   //
			   // AnalyzeButton
			   //
			   this->AnalyzeButton->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			   this->AnalyzeButton->Font = (gcnew System::Drawing::Font(L"Clear Sans", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->AnalyzeButton->Location = System::Drawing::Point(433, 17);
			   this->AnalyzeButton->Margin = System::Windows::Forms::Padding(0);
			   this->AnalyzeButton->Name = L"AnalyzeButton";
			   this->AnalyzeButton->Size = System::Drawing::Size(90, 80);
			   this->AnalyzeButton->TabIndex = 6;
			   this->AnalyzeButton->Text = L"ANALYSE";
			   this->AnalyzeButton->UseVisualStyleBackColor = true;
			   this->AnalyzeButton->Click += gcnew System::EventHandler(this, &MasterWin::AnalyzeButton_Click);
			   //
			   // MasterWin
			   //
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->BackColor = System::Drawing::Color::Gray;
			   this->ClientSize = System::Drawing::Size(684, 661);
			   this->Controls->Add(this->TextPanel);
			   this->Controls->Add(this->MainPanel);
			   this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			   this->MinimumSize = System::Drawing::Size(700, 700);
			   this->Name = L"MasterWin";
			   this->Text = L"UnRePit";
			   this->Load += gcnew System::EventHandler(this, &MasterWin::MasterWin_Load);
			   this->SizeChanged += gcnew System::EventHandler(this, &MasterWin::MasterWin_SizeChanged);
			   this->TextPanel->ResumeLayout(false);
			   this->TextPanel->PerformLayout();
			   this->MainPanel->ResumeLayout(false);
			   this->SettingsPanel->ResumeLayout(false);
			   this->FrequGB->ResumeLayout(false);
			   this->SizeGB->ResumeLayout(false);
			   this->LangGB->ResumeLayout(false);
			   this->LangGB->PerformLayout();
			   this->ResumeLayout(false);
		   }
#pragma endregion

	private: System::Void LangDom_SelectedItemChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->LangDom->SelectedItem == nullptr) return;

		auto Select = dynamic_cast<System::String^>(this->LangDom->SelectedItem);

		if (Select->Equals("English")) {
			this->AnalyzeButton->Text = "ANALYSE";
			this->ReformButton->Text = "REFORM";
			//this->DescText->Text = "Counts frequency of words and word parts in the text";
			this->FrequGB->Text = "Min Frequency";
			this->SizeGB->Text = "Min Size";
			this->LangGB->Text = "Language";
			if (IsJustOpened) {
				this->InputTextBox->Text = "Insert text here.\r\nClick \"Analyze\" to find repeated words, or \"Reform\" to break the text into paragraphs, one per sentences.";
				this->OutputWin->Text = "word(3\r\nfreqency(4\r\nhere(5";
			}
		}
		else if (Select->Equals(L"Русский")) {
			this->AnalyzeButton->Text = L"АНАЛИЗ";
			this->ReformButton->Text = L"РЕФОРМА";
			//this->DescText->Text = "Считает количество повторов в тексте";
			this->FrequGB->Text = L"Мин.Частота";
			this->SizeGB->Text = L"Мин.Длинна";
			this->LangGB->Text = L"Интерфейс";
			if (IsJustOpened) {
				this->InputTextBox->Text = L"Вставьте сюда текст.\r\nНажмите \"Анализ\", чтобы найти повторяющиеся слова, или \"Реформа\", чтобы разбить текст по предложению на абзац.";
				this->OutputWin->Text = L"количество(2\r\nповторов(3\r\nбудет(4\r\nздесь(5";
			}
		}
	}

	private: System::Void MasterWin_Load(System::Object^ sender, System::EventArgs^ e) {
		if (GetLuaBool("HideSettings")) {
			this->LangDom->SelectedIndex = GetLuaInt("InterfaceLang");
			this->SettingsPanel->Hide();
			//this->LangDom->Hide();
			//this->LangGB->Hide();
			//this->FrequGB->Hide();
			//this->SizeDom->Hide();
			//this->SizeDom->Hide();
			//this->FrequDom->Hide();
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
		this->LangDom->SelectedIndex = GetLuaInt("InterfaceLang");

		this->SizeDom->SelectedIndex = 9 - SizeIn;
		this->FrequDom->SelectedIndex = 9 - FreqIn;
	}

		   ////////////TEXT WONDOWS CONTROL////////////

		   //Function to clears Input window from prompt
	private: System::Void InputTextBox_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (IsJustOpened) {
			this->InputTextBox->Text = "";
			this->OutputWin->Text = "";
			IsJustOpened = false;
		}
	}
		   //Function to scale Input & Output windows
	private: System::Void MasterWin_SizeChanged(System::Object^ sender, System::EventArgs^ e) {
		int OutWinWidth = static_cast<int>(this->ClientSize.Width * 0.33f);
		int OutWinHeight = this->OutputWin->Size.Height;

		int InWinWidth = static_cast<int>(this->ClientSize.Width - OutWinWidth - 5);
		int InWinHeight = this->InputTextBox->Size.Height;

		this->OutputWin->Size = System::Drawing::Size(OutWinWidth, OutWinHeight);
		this->InputTextBox->Size = System::Drawing::Size(InWinWidth, InWinHeight);
	}

		   ////////////TOKENIZER STAT CONTROL////////////

	private: System::Void SizeDom_SelectedItemChanged(System::Object^ sender, System::EventArgs^ e) {
		TokenSearcher::getInstance().setTokenMinSize(System::Convert::ToInt32(SizeDom->SelectedItem->ToString()));
	}
	private: System::Void FrequDom_SelectedItemChanged(System::Object^ sender, System::EventArgs^ e) {
		TokenSearcher::getInstance().setTokenMinFrequency(System::Convert::ToInt32(FrequDom->SelectedItem->ToString()));
	}

		   ////////////BUTTONS////////////

	private: System::Void AnalyzeButton_Click(System::Object^ sender, System::EventArgs^ e) {
		IsJustOpened = false;

		std::string TextToDo = msclr::interop::marshal_as<std::string>(this->InputTextBox->Text);
		std::string TokenList = TokenSearcher::getInstance().AnalyzeTokens(TextToDo);

		this->OutputWin->Text = msclr::interop::marshal_as<System::String^>(TokenList);
	}

	private: System::Void ReformButton_Click(System::Object^ sender, System::EventArgs^ e) {
		IsJustOpened = false;

		if (GetLuaBool("UseLuaCode")) {
			std::string TextToDo = msclr::interop::marshal_as<std::string>(this->InputTextBox->Text);

			bool IsLuaError = CallLuaReformer(TextToDo);

			if (IsLuaError) {
				this->OutputWin->Text = msclr::interop::marshal_as<System::String^>(TextToDo);
				return;
			}
			else {
				this->InputTextBox->Text = msclr::interop::marshal_as<System::String^>(TextToDo);
				this->OutputWin->Text = "";
				return;
			}
		}
		else {
			const std::wstring input = msclr::interop::marshal_as<std::wstring>(this->InputTextBox->Text);
			std::wstring output = TextReformer::getInstance().ReformText(input);
			this->InputTextBox->Text = msclr::interop::marshal_as<System::String^>(output);
			return;
		}
	}

	private: System::Void ButtonLUAread_Click(System::Object^ sender, System::EventArgs^ e) {
		IsJustOpened = false;

		LuaInterpreter::FlushOutput();

		const std::string input = msclr::interop::marshal_as<std::string>(this->InputTextBox->Text);

		auto output = LuaInterpreter::ReadLua(input.c_str());

		this->OutputWin->Text = msclr::interop::marshal_as<System::String^>(output);
	}
	};
}

#endif // MASTERWIN_H
