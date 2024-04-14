#pragma once

#include <Arduino.h>

#define LOG_LENGTH 25


class Logger {
    public:

        bool update = true;
        String consoleLog[LOG_LENGTH];

        void consolePrint(String str) {
            update = true;
            str.replace("\n", "$n");
            consoleLog[LOG_LENGTH-1] += str;
        }

        void consolePrintLn(String str) {
            consolePrint(str);

            for(int i = 1; i < LOG_LENGTH; i++) {
                consoleLog[i - 1] = consoleLog[i];
            }
            consoleLog[LOG_LENGTH-1] = "";
        }
    private:
};