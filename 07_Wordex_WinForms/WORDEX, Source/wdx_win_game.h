#pragma once
#ifndef _WDX_GAMEWIN_H_
#define _WDX_GAMEWIN_H_
//Copyright (c) 2023 Alex Lykov. All rights reserved.

#include <msclr/marshal_cppstd.h>

#include "wdx_core.h"

#include "wdx_win_settings.h"

namespace WDX {
	public ref class GameWin : public System::Windows::Forms::Form {
	public:
		GameWin(void) {
			InitializeComponent();
		}
		GameWin(System::Windows::Forms::Form^ MainWinF, SettingsWin^ SetWinF) {
			InitializeComponent();
			MainWin = MainWinF;
			SettingWin = SetWinF;
		}

	protected:
		~GameWin() {
			if (components) {
				delete components;
			}
		}

	private: System::Windows::Forms::Button^ ButtonHint;
	private: System::Windows::Forms::Button^ ButtonGiveUp;
	private: System::Windows::Forms::Button^ ButtonToMain;
	private: System::Windows::Forms::TextBox^ TextGuess;
	private: System::Windows::Forms::TextBox^ TextHint;

	private: System::Windows::Forms::Label^ LabelGuess;

	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::Label^ LabelHello;
	private: System::Windows::Forms::Label^ LabelToGet;

	private: System::Windows::Forms::CheckBox^ CheckMean;
	private: System::Windows::Forms::Label^ LabelScore;

	private: System::Windows::Forms::Button^ ButtonTry;

		   System::Drawing::Point IniLoc_DomLists = System::Drawing::Point(440, 425);
		   System::Drawing::Point IniLoc_LabelScore = System::Drawing::Point(25, 360);
		   System::Drawing::Point IniLoc_LabelHello = System::Drawing::Point(175, 40);

		   System::Drawing::Size ButtonsTriSize = System::Drawing::Size(155, 90);

		   System::Windows::Forms::Button^ ButtonSettings;
		   System::Drawing::Size ButtonsDuoSize = System::Drawing::Size(210, 90);

		   ////////////////////////////////////////////////////////

		   System::Windows::Forms::Form^ MainWin;

		   SettingsWin^ SettingWin;

		   int HintNum{ 0 };

		   int PointsToGet{ 0 };

#pragma region Windows Form Designer generated code
		   void InitializeComponent(void) {
			   this->ButtonHint = (gcnew System::Windows::Forms::Button());
			   this->ButtonGiveUp = (gcnew System::Windows::Forms::Button());
			   this->ButtonToMain = (gcnew System::Windows::Forms::Button());
			   this->TextGuess = (gcnew System::Windows::Forms::TextBox());
			   this->TextHint = (gcnew System::Windows::Forms::TextBox());
			   this->LabelGuess = (gcnew System::Windows::Forms::Label());
			   this->LabelHello = (gcnew System::Windows::Forms::Label());
			   this->LabelToGet = (gcnew System::Windows::Forms::Label());
			   this->CheckMean = (gcnew System::Windows::Forms::CheckBox());
			   this->LabelScore = (gcnew System::Windows::Forms::Label());
			   this->ButtonTry = (gcnew System::Windows::Forms::Button());
			   this->ButtonSettings = (gcnew System::Windows::Forms::Button());
			   this->SuspendLayout();
			   //
			   // ButtonHint
			   //
			   this->ButtonHint->Font = (gcnew System::Drawing::Font(L"Clear Sans", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ButtonHint->Location = System::Drawing::Point(50, 510);
			   this->ButtonHint->Name = L"ButtonHint";
			   this->ButtonHint->Size = System::Drawing::Size(155, 90);
			   this->ButtonHint->TabIndex = 0;
			   this->ButtonHint->Text = L"Get a Hint";
			   this->ButtonHint->UseVisualStyleBackColor = true;
			   this->ButtonHint->Click += gcnew System::EventHandler(this, &GameWin::ButtonHint_Click);
			   //
			   // ButtonGiveUp
			   //
			   this->ButtonGiveUp->Font = (gcnew System::Drawing::Font(L"Clear Sans", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ButtonGiveUp->Location = System::Drawing::Point(265, 510);
			   this->ButtonGiveUp->Name = L"ButtonGiveUp";
			   this->ButtonGiveUp->Size = System::Drawing::Size(155, 90);
			   this->ButtonGiveUp->TabIndex = 1;
			   this->ButtonGiveUp->Text = L"GiveUp";
			   this->ButtonGiveUp->UseVisualStyleBackColor = true;
			   this->ButtonGiveUp->Click += gcnew System::EventHandler(this, &GameWin::ButtonGiveUp_Click);
			   //
			   // ButtonToMain
			   //
			   this->ButtonToMain->Font = (gcnew System::Drawing::Font(L"Clear Sans", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ButtonToMain->Location = System::Drawing::Point(475, 510);
			   this->ButtonToMain->Name = L"ButtonToMain";
			   this->ButtonToMain->Size = System::Drawing::Size(155, 90);
			   this->ButtonToMain->TabIndex = 2;
			   this->ButtonToMain->Text = L"To Main Menu";
			   this->ButtonToMain->UseVisualStyleBackColor = true;
			   this->ButtonToMain->Click += gcnew System::EventHandler(this, &GameWin::ButtonToMain_Click);
			   //
			   // TextGuess
			   //
			   this->TextGuess->Font = (gcnew System::Drawing::Font(L"Courier New", 26.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->TextGuess->Location = System::Drawing::Point(80, 259);
			   this->TextGuess->MaxLength = 1000;
			   this->TextGuess->Name = L"TextGuess";
			   this->TextGuess->Size = System::Drawing::Size(525, 47);
			   this->TextGuess->TabIndex = 3;
			   this->TextGuess->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->TextGuess->WordWrap = false;
			   this->TextGuess->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &GameWin::TextGuess_KeyUp);
			   //
			   // TextHint
			   //
			   this->TextHint->Font = (gcnew System::Drawing::Font(L"Courier New", 26.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->TextHint->Location = System::Drawing::Point(80, 155);
			   this->TextHint->MaxLength = 1000;
			   this->TextHint->Name = L"TextHint";
			   this->TextHint->ReadOnly = true;
			   this->TextHint->Size = System::Drawing::Size(525, 47);
			   this->TextHint->TabIndex = 4;
			   this->TextHint->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->TextHint->WordWrap = false;
			   //
			   // LabelGuess
			   //
			   this->LabelGuess->AutoSize = true;
			   this->LabelGuess->Font = (gcnew System::Drawing::Font(L"Clear Sans", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->LabelGuess->Location = System::Drawing::Point(74, 222);
			   this->LabelGuess->Name = L"LabelGuess";
			   this->LabelGuess->Size = System::Drawing::Size(83, 34);
			   this->LabelGuess->TabIndex = 6;
			   this->LabelGuess->Text = L"Guess";
			   //
			   // LabelHello
			   //
			   this->LabelHello->Dock = System::Windows::Forms::DockStyle::Top;
			   this->LabelHello->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->LabelHello->Location = System::Drawing::Point(0, 0);
			   this->LabelHello->Margin = System::Windows::Forms::Padding(0);
			   this->LabelHello->Name = L"LabelHello";
			   this->LabelHello->Size = System::Drawing::Size(684, 125);
			   this->LabelHello->TabIndex = 7;
			   this->LabelHello->Text = L"Here are secret words!\r\nCatch ’em all!";
			   this->LabelHello->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   //
			   // LabelToGet
			   //
			   this->LabelToGet->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->LabelToGet->Location = System::Drawing::Point(27, 424);
			   this->LabelToGet->Name = L"LabelToGet";
			   this->LabelToGet->Size = System::Drawing::Size(350, 35);
			   this->LabelToGet->TabIndex = 8;
			   this->LabelToGet->Text = L"We still can get 888 points!";
			   this->LabelToGet->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   //
			   // CheckMean
			   //
			   this->CheckMean->AutoSize = true;
			   this->CheckMean->CheckAlign = System::Drawing::ContentAlignment::MiddleRight;
			   this->CheckMean->Checked = true;
			   this->CheckMean->CheckState = System::Windows::Forms::CheckState::Checked;
			   this->CheckMean->Font = (gcnew System::Drawing::Font(L"Clear Sans", 18, System::Drawing::FontStyle::Bold));
			   this->CheckMean->Location = System::Drawing::Point(79, 115);
			   this->CheckMean->Name = L"CheckMean";
			   this->CheckMean->Size = System::Drawing::Size(78, 38);
			   this->CheckMean->TabIndex = 9;
			   this->CheckMean->Text = L"Pair";
			   this->CheckMean->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->CheckMean->UseVisualStyleBackColor = true;
			   this->CheckMean->CheckedChanged += gcnew System::EventHandler(this, &GameWin::CheckMean_CheckedChanged);
			   //
			   // LabelScore
			   //
			   this->LabelScore->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Left)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->LabelScore->Font = (gcnew System::Drawing::Font(L"Palatino Linotype", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->LabelScore->Location = System::Drawing::Point(25, 360);
			   this->LabelScore->Name = L"LabelScore";
			   this->LabelScore->Size = System::Drawing::Size(150, 35);
			   this->LabelScore->TabIndex = 10;
			   this->LabelScore->Text = L"Score: 999";
			   this->LabelScore->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   //
			   // ButtonTry
			   //
			   this->ButtonTry->Font = (gcnew System::Drawing::Font(L"Clear Sans", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ButtonTry->Location = System::Drawing::Point(297, 347);
			   this->ButtonTry->Name = L"ButtonTry";
			   this->ButtonTry->Size = System::Drawing::Size(98, 47);
			   this->ButtonTry->TabIndex = 12;
			   this->ButtonTry->Text = L"Try";
			   this->ButtonTry->UseVisualStyleBackColor = true;
			   this->ButtonTry->Click += gcnew System::EventHandler(this, &GameWin::ButtonTry_Click);
			   //
			   // ButtonSettings
			   //
			   this->ButtonSettings->AutoSize = true;
			   this->ButtonSettings->Font = (gcnew System::Drawing::Font(L"Clear Sans", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->ButtonSettings->Location = System::Drawing::Point(510, 352);
			   this->ButtonSettings->Name = L"ButtonSettings";
			   this->ButtonSettings->Size = System::Drawing::Size(95, 37);
			   this->ButtonSettings->TabIndex = 13;
			   this->ButtonSettings->Text = L"Settings";
			   this->ButtonSettings->UseVisualStyleBackColor = true;
			   this->ButtonSettings->Click += gcnew System::EventHandler(this, &GameWin::ButtonSettings_Click);
			   //
			   // GameWin
			   //
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->AutoSize = true;
			   this->BackColor = System::Drawing::Color::DarkOliveGreen;
			   this->ClientSize = System::Drawing::Size(684, 661);
			   this->Controls->Add(this->ButtonSettings);
			   this->Controls->Add(this->ButtonTry);
			   this->Controls->Add(this->LabelScore);
			   this->Controls->Add(this->CheckMean);
			   this->Controls->Add(this->LabelToGet);
			   this->Controls->Add(this->LabelHello);
			   this->Controls->Add(this->LabelGuess);
			   this->Controls->Add(this->TextHint);
			   this->Controls->Add(this->TextGuess);
			   this->Controls->Add(this->ButtonToMain);
			   this->Controls->Add(this->ButtonGiveUp);
			   this->Controls->Add(this->ButtonHint);
			   this->MaximumSize = System::Drawing::Size(700, 700);
			   this->MinimumSize = System::Drawing::Size(700, 700);
			   this->Name = L"GameWin";
			   this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;

#ifdef _WIN64
			   // Program Name specific to x64 architecture
			   this->Text = L"WORDEX 64";
#else
			   // Program Name specific to x86 architecture
			   this->Text = L"WORDEX 86";
#endif

			   this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GameWin::GameWin_FormClosed);
			   this->VisibleChanged += gcnew System::EventHandler(this, &GameWin::GameWin_VisibleChanged);
			   this->ResumeLayout(false);
			   this->PerformLayout();
		   }
#pragma endregion

		   /////////////////////////////////////////////////
		   /////////////////////////////////////////////////
		   /////////////////////////////////////////////////

		/////////////////////////////////////////////////
		/////////////////////////////////////////////////
		/////////////////////////////////////////////////

		/////////////////////////////////////////////////
		/////////////////////////////////////////////////
		/////////////////////////////////////////////////

	public: inline void SetSettingWin(SettingsWin^ SetWin) {
		if (!SettingWin) {
			SettingWin = SetWin;
		}
	}

		  /////////////////////////////////////////////////

	private:
		System::Void CheckMean_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		void InitialState();
		void VictoryState();
		void UpdateGameWin();

		/////////////////////////////////////////////////

	private: inline System::Void ButtonToMain_Click(System::Object^ sender, System::EventArgs^ e) {
		MainWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		MainWin->Location = this->Location;
		MainWin->Size = this->Size;
		this->Hide();
		MainWin->Show();
	}

	private: inline System::Void GameWin_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
		System::Windows::Forms::Application::Exit();
	}

	private: inline void UpdateHintText() {
		if (!this->CheckMean->Checked) {
			this->TextHint->Text = msclr::interop::marshal_as<System::String^>(
				GameCoreClass::GetCore().GetWordHinted(HintNum));
			PointsToGet = static_cast<int>(GameCoreClass::GetCore().GetWordSecond().length() - 1 - HintNum);
		}
		this->LabelToGet->Text =
			"We still can get "
			+ msclr::interop::marshal_as<System::String^>(std::to_string(PointsToGet))
			+ " points!";
	}

	private: inline System::Void ButtonGiveUp_Click(System::Object^ sender, System::EventArgs^ e) {
		UpdateGameWin();
	}
	private: inline System::Void ButtonHint_Click(System::Object^ sender, System::EventArgs^ e) {
		++HintNum;
		UpdateHintText();
		if (HintNum == GameCoreClass::GetCore().GetWordSecond().length() - 1) {
			this->ButtonHint->Hide();
			this->ButtonTry->Hide();
			this->LabelToGet->Hide();
		}
	}
	private: inline void CheckThisTry() {
		if (System::String::Equals(
			msclr::interop::marshal_as<System::String^>(GameCoreClass::GetCore().GetWordSecond()),
			this->TextGuess->Text))
		{
			GameCoreClass::GetCore().AddScorePoints(PointsToGet);
			GameCoreClass::GetCore().EraseWordPair();
			UpdateGameWin();
			if (GameCoreClass::GetCore().SaveProfile() == false) {
				this->LabelToGet->Text = "Can't save the profile!";
				return;
			}
		}
	}

	private: inline System::Void ButtonTry_Click(System::Object^ sender, System::EventArgs^ e) {
		CheckThisTry();
	}

	private: inline System::Void TextGuess_KeyUp(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
		if (e->KeyCode == System::Windows::Forms::Keys::Enter
			&& HintNum != GameCoreClass::GetCore().GetWordSecond().length() - 1) {
			CheckThisTry();
		}
	}

	private: inline System::Void ButtonSettings_Click(System::Object^ sender, System::EventArgs^ e) {
		SettingWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		SettingWin->Location = this->Location;
		SettingWin->Size = this->Size;
		GameCoreClass::GetCore().SetAccessPoint(AccessPoint::kFromGame);
		this->Hide();
		SettingWin->Show();
	}

	private: inline System::Void GameWin_VisibleChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->Visible == false) return;
		UpdateGameWin();
	}

		   /////////////////////////////////////////////////
	};

	/////////////////////////////////////////////////
	/////////////////////////////////////////////////
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	/////////////////////////////////////////////////
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	/////////////////////////////////////////////////
	/////////////////////////////////////////////////

	System::Void GameWin::CheckMean_CheckedChanged(
		System::Object^ sender, System::EventArgs^ e) {
		if (this->CheckMean->Checked) {
			this->CheckMean->Text = "Pair";
			this->ButtonHint->Hide();
			PointsToGet = static_cast<int>(GameCoreClass::GetCore().GetWordSecond().length() / 3);
			PointsToGet = (PointsToGet > 0) ? PointsToGet : 1;
			this->TextHint->Text = msclr::interop::marshal_as<System::String^>(GameCoreClass::GetCore().GetWordFirst());
			this->CheckMean->Enabled = false;
			UpdateHintText();

			this->ButtonGiveUp->Size = ButtonsDuoSize;
			this->ButtonToMain->Size = ButtonsDuoSize;

			this->ButtonGiveUp->Location = System::Drawing::Point(
				(this->Size.Width / 5) - (ButtonHint->Size.Width / 2),
				this->ButtonHint->Location.Y);
			this->ButtonToMain->Location = System::Drawing::Point(
				this->Size.Width - (this->ButtonToMain->Size.Width + (this->Size.Width / 11)),
				this->ButtonToMain->Location.Y);
		}
		else {
			this->CheckMean->Text = "Hint";
		}
	}

	void GameWin::InitialState() {
		this->CheckMean->Checked = false;
		this->LabelHello->AutoSize = false;
		this->ButtonToMain->AutoSize = false;
		this->CheckMean->Enabled = true;
		this->CheckMean->Show();
		this->ButtonHint->Show();
		this->LabelToGet->Show();
		this->LabelScore->Show();
		this->TextGuess->Show();
		this->ButtonTry->Show();
		this->ButtonGiveUp->Show();
		this->TextHint->Show();
		this->LabelGuess->Show();
		this->TextGuess->Text = "";
		this->LabelScore->Text = "Score: " + msclr::interop::marshal_as<System::String^>(std::to_string(GameCoreClass::GetCore().GetScore()));

		this->LabelScore->Location = IniLoc_LabelScore;

		this->ButtonGiveUp->Size = ButtonsTriSize;
		this->ButtonToMain->Size = ButtonsTriSize;
		this->ButtonToMain->Size = ButtonsTriSize;

		this->ButtonGiveUp->Location = System::Drawing::Point(
			((this->Size.Width - ButtonGiveUp->Size.Width) / 2),
			this->ButtonGiveUp->Location.Y);
		this->ButtonHint->Location = System::Drawing::Point(
			(this->Size.Width / 5) - (ButtonHint->Size.Width / 2),
			this->ButtonHint->Location.Y);
		this->ButtonToMain->Location = System::Drawing::Point(
			this->Size.Width - (this->ButtonToMain->Size.Width + (this->Size.Width / 11)),
			this->ButtonToMain->Location.Y);
	}

	void GameWin::VictoryState() {
		this->LabelHello->Text = "Wow!\r\nYou've got them all!!!\r\nTry another deck or make a new profile...";
		this->ButtonToMain->AutoSize = true;
		this->LabelScore->Show();
		this->CheckMean->Hide();
		this->ButtonHint->Hide();
		this->ButtonTry->Hide();
		this->ButtonGiveUp->Hide();
		this->LabelToGet->Hide();
		this->TextGuess->Hide();
		this->TextHint->Hide();
		this->LabelGuess->Hide();
		this->LabelScore->Location = System::Drawing::Point(
			(this->Size.Width / 2) - (LabelScore->Size.Width / 2),
			this->LabelScore->Location.Y);
		this->ButtonToMain->Location = System::Drawing::Point(
			(this->Size.Width / 2) - (ButtonToMain->Size.Width / 2),
			this->ButtonToMain->Location.Y);
	}

	void GameWin::UpdateGameWin() {
		InitialState();
		HintNum = 0;

		size_t CurrentDeckSize{ GameCoreClass::GetCore().GetCurrentDeckSize() };

		if (CurrentDeckSize == 0) {
			VictoryState();
			return;
		}
		GameCoreClass::GetCore().UpdateWordPair();
		if (!GameCoreClass::GetCore().CurrentProfile_.LettersOnly) {
			this->CheckMean->Checked = true;
		}
		UpdateHintText();
		this->LabelHello->Text =
			msclr::interop::marshal_as<System::String^>(
				GameCoreClass::GetCore().GetCurrentDeckName())
			+ " ( "
			+ msclr::interop::marshal_as<System::String^>(std::to_string(CurrentDeckSize))
			+ " )";
	}

	/////////////////////////////////////////////////
	/////////////////////////////////////////////////
	/////////////////////////////////////////////////
}
#endif
