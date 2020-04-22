//#include<iostream>
//#include<cstdlib>
#include<SFML/Graphics.hpp>
//#include<SFML/Window.hpp>

using namespace std;
using namespace sf;

//const unsigned int v1MaxSqPerSide = 5;

//To be used when setting states
/*
enum HouseFeature {
	Regular,
	Home,
	SafeHouse,
	Palace
}feature;
*/

class Border : public sf::RectangleShape {
	float x, y;
public:
	
	Border(float a, float b)
	{
		x = a;
		y = b;
	}
	Border() {}
	~Border() {}

	sf::RectangleShape fence;
	void BuildFence()
	{
		fence.setSize(sf::Vector2f(x, y));
		fence.setOutlineColor(sf::Color::Black);
		fence.setOutlineThickness(1);
		fence.setPosition(10,10);
	}

};

int main()
{
	//Window creation
	unsigned int xWin = 800, yWin = 600;
	int v1SquareSize = 5;
	float playerPositionInitX = (xWin / 2);
	float playerPositionInitY = yWin - (yWin / v1SquareSize);
	sf::RenderWindow window(sf::VideoMode(xWin,yWin), "Vyuha, Chapter 1");
	
	//Texture and Sprite creation
	//sf::Texture terrainTexture;
	sf::Texture playerTexture;

	//terrainTexture.loadFromFile("cave1.jpg");
	//sf::Sprite background(terrainTexture);

	playerTexture.loadFromFile("piratefront.png");
	sf::Sprite player;
	player.setTexture(playerTexture);
	player.setPosition(playerPositionInitX, playerPositionInitY);

	//Event
	sf::Event event;

	//Move
	float moveUnitMin, moveUnitMax;
	moveUnitMin = -24, moveUnitMax = 24;

	//Border
	unsigned int xBorder = xWin - 20, yBorder = yWin - 20;
	Border b1(xBorder, yBorder);
	b1.BuildFence();
	
	// adapt sprite position to be within window boundary
	sf::FloatRect windowBounds(sf::Vector2f(0.f, 0.f), window.getDefaultView().getSize());

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Up)
				{
					player.move(0, moveUnitMin);
					playerTexture.loadFromFile("pirateback.png");
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					player.move(moveUnitMax, 0);
					playerTexture.loadFromFile("pirateright.png");
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					player.move(0, moveUnitMax);
					playerTexture.loadFromFile("piratefront.png");
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					player.move(moveUnitMin, 0);
					playerTexture.loadFromFile("pirateleft.png");
				}
			}
		}
		window.clear(sf::Color::White);
		window.draw(b1.fence);

		// adapt sprite position to be within window boundary
		sf::Vector2f position = player.getPosition();
		//Freeze left and top walls
		position.x = std::max(position.x, windowBounds.left+10);
		position.y = std::max(position.y, windowBounds.top+10);
		player.setPosition(position);
		//Freeze right and bottom walls
		position.x = std::min(position.x, windowBounds.left + windowBounds.width - (playerTexture.getSize().x+10));
		position.y = std::min(position.y, windowBounds.top + windowBounds.height - (playerTexture.getSize().y+10));
		player.setPosition(position);

		//window.draw(background);
		window.draw(player);
		window.display();
	}
	return 0;
}