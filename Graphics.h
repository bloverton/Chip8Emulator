#pragma once

// STL Libs
#include <filesystem>
#include <memory>
#include <string>

// External Libs
#include <SDL.h>
#include <SDL_image.h>

enum class SdlReturnType
{
	SDL_INIT_SUCCESS,
	SDL_INIT_ERROR
};

class Graphics
{
public:
	static constexpr uint32_t SCREEN_WIDTH = 64;
	static constexpr uint32_t SCREEN_HEIGHT = 32;
	static constexpr uint32_t SCALE = 15;
	static constexpr uint32_t MS_PER_CYCLE = 1000;
	static constexpr uint32_t FPS = 60;
	static constexpr uint32_t COLOR_OFF = 0xFF000000;
	static constexpr uint32_t COLOR_ON = 0xFFFFFFFF;

	Graphics();
	~Graphics();

	SdlReturnType init(const std::string& windowName = "");
	void drawDisplay(const std::vector<uint32_t>& pixels);

private:
	std::vector<uint32_t> display_size;

	std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
	std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;
};