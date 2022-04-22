#include "CPU.h"

CPU::CPU() :
	memory(MEMORY_SIZE), // 4KB
	registers(16),
	stack(16),
	keys(16),
	pixels(Graphics::SCREEN_WIDTH * Graphics::SCREEN_HEIGHT),
	program_counter(0x200), // Start of program
	index_register(0),
	stack_pointer(0),
	delay_timer(0),
	sound_timer(0),
	drawFlag(false),
	mt(std::random_device()()),
	dist(0, 255)
{
}

void CPU::setMemory(std::ifstream& in)
{
	// Place fontset data at beginning of memory
	uint16_t fontMemLoc = MEMORY_FONTSET_START;
	for (const auto& font : CPU_FONTSET)
	{
		memory[fontMemLoc++] = font;
	}

	// Create temporary buffer for application data
	std::vector<char> tempBuffer(in.tellg());
	std::copy(
		std::istreambuf_iterator<char>(in),
		std::istreambuf_iterator<char>(),
		std::back_inserter(tempBuffer)
	);
	
	// Move application data in memory
	uint16_t appMemLoc = MEMORY_APP_START;
	for (const auto& byte : tempBuffer)
	{
		memory[appMemLoc++] = byte;
	}
}

void CPU::emulateCycle()
{
	// Fetch opcode
	uint16_t opcode = memory[program_counter] << 8 | memory[program_counter + 1];

	// Decode and execute opcode
	switch (opcode & 0xF000)
	{
	case 0x0000:
	{
		switch (opcode & 0x000F)
		{
		case 0x0000:
			std::fill(pixels.begin(), pixels.end(), Graphics::COLOR_OFF);
			drawFlag = true;
			break;

		case 0x000E:
			program_counter = stack[--stack_pointer];
			break;
		}
		program_counter += 2;
		break;
	}

	case 0x1000:
		program_counter = opcode & 0x0FFF;
		break;

	case 0x2000:
		stack[stack_pointer++] = program_counter;
		program_counter = opcode & 0x0FFF;
		break;

	case 0x3000:
		(registers[opcode & (0x0F00 >> 8)] == (opcode & 0x00FF)) ? program_counter += 4 : program_counter += 2;
		break;

	case 0x4000:
		(registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? program_counter += 4 : program_counter += 2;
		break;

	case 0x5000:
		registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4] ? program_counter += 4 : program_counter += 2;
		break;

	case 0x6000:
		registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		program_counter += 2;
		break;

	case 0x7000:
		registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		program_counter += 2;
		break;

	case 0x8000:
	{
		switch (opcode & 0x000F)
		{
		case 0x0000:
			registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
			break;

		case 0x0001:
			registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
			break;

		case 0x0002:
			registers[(opcode & 0x0F00) >> 8] &= registers[(opcode & 0x00F0) >> 4];
			break;

		case 0x0003:
			registers[(opcode & 0x0F00) >> 8] ^= registers[(opcode & 0x00F0) >> 4];
			break;

		case 0x0004:
			registers[(opcode & 0x00F0) >> 4] > (0xFF - registers[(opcode & 0x0F00) >> 8]) ? registers[0xF] = 1 : registers[0xF] = 0; // 1 if V[0x0F00] over 255
			registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
			break;

		case 0x0005:
			registers[(opcode & 0x0F00) >> 8] < registers[(opcode & 0x00F0) >> 4] ? registers[0xF] = 0 : registers[0xF] = 1;
			registers[(opcode & 0x0F00) >> 8] -= registers[(opcode & 0x00F0) >> 4];
			break;

		case 0x0006:
			registers[0xF] = registers[(opcode & 0x0F00) >> 8] & 0x1;
			registers[(opcode & 0x0F00) >> 8] >>= 1; // Divide by 2
			break;

		case 0x0007:
			(registers[(opcode & 0x00F0) >> 4] < registers[(opcode & 0x0F00) >> 8]) ? registers[0xF] = 0 : registers[0xF] = 1;
			registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
			break;

		case 0x000E:
			registers[0xF] = registers[(opcode & 0x0F00) >> 8] >> 7;
			registers[(opcode & 0x0F00) >> 8] <<= 1; // Multiply by 2
			break;
		}
		program_counter += 2;
		break;
	}

	case 0x9000:
		registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4] ? program_counter += 4 : program_counter += 2;
		break;

	case 0xA000:
		index_register = opcode & 0x0FFF;
		program_counter += 2;
		break;

	case 0xB000:
		program_counter = (opcode & 0x0FFF) + registers[0x0];
		break;

	case 0xC000:
		registers[(opcode & 0x0F00) >> 8] = (static_cast<int>(dist(mt)) % 0xFF) & (opcode & 0x00FF);
		program_counter += 2;
		break;

	case 0xD000:
	{
		registers[0xF] = 0;
		uint32_t pixelIndex = 0;
		for (int yline = 0; yline < (opcode & 0x000F); yline++)
		{
			for (int xline = 0; xline < 8; xline++)
			{
				if ((memory[index_register + yline] & (0x80 >> xline)) != 0)
				{
					pixelIndex = (registers[(opcode & 0x0F00) >> 8] + xline) % Graphics::SCREEN_WIDTH +
								 ((registers[(opcode & 0x00F0) >> 4] + yline) % Graphics::SCREEN_HEIGHT) * Graphics::SCREEN_WIDTH;
					if (pixels[pixelIndex] == Graphics::COLOR_ON)
					{
						registers[0xF] = 1;
						pixels[pixelIndex] = Graphics::COLOR_OFF;
					}
					pixels[pixelIndex] = Graphics::COLOR_ON;
					drawFlag = true;
				}
			}
		}
		program_counter += 2;
		break;
	}

	case 0xE000:
	{
		switch (opcode & 0x00FF)
		{
		case 0x009E:
			!SDL_GetKeyboardState(NULL)[CPU_KEYMAP[opcode & 0x0F00 >> 8]] ? program_counter += 4 : program_counter += 2;
			break;

		case 0x00A1:
			SDL_GetKeyboardState(NULL)[CPU_KEYMAP[opcode & 0x0F00 >> 8]] ? program_counter += 4 : program_counter += 2;
			break;
		}
		break;
	}

	case 0xF000:
	{
		switch (opcode & 0x00FF)
		{
		case 0x0007:
			registers[(opcode & 0x0F00) >> 8] = delay_timer;
			break;

		case 0x000A:
			for (int i = 0; i < 16; i++)
			{
				if (SDL_GetKeyboardState(NULL)[CPU_KEYMAP[i]])
				{
					registers[(opcode & 0x0F00) >> 8] = i;
					break;
				}
			}
			break;

		case 0x0015:
			delay_timer = registers[(opcode & 0x0F00) >> 8];
			break;

		case 0x0018:
			sound_timer = registers[(opcode & 0x0F00) >> 8];
			break;

		case 0x001E:
			(index_register + registers[(opcode & 0x0F00) >> 8] > 0xFFF) ? registers[0xF] = 1 : registers[0xF] = 0;
			index_register += registers[(opcode & 0x0F00) >> 8];
			break;

		case 0x0029:
			index_register = registers[(opcode & 0x0F00) >> 8] * 5;
			break;

		case 0x0033:
			memory[index_register] = registers[(opcode & 0x0F00) >> 8] / 100;
			memory[index_register + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[index_register + 2] = registers[(opcode & 0x0F00) >> 8] % 10;
			break;

		case 0x0055:
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
			{
				memory[index_register + i] = registers[i];
			}
			index_register += ((opcode & 0x0F00) >> 8) + 1;
			break;

		case 0x0065:
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
			{
				registers[i] = memory[index_register + i];
			}
			index_register += ((opcode & 0x0F00) >> 8) + 1;
			break;
		}
		program_counter += 2;
		break;
	}
	
	default:
		printf("Unknown Opcode: 0x%x\n", opcode);
	}

	// Update Timers
	(delay_timer > 0) ? delay_timer -= 1 : 0;
	(sound_timer > 0) ? sound_timer -= 1 : 0;
}
