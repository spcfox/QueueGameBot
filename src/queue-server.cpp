// src.cpp: определяет точку входа для приложения.

#define _CRT_SECURE_NO_WARNINGS

#include "queue-server.h"
#include "player.cpp"
#include "office.cpp"
#include "output.cpp"

using namespace std;

const int EXTRATIME = 0; // Время работы цикла после конца дня во избежание ошибок из-за задержки

Office office;
vector<Player> allPlayers;

int lastLine = 0;

struct playerInfo {
	int id;
	int comment_id;
	int time;
	string name;
	time_t unixTime;
};

vector<playerInfo> newPlayers;

struct queueInfo {
	int openingTime;
	int closingTime;
	int procedureTime;
};

// Получает параметры очереди
queueInfo getQueueOptions() {
	queueInfo qInfo;

	string line;
	ifstream in("txt/queueInfo.txt");

	getline(in, line);
	qInfo.openingTime = stoi(line);

	getline(in, line);
	qInfo.closingTime = stoi(line);

	getline(in, line);
	qInfo.procedureTime = stoi(line);

	in.close();

	return qInfo;
}

void printAllPlayers() {
	cout << "-------\n";
	for (Player player : allPlayers)
		cout << player.print() << "\n";
	cout << "-------\n";
}

// Создаёт очередь
void createOffice(queueInfo qInfo) {
	office = Office(qInfo.openingTime, qInfo.closingTime, qInfo.procedureTime);
}

// Создаёт игрока
int createPlayer(playerInfo pInfo) {
	int gameId = allPlayers.size();
	Player player = Player(pInfo.id, pInfo.comment_id, pInfo.time, gameId, pInfo.name);
	allPlayers.push_back(player);
	return gameId;
}

// Возвращает игрока по его номеру в этой игре
Player getPlayer(int gameId) {
	return allPlayers[gameId];
}

// Возвращает номер игрока по глобальному id; -1 если игрок новый
int getIdPlayer(int id) {
	for (Player player : allPlayers) {
		if (player.id == id)
			return player.gameId;
	}

	return -1;
}

int unixTimeToDayTime(time_t unixTime) {
	tm* ltm = localtime(&unixTime);
	int dayTime = 60 * (60 * ltm->tm_hour + ltm->tm_min) + ltm->tm_sec;
	return dayTime;
}

// Возвращает время в секундах с начала дня
int currentTime() {
	time_t unixTime = time(0);
	int gameTime = unixTimeToDayTime(unixTime);

	return gameTime;
}

string gameTimeToNormal(int gameTime) {
	int s = gameTime % 60;
	gameTime /= 60;
	int m = gameTime % 60;
	gameTime /= 60;
	int h = gameTime % 60;

	string time = "";
	if (h < 10)
		time += "0";
	time += to_string(h);

	time += ":";
	if (m < 10)
		time += "0";
	time += to_string(m);

	time += ":";
	if (s < 10)
		time += "0";
	time += to_string(s);

	return time;
}

string timeInQueueToNormal(int gameTime) {
	int s = gameTime % 60;
	gameTime /= 60;
	int m = gameTime % 60;
	gameTime /= 60;
	int h = gameTime % 60;

	string time = "";
	if (h < 10)
		time += "0";
	time += to_string(h);

	time += ":";
	if (m < 10)
		time += "0";
	time += to_string(m);

	time += ":";
	if (s < 10)
		time += "0";
	time += to_string(s);

	return time;
}

void input() {
	playerInfo pInfo;

	ifstream in("txt/players.txt");

	int i = 0;
	while (in >> pInfo.id >> pInfo.unixTime >> pInfo.name >> pInfo.comment_id) {
		i++;
		if (i > lastLine) {
			pInfo.time = pInfo.unixTime;
			// pInfo.time = unixTimeToDayTime(pInfo.unixTime); // Если во входном файле unix время
			newPlayers.push_back(pInfo);
		}
	}

	if (i > lastLine)
		lastLine = i;

	in.close();
}

bool lessTime(int id1, int id2) {
	return getPlayer(id1).timeInQueue < getPlayer(id2).timeInQueue;
}

