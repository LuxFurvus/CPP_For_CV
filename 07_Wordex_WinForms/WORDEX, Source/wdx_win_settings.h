#pragma once
#ifndef _WDX_SETTINGSWIN_H_
#define _WDX_SETTINGSWIN_H_
//Copyright (c) 2023 Alex Lykov. All rights reserved.

#include <unordered_set>

#include "wdx_core.h"

namespace WDX {
	public ref class SettingsWin : public System::Windows::Forms::Form {
	public:
		SettingsWin(
			System::Windows::Forms::Form^ MainWinF,
			System::Windows::Forms::Form^ GameWinF);

	private: System::Windows::Forms::CheckedListBox^ ListAlailableDecks;
	private: System::Windows::Forms::GroupBox^ GroupCurrentDeck;

	private: System::Windows::Forms::Label^ LabelInfo;
	private: System::Windows::Forms::TextBox^ TxtProfName;
	private: System::Windows::Forms::Label^ LabelAsk;
	private: System::Windows::Forms::Button^ ButtonToGame;
	private: System::Windows::Forms::GroupBox^ GroupAvailDecks;

	private: System::Windows::Forms::RadioButton^ RadioDelimCustom;

	private: System::Windows::Forms::RadioButton^ RadioDelimDefault;
	private: System::Windows::Forms::TextBox^ TextDelimiter;
	private: System::Windows::Forms::RadioButton^ RadioLetters;
	private: System::Windows::Forms::RadioButton^ RadioPairs;
	private: System::Windows::Forms::GroupBox^ GroupHints;

	private: System::Windows::Forms::DomainUpDown^ ListLoadedDecks;
	private: System::Windows::Forms::Button^ ButtonAddDecks;
	private: System::Windows::Forms::Label^ LabelDeckSize;

	private: System::Windows::Forms::Button^ ButtonToMain;
	private: System::ComponentModel::IContainer^ components;

	protected:
		~SettingsWin() {
			if (components) {
				delete components;
			}
		}

	private:
		System::Windows::Forms::Form^ GameplayWin;
		System::Windows::Forms::Form^ MainWin;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void) {
			this->ButtonToMain = (gcnew System::Windows::Forms::Button());
			this->ListAlailableDecks = (gcnew System::Windows::Forms::CheckedListBox());
			this->GroupCurrentDeck = (gcnew System::Windows::Forms::GroupBox());
			this->ListLoadedDecks = (gcnew System::Windows::Forms::DomainUpDown());
			this->LabelDeckSize = (gcnew System::Windows::Forms::Label());
			this->LabelInfo = (gcnew System::Windows::Forms::Label());
			this->TxtProfName = (gcnew System::Windows::Forms::TextBox());
			this->LabelAsk = (gcnew System::Windows::Forms::Label());
			this->ButtonToGame = (gcnew System::Windows::Forms::Button());
			this->GroupAvailDecks = (gcnew System::Windows::Forms::GroupBox());
			this->ButtonAddDecks = (gcnew System::Windows::Forms::Button());
			this->TextDelimiter = (gcnew System::Windows::Forms::TextBox());
			this->RadioDelimCustom = (gcnew System::Windows::Forms::RadioButton());
			this->RadioDelimDefault = (gcnew System::Windows::Forms::RadioButton());
			this->RadioLetters = (gcnew System::Windows::Forms::RadioButton());
			this->RadioPairs = (gcnew System::Windows::Forms::RadioButton());
			this->GroupHints = (gcnew System::Windows::Forms::GroupBox());
			this->GroupCurrentDeck->SuspendLayout();
			this->GroupAvailDecks->SuspendLayout();
			this->GroupHints->SuspendLayout();
			this->SuspendLayout();
			//
			// ButtonToMain
			//
			this->ButtonToMain->AutoSize = true;
			this->ButtonToMain->Font = (gcnew System::Drawing::Font(L"Consolas", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ButtonToMain->Location = System::Drawing::Point(117, 564);
			this->ButtonToMain->Margin = System::Windows::Forms::Padding(0);
			this->ButtonToMain->Name = L"ButtonToMain";
			this->ButtonToMain->Size = System::Drawing::Size(150, 45);
			this->ButtonToMain->TabIndex = 0;
			this->ButtonToMain->Text = L"Back";
			this->ButtonToMain->UseVisualStyleBackColor = true;
			this->ButtonToMain->Click += gcnew System::EventHandler(this, &SettingsWin::ButtonToMain_Click);
			//
			// ListAlailableDecks
			//
			this->ListAlailableDecks->BackColor = System::Drawing::Color::PeachPuff;
			this->ListAlailableDecks->Font = (gcnew System::Drawing::Font(L"Raleway", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ListAlailableDecks->FormattingEnabled = true;
			this->ListAlailableDecks->Location = System::Drawing::Point(18, 40);
			this->ListAlailableDecks->Name = L"ListAlailableDecks";
			this->ListAlailableDecks->ScrollAlwaysVisible = true;
			this->ListAlailableDecks->Size = System::Drawing::Size(303, 166);
			this->ListAlailableDecks->Sorted = true;
			this->ListAlailableDecks->TabIndex = 6;
			//
			// GroupCurrentDeck
			//
			this->GroupCurrentDeck->BackColor = System::Drawing::Color::Transparent;
			this->GroupCurrentDeck->Controls->Add(this->ListLoadedDecks);
			this->GroupCurrentDeck->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 18, System::Drawing::FontStyle::Bold));
			this->GroupCurrentDeck->Location = System::Drawing::Point(41, 326);
			this->GroupCurrentDeck->Name = L"GroupCurrentDeck";
			this->GroupCurrentDeck->Size = System::Drawing::Size(397, 136);
			this->GroupCurrentDeck->TabIndex = 7;
			this->GroupCurrentDeck->TabStop = false;
			this->GroupCurrentDeck->Text = L"Current Deck";
			//
			// ListLoadedDecks
			//
			this->ListLoadedDecks->InterceptArrowKeys = false;
			this->ListLoadedDecks->Location = System::Drawing::Point(18, 59);
			this->ListLoadedDecks->Name = L"ListLoadedDecks";
			this->ListLoadedDecks->ReadOnly = true;
			this->ListLoadedDecks->Size = System::Drawing::Size(303, 40);
			this->ListLoadedDecks->TabIndex = 11;
			this->ListLoadedDecks->Text = L"domainupdown1";
			this->ListLoadedDecks->SelectedItemChanged += gcnew System::EventHandler(this, &SettingsWin::ListLoadedDecks_SelectedItemChanged);
			//
			// LabelDeckSize
			//
			this->LabelDeckSize->BackColor = System::Drawing::SystemColors::ControlLight;
			this->LabelDeckSize->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->LabelDeckSize->Font = (gcnew System::Drawing::Font(L"Clear Sans", 14, System::Drawing::FontStyle::Bold));
			this->LabelDeckSize->Location = System::Drawing::Point(368, 385);
			this->LabelDeckSize->Name = L"LabelDeckSize";
			this->LabelDeckSize->Size = System::Drawing::Size(64, 40);
			this->LabelDeckSize->TabIndex = 12;
			this->LabelDeckSize->Text = L"0000";
			this->LabelDeckSize->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			//
			// LabelInfo
			//
			this->LabelInfo->BackColor = System::Drawing::Color::Transparent;
			this->LabelInfo->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->LabelInfo->Font = (gcnew System::Drawing::Font(L"Libre Franklin", 15, System::Drawing::FontStyle::Bold));
			this->LabelInfo->ForeColor = System::Drawing::Color::DarkRed;
			this->LabelInfo->Location = System::Drawing::Point(0, 487);
			this->LabelInfo->Name = L"LabelInfo";
			this->LabelInfo->Size = System::Drawing::Size(684, 174);
			this->LabelInfo->TabIndex = 12;
			this->LabelInfo->Text = L"Profile Name must have only Latin letters and numbers.\r\nAdd at least one deck!";
			this->LabelInfo->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			//
			// TxtProfName
			//
			this->TxtProfName->BackColor = System::Drawing::Color::PeachPuff;
			this->TxtProfName->Font = (gcnew System::Drawing::Font(L"Clear Sans", 17, System::Drawing::FontStyle::Bold));
			this->TxtProfName->Location = System::Drawing::Point(362, 35);
			this->TxtProfName->MaxLength = 7;
			this->TxtProfName->Name = L"TxtProfName";
			this->TxtProfName->Size = System::Drawing::Size(293, 38);
			this->TxtProfName->TabIndex = 11;
			this->TxtProfName->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			//
			// LabelAsk
			//
			this->LabelAsk->AutoSize = true;
			this->LabelAsk->BackColor = System::Drawing::Color::Transparent;
			this->LabelAsk->Font = (gcnew System::Drawing::Font(L"Century", 18, System::Drawing::FontStyle::Bold));
			this->LabelAsk->Location = System::Drawing::Point(36, 40);
			this->LabelAsk->Name = L"LabelAsk";
			this->LabelAsk->Size = System::Drawing::Size(320, 28);
			this->LabelAsk->TabIndex = 10;
			this->LabelAsk->Text = L"Enter your Profile Name:";
			//
			// ButtonToGame
			//
			this->ButtonToGame->AutoSize = true;
			this->ButtonToGame->Font = (gcnew System::Drawing::Font(L"Consolas", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ButtonToGame->Location = System::Drawing::Point(424, 564);
			this->ButtonToGame->Margin = System::Windows::Forms::Padding(0);
			this->ButtonToGame->Name = L"ButtonToGame";
			this->ButtonToGame->Size = System::Drawing::Size(150, 45);
			this->ButtonToGame->TabIndex = 13;
			this->ButtonToGame->Text = L"Begin";
			this->ButtonToGame->UseVisualStyleBackColor = true;
			this->ButtonToGame->Click += gcnew System::EventHandler(this, &SettingsWin::ButtonToGame_Click);
			//
			// GroupAvailDecks
			//
			this->GroupAvailDecks->BackColor = System::Drawing::Color::Transparent;
			this->GroupAvailDecks->Controls->Add(this->ButtonAddDecks);
			this->GroupAvailDecks->Controls->Add(this->TextDelimiter);
			this->GroupAvailDecks->Controls->Add(this->RadioDelimCustom);
			this->GroupAvailDecks->Controls->Add(this->RadioDelimDefault);
			this->GroupAvailDecks->Controls->Add(this->ListAlailableDecks);
			this->GroupAvailDecks->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 18, System::Drawing::FontStyle::Bold));
			this->GroupAvailDecks->Location = System::Drawing::Point(41, 94);
			this->GroupAvailDecks->Name = L"GroupAvailDecks";
			this->GroupAvailDecks->Size = System::Drawing::Size(397, 226);
			this->GroupAvailDecks->TabIndex = 8;
			this->GroupAvailDecks->TabStop = false;
			this->GroupAvailDecks->Text = L"Available Decks";
			//
			// ButtonAddDecks
			//
			this->ButtonAddDecks->AutoSize = true;
			this->ButtonAddDecks->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->ButtonAddDecks->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 15, System::Drawing::FontStyle::Bold));
			this->ButtonAddDecks->Location = System::Drawing::Point(329, 162);
			this->ButtonAddDecks->Name = L"ButtonAddDecks";
			this->ButtonAddDecks->Size = System::Drawing::Size(62, 47);
			this->ButtonAddDecks->TabIndex = 10;
			this->ButtonAddDecks->Text = L"Add";
			this->ButtonAddDecks->UseVisualStyleBackColor = false;
			this->ButtonAddDecks->Click += gcnew System::EventHandler(this, &SettingsWin::ButtonAddDecks_Click);
			//
			// TextDelimiter
			//
			this->TextDelimiter->BackColor = System::Drawing::Color::PeachPuff;
			this->TextDelimiter->Font = (gcnew System::Drawing::Font(L"Clear Sans", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->TextDelimiter->Location = System::Drawing::Point(351, 113);
			this->TextDelimiter->MaxLength = 1;
			this->TextDelimiter->Name = L"TextDelimiter";
			this->TextDelimiter->Size = System::Drawing::Size(29, 29);
			this->TextDelimiter->TabIndex = 9;
			this->TextDelimiter->Text = L"=";
			this->TextDelimiter->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->TextDelimiter->WordWrap = false;
			this->TextDelimiter->TextChanged += gcnew System::EventHandler(this, &SettingsWin::TextDelimiter_TextChanged);
			//
			// RadioDelimCustom
			//
			this->RadioDelimCustom->AutoSize = true;
			this->RadioDelimCustom->Location = System::Drawing::Point(329, 120);
			this->RadioDelimCustom->Name = L"RadioDelimCustom";
			this->RadioDelimCustom->Size = System::Drawing::Size(14, 13);
			this->RadioDelimCustom->TabIndex = 8;
			this->RadioDelimCustom->UseVisualStyleBackColor = true;
			this->RadioDelimCustom->CheckedChanged += gcnew System::EventHandler(this, &SettingsWin::RadioDelimCustom_CheckedChanged);
			//
			// RadioDelimDefault
			//
			this->RadioDelimDefault->AutoSize = true;
			this->RadioDelimDefault->Checked = true;
			this->RadioDelimDefault->Font = (gcnew System::Drawing::Font(L"Clear Sans Medium", 15));
			this->RadioDelimDefault->Location = System::Drawing::Point(329, 56);
			this->RadioDelimDefault->Name = L"RadioDelimDefault";
			this->RadioDelimDefault->Size = System::Drawing::Size(64, 32);
			this->RadioDelimDefault->TabIndex = 7;
			this->RadioDelimDefault->TabStop = true;
			this->RadioDelimDefault->Text = L"Tab";
			this->RadioDelimDefault->UseVisualStyleBackColor = true;
			this->RadioDelimDefault->CheckedChanged += gcnew System::EventHandler(this, &SettingsWin::RadioDelimDefault_CheckedChanged);
			//
			// RadioLetters
			//
			this->RadioLetters->AutoSize = true;
			this->RadioLetters->Font = (gcnew System::Drawing::Font(L"Clear Sans Medium", 15));
			this->RadioLetters->Location = System::Drawing::Point(44, 56);
			this->RadioLetters->Name = L"RadioLetters";
			this->RadioLetters->Size = System::Drawing::Size(94, 32);
			this->RadioLetters->TabIndex = 10;
			this->RadioLetters->Text = L"Letters";
			this->RadioLetters->UseVisualStyleBackColor = true;
			this->RadioLetters->CheckedChanged += gcnew System::EventHandler(this, &SettingsWin::RadioLetters_CheckedChanged);
			//
			// RadioPairs
			//
			this->RadioPairs->AutoSize = true;
			this->RadioPairs->Checked = true;
			this->RadioPairs->Font = (gcnew System::Drawing::Font(L"Clear Sans Medium", 15));
			this->RadioPairs->Location = System::Drawing::Point(44, 110);
			this->RadioPairs->Name = L"RadioPairs";
			this->RadioPairs->Size = System::Drawing::Size(76, 32);
			this->RadioPairs->TabIndex = 14;
			this->RadioPairs->TabStop = true;
			this->RadioPairs->Text = L"Pairs";
			this->RadioPairs->UseVisualStyleBackColor = true;
			this->RadioPairs->CheckedChanged += gcnew System::EventHandler(this, &SettingsWin::RadioPairs_CheckedChanged);
			//
			// GroupHints
			//
			this->GroupHints->BackColor = System::Drawing::Color::Transparent;
			this->GroupHints->Controls->Add(this->RadioPairs);
			this->GroupHints->Controls->Add(this->RadioLetters);
			this->GroupHints->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 18, System::Drawing::FontStyle::Bold));
			this->GroupHints->Location = System::Drawing::Point(455, 94);
			this->GroupHints->Name = L"GroupHints";
			this->GroupHints->Size = System::Drawing::Size(200, 226);
			this->GroupHints->TabIndex = 10;
			this->GroupHints->TabStop = false;
			this->GroupHints->Text = L"Hint type";
			//
			// SettingsWin
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::RosyBrown;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(684, 661);
			this->Controls->Add(this->LabelDeckSize);
			this->Controls->Add(this->GroupAvailDecks);
			this->Controls->Add(this->ButtonToGame);
			this->Controls->Add(this->TxtProfName);
			this->Controls->Add(this->LabelAsk);
			this->Controls->Add(this->ButtonToMain);
			this->Controls->Add(this->GroupCurrentDeck);
			this->Controls->Add(this->LabelInfo);
			this->Controls->Add(this->GroupHints);
			this->MaximumSize = System::Drawing::Size(700, 700);
			this->MinimumSize = System::Drawing::Size(700, 700);
			this->Name = L"SettingsWin";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"WORDEX";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &SettingsWin::SettingsWin_FormClosed);
			this->VisibleChanged += gcnew System::EventHandler(this, &SettingsWin::SettingsWin_VisibleChanged);
			this->GroupCurrentDeck->ResumeLayout(false);
			this->GroupAvailDecks->ResumeLayout(false);
			this->GroupAvailDecks->PerformLayout();
			this->GroupHints->ResumeLayout(false);
			this->GroupHints->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////

