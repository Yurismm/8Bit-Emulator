// Valmac8.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Emulation execution is separate, mostly controller by PC.
//
// NB students may be more familiar with uint16_t than unsigned short
// similarly   may be more familiar with uint8_t  than unsigned char
//
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Valmac.h"


#include <iostream>
#include <ctime>

using namespace std;


void DrawPixel(int x, int y);

void keyboardpresseddown(uint8_t& key);

void keyboardpressedup(uint8_t& key);


// will remove structure to make it a class, so that the headerfile is easier to impliment within the framework, eventually


void Valmac::emulateCycle()		//run c clock tick
{
	// Fetch Opcode

	opcode = get_program_opcode();

	uint16_t NNN = opcode & 0x0FFF;

	uint16_t  NN = opcode & 0x00FF;

	uint16_t   N = opcode & 0x000F;

	uint16_t   X = (opcode & 0x0F00) >> 8;

	uint16_t   Y = (opcode & 0x00F0) >> 4;

	std::cout << "PC: " << hex << PC << " OPCODE: " << (unsigned)opcode << endl;

	// pc step / increment

	step_PC();

	// Decode & Execute Opcode

	switch (opcode & 0xF000)
	{
	case 0x0000:
	{
		switch (opcode)
		{
		case 0x0000:

			PC += 2;

			cout << "Spacer" << endl;

			break;


		case 0x00E0:
			// Clr screen

			cout << "Clear screen" << endl;
			PC += 2;
			break;
		case 0x00EE:

			cout << "Returns from a subroutine" << endl;
			PC = stack[SP - 1] + 2;

			SP--;

			stack[SP] = 0;
			break;
		case 0x00FE:

			cout << "Exitting program." << endl;
			isRunning = false;
			break;

		default:

			PC = NNN;
			break;
		}

	}
	break;


	case 0x1000:

		cout << "Jumping to address: " << NNN << endl;
		PC = NNN;
		break;

	case 0x2000:

		cout << "Calling subrouting at " << NNN << endl;
		stack[SP] = PC;

		SP++;

		PC = NNN;
		break;
	case 0x3000:

		if (V[X] == NN)
		{
			PC += 4;
			cout << "Skipping next instruction" << endl;
		}

		else
		{
			cout << "Not skipping next intruction" << endl;
			PC += 2;

		}
		break;
	case 0x4000:

		if (V[X] != NN)
		{
			cout << "Skipping next instruction" << endl;
			PC += 4;
		}

		else
		{
			PC += 2;
		}
		break;
	case 0x5000:

		if (V[X] == V[Y])
		{
			cout << "Skipping next instruction" << endl;
			PC += 4;
		}

		else
		{
			PC += 2;
		}
		break;
	case 0x6000:
		cout << "Setting: " << V[X] << " to " << NN << endl;

		V[X] = NN;

		PC += 2;
		break;
	case 0x7000:

		cout << "Adding: " << V[X] << " to " << NN << endl;
		V[X] += NN;

		PC += 2;

		break;
	case 0x8000:
	{
		switch (opcode & 0x000F)
		{

		case 0x0000:

			cout << "Setting: " << V[X] << " to " << V[Y] << endl;

			V[X] = V[Y];

			PC += 2;
			break;
		case 0x0001:
			cout << "Setting: " << V[X] << " to " << V[Y] << " OR " << V[X] << endl;
			V[X] = V[X] | V[Y];

			PC += 2;
			break;
		case 0x0002:
			cout << "Setting: " << V[X] << " to " << V[Y] << " AND " << V[X] << endl;
			V[X] = V[X] & V[Y];

			PC += 2;
			break;
		case 0x0003:
			cout << "Setting: " << V[X] << " to " << V[Y] << "XOR" << V[X] << endl;
			V[X] = V[X] ^ V[Y];

			PC += 2;
			break;
		case 0x0004:

			cout << "Adding: " << V[X] << " to " << V[Y] << endl;

			if (V[X] > 0xFF - V[Y]) // check for carry
			{
				cout << "Carry" << endl;
				V[0xf] = 1;
			}

			else
			{
				cout << "No carry" << endl;
				V[0xf] = 0;
			}

			V[X] = V[X] + V[Y];

			PC += 2;
			break;
		case 0x0005:
			cout << "Subtracting: " << V[X] << " to " << V[Y] << endl;

			if (V[X] > 0xFF - V[Y]) // checks for carry
			{
				cout << "Carry" << endl;
				V[0xf] = 1;
			}

			else
			{
				cout << "No carry" << endl;
				V[0xf] = 0;
			}
			V[X] = V[X] - V[Y];

			PC += 2;
			break;
		case 0x0006:
			cout << "Shifting VX right, VX set to 0" << endl;
			V[0xf] = (V[X] & 0x1);

			V[X] >>= 1;

			PC += 2;
			break;
		case 0x0007:
			if (V[X] > V[Y]) {
				cout << "Borrow, setting VX to VY - VX. VF is now set to 0." << endl;
				V[0xF] = 0; //check for borrow 

			}

			else {
				cout << "No Borrow, setting VX to VY - VX. VF is now set to 1." << endl;
				V[0xF] = 1;
			}

			V[0xF] = V[X] >> 7; // by 7 shift

			V[0xF] = V[X] <<= 1;

			PC += 2;
			break;
		case 0x000E:

			cout << "Stored most significant bit in VX to VF. Shifted VX leftwise by 1." << endl;
			V[0xF] = V[X] >> 7;

			V[X] <<= 1;

			PC += 2;
			break;

		default:

			cout << "Unknown Opcode" + opcode << endl;

			// error handling here
			break;

		}

	}
	break;

	case 0x9000: // skip next instruct if VY isnt equal to VY

		cout << "Skipping next instruction" << "VX does NOT equal VY" << endl;
		if (V[X] != V[Y])
			PC += 4;

		else
			PC += 2;
		break;

	case 0xA000:

		cout << "Setting next instruction to " << NNN << endl;

		I = NNN;

		PC += 2;
		break;

	case 0xB000:

		cout << "Jumping to next instruction to " << NNN << endl;

		PC = NNN + V[0];
		break;

	case 0xC000:

		V[X] = (rand() % 0xFF & NN);

		PC += 2;
		break;

	case 0xD000:

	{
		uint8_t x = X;
		uint8_t y = Y;

		unsigned short height = N;

		// no shifting needed for last bit

		unsigned short pixel;

		V[0xF] = 0;

		// drawing by xor the screen

		// read image from I


		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];

			for (int xline = 0; xline < 8; xline++)
			{
				// 0x80 is 256

				if ((pixel & (0x80 >> xline)) != 0) {

					if (gfx[(X + xline + ((Y + yline) * 64))] == 1)
					{
						V[0xF] = 1;
					}

					gfx[X + xline + (Y + yline * 64)] ^= 1;
				}
			}

			// make this to sfml

			drawFlag = true;

			PC += 2;
		}


		break;

	}

	case 0xE000:

		switch (NN) {

		case 0x009E:

			if (key[V[X]] != 0)
				PC += 4;
			else
				PC += 2;
			break;

		case 0x00A1:

			if (key[V[X]] == 0)
				PC += 4;

			else
				PC += 2;
			break;

		default:

			std::cout << "Unknown opcode";

		}
		break;

	case 0xf000:

		// switch statement

		switch (NN) {

		case 0x0007:
			V[X] = delay_timer;

			PC += +2;
			break;
		case 0x000A: {

			bool kp = false; // key press is awaited for, then stored in vx

			for (int i = 0; i < 16; ++i) {

				if (key[i] != 0) 
				{

					V[X] = 1;
					kp = true;

				}

			}

			if (!kp)
				return;

			PC += +2;


			break;

		}

		case 0x0015:

			delay_timer = V[X];


			PC += +2;
			break;
		case 0x0018:

			sound_timer = V[X];

			PC += +2;
			break;

		case 0x001E:
			if (I + V[X] > 0xFFF)
				V[0xF] = 1;
			else
				V[0xF] = 0;

			I += V[X];
			PC += +2;
			break;
		case 0x0029:
			I = V[X] * 0x5;

			PC += +2;
			break;
		case 0x0033:

			memory[I] = V[X] / 100;
			memory[I] = (V[X] / 10) % 10;
			memory[I] = (V[X] & 100) % 10;


			PC += +2;
			break;
		case 0x0055:

			for (int i = 0; i <= ((X)); ++i)
				memory[I + i] = V[i];

			PC += +2;
			break;
		case 0x0065:

			for (int i = 0; i <= (X); ++i)
				V[i] = memory[I + i];

			I += X + 1;

			PC += +2;
			break;
		}

		std::cout << '\7';
		break;


	default:

		cout << "Unknown Opcode" + opcode << endl;

		// error handling here
		break;

	}

	// Update timers

	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer == 1)
		--sound_timer;
}


