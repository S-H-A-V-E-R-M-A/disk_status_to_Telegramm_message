# disk_status_to_Telegramm_message
Скрипт для рассылки в телеграм канал информации о состоянии дисков - сообщение приходит только если диск занят более чем на 80%. 
При сборке в .exe нужно чтобы в папке с ним оказался curl.exe. 
Я качал с оф сайта curl-8.14.1_1-win64-mingw и оттуда прсто копировал в папку со своим exe этот curl. 
Так же будет нужна кучка dll, их определил при помощи Qt Creator и из его же файлов их скопировал.
В exe компилировал при помощи Qt.
