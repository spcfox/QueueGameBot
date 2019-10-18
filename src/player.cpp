#include "player.h"

using namespace std;

class Player {
public:
	int timeInQueue = 0; // Общее время в очереди за игру
	int id; // Глобальный id
	int comment_id;
	int time; // Время отправки сообщения
	int gameId; // Локальный id в данной игре
	string name; // Имя

	bool inQueue = false; // В очереди
	bool inOffice = false; // В кабинете
	bool doc = false;
	bool newPlayer = false;

	Player(int c_id, int c_comment_id, int c_time, int c_gameId, string c_name) {
		id = c_id;
		comment_id = c_comment_id;
		time = c_time;
		gameId = c_gameId;
		name = c_name;
	}

	Player() {}

	void setParams(int c_comment_id, int c_time, string c_name) {
		comment_id = c_comment_id;
		time = c_time;
		name = c_name;
	}

	void setCommentId(int c_comment_id) {
		comment_id = c_comment_id;
	}

	void setTime(int c_time) {
		time = c_time;
		newPlayer = true;
	}

	void addTime(int gameTime) {
		timeInQueue += gameTime - time;
	}

	int getTime() {
		return timeInQueue;
	}

	string getMention() {
		string mention = "@id" + to_string(id) + "(" + name + ")";
		return mention;
	}

	void giveDoc() {
		doc = true;
	}

	void joinQueue() {
		inQueue = true;
	}

	void leaveQueue(int gameTime) {
		inQueue = false;
		addTime(gameTime);
	}

	void enter() {
		inQueue = false;
		inOffice = true;
	}

	void quit(int gameTime) {
		inOffice = false;
		addTime(gameTime);
	}

	string print() {
		string str = "Player [";
		str += "id=" + to_string(id);
		str += ",gameId=" + to_string(gameId);
		str += ",inQueue=" + to_string(inQueue);
		str += ",inOffice=" + to_string(inOffice);
		str += "]";
		return str;
	}
};
