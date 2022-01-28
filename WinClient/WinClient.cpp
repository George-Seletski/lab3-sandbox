// WinClient.cpp: 
//lab3-sandox

#include "targetver.h"

#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* args[])
{
	HANDLE hPipe = NULL;
	DWORD dwMode = 0;
	BOOL isSuccess = FALSE;
	DWORD iCountIO = 0;
	LPWSTR str = new WCHAR[260];
	memset(str, 0, 259 * 2); //Sets the first num bytes of the block of memory

	 //wstring server;
	 //wcout << L"Print name of server, or . for localhost" <<  endl;
	 //getline( wcin, server);
	 wstring server;
	 server = '.';

	 wstring pipe;
	 wcout << L"Print name of pipe:" <<  endl;
	 getline(wcin, pipe);

	 wstring namePipe(L"\\\\");
	namePipe.append(server);
	namePipe.append(L"\\pipe\\");
	namePipe.append(pipe);
	 wcout << L"Path to pipe: " << namePipe <<  endl;

	 wstring username;
	 wcout << L"Print the username:" <<  endl;
	 getline( wcin, username);

	//namePipe.append(L" ");
	//namePipe.append(username);

	//////////////////////////////////////////////////////////
	//WaitNamedPipe(namePipe.c_str(), NMPWAIT_WAIT_FOREVER);

	hPipe = CreateFile(
		namePipe.c_str(),
		GENERIC_ALL,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	WriteFile(
		hPipe,
		username.c_str(),
		(username.size() + 1) * sizeof(wchar_t),
		&iCountIO,
		NULL);

	while (true)
	{
		WaitNamedPipe(namePipe.c_str(), INFINITE);

		 wstring message;
		 wcout << L"Write message: "; // <<  endl;
		 getline( wcin, message);

		dwMode = PIPE_READMODE_MESSAGE;

		isSuccess = SetNamedPipeHandleState(
			hPipe,
			&dwMode,
			NULL,
			NULL);
		if (!isSuccess)
		{
			 wcout << L"Can't edit mode of pipe!";
			return EXIT_FAILURE;
			break;
		}

		 wcout << L"Sending message to server" <<  endl;

		isSuccess = WriteFile(
			hPipe,
			message.c_str(),
			(message.size() + 1) * sizeof(wchar_t),
			&iCountIO,
			NULL);

		if (iCountIO != (message.size() + 1) * sizeof(wchar_t) || !isSuccess)
		{
			 wcout << L"Error of push message to server!" <<  endl;
			return EXIT_FAILURE;
			break;
		}

		 wcout << "Message pushed to server" <<  endl;

		do
		{
			isSuccess = ReadFile(
				hPipe,
				str,
				259 * sizeof(WCHAR),
				&iCountIO,
				NULL);

			if (iCountIO > 0 && isSuccess)
				 wcout << L"Message from server: " << str <<  endl;

		} while (!isSuccess);
	}

	CloseHandle(hPipe);
	delete[] str;
	system("pause");
	return EXIT_SUCCESS;
}

