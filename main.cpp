#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>



struct Config {
	sf::Vector2u WinSize = { 600, 600 };
	float ZoomingStep = 1.12;

	uint32_t RecursionDepth = 9;
	float RecursionStep = 2;

	uint32_t BrightLimit = 255;
	float BrightStep = 0.8;

	uint32_t MeshColor = 0x00ffbaff;
	bool DrawingFrstMesh = false;

	bool VisualModeOn = false;

} config;



// находим среднюю €ркость
void func(sf::Image& img, uint32_t _x, uint32_t _y, uint32_t _step, uint8_t _lim) {
	if (_step == 0) return;

	uint32_t bright = 0;
	for (size_t y = _y; y < std::min(img.getSize().y, _y + _step); y++) {
		for (size_t x = _x; x < std::min(img.getSize().x, _x + _step); x++) {
			bright += (img.getPixel(x, y).r + img.getPixel(x, y).g + img.getPixel(x, y).b) / 3;
		}
	}

	bright /= _step * _step;

	if (bright > _lim) {
		for (size_t y = _y; y < std::min(img.getSize().y, _y + _step); y++) {
			for (size_t x = _x; x < std::min(img.getSize().x, _x + _step); x++) {
				if (y % _step == 0 or x % _step == 0) {
					img.setPixel(x, y, sf::Color(config.MeshColor));
				}
				else img.setPixel(x, y, sf::Color(bright, bright, bright));
			}
		}
	}
	else {
		uint32_t step = _step / config.RecursionStep;
		uint8_t lim = _lim * config.BrightStep;

		for (size_t y = 0; y < config.RecursionStep; y++) {
			for (size_t x = 0; x < config.RecursionStep; x++) {
				func(img, _x + x * step, _y + y * step, step, lim);
			}
		}
	}
}


int main()
{
	sf::Image img;
	img.loadFromFile("test.jpg");
	sf::Texture texr;
	texr.loadFromImage(img);
	sf::Sprite sprite(texr);

	auto imgSize = img.getSize();
	uint32_t step = pow(config.RecursionStep, config.RecursionDepth);


	// чернобелый канал
	for (size_t y = 0; y < imgSize.y; y++) {
		for (size_t x = 0; x < imgSize.x; x++) {
			sf::Uint8 clr = (
				img.getPixel(x, y).r +
				img.getPixel(x, y).g +
				img.getPixel(x, y).b) / 3;
			

			if ((y % step == 0 or x % step == 0) and config.DrawingFrstMesh) {
				img.setPixel(x, y, sf::Color(config.MeshColor));
			}
			else img.setPixel(x, y, sf::Color(clr, clr, clr));
		}
	}

	

	while (true)
	{
		// запускаем алгоритм 
		static size_t y = 0;
		static size_t x = 0;

		if (x < imgSize.x) {
			func(img, x, y, step, config.BrightLimit);
			texr.loadFromImage(img);
			sprite.setTexture(texr);
			x += step;
		}
		else if (y + step < imgSize.y) {
			x = 0;
			y += step;

		}
		else {
			static bool save = true;
			if (save) {
				save = false;
				img.saveToFile("result.png");
				if (not config.VisualModeOn) return 0;
			}
		}
		std::cout << ((double)x * y) / (imgSize.x * imgSize.y) << std::endl;

		if (config.VisualModeOn) {
			static sf::RenderWindow window(sf::VideoMode(config.WinSize.x, config.WinSize.y, 32), "дурка");
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) {
					window.close();
					return 0;
				}

				if (event.type == sf::Event::Resized) {
					sf::Vector2f scale(
						(float)config.WinSize.x / ((double)event.size.width * sprite.getScale().x),
						(float)config.WinSize.y / ((double)event.size.height * sprite.getScale().y));
					sprite.setScale(scale);
				}

				if (event.type == sf::Event::MouseWheelMoved) {
					static float scale = 1.0;
					scale += event.mouseWheel.delta;
					sprite.setScale(
						pow(config.ZoomingStep, scale) * config.WinSize.y / window.getSize().x,
						pow(config.ZoomingStep, scale) * config.WinSize.y / window.getSize().y);
				}
			}

			window.clear();
			window.draw(sprite);
			window.display();
		}
	}

	
	return 0;
}