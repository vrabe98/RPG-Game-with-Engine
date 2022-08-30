#include "CustomGame.h"

void CustomGame::Play(){
    ImVec4 bkg_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        Button button_play, button_exit;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(render_context.window))
                done = true;
        }
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
        ImGui::SetWindowPos(ImVec2((WIN_W - 500) / 2, (WIN_H - 500) / 2));
        ImGui::SetWindowSize(ImVec2(500, 500));
        ImGui::PushFont(getRenderContext()->font);

        button_play = Button("Play", &render_context, ImVec2(200, 100), ImVec2(150, 140), ImVec4(0.0f, 0.0f, 0.7f, 1.0f));
        button_exit = Button("Exit", &render_context, ImVec2(200, 100), ImVec2(150, 260), ImVec4(0.7f, 0.0f, 0.0f, 1.0f));

        button_play.render();
        if (button_exit.render()) {
            done = true;
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(render_context.renderer, (Uint8)(bkg_color.x * 255), (Uint8)(bkg_color.y * 255), (Uint8)(bkg_color.z * 255), (Uint8)(bkg_color.w * 255));
        SDL_RenderClear(render_context.renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(render_context.renderer);
    }
}