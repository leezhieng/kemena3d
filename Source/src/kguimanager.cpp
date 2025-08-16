#include "kguimanager.h"

namespace kemena
{
	kGuiManager::kGuiManager()
	{
		// ctor
	}

	kGuiManager::~kGuiManager()
	{
		// dtor
	}

	void kGuiManager::init(kRenderer *newRenderer)
	{
		renderer = newRenderer;

		float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.IniFilename = nullptr; // Disable imgui.ini

		ImGui::GetStyle().WindowMinSize = ImVec2(200, 200); // Minimum window size

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();

		// Disable hide tab bar button
		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;

		// Setup scaling
		ImGuiStyle &style = ImGui::GetStyle();
		style.ScaleAllSizes(mainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
		style.FontScaleDpi = mainScale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

		// Setup Platform/Renderer backends
		ImGui_ImplSDL3_InitForOpenGL(renderer->getWindow()->getSdlWindow(), renderer->getOpenGlContext());
		const char *glsl_version = "#version 150";
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	void kGuiManager::processEvent(kSystemEvent event)
	{
		ImGui_ImplSDL3_ProcessEvent(event.getSdlEvent());
	}

	void kGuiManager::canvasStart()
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void kGuiManager::canvasEnd()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// IMPORTANT: multi-viewport handling
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}
	}

	void kGuiManager::windowStart(std::string title, bool *open, ImGuiWindowFlags flags)
	{
		ImGui::Begin(title.c_str(), open, flags);
	}

	void kGuiManager::windowEnd()
	{
		ImGui::End();
	}

	void kGuiManager::dockSpaceStart(std::string name)
	{
		bool show_main_dockspace = true;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		// Get main viewport
		const ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		// Remove window decorations and make it fixed
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// Remove padding
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(name.c_str(), &show_main_dockspace, window_flags);
		ImGui::PopStyleVar();

		ImGuiID dockspaceID = ImGui::GetID(name.c_str());
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::DockSpaceOverViewport();
	}

	void kGuiManager::dockSpaceEnd()
	{
		ImGui::End();
	}

	bool kGuiManager::menuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	void kGuiManager::menuBarEnd()
	{
		ImGui::EndMainMenuBar();
	}

	bool kGuiManager::menu(std::string text)
	{
		return ImGui::BeginMenu(text.c_str());
	}

	void kGuiManager::menuEnd()
	{
		ImGui::EndMenu();
	}

	bool kGuiManager::menuItem(std::string text, std::string shortcut, bool selected, bool enabled)
	{
		return ImGui::MenuItem(text.c_str(), shortcut.c_str(), selected, enabled);
	}

	void kGuiManager::groupStart()
	{
		ImGui::BeginGroup();
	}

	void kGuiManager::groupEnd()
	{
		ImGui::EndGroup();
	}

	void kGuiManager::sameLine()
	{
		ImGui::SameLine();
	}

	void kGuiManager::spacing()
	{
		ImGui::Spacing();
	}

	void kGuiManager::separator()
	{
		ImGui::Separator();
	}

	void kGuiManager::text(std::string text)
	{
		ImGui::Text(text.c_str());
	}

	bool kGuiManager::button(std::string text, ivec2 size)
	{
		if (size == ivec2(0, 0))
			return ImGui::Button(text.c_str());
		else
			return ImGui::Button(text.c_str(), ImVec2(size.x, size.y));
	}

	bool kGuiManager::checkbox(std::string text, bool *output)
	{
		return ImGui::Checkbox(text.c_str(), output);
	}

	void kGuiManager::destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}
}
