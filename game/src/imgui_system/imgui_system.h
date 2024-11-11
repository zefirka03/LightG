#pragma once
#include "core/core.h"
#include "render/render.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImguiSystem : public System {
public:
    bool physics_draw_debug = false;
    bool rtx_rendering = false;
private:
    TextureManager* TM;

    void init() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(Application::get_instance().get_native_window(), true);
        ImGui_ImplOpenGL3_Init("#version 130");

        TM = &m_scene->get_system<RenderingSystem>()->get_texture_manager();
        TM->load_texture_by_data(m_scene->get_system<EnvironmentSystem>()->map, { 256, 256, AIR_TEXTURE_RG, AIR_TEXTURE_RG }, "heatmap");
    }
    
    void update(float delta_time) override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Stats");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", delta_time, 1.f / delta_time);

            ImGui::Checkbox("Physics Debug", &physics_draw_debug);
            ImGui::Checkbox("RT rendering", &rtx_rendering);


            ImGui::Image(TM->get_texture("heatmap")->id, ImVec2(256, 256));
                
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

    }
};