	private:
		System::Void SettingsWin_VisibleChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void SettingsWin_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e);
		System::Void ListLoadedDecks_SelectedItemChanged(
			System::Object^ sender,
			System::EventArgs^ e);

		System::Void ButtonToMain_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ButtonToGame_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ButtonAddDecks_Click(
			System::Object^ sender,
			System::EventArgs^ e);

		bool IsAlNum(char c);
		bool AddSelectedDecks();
		void FormDeckLists(bool IsNewGame);
		bool CanWePlay();

		/////////////////////////////////////////////////
		/////////////////////////////////////////////////
		/////////////////////////////////////////////////

	private:
		System::Void RadioLetters_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void RadioPairs_CheckedChanged(System::Object^ sender, System::EventArgs^ e);

		////////////////////////////////////////////////////////////
		/////////////DELIMITER SETTINGS/////////////////////////////
		////////////////////////////////////////////////////////////

	private:
		System::Void RadioDelimCustom_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void RadioDelimDefault_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void TextDelimiter_TextChanged(System::Object^ sender, System::EventArgs^ e);

		//////////////////////////////////////////////////////////////////
	};

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	SettingsWin::SettingsWin(
		System::Windows::Forms::Form^ MainWinF,
		System::Windows::Forms::Form^ GameWinF) {
		InitializeComponent();
		this->MainWin = MainWinF;
		this->GameplayWin = GameWinF;
	}

	System::Void SettingsWin::ButtonToMain_Click(System::Object^ sender, System::EventArgs^ e) {
		MainWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		MainWin->Location = this->Location;
		MainWin->Size = this->Size;
		this->Visible = false;
		MainWin->Show();
	}

	System::Void SettingsWin::RadioDelimCustom_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->RadioDelimCustom->Checked) {
			this->RadioDelimDefault->Checked = false;
			GameCoreClass::GetCore().SetDelimiter(static_cast<char>(this->TextDelimiter->Text[0]));
		}
	}
	System::Void SettingsWin::RadioDelimDefault_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->RadioDelimDefault->Checked) {
			this->RadioDelimCustom->Checked = false;
			GameCoreClass::GetCore().SetDelimiter('\t');
		}
	}
	System::Void SettingsWin::TextDelimiter_TextChanged(System::Object^ sender, System::EventArgs^ e) {
		this->RadioDelimCustom->Checked = true;
	}

	System::Void SettingsWin::SettingsWin_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
		System::Windows::Forms::Application::Exit();
	}

	System::Void SettingsWin::RadioLetters_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		GameCoreClass::GetCore().CurrentProfile_.LettersOnly = true;
		if (this->RadioLetters->Checked) {
			this->RadioPairs->Checked = false;
		}
	}
	System::Void SettingsWin::RadioPairs_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		GameCoreClass::GetCore().CurrentProfile_.LettersOnly = false;
		if (this->RadioPairs->Checked) {
			this->RadioLetters->Checked = false;
		}
	}

	System::Void SettingsWin::SettingsWin_VisibleChanged(
		System::Object^ sender, System::EventArgs^ e)
	{
		if (this->Visible == false) return;

		this->LabelInfo->Hide();

		/////LOADED DECKS PANEL////////////////////////////
		if (GameCoreClass::GetCore().GetLoadedDecksNum() != 0) {
			this->GroupCurrentDeck->Show();
			this->ListLoadedDecks->Show();
			this->LabelDeckSize->Show();
			this->LabelDeckSize->Text = System::Convert::ToString(GameCoreClass::GetCore().GetCurrentDeckSize());
		}
		else {
			this->GroupCurrentDeck->Hide();
			this->ListLoadedDecks->Hide();
			this->LabelDeckSize->Hide();
		}

		switch (GameCoreClass::GetCore().GetAccessPoint()) {
		case AccessPoint::kGameStart:

			this->GroupCurrentDeck->Hide();
			this->ListLoadedDecks->Hide();
			this->LabelDeckSize->Hide();

			this->LabelAsk->Show();
			this->TxtProfName->Show();

			this->ButtonToMain->Show();

			this->ButtonToMain->Text = L"Main Menu";

			this->ButtonToGame->Show();
			this->ButtonToGame->Text = L"Start Game";

			this->RadioDelimDefault->Checked = true;
			this->ButtonToMain->Location = System::Drawing::Point(
				(this->Size.Width / 4) - (ButtonToMain->Size.Width / 2),
				this->ButtonToMain->Location.Y);
			this->ButtonToGame->Location = System::Drawing::Point(
				(this->Size.Width * 3 / 4) - (ButtonToMain->Size.Width / 2),
				this->ButtonToMain->Location.Y);

			FormDeckLists(true);

			if (this->ListAlailableDecks->Items->Count == 0) {
				this->LabelInfo->Show();
				this->LabelInfo->Text = "There are no available decks!";
				this->ButtonToGame->Hide();
				return;
			}

			return;
		case AccessPoint::kFromGame:
			this->LabelAsk->Hide();
			this->TxtProfName->Hide();

			this->ButtonToMain->Show();
			this->ButtonToMain->Text = L"Main Menu";

			this->ButtonToGame->Show();
			this->ButtonToGame->Text = L"Back to Game";

			this->ButtonToMain->Location = System::Drawing::Point(
				(this->Size.Width / 4) - (ButtonToMain->Size.Width / 2),
				this->ButtonToMain->Location.Y);
			this->ButtonToGame->Location = System::Drawing::Point(
				(this->Size.Width * 3 / 4) - (ButtonToMain->Size.Width / 2),
				this->ButtonToMain->Location.Y);

			FormDeckLists(false);

			for (int i = 0; i < this->ListLoadedDecks->Items->Count; ++i) {
				std::string CurrentDeckName_ = GameCoreClass::GetCore().GetCurrentDeckName();
				System::String^ item = this->ListLoadedDecks->Items[i]->ToString();

				// Compare the item with the CurrentDeckName_
				if (item->Equals(gcnew System::String(CurrentDeckName_.c_str()))) {
					// Check the check box at index i
					this->ListLoadedDecks->SelectedIndex = i;
					// Exit the loop once the item is found
					break;
				}
			}

			return;
		default:
			return;
		}
	}

	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	bool SettingsWin::AddSelectedDecks() {
		if (this->ListAlailableDecks->CheckedItems->Count == 0
			&& GameCoreClass::GetCore().GetAccessPoint() == AccessPoint::kFromGame) {
			return true;
		}
		if (this->ListAlailableDecks->CheckedItems->Count == 0
			&& GameCoreClass::GetCore().GetAccessPoint() == AccessPoint::kGameStart) {
			return false;
		}

		std::unordered_set<std::string> CheckedDecks;
		for (int i = 0; i < this->ListAlailableDecks->CheckedItems->Count; ++i) {
			CheckedDecks.emplace(msclr::interop::marshal_as<std::string>(this->ListAlailableDecks->CheckedItems[i]->ToString())
			);
		}
		std::vector<std::string> FailedDecks = GameCoreClass::GetCore().CollectSelectedDecks(CheckedDecks);
		CheckedDecks.clear();

		///Inform if any invalid decks are selected
		if (FailedDecks.size() > 0) {
			this->LabelInfo->Show();
			this->LabelInfo->Text = "Selected decks are invalid (empty or with a wrong delimiter): ";

			for (const auto& FailedName : FailedDecks) {
				this->LabelInfo->Text += msclr::interop::marshal_as<System::String^>(FailedName)
					+ ", ";
			}
			this->LabelInfo->Text = this->LabelInfo->Text->Substring(0, this->LabelInfo->Text->Length - 2);

			return false;
		}
		return true;
	}

	///////////////////////////////////////////////////////////

	System::Void SettingsWin::ListLoadedDecks_SelectedItemChanged(
		System::Object^ sender,
		System::EventArgs^ e)
	{
		GameCoreClass::GetCore().SelectDeckCurrent(msclr::interop::marshal_as<std::string>(this->ListLoadedDecks->SelectedItem->ToString()));

		this->LabelDeckSize->Text = System::Convert::ToString(GameCoreClass::GetCore().GetCurrentDeckSize());
	}

	///////////////////////////////////////////////////////////////

	System::Void SettingsWin::ButtonAddDecks_Click(
		System::Object^ sender,
		System::EventArgs^ e)
	{
		if (AddSelectedDecks() == false) {
			return;
		}

		FormDeckLists(false);
		if (GameCoreClass::GetCore().GetLoadedDecksNum() != 0) {
			this->GroupCurrentDeck->Show();
			if (GameCoreClass::GetCore().GetAccessPoint() == AccessPoint::kGameStart) {
				this->ListLoadedDecks->SelectedIndex = 0;
			}

			this->ListLoadedDecks->Show();
			GameCoreClass::GetCore().SelectDeckCurrent(msclr::interop::marshal_as<std::string>(this->ListLoadedDecks->SelectedItem->ToString()));

			this->LabelDeckSize->Show();
			this->LabelDeckSize->Text = System::Convert::ToString(GameCoreClass::GetCore().GetCurrentDeckSize());
		}
		else {
			this->GroupCurrentDeck->Hide();
			this->ListLoadedDecks->Hide();
			this->LabelDeckSize->Hide();
		}

		this->LabelInfo->Hide();
	}
	/////////////////////////////////////////////////////////

	System::Void SettingsWin::ButtonToGame_Click(
		System::Object^ sender,
		System::EventArgs^ e)
	{
		switch (GameCoreClass::GetCore().GetAccessPoint()) {
		case AccessPoint::kGameStart:
			if (CanWePlay() == false) {
				return;
			}
			if (GameCoreClass::GetCore().GetLoadedDecksNum() == 0 && AddSelectedDecks() == false) {
				return;
			}
			GameCoreClass::GetCore().SetDeckCurrent();
			break;
		case AccessPoint::kFromGame:
			if (AddSelectedDecks() == false) return;
			break;
		default:
			break;
		}

		if (this->ListLoadedDecks->Items->Count != 0) {
			std::string SelectedDeckName = msclr::interop::marshal_as<std::string>(this->ListLoadedDecks->SelectedItem->ToString());
			GameCoreClass::GetCore().SelectDeckCurrent(SelectedDeckName);
		}

		//If everything is ok, start or resume game
		this->LabelInfo->Hide();
		this->Hide();
		this->TxtProfName->Text = "";
		this->GameplayWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		this->GameplayWin->Location = this->Location;
		this->GameplayWin->Size = this->Size;
		this->GameplayWin->Show();
		return;
	}

	////////////////////////////////////////////////////////////////

	bool SettingsWin::IsAlNum(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
	}
	void SettingsWin::FormDeckLists(bool IsNewGame) {
		std::unordered_set<std::string> AvailableDecks;
		std::unordered_set<std::string> AllLoadedDecks;
		GameCoreClass::GetCore().UpdateDeckLists(AvailableDecks, AllLoadedDecks, IsNewGame);

		this->ListAlailableDecks->Items->Clear();
		for (const auto& name : AvailableDecks) {
			this->ListAlailableDecks->Items->Add(msclr::interop::marshal_as<System::String^>(name));
		}
		this->ListLoadedDecks->Items->Clear();
		for (const auto& name : AllLoadedDecks) {
			this->ListLoadedDecks->Items->Add(msclr::interop::marshal_as<System::String^>(name));
		}
	}

	////////////////////////////////////////////////////////////////

	bool SettingsWin::CanWePlay() {
		std::string Name = msclr::interop::marshal_as<std::string>(
			this->TxtProfName->Text->ToString()
			);

		std::filesystem::path DirPath;
		if (GameCoreClass::GetCore().GetDirDecks(DirPath) == false) {
			this->LabelInfo->Text = "Can't access Deck directory!";
			this->LabelInfo->Show();
			return false;
		}

		if (Name.empty()) {
			this->LabelInfo->Text = "Name your profile first!";
			this->LabelInfo->Show();
			return false;
		}

		for (char ch : Name) {
			if (!IsAlNum(ch)) {
				this->LabelInfo->Text = "Profile name must have only Latin letters and numbers!";
				this->LabelInfo->Show();
				return false;
			}
		}

		// Iterate over the directory and check, if there already is file with this name
		for (const auto& entry : std::filesystem::directory_iterator(DirPath)) {
			if (entry.is_regular_file()
				&& entry.path().extension() == GameCoreClass::GetCore().kProfileFormat_
				&& entry.path().stem() == Name) {
				this->LabelInfo->Text = "A file with this name already exists!";
				this->LabelInfo->Show();
				return false;
			}
		}

		if (this->ListAlailableDecks->CheckedItems->Count == 0
			&& GameCoreClass::GetCore().GetLoadedDecksNum() == 0) {
			this->LabelInfo->Text = "Select at least one deck!";
			this->LabelInfo->Show();
			return false;
		}

		if (!GameCoreClass::GetCore().MakeNewProfile(Name)) {
			this->LabelInfo->Text = "Failed to create profile!";
			this->LabelInfo->Show();
			return false;
		}
		return true;
	}

	///////////////////////////////////////////////
}
#endif
