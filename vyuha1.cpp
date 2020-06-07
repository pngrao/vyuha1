#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<iostream>
#include<list>
using namespace std;
using namespace sf;

//Window size set using only modes supporting fullscreeen
unsigned int xWin = 1280, yWin = 600;
//Framerate setting
unsigned int FRLimit = 60;
//All media files
string playerAnim = "resources/images/pirate_motion.png";
string wallAnim = "resources/images/wall.png";
string keyAnim = "resources/images/gazebo_key.png";
string iconImage = "resources/images/pirateicon64.png";
string enemyAnim = "resources/images/cannonball.png";
string fontFile = "resources/fonts/clarenbd.ttf";
string bgmusicFile = "resources/music/bgmusic.ogg";

class Character {
public:
	Sprite sprite;
	Texture texture;
	int left, top, width, height;
	int frameCount;
	string name;
	bool notCollided = true;
	int score = 0;

	//cannonball, code to be moved to enemy class
	int step=1;
	float speed = 1.f;
	int interval = 10;
	int startFrame = 40;

	Character():left(0), top(0), width(0), height(0), frameCount(1) {}
	Character(string filename, string n, int x=0, int y=0, int w=0, int h=0, int f=1):\
	                                     left(x), top(y), width(w), height(h), frameCount(f)
	{
		name = n;
		texture.loadFromFile(filename);
		texture.setSmooth(true);
		changePosture(x, y, w, h, f);
	}
	
	void changePosture(int x, int y, int w, int h, int f)
	{
		std::vector<IntRect> animFrames;
		int i = 0;
		while (i < f)
		{
			animFrames.push_back(IntRect(x + (w * i), y, w, h));
			i++;
		}
		sprite.setTexture(texture);
		sprite.setTextureRect(animFrames[0]);
	}

	void checkBoundaryCollision(sf::FloatRect bounds)
	{
		//Collision detection - Player vs Window boundary line(2D obj vs 1D obj)
		//Adapt player position to be within window boundary
		Vector2f position = sprite.getPosition();
		position.x = std::max(position.x, bounds.left);
		position.y = std::max(position.y, bounds.top);
		sprite.setPosition(position);
		position.x = std::min(position.x, bounds.width - width);
		position.y = std::min(position.y, bounds.height - height);
		sprite.setPosition(position);
	}

	void checkObjectCollision(Character* a, Character* b, Vector2f &v)
	{
		//Collision detection - Player vs Enemy/Wall/Key(2D obj vs 2D obj)
		FloatRect aBounds = a->sprite.getGlobalBounds();
		FloatRect bBounds = b->sprite.getGlobalBounds();
		aBounds.left += v.x;
		aBounds.top += v.y;
		if (aBounds.intersects(bBounds))
		{
			//right wall
			if (aBounds.left < bBounds.left &&
				aBounds.left + aBounds.width < bBounds.left + bBounds.width &&
				aBounds.top < bBounds.top + bBounds.height &&
				aBounds.top + aBounds.height > bBounds.top)
			{
				a->sprite.setPosition(bBounds.left - aBounds.width, aBounds.top);
			}
			//left wall
			else if (aBounds.left > bBounds.left &&
				aBounds.left + aBounds.width > bBounds.left + bBounds.width &&
				aBounds.top < bBounds.top + bBounds.height &&
				aBounds.top + aBounds.height > bBounds.top)
			{
				a->sprite.setPosition(bBounds.left + bBounds.width, aBounds.top);
			}
			//bottom wall
			else if (aBounds.top < bBounds.top &&
				aBounds.top + aBounds.height < bBounds.top + bBounds.height &&
				aBounds.left < bBounds.left + bBounds.width &&
				aBounds.left + aBounds.width > bBounds.left)
			{
				a->sprite.setPosition(aBounds.left, bBounds.top - aBounds.height);
			}
			//top wall
			else if (aBounds.top > bBounds.top &&
				aBounds.top + aBounds.height > bBounds.top + bBounds.height &&
				aBounds.left < bBounds.left + bBounds.width &&
				aBounds.left + aBounds.width > bBounds.left)
			{
				a->sprite.setPosition(aBounds.left, bBounds.top + aBounds.height - 40.f);
			}
			if (b->name == "cannonball")
			{
				if (b->notCollided)
				{
					a->score -= 1000;
					cout << "Debug: Oops!" << endl;
				}
				b->notCollided = false;
			}
		}
	}

	void movementSetting(float s, int i, int sf)
	{
		step = 1;
		speed = s;
		interval = i;
		startFrame = sf;
	}
};

void setup_score(Text* Score)
{
	Score->setFillColor(Color(200, 90, 90, 255));
	Score->setOutlineColor(Color::Black);
	Score->setOutlineThickness(1.f);
	Score->setLetterSpacing(1.f);
	Score->setStyle(Text::Italic);
}

void setup_bgmusic(Music* music, string file)
{
	music->openFromFile(file);
	music->setLoop(true);
	music->setPitch(float(1.05));
	music->setVolume(20);
}

