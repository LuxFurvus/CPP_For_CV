#pragma once
#ifndef _WDX_LOADPROFILEWIN_H_
#define _WDX_LOADPROFILEWIN_H_
//Copyright (c) 2023 Alex Lykov. All rights reserved.

#include <string>
#include <vector>

#include "wdx_core.h"

#include <msclr/marshal_cppstd.h>

#include "wdx_win_game.h"

namespace WDX {
	public ref class LoadProfileWin : public System::Windows::Forms::Form {
	public:
		LoadProfileWin(void) {
			InitializeComponent();
		}
		LoadProfileWin(System::Windows::Forms::Form^ MainWinF, GameWin^ GameWinF) {
			InitializeComponent();
			this->MainWin = MainWinF;
			this->GameplayWin = GameWinF;
		}

	protected:
		~LoadProfileWin() {
			if (components) {
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ LabelHello;
	private: System::Windows::Forms::Button^ ButtonBack;
	private: System::Windows::Forms::Button^ ButtonLoad;
	private: System::Windows::Forms::CheckedListBox^ ProfileList;

	private:
		System::ComponentModel::Container^ components;
		System::Windows::Forms::Form^ MainWin;
		System::Windows::Forms::Label^ LabelInfo;
		GameWin^ GameplayWin;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void) {
			this->LabelHello = (gcnew System::Windows::Forms::Label());
			this->ButtonBack = (gcnew System::Windows::Forms::Button());
			this->ButtonLoad = (gcnew System::Windows::Forms::Button());
			this->ProfileList = (gcnew System::Windows::Forms::CheckedListBox());
			this->LabelInfo = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			//
			// LabelHello
			//
			this->LabelHello->AutoSize = true;
			this->LabelHello->BackColor = System::Drawing::Color::Transparent;
			this->LabelHello->Font = (gcnew System::Drawing::Font(L"Clear Sans", 48, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->LabelHello->Location = System::Drawing::Point(41, 47);
			this->LabelHello->Name = L"LabelHello";
			this->LabelHello->Size = System::Drawing::Size(594, 89);
			this->LabelHello->TabIndex = 0;
			this->LabelHello->Text = L"Select your profile:";
			//
			// ButtonBack
			//
			this->ButtonBack->Font = (gcnew System::Drawing::Font(L"Century", 26.25F, System::Drawing::FontStyle::Bold));
			this->ButtonBack->Location = System::Drawing::Point(32, 509);
			this->ButtonBack->Name = L"ButtonBack";
			this->ButtonBack->Size = System::Drawing::Size(285, 75);
			this->ButtonBack->TabIndex = 4;
			this->ButtonBack->Text = L"To Main Menu";
			this->ButtonBack->UseVisualStyleBackColor = true;
			this->ButtonBack->Click += gcnew System::EventHandler(this, &LoadProfileWin::ButtonBack_Click);
			//
			// ButtonLoad
			//
			this->ButtonLoad->Font = (gcnew System::Drawing::Font(L"Century", 26.25F, System::Drawing::FontStyle::Bold));
			this->ButtonLoad->Location = System::Drawing::Point(361, 509);
			this->ButtonLoad->Name = L"ButtonLoad";
			this->ButtonLoad->Size = System::Drawing::Size(285, 75);
			this->ButtonLoad->TabIndex = 5;
			this->ButtonLoad->Text = L"Load Game";
			this->ButtonLoad->UseVisualStyleBackColor = true;
			this->ButtonLoad->Click += gcnew System::EventHandler(this, &LoadProfileWin::ButtonLoad_Click);
			//
			// ProfileList
			//
			this->ProfileList->BackColor = System::Drawing::Color::PeachPuff;
			this->ProfileList->Font = (gcnew System::Drawing::Font(L"Clear Sans", 21.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ProfileList->FormattingEnabled = true;
			this->ProfileList->Location = System::Drawing::Point(144, 157);
			this->ProfileList->Margin = System::Windows::Forms::Padding(0);
			this->ProfileList->Name = L"ProfileList";
			this->ProfileList->ScrollAlwaysVisible = true;
			this->ProfileList->Size = System::Drawing::Size(385, 256);
			this->ProfileList->Sorted = true;
			this->ProfileList->TabIndex = 6;
			//
			// LabelInfo
			//
			this->LabelInfo->AutoSize = true;
			this->LabelInfo->BackColor = System::Drawing::Color::Transparent;
			this->LabelInfo->Font = (gcnew System::Drawing::Font(L"Clear Sans", 20.25F, System::Drawing::FontStyle::Bold));
			this->LabelInfo->ForeColor = System::Drawing::Color::DarkRed;
			this->LabelInfo->Location = System::Drawing::Point(174, 441);
			this->LabelInfo->Name = L"LabelInfo";
			this->LabelInfo->Size = System::Drawing::Size(320, 38);
			this->LabelInfo->TabIndex = 7;
			this->LabelInfo->Text = L"Select some profile first!";
			this->LabelInfo->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			//
			// LoadProfileWin
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::PaleVioletRed;
			this->ClientSize = System::Drawing::Size(684, 661);
			this->Controls->Add(this->LabelInfo);
			this->Controls->Add(this->ProfileList);
			this->Controls->Add(this->ButtonLoad);
			this->Controls->Add(this->ButtonBack);
			this->Controls->Add(this->LabelHello);
			this->Name = L"LoadProfileWin";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"WORDEX";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &LoadProfileWin::LoadProfileWin_FormClosed);
			this->VisibleChanged += gcnew System::EventHandler(this, &LoadProfileWin::LoadProfileWin_VisibleChanged);
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

		//////////////////////////////////////////////////////////

	private: inline System::Void ButtonBack_Click(System::Object^ sender, System::EventArgs^ e) {
		this->Hide();
		MainWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		MainWin->Location = this->Location;
		MainWin->Size = this->Size;
		MainWin->Show();
	}

		   //////////////////////////////////////////////////////////

	private: inline System::Void LoadProfileWin_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
		System::Windows::Forms::Application::Exit();
	}

		   //////////////////////////////////////////////////////////

	private:
		System::Void ButtonLoad_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void LoadProfileWin_VisibleChanged(System::Object^ sender, System::EventArgs^ e);

		//////////////////////////////////////////////////////////
	};
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////

	System::Void LoadProfileWin::LoadProfileWin_VisibleChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->Visible == false) return;

		this->LabelInfo->Hide();
		ProfileList->Items->Clear();

		std::vector<std::string> ProfileNames;

		if (GameCoreClass::GetCore().FormProlileList(ProfileNames) == false) {
			this->LabelInfo->Text = "Can't access Profile directory!";
			this->LabelInfo->Location = System::Drawing::Point(
				(this->Size.Width / 2) - (LabelInfo->Size.Width / 2),
				this->LabelInfo->Location.Y);
			this->LabelInfo->Show();
			return;
		}

		if (ProfileNames.size() == 0) return;

		for (std::string binfile : ProfileNames) {
			binfile.erase(binfile.size() - 6);
			ProfileList->Items->Add(gcnew System::String(binfile.c_str()));
		}
	}

	//////////////////////////////////////////////////////////

	System::Void LoadProfileWin::ButtonLoad_Click(System::Object^ sender, System::EventArgs^ e) {
		//If there are no profiles in the respective directory
		if (ProfileList->CheckedIndices->Count == 0) {
			this->LabelInfo->Text = "There are no profiles to load!";
			goto LoadFailed;
		}

		//If we can't open the file
		if (GameCoreClass::GetCore().LoadProfile(msclr::interop::marshal_as<std::string>(ProfileList->SelectedItem->ToString())) == false) {
			this->LabelInfo->Text = "Can't load this profile!";
			goto LoadFailed;
		}

		//If file loaded successfully, open Game Window
		this->LabelInfo->Hide();
		GameplayWin->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		GameplayWin->Location = this->Location;
		GameplayWin->Size = this->Size;
		this->Hide();
		GameplayWin->Show();
		return;

		//If file loading failed, inform the player
	LoadFailed:
		this->LabelInfo->Location = System::Drawing::Point(
			(this->Size.Width / 2) - (LabelInfo->Size.Width / 2),
			this->LabelInfo->Location.Y);
		this->LabelInfo->Show();
		return;
	}

	//////////////////////////////////////////////////////////
}
#endif
