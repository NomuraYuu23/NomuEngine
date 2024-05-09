#include <Windows.h>
#include "Engine/Framework/MyFramework.h"
#include "Engine/Framework/MyGame.h"
#include "Engine/base/CrashHandler.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	__try {

		MyFramework* game = new MyGame();

		game->Run();

		delete game;

	}
	__except (CrashHandler::GenerateDump(GetExceptionInformation())) {}

	return 0;

}