int main()
{
	//Window
	RenderWindow window(VideoMode(xWin, yWin), "Vyuha-Level 1", Style::Default);
	window.setFramerateLimit(FRLimit);
	//Get window boundary, getSize returns only float so convert the rest
	FloatRect windowBounds(Vector2f(0.f, 0.f), window.getDefaultView().getSize());

	//Set Icon
	Image icon;
	icon.loadFromFile(iconImage);
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	
	//Create game entities
	//Player
	Character player(playerAnim, "player", 0, 0, 55, 90);
	player.sprite.setPosition(0.f, float(yWin - 90));
	vector<vector<int>>playerPostures{ {0, 0, 60, 90, 1}, {60, 0, 55, 90, 1}, {225, 0, 55, 90, 1} };
	int posture = 0;

	//Key: Gazebo
	Character key(keyAnim, "key", 0, 0, 110, 140);
	key.sprite.setPosition(float(xWin) - 110.f, 0.f);

	//Wall: Build Town structures
	list<Character>walls;
	vector<vector<float>>wallCoverage{ \
	                    {44.f, 0.f, 600.f, 50.f,-5.f, 1.f}, {534.f, 0.f, 616.f, 50.f,700.f,1.f }, \
	                    {644.f, 0.f, 308.f,50.f,-5.f,2.f}, {309.f, 0.f, 616.f,50.f,400.f,2.f}, {670.f, 0.f, 200.f,50.f,1100.f,2.f}, \
	                    {100.f, 0.f, 1100.f,50.f,-5.f, 3.f} };
	for (unsigned int i = 0; i < wallCoverage.size(); i++)
	{
		Character block(wallAnim, "wall", int(wallCoverage[i][0]), int(wallCoverage[i][1]),\
		                int(wallCoverage[i][2]), int(wallCoverage[i][3]));
		block.sprite.setPosition(float(wallCoverage[i][4]), \
		                         float(yWin - (50 + 90) * wallCoverage[i][5] - (50.f * wallCoverage[i][5])));
		walls.push_back(block);
	}

	//Enemies : Cannonball
	list<Character> enemies;
	//Initializer: x position, y position, speed, interval, starting frame
	vector<vector<float>> enemyStartPosition{ {float(xWin - 40), float(yWin - 40), -2.f, 10.f, 320.f},\
	                                          {0.f, float(yWin - 40), 1.5, 15.f, 40.f}, \
	                                          {float(xWin - 40), float(yWin - 40), -0.75, 20.f, 320.f}, \
	                                          {float(xWin + 300 - 40), float(yWin - 90), -2.f, 10.f, 320.f}, \
	                                          {-300.f, float(yWin - 90), 1.5, 15.f, 40.f}, \
	                                          {float(xWin+500 - 40), float(yWin - 90), -0.55f, 20.f, 320.f} };
	for (unsigned int i = 0; i < enemyStartPosition.size(); i++)
	{
		Character cannonball(enemyAnim, "cannonball", 0, 0, 40, 40);
		cannonball.sprite.setPosition(float(enemyStartPosition[i][0]), float(enemyStartPosition[i][1] - i%3*(90 + 100)));
		cannonball.movementSetting(enemyStartPosition[i][2], int(enemyStartPosition[i][3]), int(enemyStartPosition[i][4]));
		enemies.push_back(cannonball);
	}

	//Event
	Event event;

	//Move
	float moveUnitMin, moveUnitMax;
	moveUnitMin = -2, moveUnitMax = 2;

	//Set Score
	Font font;
	font.loadFromFile(fontFile);
	string scoretext = "  SCORE: ";
	string lifetext = "  LIFE: 1";
	string scoreboard = lifetext + scoretext + to_string(player.score);
	Text Score(scoreboard, font, 20);
	setup_score(&Score);

	//Stream music
	Music music;
	setup_bgmusic(&music, bgmusicFile);
	music.play();

	//Game loop
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				cout << "Score: " << player.score;
				window.close();
			}
		}
		Vector2f velocity(0.f, 0.f);
		//Movement controls left, right, up and down arrow keys
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			velocity.x = moveUnitMax;
			posture = 1;
		}
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			velocity.x = moveUnitMin;
			posture = 2;
		}
		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			velocity.y = moveUnitMin;
			if (posture == 1)
				posture = 1;
			else if (posture == 2)
				posture = 2;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			velocity.y = moveUnitMax;
			if (posture == 1)
				posture = 1;
			else if (posture == 2)
				posture = 2;
		}
		
		//Update player movement and posture
		player.sprite.move(velocity.x, velocity.y);
		player.changePosture(playerPostures[posture][0], playerPostures[posture][1], \
		                     playerPostures[posture][2], playerPostures[posture][3], \
		                     playerPostures[posture][4]);
		
		//Update Score
		player.score += int(abs(velocity.x) + abs(velocity.y));

		//Boundary collision detection
		player.checkBoundaryCollision(windowBounds);

		//Player to wall collision detection
		for (auto wall : walls)
		{
			player.checkObjectCollision(&player, &wall, velocity);
		}

		//Player to key collision detection
		player.checkObjectCollision(&player, &key, velocity);

		//Enemy cannonball movement and collision detection
		for (auto &enemy : enemies)
		{
			if (enemy.notCollided)
			{
				enemy.sprite.move(enemy.speed, 0.f);
				if (int(enemy.sprite.getGlobalBounds().left) % enemy.interval == 0)
				{
					if (enemy.step < 9)
					{
						enemy.changePosture(abs(enemy.startFrame - 40 * enemy.step), 0, 40, 40, 1);
						enemy.step++;
					}
				}
				if (enemy.step == 9)
					enemy.step = 1;
			}
			player.checkObjectCollision(&player, &enemy, velocity);
		}

		//Update score
		scoreboard = lifetext + scoretext + to_string(player.score);
		Score.setString(scoreboard);

		//Display all
		window.clear(Color(30, 30, 40,255));
		window.draw(player.sprite);
		for(auto wall: walls)
			window.draw(wall.sprite);
		window.draw(key.sprite);
		for (auto enemy : enemies)
			window.draw(enemy.sprite);
		window.draw(Score);
		window.display();
	}
	return 0;
}