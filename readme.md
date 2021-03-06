Простой менеджер буфера обмена.
Функции:
- Слежение за буфером обмена
- Сохранение содержимого буфера обмена в списке
- Удаление итемов из списка
- режим с отключением слежки
- Режим "Всегда на верху"
- Просмотр содержимого фрагментов.
- Прячется в трее.
- Закрывается кликом средней клавишей мыши в трее.
- Настройка автозагрузки при старте windows.
- Настройка считывания истории при старте
- Добавил минимальную длину сохраняемого текста.

![screenshot](https://github.com/trdm/Clint/blob/master/resources/scrinshot.png)


---------------------------------------
Предок: https://github.com/sje397/Clint
Clint - shared multiple-entry clipboard
---------------------------------------
Usage:

  Clint [-p <portnum>] [-n <node address1> [-n <node address2> ...] ]

The Clint application creates a taskbar icon. Clicking the icon opens a window with a list of previous clipboard
entries, and selecting an item will set it as the current clipboard contents and move it to the top of the list.

The applications also listens on the given port, and sends messages to the node addresses provided whenever the
clipboard contents changes. This keeps the clipboard synchronised across the different nodes. Note that all
connected nodes must use the same port. The messages are sent over UDP.

Clint is written using the Qt C++ toolkit, and works with Windows, Linux, and OSX.