string results() {
	string res = "";

	vector<int> winners;
	vector<int> losers;

	for (Player player : allPlayers) {
		if (player.doc)
			winners.push_back(player.gameId);
		else
			losers.push_back(player.gameId);
	}

	sort(winners.begin(), winners.end(), lessTime);

	if (winners.size() == 0) {
		if (losers.size() != 0)
			res = "Никто не получил справку";
		else
			res = "Никто не пришёл на фан встречу";
	}
	else {
		res += "Получили справки: \n";

		for (int gameId : winners) {
			Player player = getPlayer(gameId);
			res += player.getMention() + " - " + timeInQueueToNormal(player.timeInQueue) + "\n";
		}

		if (losers.size() != 0) {
			res += "\n";
			res += "Так и не смогли получить справку: \n";

			for (int gameId : losers) {
				Player player = getPlayer(gameId);
				res += player.getMention() + " - " + timeInQueueToNormal(player.timeInQueue) + "\n";
			}
		}
	}

	return res;
}


void addToComment(map<int, string> &comments, string str, int reply) {
	if (comments.find(reply) != comments.end())
		comments[reply] += "\n" + str;
	else
		comments[reply] = str;
}

bool eventBefore(map<string, int> e1, map<string, int> e2) {
	// Чем меньше приоритет, тем раньше выводится
	int priorities[] = {
		0, // открытие кабинета
		5, // закрытие кабинета
		1, // встать в очередь
		3, // выйти из кабинета
		4, // войти в кабинет
		2, // покинуть очередь
	};

	if (e1["time"] == e2["time"])
		return priorities[e1["type"]] < priorities[e2["type"]];
	else
		return e1["time"] < e2["time"];
}

// Вывод
void output(int gameTime, vector<map<string, int>> events) {
	sort(events.begin(), events.end(), eventBefore);

	map<int, string> comments;

	for (map<string, int> e : events) {
		string timeStr = gameTimeToNormal(e["time"]);
		string str;

		switch (e["type"]) {
			case 0:
				str += "🙋‍♂🚪 Кабинет открылся!";
				break;
			case 1:
				str += "🙅‍♀🚪 Кабинет закрылся!";
				break;
			case 2:
				if (e["haveDoc"])
					str += "🔄👥 " + getPlayer(e["player"]).getMention() + ", ты снова в очереди! У тебя же есть справка...";
				else
					str += "➡👥 " + getPlayer(e["player"]).getMention() + ", теперь ты в очереди!";
				break;
			case 3:
				if (e["doc"])
					str += "⬅🚪📝 " + getPlayer(e["player"]).getMention() + ", справка получена! Ты выходишь из кабинета.";
				else if (e["leave"])
					if (e["haveDoc"])
						str += "💢🚪🧠 " + getPlayer(e["player"]).getMention() + ", ход гения! Ты выходишь из кабинета до получения справки.";
					else
						str += "💢🚪🧠 " + getPlayer(e["player"]).getMention() + ", ход гения! Ты выходишь из кабинета до получения справки.";
				else
					str += "⬅🚪🧠 " + getPlayer(e["player"]).getMention() + ", у тебя уже была справка! Ты выхоишь из кабинета.";
				break;
			case 4:
				str += "➡🚪 В кабинет заходит " + getPlayer(e["player"]).getMention() + "!";
				break;
			case 5:
				str += "💢👥 " + getPlayer(e["player"]).getMention() + ", ты выходишь из очереди! Пусть сами стоят!";
				break;
		}

		addToComment(comments, str, e["reply"]);

		cout << timeStr + " " + str << "\n"; // Консоль
		log(timeStr + " " + str); // Файл
	}

	for (pair<int, string> comment : comments) {
		createComment(comment.second, comment.first); // Коммент от бота
	}
}

void printResults() {
	string res = results();

	cout << res << "\n"; // Консоль
	log("\n" + res); // Файл
	createComment(res, 0);
}

void officeEvents(vector<map<string, int>>& events, int gameTime) {
	if (office.shouldQuit(gameTime)) {
		map<string, int> e = office.quit();

		Player& player = allPlayers[e["player"]];

		if (player.doc)
			e["doc"] = 0;
		else {
			e["doc"] = 1;
			player.giveDoc();
		}

		player.quit(e["time"]);

		events.push_back(e);

		gameTime = e["time"];
	}
	if (office.shouldEnter(gameTime)) {
		map<string, int> e = office.enter(gameTime);
		Player& player = allPlayers[e["player"]];
		player.enter();

		if (player.newPlayer) {
			e["time"] = player.time;
			player.newPlayer = false;
		}

		events.push_back(e);
		player.newPlayer = false;
	}
}

