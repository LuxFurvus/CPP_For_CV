#CppQuiz Parser

**Eng**

The program parses any website whose URL addresses have numerical indexing. As a result, the program compiles a list of URL addresses in which the HTML code does not contain the stop phrase.

The program settings (URL address template, stop phrase, search range, output file) are specified in the settings file (settings.txt). The program is launched with one argument - the path to the settings file. For example:

./site_parser settings.txt

As a demonstration, the program is set by default to search for valid addresses on the cppquiz.org website.

**Rus**

Программа предназначена для парсинга любого сайта, URL адреса которого имеют нумерную индексацию.
В итоге программа составляет список URL адресов, в HTML-коде которых отсутствует стоп-фраза.

Настройки программы (шаблон URL адреса, стоп-фраза, диапазон поиска, файл вывода) прописаны в файле настроек (settings.txt).
Программа запускается с одним аргументом - путем к файлу настроек. Например: 
./site_parser settings.txt

В качестве демонстрации по умолчанию программа настроена для поиска корректных адресов на сайте cppquiz.org

