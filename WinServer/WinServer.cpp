//lab3-sandbox
#include "targetver.h"

#include <Windows.h>
#include <fstream>
#include<cstdio>
#include <list>
#include <algorithm>
#include <locale> 
#include <codecvt>
#include "sqlite_database.h"
#include <iomanip>

#include "sqlite_clients_db.h"
#include"sqlite_database.h" //for messages' logging
#include "sql_events.h"

using namespace std;

#define BASE_PATH_PIPE L"\\\\.\\pipe\\"
#define SIZE_BUFFER 2048

#define MESSAGE L"SERVER!!!"
#define SIZE_MSG_BYTES 16
// ofstream file;
int rc;
int rc_client;

DWORD WINAPI Server(LPVOID);
void logging( wstring);
string cnvrt(wstring);

int main(int argc, char* params[])
{
	BOOL isConnected = FALSE;
	DWORD dwThreadId = 0;
	HANDLE hPipe = NULL;
	wstring namePipe(BASE_PATH_PIPE);
	wstring tmp;
	wstring t;
	list<HANDLE> threads;

	cout << "Write name of pipe: " <<  endl;
	wcin >> tmp;

	namePipe.append(tmp);

	wcout << "Main thread awake creating pipe with path: " << namePipe <<  endl;

	t.assign(L"Main thread awake creating pipe with path: ");
	t.append(namePipe);

	rc = sqlite3_open(DB_FILE_NAME, &db);
	rc_client = sqlite3_open(DB_FILE_NAME_C, &db_c);        //database for clients


	if (rc) {
		 cout << "Error code: " << rc <<  endl;
		return rc;
	}
	if (rc_client) {
		cout << "Error code: " << rc_client << endl;
		return rc_client;
	}

	rc = create_tables();
	rc = prepare_statements();

	rc_client = create_tables_c();									//creating database for clients
	rc_client = prepare_statements_c();

	if (rc) {
		 cout << "Error code: " << rc <<  endl;
		return rc;
	}
	//rc = list_logs();
	if (rc) {
		 cout << "Error code: " << rc <<  endl;
		return rc;
	}
	logging(L"Server is started.");
	logging(t);

	std::string tmp_s = "Server";
	std::string tmp_s2 = "Started";
	logging_cl(tmp_s, tmp_s2);

	t.clear();
	while (true)
	{
		// wcout << L"Wait for client" <<  endl;
		hPipe = CreateNamedPipe(
			namePipe.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			SIZE_BUFFER,
			SIZE_BUFFER,
			INFINITE,
			NULL);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			logging(L"Error of creating pipe! Process will be terminated!");
			 wcout << L"Error of creating pipe! Process will be terminated!\n";
			return EXIT_FAILURE;
		}

		isConnected = ConnectNamedPipe(hPipe, NULL);
		if (isConnected)
		{
			HANDLE hThread = CreateThread(
				NULL,
				NULL,
				Server,
				(LPVOID)hPipe,
				NULL,
				&dwThreadId);
			if (hThread == INVALID_HANDLE_VALUE)
			{
				logging(L"Error of making thread!");
				 wcout << L"Error of making thread!\n";
			}
			else
			{
				++dwThreadId;
				threads.push_back(hThread);
			}
		}
		else
		{
			logging(L"Error of connection user");

			tmp_s = "USER";
			tmp_s2 = "Error of connection user";

			logging_cl(tmp_s, tmp_s2);

			wcout << L"Error of connection user\n";
			CloseHandle(hPipe);
		}
	}

	 for_each(threads.cbegin(), threads.cend(),
		[](HANDLE h)
		{
			CloseHandle(h);
		});
	CloseHandle(hPipe);
	sqlite3_close(db);
	return EXIT_SUCCESS;
}

DWORD WINAPI Server(LPVOID hPipe)
{
	LPWSTR strRequest = new WCHAR[SIZE_BUFFER + 1];
	memset(strRequest, 0, (SIZE_BUFFER + 1) * sizeof(WCHAR));
	DWORD cntBytesRead = 0;
	DWORD cntBytesWrited = 0;
	BOOL isSucsses = FALSE;
	wstring tmp;
	wstring tmp_nm;

	HANDLE pipe = (HANDLE)hPipe;
	LPWSTR username = new WCHAR[SIZE_BUFFER + 1];

	isSucsses = ReadFile(
		pipe,
		username,
		SIZE_BUFFER * sizeof(WCHAR),
		&cntBytesRead,
		NULL);

	wcout << L"Client " << username << L" connected\n";

	tmp.assign(L"Client ");
	tmp.append(username);
	tmp.append(L" connected");

	tmp_nm.assign(L"");
	tmp_nm.append(username);
	tmp_nm.append(L"");


	logging(tmp);
	tmp.clear();

	string tmp_s = "Conncected";

	logging_cl(cnvrt(username), tmp_s);

	logging(L"Instance created, and wait for messages");
	 wcout << L"Instance created, and wait for messages\n";

	while (true)
	{
		isSucsses = ReadFile(
			pipe,
			strRequest,
			SIZE_BUFFER * sizeof(WCHAR),
			&cntBytesRead,
			NULL);

		if (!isSucsses)
		{
			tmp.assign(L"User ");
			tmp.append(username);
			tmp.append(L" disconnect");

			logging(tmp);

			tmp.clear();

			wcout << "User " << username << " disconnect\n";
			DisconnectNamedPipe(hPipe);
			break;
		}
		else
		{
			tmp.assign(username);
			tmp.append(L"{");
			tmp.append(strRequest);
			tmp.append(L"  }  ");
			wcout << "[" <<username <<"]" << ": " << strRequest << "  :: ";
		}

		isSucsses = WriteFile(
			pipe,
			MESSAGE,
			SIZE_MSG_BYTES,
			&cntBytesWrited,
			NULL
		);

		tmp_s = "Server";
		string tmp_s2 = " Message will be replied";

		if (!isSucsses || cntBytesWrited != SIZE_MSG_BYTES)
		{
			tmp.append(L"Error of reply!");

			logging(tmp);

			tmp.clear();

			wcout << L"Error of reply!\n";
			break;
		}
		else
		{
			tmp.append(L"Message will be replied");

			logging(tmp);

			tmp.clear();

			 wcout << L"Message will be replied\n";
		}

	}
	logging(L"Server is finished.");

	string tmp_s2 = "Finished";

	logging_cl(tmp_s, tmp_s2);

	if (strRequest)
		delete[] strRequest;
	CloseHandle(pipe);
	ExitThread(0);
}

void logging( wstring st)
{
	using convert_type =  codecvt_utf8<wchar_t>;
	 wstring_convert<convert_type, wchar_t> converter;

	 string converted_str = converter.to_bytes(st);

	rc = insert_log(converted_str);
}

std::string cnvrt(std::wstring tmp_s) {
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string converted_str = converter.to_bytes(tmp_s);
	return converted_str;
}