void playerEvents(vector<map<string, int>> &events, int gameTime) {
	while (newPlayers.size()) {
		playerInfo pInfo = newPlayers.front();
		newPlayers.erase(newPlayers.begin());

		if (pInfo.time > gameTime)
			break;

		int time = pInfo.time; // Время для officeEvents

		officeEvents(events, time);

		map<string, int> e;

		int gameId = getIdPlayer(pInfo.id);
		if (gameId == -1) {
			gameId = createPlayer(pInfo);
			Player& player = allPlayers[gameId];
			player.joinQueue();
			e = office.add(pInfo.time, gameId);
			e["reply"] = player.comment_id;
			if (player.doc)
				e["haveDoc"] = 1;
			else
				e["haveDoc"] = 0;
		}
		else {
			Player& player = allPlayers[gameId];

			if (player.inQueue) {
				player.setCommentId(pInfo.comment_id);
				e = office.leave(pInfo.time, gameId);
				e["reply"] = player.comment_id;
				e["leave"] = 1;
				player.leaveQueue(e["time"]);
				player.setTime(pInfo.time);
			}
			else if (player.inOffice) {
				player.setCommentId(pInfo.comment_id);
				e = office.quit(pInfo.time);
				player.quit(e["time"]);
				e["reply"] = player.comment_id;
				e["doc"] = 0;
				e["leave"] = 1;
				if (player.doc)
					e["haveDoc"] = 1;
				else
					e["haveDoc"] = 0;
				player.setTime(pInfo.time);
			}
			else {
				player.setCommentId(pInfo.comment_id);
				player.joinQueue();
				e = office.add(pInfo.time, gameId);
				e["reply"] = player.comment_id;
				if (player.doc)
					e["haveDoc"] = 1;
				else
					e["haveDoc"] = 0;
				player.setTime(pInfo.time);
			}
		}

		Player& player = allPlayers[e["player"]];

		if (player.newPlayer) {
			e["time"] = player.time;
			player.newPlayer = false;
		}

		events.push_back(e);

		officeEvents(events, time);
	}
}

vector<map<string, int>> checkEvents(int gameTime) {
	vector<map<string, int>> events;

	if (office.shouldOpen(gameTime)) {
		map<string, int> e = office.open();
		events.push_back(e);
	}
	if (office.shouldClose(gameTime)) {
		map<string, int> e = office.close();
		events.push_back(e);
	}

	playerEvents(events, gameTime);
	officeEvents(events, gameTime);

	return events;
}

// Всё время в секундах
void cyrcle() {
	int gameTime = currentTime(); // Время с начала дня
	bool endGame = false;
	int endGameTime = 0;

	while (!endGame || gameTime <= endGameTime + EXTRATIME) {
		gameTime = currentTime(); // Обновляем время

		input(); // Проверяем, сделали ли что-то игроки

		vector<map<string, int>> events = checkEvents(gameTime); // Проверяем, должно ли что-то произойти

		output(gameTime, events);

		//string str = to_string(gameTime) + " ";
		//str += "endGame=" + to_string(endGame) + ", ";
		//str += "isProcedure()=" + to_string(office.isProcedure()) + ", ";
		//str += "timeIsOver()=" + to_string(office.timeIsOver(gameTime));
		//str += "officeIsOver()=" + to_string(office.officeIsOver(gameTime));
		//log(str);

		// Проверяем, должна ли игра закончится
		if (!endGame && office.officeIsOver(gameTime)) {
			endGame = true;
			endGameTime = gameTime;
		}

		this_thread::sleep_for(std::chrono::milliseconds(1000)); // Пауза 1 секунда
	}

	while (!office.queueIsEmpty()) {
		list<int> queue = office.clearQueue();
		int closingTime = office.getClosingTime();
		for (int gameId : queue) {
			allPlayers[gameId].addTime(closingTime);
		}
	}

	printResults();
}

int main() {
	// Сброс output.txt
	ofstream out;
	out.open("txt/output.txt");
	out.close();

	getOptions(); // Читаем настройки вывода

	queueInfo qInfo = getQueueOptions(); // Читаем параметры очереди
	createOffice(qInfo); // Создаём очередь

	cyrcle(); // Запускаем цикл

	return 0;
}