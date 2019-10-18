#include <curl/curl.h>
#include <algorithm>

using namespace std;

#pragma comment(lib,"libcurl.lib")

struct options {
	string token;
	string owner_id;
	string post_id;
	bool onComments;
};

options opt;

void getOptions() {
	string line;
	ifstream in("txt/options.txt");

	getline(in, line);
	line.erase(line.find('\r'), 1);
	opt.token = line;

	getline(in, line);
	line.erase(line.find('\r'), 1);
	opt.owner_id = line;

	getline(in, line);
	line.erase(line.find('\r'), 1);
	opt.post_id = line;

	getline(in, line);
	line.erase(line.find('\r'), 1);
	opt.onComments = line == "true";

	in.close();
}

void log(string str) {
	ofstream out;
	out.open("txt/output.txt", std::ios::app);
	out << str << "\n";
	out.close();
}

string deleteSpaces(string str) {
	int i;
	while ((i = str.find(" ", 0)) != string::npos)
		str.replace(i, 1, "%20");
	while ((i = str.find("\n", 0)) != string::npos)
		str.replace(i, 1, "%0A");
	return str;
}

void createComment(string message, int reply) {
	if (opt.onComments) {
		CURL* curl;
		CURLcode res;

		curl = curl_easy_init();
		if (curl) {
			string v = "5.102";

			string token = opt.token;
			string owner_id = opt.owner_id;
			string post_id = opt.post_id;
			string reply_to_comment = to_string(reply);

			string request = "https://api.vk.com/method/wall.createComment?";

			request += "v=" + v + "&";
			request += "access_token=" + token + "&";
			request += "owner_id=" + owner_id + "&";
			request += "post_id=" + post_id + "&";
			request += "reply_to_comment=" + reply_to_comment + "&";
			request += "message=" + deleteSpaces(message);

			curl_easy_setopt(curl, CURLOPT_URL, request.c_str());

			/* Выполнение запроса, res будет содержать код возврата */
			res = curl_easy_perform(curl);
			/* Проверка на ошибки */
			//if (res != CURLE_OK)
			//	fprintf(stderr, "curl_easy_perform() failed: %s\n",
			//		curl_easy_strerror(res));

			/* Очистка */
			curl_easy_cleanup(curl);
		}
	}
}