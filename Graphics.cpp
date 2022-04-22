#include "Graphics.h"

#if _DEBUG
#include <chrono>
#include <thread>
#endif

Graphics::Graphics() :
	window(nullptr, SDL_DestroyWindow),
	renderer(nullptr, SDL_DestroyRenderer),
	texture(nullptr, SDL_DestroyTexture),
	display_size(SCREEN_WIDTH * SCREEN_HEIGHT)
{

}

Graphics::~Graphics()
{
	SDL_Quit();
}

SdlReturnType Graphics::init(const std::string& windowName)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return SdlReturnType::SDL_INIT_ERROR;
	}

	//Create window
	window.reset(SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN));
	if (!window)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return SdlReturnType::SDL_INIT_ERROR;
	}

	renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));
	if (!renderer)
	{
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return SdlReturnType::SDL_INIT_ERROR;
	}

	texture.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT));
	if (!texture)
	{
		printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
		return SdlReturnType::SDL_INIT_ERROR;
	}

	return SdlReturnType::SDL_INIT_SUCCESS;
}

void Graphics::drawDisplay(const std::vector<uint32_t>& pixels)
{
	SDL_UpdateTexture(texture.get(), NULL, pixels.data(), SCREEN_WIDTH * sizeof(uint32_t));
	SDL_RenderCopy(renderer.get(), texture.get(), NULL, NULL);
	SDL_RenderPresent(renderer.get());
}