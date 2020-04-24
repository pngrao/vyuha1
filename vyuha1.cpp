#include<SFML/Graphics.hpp>
using namespace std;
using namespace sf;

//Window size
unsigned int xWin = 1000, yWin = 600;
unsigned int FRLimit = 60;
string playerAnim = "pirate_sprite_sheet.png";
string treasureAnim = "treasurefront.png";

class Character {
public:
	sf::Sprite sprite;
	sf::Texture texture;
	int left, top, width, height;
	Character():left(0), top(0), width(0), height(0) {}
	Character(sf::Texture& t, string filename, int x=0, int y=0, int w=0, int h=0):left(x), top(y), width(w), height(h)
	{
		texture = t;
		texture.loadFromFile(filename);
		texture.setSmooth(true);
		changePosture(x, y, w, h);
	}
	void changePosture(int x, int y, int w, int h)
	{
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(x, y, w, h));
	}
	void checkBoundaryCollision(sf::FloatRect bounds)
	{
		//Collision detection - Player vs Window boundary line(2D obj vs 1D obj)
		//Adapt player position to be within window boundary
		sf::Vector2f position = sprite.getPosition();
		position.x = std::max(position.x, bounds.left);
		position.y = std::max(position.y, bounds.top);
		sprite.setPosition(position);
		position.x = std::min(position.x, bounds.width - width);
		position.y = std::min(position.y, bounds.height - height);
		sprite.setPosition(position);
	}
};
int main()
{
	//Window
	sf::RenderWindow window(sf::VideoMode(xWin, yWin), "Vyuha-Level 1", sf::Style::Close);
	window.setFramerateLimit(FRLimit);
	//Get window boundary, getSize returns only float so convert the rest
	sf::FloatRect windowBounds(sf::Vector2f(0.f, 0.f), window.getDefaultView().getSize());
	//Texture & Sprite
	sf::Texture pTexture, tTexture;
	Character player(pTexture, playerAnim, 0, 0, 60, 89);
	player.sprite.setPosition(float(xWin / 2 - 60 / 2), float(yWin - 89));
	Character treasure(tTexture, treasureAnim, 0, 0, 198, 150);
	treasure.sprite.setPosition(float(xWin / 2 -198/2), float(yWin / 2 - 150 / 2));
	//Event
	sf::Event event;
	//Move
	float moveUnitMin, moveUnitMax;
	moveUnitMin = -8, moveUnitMax = 8;

	//Game loop
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				//Movement controls left, right, up and down arrow keys
				if (event.key.code == sf::Keyboard::Right)
				{
					player.sprite.move(moveUnitMax, 0);
					player.changePosture(60, 0, 60, 89);
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					player.sprite.move(moveUnitMin, 0);
					player.changePosture(120, 0, 60, 89);
				}
				if (event.key.code == sf::Keyboard::Up)
				{
					player.sprite.move(0, moveUnitMin);
					player.changePosture(180, 0, 60, 89);
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					player.sprite.move(0, moveUnitMax);
					player.changePosture(0, 0, 60, 89);
				}
				//Weapon display controls A, D, W and S letter keys
				if (event.key.code == sf::Keyboard::A)
				{
					player.changePosture(120, 90, 60, 89);
				}
				if (event.key.code == sf::Keyboard::D)
				{
					player.changePosture(60, 90, 60, 89);
				}
				if (event.key.code == sf::Keyboard::W)
				{
					player.changePosture(180, 90, 60, 89);
				}
				if (event.key.code == sf::Keyboard::S)
				{
					player.changePosture(0, 90, 60, 89);
				}
			}
		}
		//Collision detection
		player.checkBoundaryCollision(windowBounds);
		//Display all
		window.clear(sf::Color::White);
		window.draw(player.sprite);
		window.draw(treasure.sprite);
		window.display();
	}
	return 0;
}