#include "include/DropboxServer.hpp"


int main()
{
	DropboxServer dropbox_server;
	dropbox_server.start();

	system("pause");
	return 0;
}