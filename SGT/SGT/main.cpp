#include "Valmac.h"
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


#include <iostream>
#include <ctime>

using namespace std;


void DrawPixel(int x, int y);

void keyboardpresseddown(uint8_t& key);

void keyboardpressedup(uint8_t& key);

void updategraphics(uint8_t(&gfx)[]);

