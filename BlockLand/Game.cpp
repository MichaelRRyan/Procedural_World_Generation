/// @Author Michael Rainsford Ryan

#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ WINDOW_WIDTH, WINDOW_HEIGHT, 32u }, "Basic Game" },
	m_exitGame{ false },
	m_currentView{ m_window.getDefaultView() }
{
	setupShapes();
	setupWorld();
	setupPlayer();
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
				setupPlayer();
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

	// Get horisontal input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_playerVelocity.x = -m_playerMovementSpeed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_playerVelocity.x = m_playerMovementSpeed;
	}
	else
	{
		m_playerVelocity.x = 0.0f;
	}

	// Check if on the ground
	if (isColliding(m_player, sf::Vector2f{ m_player.getPosition().x, m_player.getPosition().y + 1.0f }))
	{
		// Jump
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			m_playerVelocity.y = -m_playerJumpSpeed;
		}

		m_playerAcceleration.y = 0;
	}
	else
	{
		// Add gravity
		m_playerAcceleration.y = m_gravity;
	}

	m_playerVelocity.x += m_playerAcceleration.x;
	if (m_playerVelocity.y < 15)
	{
		m_playerVelocity.y += m_playerAcceleration.y;
	}

	// Check for horisontal collisions
	if (isColliding(m_player, sf::Vector2f{ m_player.getPosition().x + m_playerVelocity.x, m_player.getPosition().y }))
	{
		while (!isColliding(m_player, sf::Vector2f{ m_player.getPosition().x + signOf(m_playerVelocity.x), m_player.getPosition().y }))
		{
			m_player.move(signOf(m_playerVelocity.x), 0.0f);
		}
		m_playerVelocity.x = 0.0f;
	}
	else
	{
		m_player.move(m_playerVelocity.x, 0.0f);
	}

	// Check for vertical collisions
	if (isColliding(m_player, sf::Vector2f{ m_player.getPosition().x, m_player.getPosition().y + m_playerVelocity.y }))
	{
		while (!isColliding(m_player, sf::Vector2f{ m_player.getPosition().x, m_player.getPosition().y + signOf(m_playerVelocity.y) }))
		{
			m_player.move(0.0f, signOf(m_playerVelocity.y));
		}
		m_playerVelocity.y = 0.0f;
	}
	else
	{
		m_player.move(0.0f, m_playerVelocity.y);
	}

	// Update the view to follow the player
	m_currentView.setCenter(m_player.getPosition().x, m_currentView.getCenter().y);
	m_window.setView(m_currentView);
}

void Game::render()
{
	m_window.clear(SKY_COLOUR);

	m_window.draw(m_player);
	drawWorld();

	m_window.display();
}

void Game::setupShapes()
{
	m_renderBlock.setSize({ BLOCK_SIZE, BLOCK_SIZE });
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
				m_blocks[rows][cols] = 5;
			}
			else
			{
				m_blocks[rows][cols] = 0;
			}
		}
	}

	// generate the ground
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
		(groundLevel < SEA_LEVEL) ? m_blocks[groundLevel][cols] = 1 : m_blocks[groundLevel][cols] = 4;

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
			(groundLevel < SEA_LEVEL) ? m_blocks[currentBlock][cols] = 2 : m_blocks[currentBlock][cols] = 4;
		}

		// Add stone to this column
		for (; currentBlock < LEVEL_ROWS; currentBlock++)
		{
			m_blocks[currentBlock][cols] = 3;
		}
	}
}

void Game::drawWorld()
{
	int onScreenMin = (m_currentView.getCenter().x - WINDOW_WIDTH / 2) / BLOCK_SIZE;
	int onScreenMax = (m_currentView.getCenter().x + WINDOW_WIDTH / 2) / BLOCK_SIZE + 1;

	// Increment the values if they're below zero to keep inside the array size
	for (; onScreenMin < 0; onScreenMin++);
	for (; onScreenMax < 0; onScreenMax++);

	for (int i = 0; i < LEVEL_ROWS; i++)
	{
		for (int j = onScreenMin; j < LEVEL_COLS && j < onScreenMax; j++)
		{
			if (m_blocks[i][j] != 0)
			{
				m_renderBlock.setPosition(static_cast<float>(j *BLOCK_SIZE), static_cast<float>(i * BLOCK_SIZE));
				m_renderBlock.setFillColor(m_blockColours[m_blocks[i][j]]);
				m_window.draw(m_renderBlock);
			}
		}
	}
}

