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
	loadFiles();
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
			if (sf::Keyboard::C == nextEvent.key.code)
			{
				m_cavesVisible = !m_cavesVisible;
			}
			if (sf::Keyboard::V == nextEvent.key.code)
			{
				if (m_largeView)
				{
					m_currentView.setSize(m_window.getDefaultView().getSize() / 2.0f);
				}
				else
				{
					m_currentView.setSize(m_window.getDefaultView().getSize());
				}

				m_largeView = !m_largeView;
			}
			if (sf::Keyboard::B == nextEvent.key.code)
			{
				m_textures = !m_textures;
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

	m_inWater = isInWater(m_player);

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

	// if in water
	if (m_inWater)
	{
		// Jump
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			m_playerVelocity.y = -m_playerJumpSpeed / 2; // Jump at half the speed of a regular jump
		}

		m_playerVelocity = m_playerVelocity * 0.8f; // Slow the player's velocity
	}


	/// DEBUG
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_playerVelocity.y = -m_playerJumpSpeed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			viewAdd -= 5.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			viewAdd += 5.0f;
		}
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
	if (m_player.getPosition().x > m_currentView.getSize().x / 2)
	{
		m_currentView.setCenter(m_player.getPosition().x, m_currentView.getCenter().x);
	}
	
	m_currentView.setCenter(m_currentView.getCenter().x, /*m_window.getDefaultView().getCenter().y*/ m_player.getPosition().y + viewAdd);
	m_window.setView(m_currentView);
}

void Game::render()
{
	m_window.clear(SKY_COLOUR);

	drawWorld();
	m_window.draw(m_player);

	m_window.display();
}

void Game::loadFiles()
{
	if (!m_blockTextures.loadFromFile("ASSETS//IMAGES//blockSheet.png"))
	{
		// Error loading file
	}
	m_blockSprite.setTexture(m_blockTextures);
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

			m_caves[rows][cols] = false;
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

		m_surfaceLevel[cols] = groundLevel; // Set the surface level of this column

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



	// CAVE TESTING
	/// <summary>
	/// Places a random number of starting position then branches out
	/// from them in random directions for a random number of blocks.
	/// </summary>
	 
	int numberOfStarts = rand() % 40 + 20; // The number of starting positions for caves

	for (int i = 0; i < numberOfStarts; i++) // loop for every start position
	{
		int col = rand() % LEVEL_COLS; // Pick a random column
		int row = rand() % (LEVEL_ROWS - m_surfaceLevel[col]) + m_surfaceLevel[col]; // Pick a random row
		

		branchCave(row, col, rand() % 3 - 1, rand() % 3 - 1); // Set a point there and possibily branch out
	}



	// Remove blocks with a cave over them
	for (int col = 0; col < LEVEL_COLS; col++)
	{
		for (int row = 0; row < LEVEL_ROWS; row++)
		{
			if (m_caves[row][col] == true)
			{
				m_blocks[row][col] = 0;
			}
		}
	}

	updateWorld();




	// TREE TESTING

	for (int col = 0; col < LEVEL_COLS; col++)
	{
		if (m_surfaceLevel[col] < SEA_LEVEL && m_blocks[m_surfaceLevel[col]][col] == 1) // If the surface level is above sea level and is a grass block
		{
			if (rand() % 10 == 0)
			{
				int treeHeight = rand() % 8 + 3;
				for (int i = 1; i <= treeHeight; i++) // Loops for the height of the tree
				{
					if (m_surfaceLevel[col] - i > 0 && m_blocks[m_surfaceLevel[col] - i][col] == 0) // If the surfaceLevel - i is within the vertical bounds and the block at that index is nothing
					{
						if (i == treeHeight) // If this is the last index
						{
							paintTree(i, m_surfaceLevel[col] - i, col);

							for (int j = i; j > 0; j--) // Loop back down the tree and place wood
							{
								m_blocks[m_surfaceLevel[col] - j][col] = 6;
							}
						}
					}
					else if (i > 2) // If the tree is already a minimum height make a tree anyways
					{
						paintTree(i, m_surfaceLevel[col] - i - 1, col);

						for (int j = i - 1; j > 0; j--) // Loop back down the tree and place wood
						{
							m_blocks[m_surfaceLevel[col] - j][col] = 6;
						}
					}
					else // No room for a tree
					{
						break;
					}
				}
			}
			else if (rand() % 5 == 0) // If not placing a tree, random chance to place a flower
			{
				if (m_surfaceLevel[col] - 1 > 0 && m_blocks[m_surfaceLevel[col] - 1][col] == 0)
				{
					m_blocks[m_surfaceLevel[col] - 1][col] = 8;
				}
			}
		}
	}
}

