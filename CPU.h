#pragma once

#include "Graphics.h"

#include <algorithm>
#include <fstream>
#include <random>
#include <vector>

class CPU
{
public:

	static constexpr uint32_t MEMORY_SIZE = 4096;
	static constexpr uint32_t MEMORY_APP_START = 0x200;
	static constexpr uint32_t MEMORY_FONTSET_START = 0x50;
	std::vector<uint8_t> CPU_FONTSET
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
	std::vector<SDL_Scancode> CPU_KEYMAP
	{
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_4,
		SDL_SCANCODE_Q,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_R,
		SDL_SCANCODE_A,
		SDL_SCANCODE_S,
		SDL_SCANCODE_D,
		SDL_SCANCODE_F,
		SDL_SCANCODE_Z,
		SDL_SCANCODE_X,
		SDL_SCANCODE_C,
		SDL_SCANCODE_V
	};

	CPU();
	~CPU() = default;

	void setMemory(std::ifstream& in);
	void emulateCycle();

	// Get and Set Methods
	std::vector<uint32_t> getPixels() { return pixels; };
	bool getDrawFlag() { return drawFlag; };
	void setDrawFlag(const bool& drawFlag) { this->drawFlag = drawFlag; }

private:

	// Containers
	std::vector<uint8_t> memory;
	std::vector<uint8_t> registers;
	std::vector<uint16_t> stack;
	std::vector<uint8_t> keys;
	std::vector<uint32_t> pixels;

	// Pointers
	uint16_t program_counter;
	uint16_t index_register;
	uint16_t stack_pointer;

	// Timers
	uint8_t delay_timer;
	uint8_t sound_timer;

	// Flags
	bool drawFlag;

	// Rng
	std::mt19937 mt;
	std::uniform_int_distribution<std::mt19937::result_type> dist;
};