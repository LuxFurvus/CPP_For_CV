#pragma once

#include <vector>
#include <unordered_set>
#include <cctype>
#include <algorithm>
#include <map>
#include <msclr/marshal_cppstd.h>

namespace UNREPEATER {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

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
	private:
		std::string CleanTheText();
		bool IsAlphaEngRu(char w);
		size_t CountOneToken(const std::string& String, const std::string& Token);
		void FindLeaderPair(std::string& MaxStr, size_t& MaxNum, std::map<std::string, size_t>& WordMap);
		std::map<std::string, size_t> TokenizeText();
		std::string AnalyseText();
		///
		bool IsSpace(wchar_t ch);
		bool IsSentenceEnd(wchar_t ch);
		std::wstring RemoveDoubleSpaces();
		void BreakText();

	private: System::Windows::Forms::TextBox^ InputWin;
	private: System::Windows::Forms::TextBox^ OutputWin;

	protected:

	protected:

	private:
		/// Required designer variable.
		System::ComponentModel::Container^ components;
	private: System::Windows::Forms::Panel^ TextPanel;
	private: System::Windows::Forms::Button^ AnalyzeButton;

	private: System::Windows::Forms::GroupBox^ MainPanel;

		   //////////////VARIABLES//////////////
		   bool IsJustOpened = true;

		   uint16_t TokenMinSize = 4;
	private: System::Windows::Forms::Button^ ReformButton;

