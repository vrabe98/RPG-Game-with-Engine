#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#define WALK_FRAMES 4

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

class LTexture {
public:
	LTexture() {
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}
	~LTexture() {
		free();
	}
	void setColor(Uint8 red, Uint8 green, Uint8 blue) {
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	}
	void setBlendMode(SDL_BlendMode blending) {
		SDL_SetTextureBlendMode(mTexture, blending);
	}
	void setAlpha(uint8_t alpha) {
		SDL_SetTextureAlphaMod(mTexture, alpha);
	}
	bool loadFromFile(std::string path) {
		free();
		SDL_Texture* newTexture = nullptr;
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == nullptr) {
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
			return false;
		}
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xff, 0xff));
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == nullptr) {
			printf("Unable to create texture! SDL Error: %s\n", SDL_GetError());
			return false;
		}
		mWidth = loadedSurface->w;
		mHeight = loadedSurface->h;
		SDL_FreeSurface(loadedSurface);
		mTexture = newTexture;
		return true;
	}
	void free() {
		if (mTexture != nullptr) {
			SDL_DestroyTexture(mTexture);
			mTexture = nullptr;
			mWidth = 0;
			mHeight = 0;
		}
	}
	void render(int x, int y,SDL_Rect* clip=nullptr) {
		SDL_Rect quad = { x,y,mWidth,mHeight };
		if (clip!=nullptr) {
			quad.w = clip->w;
			quad.h = clip->h;
		}
		SDL_RenderCopy(gRenderer, mTexture, clip, &quad);
	}
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};

SDL_Rect gSpriteClips[WALK_FRAMES];
LTexture gSpriteText;

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
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
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
	if (!gSpriteText.loadFromFile("foo.png")) {
		printf("Failed to load sprite sheet!");
		return false;
	}
	gSpriteClips[0] = SDL_Rect{ 0,0,150,240 };
	gSpriteClips[1] = SDL_Rect{ 150,0,150,240 };
	gSpriteClips[2] = SDL_Rect{ 300,0,150,240 };
	gSpriteClips[3] = SDL_Rect{ 450,0,150,240 };
	return true;
}

void close() {
	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
	IMG_Quit();
	SDL_Quit();
}
int main(int argc,char*argv[]) {
	bool quit = false;
	int frame = 0;
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
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) quit = true;
		}
		SDL_SetRenderDrawColor(gRenderer, 0xff,0xff,0xff, 0xff);
		SDL_RenderClear(gRenderer);

		SDL_Rect* currentClip;
		if (frame/4 < 4) {
			currentClip = &gSpriteClips[frame / 4];
		}
		else if (frame / 4 < 6) {
			currentClip = &gSpriteClips[3];
		}
		else {
			currentClip = &gSpriteClips[8 - frame / 4];
		}
		gSpriteText.render((640 - currentClip->w) / 2, (480 - currentClip->h) / 2, currentClip);

		frame = (++frame) % 36;

		SDL_RenderPresent(gRenderer);
	}
	close();
	return 0;
}