//Loads each opcode from a given program into memory. Each opcode is spread between two slots in memory
void Valmac::load_program(uint8_t* pProgram, size_t bufferSize) {
	std::memcpy(memory + PC, pProgram, bufferSize);

	return;
}

uint16_t Valmac::get_program_opcode() {

	_ASSERT((PC & 1) == 0);
	_ASSERT(PC <= MEMORY_SIZE - sizeof(opcode));

	uint16_t l_opcode = memory[PC + 1] << 8 | memory[PC];

	return l_opcode;
}

inline void Valmac::step_PC() {
	PC += 2;
	_ASSERT(PC <= MEMORY_SIZE - sizeof(opcode));
}

void Valmac::initialize()
{
	// Initialize registers and memory once
	PC = 0x200;  // Program counter starts at 0x200
	opcode = 0;  // Reset current opcode	
	I = 0;       // Reset index register
	SP = 0;      // Reset stack pointer


	// Clear display


	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	for (int i = 0; i < 16; ++i)
		key[i] = V[i] = 0;


	// fontset

	unsigned char Valmac_fontset[80] =
	{

		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F

	};

	// Clear registers V0-VF
	// Clear keypad
	// Clear memory

	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;


	memset(memory, 0x00, MEMORY_SIZE);

	memset(stack, 0x00, 16);

	memset(V, 0x00, 16);

	//// Load fontset


	for (int i = 1; i < 80; ++i) {

		memory[i] = Valmac_fontset[i];
	}


	// Reset timers	

	delay_timer = 0;

	sound_timer = 0;

}


