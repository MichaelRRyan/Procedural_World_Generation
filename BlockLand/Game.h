/// @Author Michael Rainsford Ryan

#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>

const unsigned LEVEL_ROWS{ 50 };
const unsigned LEVEL_COLS{ 50 };
const float BLOCK_SIZE{ 16.0f };
const int SEA_LEVEL{ 32 };

// Terrain generation variables
const int LAND_RANGE{ 3 }; // Defines the range that all ground level blocks will be from the last
const int DIRT_RANGE{ 3 }; // Defines the range from 1 to DIRT_RANGE in which dirt will be placed from the ground block

// Colours
const sf::Color GRASS_COLOUR{ 30, 60, 20, 255 };
const sf::Color DIRT_COLOUR{ 100, 60, 20, 255 };
const sf::Color SAND_COLOUR{ 255, 255, 100, 255 };
const sf::Color STONE_COLOUR{ 80, 80, 80, 255 };
const sf::Color WATER_COLOUR{ 30, 40, 100, 255 };
const sf::Color SKY_COLOUR{ 130, 130, 255, 255 };

class Game
{
public:

	Game();
	~Game();

	void run();

private:

	void processEvents();
	void update(sf::Time t_deltaTime);
	void render();

	void setupShapes();
	void setupWorld();
	void drawWorld();

	sf::RenderWindow m_window;
	bool m_exitGame;

	sf::RectangleShape m_blocks[LEVEL_ROWS][LEVEL_COLS];

};

#endif // !GAME

