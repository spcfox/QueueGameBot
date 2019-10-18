using namespace std;

class Office {
	private:
		int openingTime;
		int closingTime;
		int procedureTime;

		list<int> queue;
		int currentPlayer;
		int timeStartProcedure;
		int timeEndProcedure;

	public:
		Office(int oT=0, int cT=0, int pT=0);

		int getCurrentPlayer(); // Возвращает игрока, который в кабинете
		bool queueIsEmpty(); // Пустая ли очередь
		bool isProcedure(); // Есть ли кто-то в кабинете
		bool procedureIsOver(int gameTime); // Пора ли выходить из кабинета
		bool timeIsOver(int gameTime); // Наступило ли время закрытия
		bool officeIsOver(int gameTime); // Закончилась ли игра в этом кабинете

		map<string, int> add(int gameId, bool toFront=false, int pos=0); // Добавляет игрока в очередь
		bool shouldQuit(int gameTime);// Пора ли выйти
		map<string, int> quit(); // Выпускает игрока из кабинета
		bool shouldEnter(int gameTime); // Пора ли войти
		map<string, int> enter(int gameTime); // Впускает следущего игрока в кабинет
};