Valmac myValmac;

// more headers

sf::RectangleShape Pixel(sf::Vector2f(10.f, 10.f));

// added these varibles so they are not hardcoded, and may help with rescaling

int screenWidth{ 64 };
int screenHeight{ 32 };

sf::RenderWindow window(sf::VideoMode((screenWidth*10), (screenHeight*10)), "Chip 8 ICA");
// screen multiplied by 10 so it is more visible, but still a good size for pixel, each pixel still represents a single pixel within the array

uint16_t MasterMind[3] = {

	0x1204, // 200 JMP to 204
	0x3001, // 202 IF V[0] == 1 skip
	0x1200 // 204 JMP to 200
};

void keyboardpresseddown(uint8_t& key) {

		/*
		1	2	3	C
		4	5	6	D
		7	8	9	E
		A	0	B	F
		*/

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			myValmac.key[0x1] = 1;

		};


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			myValmac.key[0x2] = 1;

		};


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		{
			myValmac.key[0x3] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		{
			myValmac.key[0xC] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			myValmac.key[0x4] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			myValmac.key[0x5] = 1;

		};


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
			myValmac.key[0x6] = 1;

		};


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			myValmac.key[0xD] = 1;

		};


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			myValmac.key[0x7] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			myValmac.key[0x8] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			myValmac.key[0x9] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		{
			myValmac.key[0xE] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			myValmac.key[0xA] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		{
			myValmac.key[0x0] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			myValmac.key[0xB] = 1;

		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		{
			myValmac.key[0xF] = 1;

		};

	}

void keyboardpressedup(uint8_t& key) {



		/*
		1	2	3	C
		4	5	6	D
		7	8	9	E
		A	0	B	F
		*/



		// fix
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			myValmac.key[0x1] = 0;


		};


		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			myValmac.key[0x2] = 0;

		};


		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		{
			myValmac.key[0x3] = 0;


		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		{
			myValmac.key[0xC] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			myValmac.key[0x4] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			myValmac.key[0x5] = 0;

		};


		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
			myValmac.key[0x6] = 0;

		};


		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			myValmac.key[0xD] = 0;

		};


		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			myValmac.key[0x7] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			myValmac.key[0x8] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			myValmac.key[0x9] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		{
			myValmac.key[0xE] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			myValmac.key[0xA] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		{
			myValmac.key[0x0] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			myValmac.key[0xB] = 0;

		};

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		{
			myValmac.key[0xF] = 0;

		};

	}


// function to draw pix
void DrawPixel(int x, int y) {


	// scaling
	x = x * 10;
	y = y * 10;

	Pixel.setPosition(((float)x), (float)y);

	window.draw(Pixel);


}

void Valmac::UpdateGraphics() {
	// graphics setup

	for (int Y = 0; Y < 32; ++Y) {
		for (int X = 0; X < 64; ++X) {
			if (gfx[(Y * 64 + X)] != 0) {
				DrawPixel(Y, X);
			}
		}
	}

}


// adding main into its own file so that the whole thing is much cleaner
int main(int argc, char** argv)
{
	srand(time(NULL));

	cout << "Hello 8-bit world!\n";

	{
		// input setup
		myValmac.initialize();

		myValmac.load_program((uint8_t*)MasterMind, sizeof(MasterMind));


		while (myValmac.isRunning && window.isOpen())
		{

			sf::Event event;
			while (window.pollEvent(event))
			{
				// to exit program
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{

					window.close();

				};

				if (event.type == sf::Event::Closed)
					window.close();
			}

			if (myValmac.drawFlag) {

				myValmac.UpdateGraphics();

				myValmac.drawFlag = false;
			}

			myValmac.emulateCycle();


			window.clear();
			// instructions
			// small test

			DrawPixel(10, 22);


			window.display();
		}

		window.close();

	}
	system("pause");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
