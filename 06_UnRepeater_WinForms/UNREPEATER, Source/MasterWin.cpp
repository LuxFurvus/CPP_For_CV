#include "MasterWin.h"
#include "Breaker.h"

using namespace System;
using namespace System::Windows::Forms;
[STAThread]

int main(array<System::String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	UNREPEATER::MasterWin MainWin;
	Application::Run(% MainWin);

	return 0;
}