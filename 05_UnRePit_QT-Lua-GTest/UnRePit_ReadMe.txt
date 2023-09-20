UN-RE-PIT: README

UnRePit boasts three distinct functionalities:

Text Analysis: This component of the program scrutinizes text content to identify instances of repeated words or word fragments. It compiles a comprehensive list detailing the frequency of each identified word or fragment within the text.

Text Reformation: Designed with a pragmatic purpose in mind, this feature discerns sentences within the provided text and subsequently organizes the text into paragraphs. This addresses a common predicament wherein text extracted from PDF files lacks proper paragraph divisions, often being presented as single lines. This impairs the efficacy of translation tools like Google Translate. UnRePit mitigates this issue by automating the conversion of such text into properly formatted paragraphs.

Lua Code Interpretation: UnRePit functions as an interpreter for Lua scripts, decoupled from any specific console environment. In cases where errors exist within the Lua code, the program adeptly identifies and reports them.

Supplementary attributes of UnRePit encompass:

Configuration via UnRePit_Settings.lua: The program's settings are conveniently stored within the UnRePit_Settings.lua file. Here, users can customize parameters such as the minimum word size for inclusion in frequency counts, preferred language for the interface (currently supporting English and Russian), and the option to hide the settings panel if deemed unnecessary.

Flexible Text Reformation Algorithms: The default functionality employs C++ scripts for text refinement, but users have the liberty to incorporate external Lua code from the UnRePit_LuaReformer.lua file. This modularity empowers users to experiment with text refinement methodologies without risking the program's integrity.

Comprehensive Testing: The program's source code is accompanied by an array of unit tests built on the foundation of Google Tests from Visual Studio. This robust testing suite ensures the program's reliability and functionality.

In summation, UnRePit presents a trio of potent features catering to text analysis, refinement, and Lua code interpretation.

=====================================================================================
=====================================================================================
=====================================================================================

UnRePit ����� ��� �������:
1. ������ ������. ��������� ���� ������������� ����� (��� ����� ����) � ������ � ����������� �� � ������ � �������� ������� ������� ����� � ������.
2. �������������� ������. ��������� �������� � ������ ����������� � ��������� ��������� ����� �� ������ �� ������ ����������� � ������. 
������� ������������� ������ ������� ������ ����������. � ���� ������ ����� ����� ���������� ���������� ����� �� PDF ������ � ���������� ��� �� �������. ����� �����, ���� �����, ������ �� �� ������, ��� ���������, � �� ������. � ���������� Google Translate �� ����� ��������� ��������� �����. ���������� ����� ������� ������� �����, ��������, ���� �� ���� ��������� ���������� ����� ����� ��������. UnRePit �������������� ���� �������.
3. ������������� Lua-����. ��������� �������� ��� ����������� �� ������� ������������� ��� Lua-��������. ���� � ���� ����� ������, ��������� �� ���� �������. 

�������������� ����:
1. � ����� UnRePit_Settings.lua ���������� ��������� ���������. ��� ����� ��������� ����������� ������ � ������� ��������� � ������ ����, ������ ���� ���������� (���� ������ ���������� � �������). ����� ����� ������ ������ ��������, ���� ��� ��� �� �����.
2. �������, �� ������ ����������, ����� ��������� ����� ��������������� �����. �� ��������� �������� ���������� �++ �������, �� ����� ���������� ������� Lua ��� �� ����� UnRePit_LuaReformer.lua. ��� ����� ������, �� �������� ���-�� ��������� � ���������.
3. � ��������� � �������� ����� ��������� ���� ����� ����-������ �� ������ Google Tests �� Visual Studio.


