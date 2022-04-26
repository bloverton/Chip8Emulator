#include "CPU.h"
#include "Graphics.h"

#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
	// Initialize Graphics
	std::unique_ptr<Graphics> graphics(new Graphics());
	graphics->init("Chip8");

	// Initialize Chip8
	std::unique_ptr<CPU> cpu(new CPU());
	std::ifstream rom((std::filesystem::current_path().string() + "\\res\\Space_Invader.ch8").c_str(), std::ios::binary);
	cpu->setMemory(rom);

	// Run Chip8
	for (;;)
	{
		uint32_t startTick = SDL_GetTicks64();
		// Emulate cycle
		cpu->emulateCycle();
		
		// Poll SDL Events
		for (SDL_Event evnt; SDL_PollEvent(&evnt);)
		{
			if (evnt.type == SDL_QUIT)
			{
				return EXIT_SUCCESS;
			}
		}

		// If draw flag is set, update screen
		if (cpu->getDrawFlag())
		{
			graphics->drawDisplay(cpu->getPixels());
			cpu->setDrawFlag(false);
		}

		// Set Framerate
		uint32_t frameSpeed = SDL_GetTicks64() - startTick;
		if (frameSpeed < (Graphics::MS_PER_CYCLE / Graphics::FPS))
		{
			SDL_Delay((Graphics::MS_PER_CYCLE / Graphics::FPS) - frameSpeed);
		}
	}
	return EXIT_SUCCESS;
}