/// <summary>
/// Creates a branch in an inputted position and gives the possibility of branching again
/// </summary>
/// <param name="t_branchPosition"></param>
void Game::branchCave(int t_branchRow, int t_branchCol, int t_xDirection, int t_yDirection)
{
	paintCave(t_branchRow, t_branchCol, rand() % 3);

	// Random 39 in 40 chance of branching
	if (rand() % 40 > 0)
	{
		int newBranchDirY = 0;
		int newBranchDirX = 0;

		// Choose a random direction from this point
		if (t_xDirection == 0 || t_yDirection == 0) // If either of the values are zero
		{
			if (t_xDirection == 0)
			{
				newBranchDirX = rand() % 3 - 1;
				newBranchDirY = t_yDirection;
			}
			else
			{
				newBranchDirX = t_xDirection;
				newBranchDirY = rand() % 3 - 1;
			}
		}
		else
		{
			(t_xDirection < 0) ? newBranchDirX = rand() % 2 - 1 : newBranchDirX = rand() % 2;
			(t_yDirection < 0) ? newBranchDirY = rand() % 2 - 1 : newBranchDirY = rand() % 2;
		}

		int branchRow = t_branchRow + newBranchDirY;
		int branchCol = t_branchCol + newBranchDirX;

		// Check the new row is within the bounds of the world
		if (branchRow < 0)
		{
			branchRow = 0;
		}
		else if (branchRow >= LEVEL_ROWS)
		{
			branchRow = LEVEL_ROWS - 1;
		}

		// Check the new col is within the bounds of the world
		if (branchCol < 0)
		{
			branchCol = 0;
		}
		else if (branchCol >= LEVEL_COLS)
		{
			branchCol = LEVEL_COLS - 1;
		}

		branchCave(branchRow, branchCol, newBranchDirX, newBranchDirY); // Branch to the new row and col
	}
}

/// <summary>
/// Paints an area with an inputted radius.
/// </summary>
void Game::paintCave(int t_row, int t_col, int t_radius)
{
	for (int col = -t_radius; col <= t_radius; col++)
	{
		int maxRow = static_cast<int>(sqrt((t_radius * t_radius) - (col * col))); // Use the circle formula to get the row
		for (int row = -maxRow; row <= maxRow; row++)
		{
			if ((t_col + col < 0 || t_col + col >= LEVEL_COLS) || (t_row + row < 0 || t_row + row >= LEVEL_ROWS)) // If either of the values are outside the world bounds
			{
				continue; // Continue and try the next point
			}
			else
			{
				m_caves[t_row + row][t_col + col] = true;
			}
		}
	}
}

void Game::paintTree(int t_treeSize, int t_row, int t_col)
{
	int radius = static_cast<int>(t_treeSize / 1.8);
	int offset = static_cast<int>(t_treeSize / 6);

	for (int col = -radius; col <= radius; col++)
	{
		int maxRow = static_cast<int>(sqrt((radius * radius) - (col * col))); // Use the circle formula to get the row
		for (int row = -maxRow; row <= maxRow; row++)
		{
			if ((t_col + col < 0 || t_col + col >= LEVEL_COLS) || (t_row + row - offset < 0 || t_row + row - offset >= LEVEL_ROWS)) // If either of the values are outside the world bounds
			{
				continue; // Continue and try the next point
			}
			else if (m_blocks[t_row + row - offset][t_col + col] == 0)
			{
				m_blocks[t_row + row - offset][t_col + col] = 7;
			}
		}
	}
}

void Game::updateWorld()
{
	for (int row = 0; row < LEVEL_ROWS; row++)
	{
		for (int col = 0; col < LEVEL_COLS; col++)
		{
			if (m_blocks[row][col] == 5)
			{
				updateBlock(row, col);
			}
		}
	}
}

void Game::updateBlock(int t_row, int t_col)
{
	if (t_col - 1 >= 0 && m_blocks[t_row][t_col - 1] == 0)
	{
		m_blocks[t_row][t_col - 1] = 5;
		updateBlock(t_row, t_col - 1);
	}
	if (t_col + 1 < LEVEL_COLS && m_blocks[t_row][t_col + 1] == 0)
	{
		m_blocks[t_row][t_col + 1] = 5;
		updateBlock(t_row, t_col + 1);
	}
	if (t_row + 1 < LEVEL_ROWS && m_blocks[t_row + 1][t_col] == 0)
	{
		m_blocks[t_row + 1][t_col] = 5;
		updateBlock(t_row + 1, t_col);
	}
}