	private: System::Windows::Forms::FlowLayoutPanel^ SettingsPanel;
	private: System::Windows::Forms::GroupBox^ FrequGB;
	private: System::Windows::Forms::DomainUpDown^ FrequDom;
	private: System::Windows::Forms::GroupBox^ SizeGB;
	private: System::Windows::Forms::DomainUpDown^ SizeDom;
	private: System::Windows::Forms::GroupBox^ LangGB;
	private: System::Windows::Forms::DomainUpDown^ LangDom;
		   uint16_t TokenMinFrequency = 3;
		   //////////////////////////////////////////

#pragma region Windows Form Designer generated code
		   /// <summary>
		   /// Required method for Designer support - do not modify
		   /// the contents of this method with the code editor.
		   /// </summary>
		   void InitializeComponent(void)
		   {
			   System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MasterWin::typeid));
			   this->InputWin = (gcnew System::Windows::Forms::TextBox());
			   this->OutputWin = (gcnew System::Windows::Forms::TextBox());
			   this->TextPanel = (gcnew System::Windows::Forms::Panel());
			   this->AnalyzeButton = (gcnew System::Windows::Forms::Button());
			   this->MainPanel = (gcnew System::Windows::Forms::GroupBox());
			   this->ReformButton = (gcnew System::Windows::Forms::Button());
			   this->SettingsPanel = (gcnew System::Windows::Forms::FlowLayoutPanel());
			   this->FrequGB = (gcnew System::Windows::Forms::GroupBox());
			   this->FrequDom = (gcnew System::Windows::Forms::DomainUpDown());
			   this->SizeGB = (gcnew System::Windows::Forms::GroupBox());
			   this->SizeDom = (gcnew System::Windows::Forms::DomainUpDown());
			   this->LangGB = (gcnew System::Windows::Forms::GroupBox());
			   this->LangDom = (gcnew System::Windows::Forms::DomainUpDown());
			   this->TextPanel->SuspendLayout();
			   this->MainPanel->SuspendLayout();
			   this->SettingsPanel->SuspendLayout();
			   this->FrequGB->SuspendLayout();
			   this->SizeGB->SuspendLayout();
			   this->LangGB->SuspendLayout();
			   this->SuspendLayout();
			   //
			   // InputWin
			   //
			   this->InputWin->AcceptsReturn = true;
			   this->InputWin->AcceptsTab = true;
			   this->InputWin->AllowDrop = true;
			   this->InputWin->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Left)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->InputWin->Font = (gcnew System::Drawing::Font(L"Clear Sans", 15.75F));
			   this->InputWin->Location = System::Drawing::Point(5, 8);
			   this->InputWin->Margin = System::Windows::Forms::Padding(0);
			   this->InputWin->Multiline = true;
			   this->InputWin->Name = L"InputWin";
			   this->InputWin->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			   this->InputWin->Size = System::Drawing::Size(421, 553);
			   this->InputWin->TabIndex = 0;
			   this->InputWin->Text = L"Insert text here.\r\nClick \"Analyze\" to find repeated words, or \"Reform\" to break t"
				   L"he text into paragraphs, one per sentences.";
			   //
			   // OutputWin
			   //
			   this->OutputWin->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->OutputWin->Font = (gcnew System::Drawing::Font(L"Clear Sans Medium", 15.75F, System::Drawing::FontStyle::Bold));
			   this->OutputWin->Location = System::Drawing::Point(433, 8);
			   this->OutputWin->Multiline = true;
			   this->OutputWin->Name = L"OutputWin";
			   this->OutputWin->ReadOnly = true;
			   this->OutputWin->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			   this->OutputWin->Size = System::Drawing::Size(247, 553);
			   this->OutputWin->TabIndex = 1;
			   this->OutputWin->Text = L"word(3\r\nfreqency(4\r\nhere(5";
			   //
			   // TextPanel
			   //
			   this->TextPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Left)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->TextPanel->Controls->Add(this->InputWin);
			   this->TextPanel->Controls->Add(this->OutputWin);
			   this->TextPanel->Location = System::Drawing::Point(0, 97);
			   this->TextPanel->Name = L"TextPanel";
			   this->TextPanel->Size = System::Drawing::Size(684, 564);
			   this->TextPanel->TabIndex = 6;
			   //
			   // AnalyzeButton
			   //
			   this->AnalyzeButton->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			   this->AnalyzeButton->Font = (gcnew System::Drawing::Font(L"Clear Sans", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->AnalyzeButton->Location = System::Drawing::Point(452, 15);
			   this->AnalyzeButton->Margin = System::Windows::Forms::Padding(0);
			   this->AnalyzeButton->Name = L"AnalyzeButton";
			   this->AnalyzeButton->Size = System::Drawing::Size(90, 80);
			   this->AnalyzeButton->TabIndex = 6;
			   this->AnalyzeButton->Text = L"ANALYSE";
			   this->AnalyzeButton->UseVisualStyleBackColor = true;
			   this->AnalyzeButton->Click += gcnew System::EventHandler(this, &MasterWin::AnalyzeButton_Click);
			   //
			   // MainPanel
			   //
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
			   // ReformButton
			   //
			   this->ReformButton->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			   this->ReformButton->Font = (gcnew System::Drawing::Font(L"Clear Sans", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ReformButton->Location = System::Drawing::Point(567, 15);
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
			   this->FrequDom->SelectedIndex = 7;
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
			   // MasterWin
			   //
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->BackColor = System::Drawing::Color::Gray;
			   this->ClientSize = System::Drawing::Size(684, 661);
			   this->Controls->Add(this->TextPanel);
			   this->Controls->Add(this->MainPanel);
			   this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			   this->Name = L"MasterWin";
			   this->Text = L"UnRepeater";
			   this->Load += gcnew System::EventHandler(this, &MasterWin::MasterWin_Load);
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
		auto Select = dynamic_cast<String^>(this->LangDom->SelectedItem);

		if (Select->Equals("English")) {
			this->AnalyzeButton->Text = "ANALYSE";
			this->ReformButton->Text = "REFORM";
			//this->DescText->Text = "Counts frequency of words and word parts in the text";
			this->FrequGB->Text = "Min Frequency";
			this->SizeGB->Text = "Min Size";
			this->LangGB->Text = "Language";
			if (IsJustOpened) {
				this->InputWin->Text = "Insert text here.\r\nClick \"Analyze\" to find repeated words, or \"Reform\" to break the text into paragraphs, one per sentences.";
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
				this->InputWin->Text = L"Вставьте сюда текст.\r\nНажмите \"Анализ\", чтобы найти повторяющиеся слова, или \"Реформа\", чтобы разбить текст по предложению на абзац.";
				this->OutputWin->Text = L"количество(2\r\nповторов(3\r\nбудет(4\r\nздесь(5";
			}
		}
	}
	private: System::Void AnalyzeButton_Click(System::Object^ sender, System::EventArgs^ e) {
		IsJustOpened = false;
		this->OutputWin->Text = gcnew System::String(AnalyseText().c_str());
	}
	private: System::Void SizeDom_SelectedItemChanged(System::Object^ sender, System::EventArgs^ e) {
		TokenMinSize = System::Convert::ToUInt16(SizeDom->SelectedItem->ToString());
	}
	private: System::Void FrequDom_SelectedItemChanged(System::Object^ sender, System::EventArgs^ e) {
		TokenMinFrequency = System::Convert::ToUInt16(FrequDom->SelectedItem->ToString());
	}
	private: System::Void ReformButton_Click(System::Object^ sender, System::EventArgs^ e) {
		BreakText();
	}
	private: System::Void MasterWin_Load(System::Object^ sender, System::EventArgs^ e) {
		this->SizeDom->SelectedIndex = 5;
		this->FrequDom->SelectedIndex = 7;
	}
	};
}