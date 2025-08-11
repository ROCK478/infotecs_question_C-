#include "Part1.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Ошибка. Используйте: " << argv[0] << " <имя файла> <уровень по умолчанию> [1-3|low|middle|high]\n";
        return 1;
    }

    string filename = argv[1];
    string levelStr = argv[2];
    LogLevel defaultLevel;

    if (levelStr == "1" || levelStr == "low") defaultLevel = LogLevel::Low;
    else if (levelStr == "2" || levelStr == "middle") defaultLevel = LogLevel::Middle;
    else if (levelStr == "3" || levelStr == "high") defaultLevel = LogLevel::High;
    else {
        cout << "Такого уровня не существует. Варианты уровня: 1/low, 2/middle, 3/high\n";
        return 1;
    }

    Logger logger(filename, defaultLevel);

    cout << "Приложение запущено. Уровень по умолчанию: " << levelStr << "\n";
    cout << "Добавление сообщения в формате: [level] message\n";
    cout << "Например: 'high Очень важное сообщение' или 'Это сообщение использует уровень по умолчанию'\n";
    cout << "Используйте 'cl <новый уровень>' для изменения уровня логирования\n";
    cout << "Используйте 'exit' для завершения программы\n";

    string input;
    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "exit") break;

        if (input.rfind("cl ", 0) == 0) {
            string newLevelStr = input.substr(3);
            if (logger.setLogLevel(newLevelStr)) {
                cout << "Уровень логирования изменен на: " << newLevelStr << "\n";
            }
            else {
                cout << "Неверный уровень логирования. Сохранен предыдущий уровень.\n";
                cout << "Допустимые значения: 1/low, 2/middle, 3/high\n";
            }
            continue;
        }

        size_t spacePos = input.find(' ');
        if (spacePos != string::npos) {
            string levelPart = input.substr(0, spacePos);
            string message = input.substr(spacePos + 1);

            try {
                int levelNum = stoi(levelPart);
                logger.Log(message, levelNum);
                continue;
            }
            catch (...) {
                logger.Log(message, levelPart);
                continue;
            }
        }

        logger.Log(input, defaultLevel);
    }

    return 0;
}