#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>


SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gTexture = nullptr;

bool init() {
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	gWindow = SDL_CreateWindow("Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == nullptr) {
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}

SDL_Texture* loadTexture(std::string path) {
	SDL_Texture* newTexture = nullptr;
	newTexture = IMG_LoadTexture(gRenderer, path.c_str());
	if (newTexture == nullptr) {
		printf("Unable to create texture! SDL_Error: %s\n", SDL_GetError());
		return nullptr;
	}
	return newTexture;
}

bool loadMedia() {
	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8* wavBuffer;
	SDL_LoadWAV("lyn.wav", &wavSpec, &wavBuffer, &wavLength);
	SDL_AudioDeviceID id = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	int success = SDL_QueueAudio(id, wavBuffer, wavLength);
	SDL_PauseAudioDevice(id, 0);
	if ((gTexture=loadTexture("preview.png"))==nullptr) {
		return false;
	}
	return true;
}

void close() {
	SDL_DestroyTexture(gTexture);
	gTexture = nullptr;
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
	IMG_Quit();
	SDL_Quit();
}
int main(int argc,char*argv[]) {
	bool quit = false;
	SDL_Event e;
	if (!init()) {
		printf("Could not initialize! SDL_Error: %s\n",SDL_GetError());
		close();
		return 0;
	}
	if (!loadMedia()) {
		printf("Could not load media! SDL_Error: %s\n", SDL_GetError());
		close();
		return 0;
	}
	double angle1 = 0, angle2 = 0, angle3 = 0, angle4 = 0;
	SDL_Rect tl = { 0,0,640 / 2,480 / 2 };
	SDL_Rect tr = { 640 / 2,0,640 / 2,480 / 2 };
	SDL_Rect botl = { 0,480 / 2,640/2,480 / 2 };
	SDL_Rect botr = { 640/2,480 / 2,640 / 2,480 / 2 };
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) quit = true;
		}
		SDL_SetRenderDrawColor(gRenderer, 0x00,0x00,0x00, 0xff);
		//SDL_RenderClear(gRenderer);

		SDL_RenderSetViewport(gRenderer, &tl);
		SDL_RenderCopyEx(gRenderer, gTexture, NULL, NULL,angle1,NULL,SDL_FLIP_NONE);

		SDL_RenderSetViewport(gRenderer, &tr);
		SDL_RenderCopyEx(gRenderer, gTexture, NULL, NULL, angle2, NULL, SDL_FLIP_NONE);

		SDL_RenderSetViewport(gRenderer, &botl);
		SDL_RenderCopyEx(gRenderer, gTexture, NULL, NULL, angle3, NULL, SDL_FLIP_NONE);

		SDL_RenderSetViewport(gRenderer, &botr);
		SDL_RenderCopyEx(gRenderer, gTexture, NULL, NULL, angle4, NULL, SDL_FLIP_NONE);

		SDL_RenderPresent(gRenderer);
		SDL_Delay(10);
		angle1 += 20 * 360.0 / 100;
		angle2 -= 0.5 * 360.0 / 100;
		angle3 += 0.75 * 360 / 100;
		angle4 -= 5 * 360.0 / 100;
	}
	close();
	return 0;
}