#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SDL2/SDL.h>

class Platform
{
	friend class Imgui;

public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~Platform();
	void Update(void const* buffer, int pitch);
	bool ProcessInput(unsigned char* keys);

private:
	SDL_Window* window{};
	SDL_GLContext gl_context{};
	unsigned int framebuffer_texture;
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};
};

#endif