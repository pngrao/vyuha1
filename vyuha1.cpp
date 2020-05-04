#include<SFML/Graphics.hpp>
#include<iostream>
#include<list>
using namespace std;
using namespace sf;

//Window size set using only modes supporting fullscreeen
unsigned int xWin = 1280, yWin = 600;
unsigned int FRLimit = 60;
string playerAnim = "pirate_sprite_sheet.png";
string wallAnim = "wall1.png";
string keyAnim = "key.png";
string iconImage = "pirateicon64.png";

class Character {
public:
	Sprite sprite;
	Texture texture;
	int left, top, width, height;
	int frameCount;
	Character():left(0), top(0), width(0), height(0), frameCount(1) {}
	Character(Texture& t, string filename, int x=0, int y=0, int w=0, int h=0, int f=1):left(x), top(y), width(w), height(h), frameCount(f)
	{
		texture = t;
		texture.loadFromFile(filename);
		texture.setSmooth(true);
		changePosture(x, y, w, h, f);
	}
	//TODO overload changePosture to accept vector<vector<int>>
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
		//TODO change sprite dimension- right turn showing gap
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
				a->sprite.setPosition(aBounds.left, bBounds.top + aBounds.height);
			}
		}
	}
	//Add a method to change frames later
};

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
	
	//Texture & Sprite
	Texture pTexture, wTexture, kTexture;

	Character player(pTexture, playerAnim, 0, 0, 60, 89);
	player.sprite.setPosition(0.f, float(yWin - 89));
	vector<vector<int>>playerPostures{ /*{60, 0, 60, 89, 1}, {120, 0, 60, 89, 1},\ */ //right and left changed
									   {60, 0, 45, 89, 1}, {130, 0, 45, 89, 1},\
									   {180, 0, 60, 89, 1}, {0, 0, 60, 89, 1},\
									   {120, 90, 60, 89, 1}, {60, 90, 60, 89, 1},\
									   {180, 90, 60, 89, 1}, { 0, 90, 60, 89, 1} };
	int posture = 3;

	list<Character>walls;
	/*wallCoverage matrix columns : width, height, xPos, level*/
	vector<vector<float>>wallCoverage{ {616.f,85.f,0.f,1.f}, {616.f,85.f,700.f,1.f}, \
									   {308.f,85.f,0.f,2.f}, {616.f,85.f,400.f,2.f}, {200.f,85.f,1100.f,2.f}, \
									   {616.f,85.f,0.f,3.f}, {480.f,85.f,610.f,3.f} };
	for (unsigned int i = 0; i < wallCoverage.size(); i++)
	{
		Character block(wTexture, wallAnim, 0, 0, int(wallCoverage[i][0]), int(wallCoverage[i][1]));
		block.sprite.setPosition(float(wallCoverage[i][2]), \
								 float(yWin - (85+89)*wallCoverage[i][3] - (15.f*wallCoverage[i][3])));
		walls.push_back(block);
	}
	
	Character key(kTexture, keyAnim, 0, 0, 105, 172);
	key.sprite.setPosition(float(xWin) - 105.f, 0.f);

	//Event
	Event event;
	//Move
	float moveUnitMin, moveUnitMax;
	moveUnitMin = -3.f, moveUnitMax = 3.f;

	//Game loop
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		Vector2f velocity(0.f, 0.f);
		//Movement controls left, right, up and down arrow keys
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			velocity.x = moveUnitMax;
			posture = 0;
		}
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			velocity.x = moveUnitMin;
			posture = 1;
			
		}
		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			velocity.y = moveUnitMin;
			posture = 2;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			velocity.y = moveUnitMax;
			posture = 3;
		}
		//Weapon display controls A, D, W and S letter keys
		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			posture = 4;
		}
		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			posture = 5;
		}
		if (Keyboard::isKeyPressed(Keyboard::W))
		{
			posture = 6;
		}
		if (Keyboard::isKeyPressed(Keyboard::S))
		{
			posture = 7;
		}

		//Update
		player.sprite.move(velocity.x, velocity.y);
		player.changePosture(playerPostures[posture][0], playerPostures[posture][1], \
							 playerPostures[posture][2], playerPostures[posture][3], \
							 playerPostures[posture][4]);
		//Boundary collision detection
		player.checkBoundaryCollision(windowBounds);
		//Player to wall collision detection
		for (auto wall : walls)
		{
			player.checkObjectCollision(&player, &wall, velocity);
		}
		//Player to key collision detection
		player.checkObjectCollision(&player, &key, velocity);

		//Display all
		window.clear(Color(30,30,40,255));//Night shade(30,30,40)
		//window.clear(Color(20, 20, 20, 255));//Dark Grey(20,20,20)
		window.draw(player.sprite);
		for (auto wall: walls)
			window.draw(wall.sprite);
		window.draw(key.sprite);
		window.display();
	}
	return 0;
}