void Game::setupPlayer()
{
	m_player.setSize({ BLOCK_SIZE,BLOCK_SIZE });
	m_player.setPosition(500.0f, 0.0f);
}

void Game::managePlayerCollisions()
{
	sf::Vector2f pos = m_player.getPosition();
	sf::Vector2f velUnit = unitVector(m_playerVelocity);
	int row;
	int col;
	bool notColliding = true;

	while (notColliding)
	{
		row = pos.y / BLOCK_SIZE;
		col = pos.x / BLOCK_SIZE;

		if (row < LEVEL_ROWS && row >= 0 && col < LEVEL_COLS && col >= 0) // Make sure the row and col and inside the array
		{
			if (m_blocks[row][col] != 0 && m_blocks[row][col] != 5) // If there's a block in the current row and col, the player is colliding
			{
				notColliding = false;
				m_playerVelocity = { 0.0f, 0.0f };
			}
			else
			{
				
			}
		}
		else
		{
			// Add code to avoid infinite loop
		}

	}
}

bool Game::isColliding(sf::RectangleShape t_subjectOne, sf::Vector2f t_position)
{
	// Set the bool
	bool colliding = false;

	int collisionRange = 6;
	int playerRow = m_player.getPosition().y / BLOCK_SIZE;
	int playerCol = m_player.getPosition().x / BLOCK_SIZE;
	int startRow = playerRow - collisionRange;
	int startCol = playerCol - collisionRange;

	for (; startRow < 0 && startRow < playerRow; startRow++); // increment the start row if it's outside the bounds
	for (; startCol < 0 && startCol < playerRow; startCol++); // increment the start col if it's outside the bounds

	for (int row = startRow; row < LEVEL_ROWS && row < playerRow + collisionRange; row++)
	{
		for (int col = startCol; col < LEVEL_COLS && col < playerCol + collisionRange; col++)
		{
			if (m_blocks[row][col] != 0 && m_blocks[row][col] != 5)
			{
				// Get the distance vector
				sf::Vector2f distanceVector = t_position - sf::Vector2f{ static_cast<float>(col) * BLOCK_SIZE, static_cast<float>(row) * BLOCK_SIZE};

				// Get the X and Y distance values
				float xDistance = sqrt(distanceVector.x * distanceVector.x);
				float yDistance = sqrt(distanceVector.y * distanceVector.y);

				// Check horisontal collisions
				if (xDistance < t_subjectOne.getGlobalBounds().width / 2 + BLOCK_SIZE / 2)
				{
					// Check vertical collisions
					if (yDistance < t_subjectOne.getGlobalBounds().height / 2 + BLOCK_SIZE / 2)
					{
						colliding = true;
						break;
					}
				}
			}
		}

		if (colliding)
		{
			break;
		}
	}

	return colliding;
}

float Game::signOf(float t_value)
{
	float sign = 0.0f;

	if (t_value > 0)
	{
		sign = 1.0f;
	}
	if (t_value < 0)
	{
		sign = -1.0f;
	}

	return sign;
}

const sf::Vector2f Game::unitVector(sf::Vector2f t_vector)
{
	const float magnitude = sqrt(t_vector.x * t_vector.x + t_vector.y * t_vector.y); // Get the magnitude of the inputted vector
	if (magnitude != 0.0f) // Check for 0 to avoid division by 0
		return t_vector / magnitude; // Return the unit vector
	return sf::Vector2f{ 0.0f,0.0f }; // Return a 0,0 vector if magnitude is 0
}
