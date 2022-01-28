// WinClient.cpp: 
//

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
	wcout << L"Print name of pipe:" << endl;
	getline(wcin, pipe);

	wstring namePipe(L"\\\\");
	namePipe.append(server);
	namePipe.append(L"\\pipe\\");
	namePipe.append(pipe);
	wcout << L"Path to pipe: " << namePipe << endl;

	wstring username;
	wcout << L"Print the username:" << endl;
	getline(wcin, username);

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
		
	}

	CloseHandle(hPipe);
	delete[] str;
	system("pause");
	return EXIT_SUCCESS;
}