void Game::drawWorld()
{
	int onScreenMin = static_cast<int>((m_currentView.getCenter().x - WINDOW_WIDTH / 2) / BLOCK_SIZE);
	int onScreenMax = static_cast<int>((m_currentView.getCenter().x + WINDOW_WIDTH / 2) / BLOCK_SIZE + 1.0f);

	// Increment the values if they're below zero to keep inside the array size
	for (; onScreenMin < 0; onScreenMin++);
	for (; onScreenMax < 0; onScreenMax++);

	for (int i = 0; i < LEVEL_ROWS; i++)
	{
		for (int j = onScreenMin; j < LEVEL_COLS && j < onScreenMax; j++)
		{
			if (m_blocks[i][j] != 0)
			{
				if (m_textures)
				{
					m_blockSprite.setPosition(static_cast<float>(j * BLOCK_SIZE), static_cast<float>(i * BLOCK_SIZE));
					m_blockSprite.setTextureRect(sf::IntRect{ m_blockSpritePositions[m_blocks[i][j]],{static_cast<int>(BLOCK_SIZE),static_cast<int>(BLOCK_SIZE)} });
					m_window.draw(m_blockSprite);
				}
				else
				{
					m_renderBlock.setPosition(static_cast<float>(j * BLOCK_SIZE), static_cast<float>(i * BLOCK_SIZE));
					m_renderBlock.setFillColor(m_blockColours[m_blocks[i][j]]);
					m_window.draw(m_renderBlock);
				}
			}
			else if (i > m_surfaceLevel[j])
			{
				if (m_textures)
				{
					m_blockSprite.setPosition(static_cast<float>(j * BLOCK_SIZE), static_cast<float>(i * BLOCK_SIZE));
					m_blockSprite.setTextureRect(sf::IntRect{ m_blockSpritePositions[3],{static_cast<int>(BLOCK_SIZE),static_cast<int>(BLOCK_SIZE)} });
					m_blockSprite.setColor(sf::Color{ 80, 80, 80, 255 });
					m_window.draw(m_blockSprite);
					m_blockSprite.setColor(sf::Color{ 255, 255, 255, 255 });
				}
				else
				{
					m_renderBlock.setPosition(static_cast<float>(j * BLOCK_SIZE), static_cast<float>(i * BLOCK_SIZE));
					m_renderBlock.setFillColor(sf::Color{ 40, 40, 40, 255 });
					m_window.draw(m_renderBlock);
				}
			}

			if (m_cavesVisible && m_caves[i][j]) // Draw the caves on screen
			{
				m_renderBlock.setPosition(static_cast<float>(j * BLOCK_SIZE), static_cast<float>(i * BLOCK_SIZE));
				m_renderBlock.setFillColor(sf::Color::Black);
				m_window.draw(m_renderBlock);
			}
		}
	}
}

void Game::setupPlayer()
{
	m_player.setSize({ 10.0f,10.0f });
	m_player.setPosition(500.0f, 0.0f);
}

bool Game::isColliding(sf::RectangleShape t_subjectOne, sf::Vector2f t_position)
{
	// Set the bool
	bool colliding = false;

	int collisionRange = 6;
	int targetRow = static_cast<int>(t_position.y / BLOCK_SIZE);
	int targetCol = static_cast<int>(t_position.x / BLOCK_SIZE);
	int startRow = targetRow - collisionRange;
	int startCol = targetCol - collisionRange;

	for (; startRow < 0 && startRow < targetRow; startRow++); // increment the start row if it's outside the bounds
	for (; startCol < 0 && startCol < targetCol; startCol++); // increment the start col if it's outside the bounds

	for (int row = startRow; row < LEVEL_ROWS && row < targetRow + collisionRange; row++)
	{
		for (int col = startCol; col < LEVEL_COLS && col < targetCol + collisionRange; col++)
		{
			if (m_blocks[row][col] != 0 && m_blocks[row][col] != 5 && m_blocks[row][col] != 6 && m_blocks[row][col] != 7 && m_blocks[row][col] != 8)
			{
				sf::Vector2f blockPosition{ static_cast<float>(col) * BLOCK_SIZE, static_cast<float>(row) * BLOCK_SIZE };

				if (t_position.x + t_subjectOne.getSize().x > blockPosition.x && t_position.x < blockPosition.x + BLOCK_SIZE)
				{
					if (t_position.y + t_subjectOne.getSize().y > blockPosition.y && t_position.y < blockPosition.y + BLOCK_SIZE)
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

/// <summary>
/// Checks each corner of the inputted rectangle to see if it's within a water block.
/// </summary>
/// <param name="t_subject">Rectangle shape to check</param>
/// <returns>Whether the shape is touching water or not</returns>
bool Game::isInWater(sf::RectangleShape t_subject)
{
	bool touchingWater = false;

	// Gets the block index at each corner of the inputted rectangle
	sf::Vector2f cornerBlocks[4]{
		{ t_subject.getPosition() / BLOCK_SIZE },
		{ sf::Vector2f{ t_subject.getPosition().x + t_subject.getSize().x, t_subject.getPosition().y } / BLOCK_SIZE },
		{ sf::Vector2f{ t_subject.getPosition().x, t_subject.getPosition().y + t_subject.getSize().y } / BLOCK_SIZE },
		{ sf::Vector2f{ t_subject.getPosition().x + t_subject.getSize().x, t_subject.getPosition().y + t_subject.getSize().y } / BLOCK_SIZE }
	};

	for (int i = 0; i < 4; i++) // Loop for each corner of the player
	{
		if (cornerBlocks[i].x > 0 && cornerBlocks[i].x < LEVEL_COLS) // check the x index is within the world range
		{
			if (cornerBlocks[i].y > 0 && cornerBlocks[i].y < LEVEL_ROWS) // check the y index is within the world range
			{
				if (m_blocks[static_cast<unsigned>(cornerBlocks[i].y)][static_cast<unsigned>(cornerBlocks[i].x)] == 5) // Check if the block is water
				{
					touchingWater = true;
					break; // doesn't need to check any more corners, is touching water already
				}
			}
		}
	}

	return touchingWater;
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
