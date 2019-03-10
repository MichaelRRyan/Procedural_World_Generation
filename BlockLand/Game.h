/// @Author Michael Rainsford Ryan

#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>

const unsigned WINDOW_WIDTH{ 800u };
const unsigned WINDOW_HEIGHT{ 800u };
const unsigned LEVEL_ROWS{ 50u };
const unsigned LEVEL_COLS{ 500u };
const float BLOCK_SIZE{ 16.0f };
const int SEA_LEVEL{ 32 };
const unsigned NUMBER_OF_BLOCKS{ 6 }; // The number of blocks plus one for the null block

// Terrain generation variables
const int LAND_RANGE{ 3 }; // Defines the range that all ground level blocks will be from the last
const int DIRT_RANGE{ 3 }; // Defines the range from 1 to DIRT_RANGE in which dirt will be placed from the ground block

// Colours
const sf::Color GRASS_COLOUR{ 30, 60, 20, 255 };
const sf::Color DIRT_COLOUR{ 100, 60, 20, 255 };
const sf::Color SAND_COLOUR{ 255, 255, 100, 255 };
const sf::Color STONE_COLOUR{ 80, 80, 80, 255 };
const sf::Color WATER_COLOUR{ 30, 40, 100, 155 };
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
	

	// Player functions
	void setupPlayer();
	bool isColliding(sf::RectangleShape t_subjectOne, sf::Vector2f t_position);
	float signOf(float t_value);
	const sf::Vector2f unitVector(sf::Vector2f t_vector);

	sf::RenderWindow m_window;
	bool m_exitGame;

	int m_blocks[LEVEL_ROWS][LEVEL_COLS];
	// Block types: 0 == nothing, 1 == grass, 2 == dirt, 3 == stone, 4 == sand, 5 == water
	sf::RectangleShape m_renderBlock;

	sf::Color m_blockColours[NUMBER_OF_BLOCKS]{sf::Color::Magenta, GRASS_COLOUR, DIRT_COLOUR, STONE_COLOUR, SAND_COLOUR, WATER_COLOUR };

	sf::Vector2f m_playerVelocity{ 0.0f, 0.0f };
	sf::Vector2f m_playerAcceleration{ 0.0f, 0.0f };
	float m_playerMovementSpeed = 4.0f;
	float m_gravity = 0.4f;
	float m_playerJumpSpeed = 6.0f;

	sf::RectangleShape m_player;

	// Screen scroll
	sf::View m_currentView;
};

#endif // !GAME

