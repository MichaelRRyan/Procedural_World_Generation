/// @Author Michael Rainsford Ryan

#ifndef GAME
#define GAME

#include <SFML/Graphics.hpp>

const unsigned WINDOW_WIDTH{ 800u };
const unsigned WINDOW_HEIGHT{ 800u };
const unsigned LEVEL_ROWS{ 100u }; // Vertical world size in blocks
const unsigned LEVEL_COLS{ 500u }; // Horisontal world size in blocks
const float BLOCK_SIZE{ 16.0f };
const int SEA_LEVEL{ 32 };
const unsigned NUMBER_OF_BLOCKS{ 9 }; // The number of blocks plus one for the null block

// Terrain generation variables
const int LAND_RANGE{ 3 }; // Defines the range that all ground level blocks will be from the last
const int DIRT_RANGE{ 3 }; // Defines the range from 1 to DIRT_RANGE in which dirt will be placed from the ground block

// Colours
const sf::Color GRASS_COLOUR{ 60, 125, 30, 255 };
const sf::Color DIRT_COLOUR{ 125, 75, 25, 255 };
const sf::Color SAND_COLOUR{ 255, 255, 100, 255 };
const sf::Color STONE_COLOUR{ 100, 100, 100, 255 };
const sf::Color WATER_COLOUR{ 30, 40, 100, 155 };
const sf::Color SKY_COLOUR{ 130, 130, 255, 255 };
const sf::Color WOOD_COLOUR{ 58, 38, 18, 255 };
const sf::Color LEAF_COLOUR{ 28, 68, 1, 255 };
const sf::Color FLOWER_COLOUR{ 186, 48, 48, 255 };

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

	void loadFiles();

	void setupShapes();
	void setupWorld();
	void branchCave(int t_branchRow, int t_branchCol, int t_xDirection, int t_yDirection);
	void paintCave(int t_row, int t_col, int t_radius);

	void paintTree(int t_treeSize, int row, int col);

	void updateWorld();
	void updateBlock(int t_row, int t_col);
	void drawWorld();

	// Player functions
	void setupPlayer();
	bool isColliding(sf::RectangleShape t_subjectOne, sf::Vector2f t_position);
	bool isInWater(sf::RectangleShape t_subject);
	float signOf(float t_value);
	const sf::Vector2f unitVector(sf::Vector2f t_vector);

	sf::RenderWindow m_window;
	bool m_exitGame;

	int m_surfaceLevel[LEVEL_COLS];
	bool m_caves[LEVEL_ROWS][LEVEL_COLS];
	int m_blocks[LEVEL_ROWS][LEVEL_COLS];
	// Block types: 0 == nothing, 1 == grass, 2 == dirt, 3 == stone, 4 == sand, 5 == water, 6 == wood
	sf::RectangleShape m_renderBlock;

	sf::Color m_blockColours[NUMBER_OF_BLOCKS]{sf::Color::Magenta, GRASS_COLOUR, DIRT_COLOUR, STONE_COLOUR, SAND_COLOUR, WATER_COLOUR, WOOD_COLOUR, LEAF_COLOUR, FLOWER_COLOUR };
	sf::Vector2i m_blockSpritePositions[NUMBER_OF_BLOCKS]{ {0,0},{0,0},{16,0},{32,0},{48,0},{0,16},{16,16},{32,16},{48,16} };

	sf::Texture m_blockTextures;
	sf::Sprite m_blockSprite;

	/// PLAYER VARIABLES
	sf::Vector2f m_playerVelocity{ 0.0f, 0.0f };
	sf::Vector2f m_playerAcceleration{ 0.0f, 0.0f };
	float m_playerMovementSpeed = 4.0f;
	float m_gravity = 0.4f;
	float m_playerJumpSpeed = 6.0f;
	bool m_inWater{ false };

	float viewAdd = 0.0f;
	bool m_cavesVisible = false;
	bool m_largeView = true;
	bool m_textures = false;

	sf::RectangleShape m_player;

	// Screen scroll
	sf::View m_currentView;
};

#endif // !GAME

