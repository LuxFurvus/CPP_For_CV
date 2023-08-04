//Copyright (c) 2023 Alex Lykov. All rights reserved.

#include "wdx_core.h"
#include "wdx_win_mainmenu.h"

[System::STAThread]

int main(array<System::String^>^ args) {
	System::Windows::Forms::Application::EnableVisualStyles();

	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

	WDX::MainMenuWin MainWin;

	System::Windows::Forms::Application::Run(% MainWin);

	return 0;
}