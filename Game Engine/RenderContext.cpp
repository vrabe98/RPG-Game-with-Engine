#include "RenderContext.h"

void RenderContext::create_empty_canvas(SDL_Rect dim) {
    canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dim.w, dim.h);
    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
    if (canvas == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

RenderContext::RenderContext(std::string name) {
    canvas = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, window_flags);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        throw std::runtime_error("Error creating SDL_Renderer!");
    }

    int imgFlags = IMG_INIT_PNG|IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        throw std::runtime_error("Error initializing SDL Image!");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 36.0f);
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

RenderContext::~RenderContext()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(canvas);
    SDL_Quit();
}
