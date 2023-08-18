#include "MasterWin.h"

[System::STAThread]

int main(array<System::String^>^ args) {
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);
	UNREPEATER::MasterWin MainWin;
	System::Windows::Forms::Application::Run(% MainWin);

	return 0;
}