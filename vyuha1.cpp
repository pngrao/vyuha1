//#include<iostream>
//#include<cstdlib>
#include<SFML/Graphics.hpp>
//#include<SFML/Window.hpp>

using namespace std;
using namespace sf;

const unsigned int v1MaxSqPerSide = 5;

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
		//cout << "1. House plot allotted! Now, building...\n";
	}
	Border() { /*cout << "2. House plot allotted! Now, building...\n"; */ }
	~Border() { /*cout << "House uninitialized!\n"; */ }

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
	unsigned int xWin = 800, yWin = 600; //540,540
	int v1SquareSize = 5;
	float playerPositionInitX = (xWin / 2);
	float playerPositionInitY = yWin - (yWin / v1SquareSize);
	sf::RenderWindow window(sf::VideoMode(xWin,yWin), "Welcome to Vyuha 1");
	
	//Texture and Sprite creation
	sf::Texture playerTexture;
	playerTexture.loadFromFile("p1.png");
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
				if (event.key.code == sf::Keyboard::Up) player.move(0, moveUnitMin);
				if (event.key.code == sf::Keyboard::Right) player.move(moveUnitMax, 0);
				if (event.key.code == sf::Keyboard::Down) player.move(0, moveUnitMax);
				if (event.key.code == sf::Keyboard::Left) player.move(moveUnitMin, 0);

			}
		}
		window.clear(sf::Color::White);
		window.draw(b1.fence);

		// adapt sprite position to be within window boundary
		//sf::FloatRect windowBounds(sf::Vector2f(0.f, 0.f), window.getDefaultView().getSize());
		sf::Vector2f position = player.getPosition();
		position.x = std::max(position.x, windowBounds.left+20);
		position.y = std::max(position.y, windowBounds.left+20);
		player.setPosition(position);
		//FIX this code
		//position.x = std::min(position.x, windowBounds.left + windowBounds.width - window.getSize().x);
		//position.x = std::min(position.x, windowBounds.left + windowBounds.width - window.getSize().x);
		position.x = std::min(position.x, 730.f);
		position.y = std::min(position.y, 480.f);
		player.setPosition(position);

		window.draw(player);
		window.display();
	}
	return 0;
}