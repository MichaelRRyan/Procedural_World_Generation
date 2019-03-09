/// Side scroller sandbox game
/// @Author Michael Rainsford Ryan
/// @Date 09/03/2019
/// Session 1: 10:57 - 11:37 - 09/03/19 /// Began creating procedural world generation
/// Session 2: 12:33 - 13:08 - 09/03/19 /// Finished basic procedural world generation
/// Session 3: 13:36 - 14:31 - 09/03/19 /// Began adding player character
/// Session 4: 15:21 - 15:53 - 09/03/19 /// Finished basic player character and screen scroll
/// Time taken: 162m (2h 42m)

#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif

#include "Game.h"
#include <ctime>
#include <cstdlib>

int main()
{
	srand(static_cast<unsigned>(time(nullptr)));

	Game game;
	game.run();

	return 0;
}
