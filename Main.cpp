#include "CPU.h"
#include "Graphics.h"

int main(int argc, char** argv)
{
	// Initialize Graphics
	std::unique_ptr<Graphics> graphics(new Graphics());
	graphics->init("Chip8");

	// Initialize Chip8
	std::unique_ptr<CPU> cpu(new CPU());
	std::ifstream rom((std::filesystem::current_path().string() + "\\res\\Pong.ch8").c_str(), std::ios::binary);
	cpu->setMemory(rom);

	uint32_t startTick = 0;
	uint32_t frameSpeed = 0;
	SDL_Event evnt;
	// Run Chip8
	for (;;)
	{
		startTick = SDL_GetTicks();

		// Emulate cycle
		cpu->emulateCycle();
		
		// If draw flag is set, update screen
		if (cpu->getDrawFlag())
		{
			graphics->drawDisplay(cpu->getPixels());
			cpu->setDrawFlag(false);
		}

		// Set Framerate
		frameSpeed = SDL_GetTicks() - startTick;
		if (frameSpeed < (Graphics::MS_PER_CYCLE / Graphics::FPS))
		{
			SDL_Delay((Graphics::MS_PER_CYCLE / Graphics::FPS) - frameSpeed);
		}

		// Poll SDL Events
		while (SDL_PollEvent(&evnt))
		{
			if(evnt.type == SDL_QUIT)
			{
				return EXIT_SUCCESS;
			}
		}
	}
	return EXIT_SUCCESS;
}