
//==========================MODE VARS==========================

// Sneks
byte currentColor = 0;

class Snek
{
public:
	uint8_t fieldWidth;
	uint8_t fieldHeight;

	Direction lastDir;
	uint8_t sameDirCount;

	int8_t curX;
	int8_t curY;
	uint8_t prevX;
	uint8_t prevY;

	CRGB color;

	Snek(CRGB c, uint8_t width = LED_WIDTH, uint8_t height = LED_HEIGHT)
	{
		color = c;
		lastDir = (Direction)random8(0, 3);
		prevX = curX = random16(0, width);
		prevY = curY = random16(0, height);
		sameDirCount = 0;
		fieldWidth = width;
		fieldHeight = height;
	};

	// Move with specified direction, constrained by howStraight
	void move(uint8_t howStraight,
		bool blockBacktrack = true,
		Direction dir = (Direction)(random16(0, 4)))
	{
		// We've gone straight for the number of specified steps already
		if (sameDirCount >= howStraight)
		{
			// Reset counter if we are in a different direction
			if (dir != lastDir)
			{
				// Modify direction to block backtracking first
				while (blockBacktrack)
				{
					switch (dir)
					{
					case UP:
						if (lastDir == DOWN)
							dir = (Direction)(random16(0, 4));
						else
							blockBacktrack = false;
						break;
					case RIGHT:
						if (lastDir == LEFT)
							dir = (Direction)(random16(0, 4));
						else
							blockBacktrack = false;
						break;
					case DOWN:
						if (lastDir == UP)
							dir = (Direction)(random16(0, 4));
						else
							blockBacktrack = false;
						break;
					case LEFT:
						if (lastDir == RIGHT)
							dir = (Direction)(random16(0, 4));
						else
							blockBacktrack = false;
						break;
					default:
						Serial.println("ERROR: invalid snek block backtrack direction request");
						break;
					}
				}
				// Leave new dir unmodified, save lastDir
				lastDir = dir;
				sameDirCount = 0;
			}
		}
		else
		{
			// Continue going straight, override input direction
			dir = lastDir;
			sameDirCount++;
		}

		processNextCoordinate(dir);
	};

	// Calculate & set new coord
	void processNextCoordinate(Direction dir, bool isWraparound = true)
	{
		// save old coords
		prevX = curX;
		prevY = curY;

		switch (dir)
		{
		case UP:
			curY++;
			break;
		case RIGHT:
			curX++;
			break;
		case DOWN:
			curY--;
			break;
		case LEFT:
			curX--;
			break;
		default:
			Serial.println("ERROR: invalid snek direction request");
			break;
		}

		// Handle wraparound
		if (isWraparound)
		{
			if (curY > fieldHeight)
			{
				curY = 0;
			}
			else if (curY < 0)
			{
				curY = fieldHeight-1;
			}
			else if (curX > fieldWidth)
			{
				curX = 0;
			}
			else if (curX < 0)
			{
				curX = fieldWidth-1;
			}
		}
		else
		{
			Serial.println("ERROR: non wraparound snek case unhandled");
		}
	};
};
