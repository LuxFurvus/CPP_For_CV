#pragma once
#ifndef _WDX_MAINMENUWIN_H_
#define _WDX_MAINMENUWIN_H_
//Copyright (c) 2023 Alex Lykov. All rights reserved.

#include "wdx_core.h"

#include "wdx_win_loadprofile.h"
#include "wdx_win_game.h"
#include "wdx_win_settings.h"

namespace WDX {
	public ref class MainMenuWin : public System::Windows::Forms::Form {
	public:

		MainMenuWin(void) {
			InitializeComponent();
			this->GameplayWin = gcnew GameWin(this, nullptr);
			this->SettingWin = gcnew SettingsWin(this, GameplayWin);
			this->LoadGameWin = gcnew LoadProfileWin(this, GameplayWin);
			GameplayWin->SetSettingWin(SettingWin);
		}

	protected:
		~MainMenuWin() {
			if (components) {
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ ButtonNewGame;
	private: System::Windows::Forms::Button^ ButtonLoadGame;
	protected:

	private: System::Windows::Forms::Button^ ButtonQuit;
	private: System::Windows::Forms::Label^ LOGO;

	private:
		System::ComponentModel::Container^ components;
		System::Windows::Forms::Label^ label1;
		GameWin^ GameplayWin;
		SettingsWin^ SettingWin;
		LoadProfileWin^ LoadGameWin;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void) {
			this->ButtonNewGame = (gcnew System::Windows::Forms::Button());
			this->ButtonLoadGame = (gcnew System::Windows::Forms::Button());
			this->ButtonQuit = (gcnew System::Windows::Forms::Button());
			this->LOGO = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			//
			// ButtonNewGame
			//
			this->ButtonNewGame->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ButtonNewGame->Font = (gcnew System::Drawing::Font(L"Century", 26.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ButtonNewGame->Location = System::Drawing::Point(217, 240);
			this->ButtonNewGame->Name = L"ButtonNewGame";
			this->ButtonNewGame->Size = System::Drawing::Size(250, 90);
			this->ButtonNewGame->TabIndex = 0;
			this->ButtonNewGame->Text = L"New Game";
			this->ButtonNewGame->UseVisualStyleBackColor = true;
			this->ButtonNewGame->Click += gcnew System::EventHandler(this, &MainMenuWin::ButtonNewGame_Click);
			//
			// ButtonLoadGame
			//
			this->ButtonLoadGame->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ButtonLoadGame->Font = (gcnew System::Drawing::Font(L"Century", 26.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ButtonLoadGame->Location = System::Drawing::Point(217, 374);
			this->ButtonLoadGame->Name = L"ButtonLoadGame";
			this->ButtonLoadGame->Size = System::Drawing::Size(250, 90);
			this->ButtonLoadGame->TabIndex = 1;
			this->ButtonLoadGame->Text = L"Load Profile";
			this->ButtonLoadGame->UseVisualStyleBackColor = true;
			this->ButtonLoadGame->Click += gcnew System::EventHandler(this, &MainMenuWin::ButtonLoadGame_Click);
			//
			// ButtonQuit
			//
			this->ButtonQuit->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ButtonQuit->Font = (gcnew System::Drawing::Font(L"Century", 26.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ButtonQuit->Location = System::Drawing::Point(217, 504);
			this->ButtonQuit->Name = L"ButtonQuit";
			this->ButtonQuit->Size = System::Drawing::Size(250, 90);
			this->ButtonQuit->TabIndex = 2;
			this->ButtonQuit->Text = L"Quit";
			this->ButtonQuit->UseVisualStyleBackColor = true;
			this->ButtonQuit->Click += gcnew System::EventHandler(this, &MainMenuWin::ButtonQuit_Click);
			//
			// LOGO
			//
			this->LOGO->BackColor = System::Drawing::Color::Transparent;
			this->LOGO->Dock = System::Windows::Forms::DockStyle::Top;
			this->LOGO->Font = (gcnew System::Drawing::Font(L"Harrington", 72, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->LOGO->Location = System::Drawing::Point(0, 0);
			this->LOGO->Name = L"LOGO";
			this->LOGO->Size = System::Drawing::Size(684, 154);
			this->LOGO->TabIndex = 3;
			this->LOGO->Text = L"Wordex";
			this->LOGO->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			//
			// label1
			//
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Dock = System::Windows::Forms::DockStyle::Top;
			this->label1->Font = (gcnew System::Drawing::Font(L"Harrington", 30, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic))));
			this->label1->Location = System::Drawing::Point(0, 154);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(684, 52);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Guess a word game";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			//
			// MainMenuWin
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::IndianRed;
			this->ClientSize = System::Drawing::Size(684, 661);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->LOGO);
			this->Controls->Add(this->ButtonQuit);
			this->Controls->Add(this->ButtonLoadGame);
			this->Controls->Add(this->ButtonNewGame);
			this->MaximumSize = System::Drawing::Size(700, 700);
			this->MinimumSize = System::Drawing::Size(700, 700);
			this->Name = L"MainMenuWin";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"WORDEX";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MainMenuWin::MainMenuWin_FormClosed);
			this->VisibleChanged += gcnew System::EventHandler(this, &MainMenuWin::MainMenuWin_VisibleChanged);
			this->ResumeLayout(false);
		}
#pragma endregion

	private:
		inline System::Void ButtonQuit_Click(System::Object^ sender, System::EventArgs^ e) {
			System::Windows::Forms::Application::Exit();
		}

	private:
		inline System::Void MainMenuWin_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
			System::Windows::Forms::Application::Exit();
		}
		//////////////////////////////////////////////

	private:
		inline System::Void ButtonNewGame_Click(System::Object^ sender, System::EventArgs^ e) {
			SettingWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			SettingWin->Location = this->Location;
			SettingWin->Size = this->Size;
			GameCoreClass::GetCore().SetAccessPoint(AccessPoint::kGameStart);
			this->Hide();
			SettingWin->Show();
		}

	private:
		inline System::Void ButtonLoadGame_Click(System::Object^ sender, System::EventArgs^ e) {
			LoadGameWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			LoadGameWin->Location = this->Location;
			LoadGameWin->Size = this->Size;
			this->Hide();
			LoadGameWin->Show();
		}

		////////////////////////////////////

	private:
		inline System::Void MainMenuWin_VisibleChanged(System::Object^ sender, System::EventArgs^ e) {
			if (this->Visible == true) {
				GameCoreClass::GetCore().ResetGameCore();
			}
		}
	};
}
#endif
