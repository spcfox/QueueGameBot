/*
Номера событий
0 - открытие кабинета
1 - закрытие кабинета
2 - встать в очередь
3 - выйти из кабинета
4 - войти в кобинет
5 - покинуть очередь
*/

#include <map>

using namespace std;

class Office {
private:
    int openingTime;
    int closingTime;
    int procedureTime;

    list<int> queue;
    int currentPlayer = -1;
    int timeStartProcedure = -1;
    int timeEndProcedure = 90000;

    bool opened = false; // Открывался ли кабинет
    bool closed = false; // Закрывался ли кабинет

public:
    Office(int oT = 0, int cT = 0, int pT = 0) {
        openingTime = oT * 60;
        closingTime = cT * 60;
        procedureTime = pT * 60;
    }

    // Возвращает игрока, который в кабинете
    int getCurrentPlayer() {
        return currentPlayer;
    }

    // Пустая ли очередь
    bool queueIsEmpty() {
        return queue.size() == 0;
    }

    // Есть ли кто-то в кабинете
    bool isProcedure() {
        return currentPlayer != -1;
    }

    // Пора ли выходить из кабинета
    bool procedureIsOver(int gameTime) {
        return isProcedure() && gameTime >= timeEndProcedure;
    }

    // Наступило ли время закрытия
    bool timeIsOver(int gameTime) {
        return gameTime >= closingTime;
    }

    // Закончилась ли игра в этом кабинете
    bool officeIsOver(int gameTime) {
        return !isProcedure() && timeIsOver(gameTime); // Если в кабинете никого нет и время вышло
    }

    bool isOpened() {
        return opened;
    }

    bool shouldOpen(int gameTime) {
        return !isOpened() && gameTime >= openingTime;
    }

    int getClosingTime() {
        return closingTime;
    }

    list<int> clearQueue() {
        list<int> queueCopy = queue;
        queue.clear();
        return queueCopy;
    }

    map<string, int> open() {
        opened = true;

        map<string, int> event;
        event["type"] = 0; // Открыть кабинет
        event["time"] = openingTime;

        return event;
    }

    bool isClosed() {
        return closed;
    }

    bool shouldClose(int gameTime) {
        return !isClosed() && gameTime >= closingTime;
    }

    map<string, int> close() {
        closed = true;

        map<string, int> event;
        event["type"] = 1; // Закрытие кабинет
        event["time"] = closingTime;

        return event;
    }

    // Добавляет игрока в очередь
    map<string, int> add(int gameTime, int gameId, bool toFront = false, int pos = 0) {
        list<int>::iterator it;
        if (!toFront) {
            it = queue.end();
            advance(it, -pos); // на pos место с конца
        } else {
            it = queue.begin();
            advance(it, pos); // на pos место с начала
        }

        queue.insert(it, gameId);

        map<string, int> event;
        event["type"] = 2; // Встать в очередь
        event["player"] = gameId;
        event["time"] = gameTime;

        return event;
    }

    // Пора ли выйти
    bool shouldQuit(int gameTime) {
        return isProcedure() && procedureIsOver(gameTime); // Внутри кто-то есть, и время процедуры вышло
    }

    // Выпускает игрока из кабинета
    map<string, int> quit(int gameTime = -1) {
        map<string, int> event;
        event["type"] = 3; // Выйти из кабинета
        event["player"] = currentPlayer;
        if (gameTime == -1)
            event["time"] = timeEndProcedure;
        else
            event["time"] = gameTime;

        timeStartProcedure = -1;
        timeEndProcedure = 90000;
        currentPlayer = -1;

        return event;
    }

    // Пора ли войти
    bool shouldEnter(int gameTime) {
        return isOpened() && !isClosed() && !isProcedure() && !queueIsEmpty(); // Внутри никого нет, и очередь непустая
    }

    // Впускает следущего игрока в кабинет
    map<string, int> enter(int gameTime) {
        currentPlayer = queue.front();
        queue.pop_front();

        timeStartProcedure = gameTime;
        timeEndProcedure = timeStartProcedure + procedureTime;

        map<string, int> event;
        event["type"] = 4; // Войти в кабинет
        event["player"] = currentPlayer;
        event["time"] = gameTime;

        return event;
    }

    map<string, int> leave(int gameTime, int gameId) {
        for (list<int>::iterator it = queue.begin(); it != queue.end(); it++) {
            if (*it == gameId)
                queue.erase(it--);
        }

        map<string, int> event;
        event["type"] = 5; // Покинуть очередь в кабинет
        event["player"] = gameId;
        event["time"] = gameTime;

        return event;
    }

};

