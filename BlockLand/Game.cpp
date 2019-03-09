/// @Author Michael Rainsford Ryan

#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ 800u, 800u, 32u }, "Basic Game" },
	m_exitGame{ false }
{
	setupShapes();
	setupWorld();
}

Game::~Game()
{

}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float FPS = 60.0f;
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // Run as many times as possible
		timeSinceLastUpdate += clock.restart();
		if (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // Run at a minimum of 60 fps
			update(timePerFrame); // 60 fps
		}
		render(); // Run as many times as possible
	}
}

void Game::processEvents()
{
	sf::Event nextEvent;
	while (m_window.pollEvent(nextEvent))
	{
		if (sf::Event::Closed == nextEvent.type) // check if the close window button is clicked on.
		{
			m_window.close();
		}
		if (sf::Event::KeyPressed == nextEvent.type)
		{
			if (sf::Keyboard::R == nextEvent.key.code)
			{
				setupWorld();
			}
		}
	}
}

void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}
}

void Game::render()
{
	m_window.clear();

	drawWorld();

	m_window.display();
}

void Game::setupShapes()
{
	// Setup the size and position of all blocks
	for (int i = 0; i < LEVEL_ROWS; i++)
	{
		for (int j = 0; j < LEVEL_COLS; j++)
		{
			m_blocks[i][j].setSize({ BLOCK_SIZE, BLOCK_SIZE });
			m_blocks[i][j].setPosition(static_cast<float>(j) * BLOCK_SIZE, static_cast<float>(i) * BLOCK_SIZE);
		}
	}
}

void Game::setupWorld()
{
	// Reset all blocks to sky colour and add water
	for (int cols = 0; cols < LEVEL_COLS; cols++)
	{
		for (int rows = 0; rows < LEVEL_ROWS; rows++)
		{
			if (rows >= SEA_LEVEL)
			{
				m_blocks[rows][cols].setFillColor(WATER_COLOUR);
			}
			else
			{
				m_blocks[rows][cols].setFillColor(SKY_COLOUR);
			}
		}
	}

	// Draw the ground
	int groundLevel = SEA_LEVEL; // First block chooses a position from sea level
	int currentBlock; // The current block being placed along a column

	for (int cols = 0; cols < LEVEL_COLS; cols++)
	{
		if (rand() % 2 == 0) // Only change ground level 1 in 2 times to add more flat areas
		{
			groundLevel = groundLevel + (rand() % static_cast<int>(LAND_RANGE * 2.4) - LAND_RANGE); // Finds the position of the first block
		}
		
		
		// Make sure the next block is within the array size
		(groundLevel < 0) ? groundLevel = 0 : groundLevel;
		(groundLevel >= LEVEL_ROWS) ? groundLevel = LEVEL_ROWS - 1 : groundLevel;

		// Sets the block to dirt above sea level and sand if below
		(groundLevel < SEA_LEVEL) ? m_blocks[groundLevel][cols].setFillColor(GRASS_COLOUR) : m_blocks[groundLevel][cols].setFillColor(SAND_COLOUR);

		currentBlock = groundLevel + 1; // Move to the next level to draw dirt

		// Add dirt to this column
		int dirtLevel = (rand() % DIRT_RANGE + 1) + currentBlock; // Picks a random amount of dirt to place after the ground block based on the dirt range

		if (dirtLevel >= LEVEL_ROWS) // Make sure the dirt doesn't go outside the array size
		{
			dirtLevel = LEVEL_ROWS - 1;
		}

		for (; currentBlock < dirtLevel; currentBlock++)
		{
			// Sets the block to dirt above sea level and sand if below
			(groundLevel < SEA_LEVEL) ? m_blocks[currentBlock][cols].setFillColor(DIRT_COLOUR) : m_blocks[currentBlock][cols].setFillColor(SAND_COLOUR);
		}

		// Add stone to this column
		for (; currentBlock < LEVEL_ROWS; currentBlock++)
		{
			m_blocks[currentBlock][cols].setFillColor(STONE_COLOUR);
		}
	}
}

void Game::drawWorld()
{
	for (int i = 0; i < LEVEL_ROWS; i++)
	{
		for (int j = 0; j < LEVEL_COLS; j++)
		{
			m_window.draw(m_blocks[i][j]);
		}
	}
}
