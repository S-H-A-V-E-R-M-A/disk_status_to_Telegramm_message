#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <iomanip>

using namespace std;

struct DiskReport {           
    std::string text;         // сформированный текст для Telegram
    bool need_send;           // true, если есть диск ≥ threshold %
};

string EscapeForCmd(const string& input);

DiskReport GetDiskInfo(double threshold = 80.0) {          
    stringstream ss;
    bool send = false;                                     

    // имя компьютера
    char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computer_name);
    if (GetComputerNameA(computer_name, &size)) {
        ss << "Компьютер: " << computer_name << "%0A%0A";
    }
    else {
        ss << "Компьютер: [не удалось определить]%0A%0A";
    }

    DWORD drives = GetLogicalDrives();
    for (char letter = 'A'; drives; ++letter, drives >>= 1) {
        if (!(drives & 1)) {
            continue;
        }

        char root_path[] = { letter, ':', '\\', '\0' };
        ULARGE_INTEGER free_bytes{}, total_bytes{};
        if (!GetDiskFreeSpaceExA(root_path, &free_bytes, &total_bytes, nullptr)) {
            continue;
        }

        double used_percent = 100.0 - (
            static_cast<double>(free_bytes.QuadPart) /
            static_cast<double>(total_bytes.QuadPart) * 100.0
            );

        if (used_percent >= threshold) {                   
            send = true;                                   
        }

        ss << "Диск " << root_path << "%0A"
            << "Всего: " << std::setw(6) << (total_bytes.QuadPart >> 30) << " GB%0A"
            << "Свободно: " << (free_bytes.QuadPart >> 30) << " GB%0A"
            << "Занято: " << std::fixed << std::setprecision(1)
            << used_percent << "%%0A";
    }

    return { ss.str(), send };                             
}

//отпарвка сообщения в телеграм
void SendToTelegram(const string& message, const string& bot_token, const string& chat_id, const string& message_id) {
    // Формируем команду для curl
    string command = "curl.exe -s -X POST "
        "\"https://api.telegram.org/bot" + bot_token + "/sendMessage\" "
        "-d chat_id=" + chat_id + " "
        "-d message_thread_id=" + message_id + " "
        "-d text=\"" + message + "\"";

    // Запускаем команду через системную консоль
    int result = system(command.c_str());

    if (result != 0) {
        cerr << "Ошибка выполнения curl! Код: " << result << endl;
        cerr << "Убедитесь, что curl.exe доступен в PATH" << endl;
    }
}
// Экранирование специальных символов CMD
string EscapeForCmd(const string& input) {
    string output;
    for (char c : input) {
        if (c == '"') {
            output += "\\\"";
        }
        else if (c == '%' || c == '!' || c == '^') {
            output += ' ';
            output += c;
        }
        else {
            output += c;
        }
    }
    return output;
}

int main() {
    // Конфигурация бота
    const string BOT_TOKEN = "тут пишем токен бота"; //токен бота
    const string CHAT_ID = "тут пишем чат id"; //чат айди
    const string MESSAGE_ID = "тут пишем мессадж id"; //мессадж айди

    // Получаем информацию о дисках
    DiskReport report = GetDiskInfo();                  

    if (report.need_send) {                             
        string escaped = EscapeForCmd(report.text);
        SendToTelegram(escaped, BOT_TOKEN, CHAT_ID, MESSAGE_ID);
    }
    else {                                            
        cout << "all disks are less than 80% occupied.\n";
        /*cin.get();*/
    }                                                   

    return 0;
}