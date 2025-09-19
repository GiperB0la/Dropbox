#include "include/DropboxApp.hpp"


//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//    LPSTR lpCmdLine, int nCmdShow)
//{
//    setlocale(LC_ALL, "");
//
//    DropboxApp dropbox_app;
//    dropbox_app.run();
//
//    return 0;
//}

int main()
{
	setlocale(LC_ALL, "");

	DropboxApp dropbox_app;
	dropbox_app.run();

	return 0;
}