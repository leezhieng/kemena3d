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

	void kGuiManager::loadDefaultFontFromResource(string resourceName)
	{
		HRSRC hRes = FindResource(NULL, resourceName.c_str(), RT_RCDATA);
		if (!hRes) return;

		HGLOBAL hData = LoadResource(NULL, hRes);
		if (!hData) return;

		void* pData = LockResource(hData);
		DWORD size = SizeofResource(NULL, hRes);

		if (pData && size > 0)
		{
			// Safer: copy into your own buffer because ImGui may rebuild fonts later
			void* fontData = malloc(size);
			memcpy(fontData, pData, size);

			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->AddFontFromMemoryTTF(fontData, size, 16.0f);

			// You must keep fontData alive as long as ImGui needs it
			// Free after ImGui::DestroyContext()
		}
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

	// ---- Window ----

	void kGuiManager::windowStart(string title, bool *open, ImGuiWindowFlags flags)
	{
		ImGui::Begin(title.c_str(), open, flags);
	}

	void kGuiManager::windowEnd()
	{
		ImGui::End();
	}

	vec2 kGuiManager::getWindowSize()
	{
		ImVec2 s = ImGui::GetWindowSize();
		return vec2(s.x, s.y);
	}

	vec2 kGuiManager::getWindowPos()
	{
		ImVec2 p = ImGui::GetWindowPos();
		return vec2(p.x, p.y);
	}

	void kGuiManager::setNextWindowSize(vec2 size, ImGuiCond cond)
	{
		ImGui::SetNextWindowSize(ImVec2(size.x, size.y), cond);
	}

	void kGuiManager::setNextWindowPos(vec2 pos, ImGuiCond cond, vec2 pivot)
	{
		ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), cond, ImVec2(pivot.x, pivot.y));
	}

	void kGuiManager::setNextWindowContentSize(vec2 size)
	{
		ImGui::SetNextWindowContentSize(ImVec2(size.x, size.y));
	}

	void kGuiManager::setNextWindowCollapsed(bool collapsed, ImGuiCond cond)
	{
		ImGui::SetNextWindowCollapsed(collapsed, cond);
	}

	void kGuiManager::setNextWindowFocus()
	{
		ImGui::SetNextWindowFocus();
	}

	void kGuiManager::setNextWindowBgAlpha(float alpha)
	{
		ImGui::SetNextWindowBgAlpha(alpha);
	}

	bool kGuiManager::isWindowFocused(ImGuiFocusedFlags flags)
	{
		return ImGui::IsWindowFocused(flags);
	}

	bool kGuiManager::isWindowHovered(ImGuiHoveredFlags flags)
	{
		return ImGui::IsWindowHovered(flags);
	}

	// ---- Dockspace ----

	void kGuiManager::dockSpaceStart(string name)
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

	// ---- Menu ----

	bool kGuiManager::menuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	void kGuiManager::menuBarEnd()
	{
		ImGui::EndMainMenuBar();
	}

	bool kGuiManager::menu(string text)
	{
		return ImGui::BeginMenu(text.c_str());
	}

	void kGuiManager::menuEnd()
	{
		ImGui::EndMenu();
	}

	bool kGuiManager::menuItem(string text, string shortcut, bool selected, bool enabled)
	{
		return ImGui::MenuItem(text.c_str(), shortcut.c_str(), selected, enabled);
	}

	bool kGuiManager::menuItem(string text, string shortcut, bool *selected, bool enabled)
	{
		return ImGui::MenuItem(text.c_str(), shortcut.c_str(), selected, enabled);
	}

	// ---- Layout ----

	void kGuiManager::groupStart()
	{
		ImGui::BeginGroup();
	}

	void kGuiManager::groupEnd()
	{
		ImGui::EndGroup();
	}

	void kGuiManager::sameLine(float offsetFromStartX, float spacing)
	{
		ImGui::SameLine(offsetFromStartX, spacing);
	}

	void kGuiManager::spacing()
	{
		ImGui::Spacing();
	}

	void kGuiManager::separator()
	{
		ImGui::Separator();
	}

	void kGuiManager::separatorText(string text)
	{
		ImGui::SeparatorText(text.c_str());
	}

	void kGuiManager::newLine()
	{
		ImGui::NewLine();
	}

	void kGuiManager::indent(float indentW)
	{
		ImGui::Indent(indentW);
	}

	void kGuiManager::unindent(float indentW)
	{
		ImGui::Unindent(indentW);
	}

	void kGuiManager::dummy(vec2 size)
	{
		ImGui::Dummy(ImVec2(size.x, size.y));
	}

	void kGuiManager::setCursorPos(vec2 pos)
	{
		ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
	}

	void kGuiManager::setCursorPosX(float x)
	{
		ImGui::SetCursorPosX(x);
	}

	void kGuiManager::setCursorPosY(float y)
	{
		ImGui::SetCursorPosY(y);
	}

	vec2 kGuiManager::getCursorPos()
	{
		ImVec2 p = ImGui::GetCursorPos();
		return vec2(p.x, p.y);
	}

	float kGuiManager::getCursorPosX()
	{
		return ImGui::GetCursorPosX();
	}

	float kGuiManager::getCursorPosY()
	{
		return ImGui::GetCursorPosY();
	}

	vec2 kGuiManager::getCursorScreenPos()
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		return vec2(p.x, p.y);
	}

	vec2 kGuiManager::getContentRegionAvail()
	{
		ImVec2 s = ImGui::GetContentRegionAvail();
		return vec2(s.x, s.y);
	}

	void kGuiManager::setNextItemWidth(float itemWidth)
	{
		ImGui::SetNextItemWidth(itemWidth);
	}

	void kGuiManager::pushItemWidth(float itemWidth)
	{
		ImGui::PushItemWidth(itemWidth);
	}

	void kGuiManager::popItemWidth()
	{
		ImGui::PopItemWidth();
	}

	float kGuiManager::calcItemWidth()
	{
		return ImGui::CalcItemWidth();
	}

	void kGuiManager::pushTextWrapPos(float wrapLocalPosX)
	{
		ImGui::PushTextWrapPos(wrapLocalPosX);
	}

	void kGuiManager::popTextWrapPos()
	{
		ImGui::PopTextWrapPos();
	}

	// ---- ID Stack ----

	void kGuiManager::pushId(string id)
	{
		ImGui::PushID(id.c_str());
	}

	void kGuiManager::pushId(int id)
	{
		ImGui::PushID(id);
	}

	void kGuiManager::pushId(const void *ptr)
	{
		ImGui::PushID(ptr);
	}

	void kGuiManager::popId()
	{
		ImGui::PopID();
	}

	// ---- Style ----

	void kGuiManager::pushStyleColor(ImGuiCol idx, vec4 color)
	{
		ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, color.a));
	}

	void kGuiManager::pushStyleColor(ImGuiCol idx, ImU32 color)
	{
		ImGui::PushStyleColor(idx, color);
	}

	void kGuiManager::popStyleColor(int count)
	{
		ImGui::PopStyleColor(count);
	}

	void kGuiManager::pushStyleVar(ImGuiStyleVar idx, float val)
	{
		ImGui::PushStyleVar(idx, val);
	}

	void kGuiManager::pushStyleVar(ImGuiStyleVar idx, vec2 val)
	{
		ImGui::PushStyleVar(idx, ImVec2(val.x, val.y));
	}

	void kGuiManager::popStyleVar(int count)
	{
		ImGui::PopStyleVar(count);
	}

	void kGuiManager::pushFont(ImFont *font)
	{
		ImGui::PushFont(font);
	}

	void kGuiManager::popFont()
	{
		ImGui::PopFont();
	}

	// ---- Text ----

	void kGuiManager::text(string text)
	{
		ImGui::Text(text.c_str());
	}

	void kGuiManager::textColored(vec4 color, string text)
	{
		ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), text.c_str());
	}

	void kGuiManager::textDisabled(string text)
	{
		ImGui::TextDisabled(text.c_str());
	}

	void kGuiManager::textWrapped(string text)
	{
		ImGui::TextWrapped(text.c_str());
	}

	void kGuiManager::labelText(string label, string text)
	{
		ImGui::LabelText(label.c_str(), text.c_str());
	}

	void kGuiManager::bulletText(string text)
	{
		ImGui::BulletText(text.c_str());
	}

	void kGuiManager::bullet()
	{
		ImGui::Bullet();
	}

	// ---- Buttons ----

	bool kGuiManager::button(string text, ivec2 size)
	{
		if (size == ivec2(0, 0))
			return ImGui::Button(text.c_str());
		else
			return ImGui::Button(text.c_str(), ImVec2((float)size.x, (float)size.y));
	}

	bool kGuiManager::smallButton(string text)
	{
		return ImGui::SmallButton(text.c_str());
	}

	bool kGuiManager::invisibleButton(string id, vec2 size, ImGuiButtonFlags flags)
	{
		return ImGui::InvisibleButton(id.c_str(), ImVec2(size.x, size.y), flags);
	}

	bool kGuiManager::arrowButton(string id, ImGuiDir dir)
	{
		return ImGui::ArrowButton(id.c_str(), dir);
	}

	bool kGuiManager::radioButton(string label, bool active)
	{
		return ImGui::RadioButton(label.c_str(), active);
	}

	bool kGuiManager::radioButton(string label, int *v, int vButton)
	{
		return ImGui::RadioButton(label.c_str(), v, vButton);
	}

	// ---- Checkbox ----

	bool kGuiManager::checkbox(string text, bool *output)
	{
		return ImGui::Checkbox(text.c_str(), output);
	}

	bool kGuiManager::checkboxFlags(string label, int *flags, int flagsValue)
	{
		return ImGui::CheckboxFlags(label.c_str(), flags, flagsValue);
	}

	bool kGuiManager::checkboxFlags(string label, unsigned int *flags, unsigned int flagsValue)
	{
		return ImGui::CheckboxFlags(label.c_str(), flags, flagsValue);
	}

	// ---- Input Text ----

	bool kGuiManager::inputText(string label, string &value, size_t maxLength, ImGuiInputTextFlags flags)
	{
		std::vector<char> buf(maxLength + 1, 0);
		strncpy(buf.data(), value.c_str(), maxLength);
		if (ImGui::InputText(label.c_str(), buf.data(), maxLength + 1, flags))
		{
			value = buf.data();
			return true;
		}
		return false;
	}

	bool kGuiManager::inputTextMultiline(string label, string &value, size_t maxLength, vec2 size, ImGuiInputTextFlags flags)
	{
		std::vector<char> buf(maxLength + 1, 0);
		strncpy(buf.data(), value.c_str(), maxLength);
		if (ImGui::InputTextMultiline(label.c_str(), buf.data(), maxLength + 1, ImVec2(size.x, size.y), flags))
		{
			value = buf.data();
			return true;
		}
		return false;
	}

	bool kGuiManager::inputTextWithHint(string label, string hint, string &value, size_t maxLength, ImGuiInputTextFlags flags)
	{
		std::vector<char> buf(maxLength + 1, 0);
		strncpy(buf.data(), value.c_str(), maxLength);
		if (ImGui::InputTextWithHint(label.c_str(), hint.c_str(), buf.data(), maxLength + 1, flags))
		{
			value = buf.data();
			return true;
		}
		return false;
	}

	// ---- Input Scalar ----

	bool kGuiManager::inputFloat(string label, float *v, float step, float stepFast, string format, ImGuiInputTextFlags flags)
	{
		return ImGui::InputFloat(label.c_str(), v, step, stepFast, format.c_str(), flags);
	}

	bool kGuiManager::inputFloat2(string label, float v[2], string format, ImGuiInputTextFlags flags)
	{
		return ImGui::InputFloat2(label.c_str(), v, format.c_str(), flags);
	}

	bool kGuiManager::inputFloat3(string label, float v[3], string format, ImGuiInputTextFlags flags)
	{
		return ImGui::InputFloat3(label.c_str(), v, format.c_str(), flags);
	}

	bool kGuiManager::inputFloat4(string label, float v[4], string format, ImGuiInputTextFlags flags)
	{
		return ImGui::InputFloat4(label.c_str(), v, format.c_str(), flags);
	}

	bool kGuiManager::inputInt(string label, int *v, int step, int stepFast, ImGuiInputTextFlags flags)
	{
		return ImGui::InputInt(label.c_str(), v, step, stepFast, flags);
	}

	bool kGuiManager::inputInt2(string label, int v[2], ImGuiInputTextFlags flags)
	{
		return ImGui::InputInt2(label.c_str(), v, flags);
	}

	bool kGuiManager::inputInt3(string label, int v[3], ImGuiInputTextFlags flags)
	{
		return ImGui::InputInt3(label.c_str(), v, flags);
	}

	bool kGuiManager::inputInt4(string label, int v[4], ImGuiInputTextFlags flags)
	{
		return ImGui::InputInt4(label.c_str(), v, flags);
	}

	bool kGuiManager::inputDouble(string label, double *v, double step, double stepFast, string format, ImGuiInputTextFlags flags)
	{
		return ImGui::InputDouble(label.c_str(), v, step, stepFast, format.c_str(), flags);
	}

	// ---- Drag ----

	bool kGuiManager::dragFloat(string label, float *v, float speed, float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragFloat(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragFloat2(string label, float v[2], float speed, float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragFloat2(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragFloat3(string label, float v[3], float speed, float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragFloat3(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragFloat4(string label, float v[4], float speed, float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragFloat4(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragInt(string label, int *v, float speed, int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragInt(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragInt2(string label, int v[2], float speed, int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragInt2(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragInt3(string label, int v[3], float speed, int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragInt3(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragInt4(string label, int v[4], float speed, int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::DragInt4(label.c_str(), v, speed, min, max, format.c_str(), flags);
	}

	bool kGuiManager::dragFloatRange2(string label, float *vCurrentMin, float *vCurrentMax, float speed, float min, float max, string format, string formatMax, ImGuiSliderFlags flags)
	{
		return ImGui::DragFloatRange2(label.c_str(), vCurrentMin, vCurrentMax, speed, min, max, format.c_str(), formatMax.empty() ? nullptr : formatMax.c_str(), flags);
	}

	bool kGuiManager::dragIntRange2(string label, int *vCurrentMin, int *vCurrentMax, float speed, int min, int max, string format, string formatMax, ImGuiSliderFlags flags)
	{
		return ImGui::DragIntRange2(label.c_str(), vCurrentMin, vCurrentMax, speed, min, max, format.c_str(), formatMax.empty() ? nullptr : formatMax.c_str(), flags);
	}

	// ---- Slider ----

	bool kGuiManager::sliderFloat(string label, float *v, float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderFloat(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderFloat2(string label, float v[2], float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderFloat2(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderFloat3(string label, float v[3], float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderFloat3(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderFloat4(string label, float v[4], float min, float max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderFloat4(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderAngle(string label, float *vRad, float vDegreesMin, float vDegreesMax, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderAngle(label.c_str(), vRad, vDegreesMin, vDegreesMax, format.c_str(), flags);
	}

	bool kGuiManager::sliderInt(string label, int *v, int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderInt(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderInt2(string label, int v[2], int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderInt2(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderInt3(string label, int v[3], int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderInt3(label.c_str(), v, min, max, format.c_str(), flags);
	}

	bool kGuiManager::sliderInt4(string label, int v[4], int min, int max, string format, ImGuiSliderFlags flags)
	{
		return ImGui::SliderInt4(label.c_str(), v, min, max, format.c_str(), flags);
	}

	// ---- Color ----

	bool kGuiManager::colorEdit3(string label, float col[3], ImGuiColorEditFlags flags)
	{
		return ImGui::ColorEdit3(label.c_str(), col, flags);
	}

	bool kGuiManager::colorEdit4(string label, float col[4], ImGuiColorEditFlags flags)
	{
		return ImGui::ColorEdit4(label.c_str(), col, flags);
	}

	bool kGuiManager::colorPicker3(string label, float col[3], ImGuiColorEditFlags flags)
	{
		return ImGui::ColorPicker3(label.c_str(), col, flags);
	}

	bool kGuiManager::colorPicker4(string label, float col[4], ImGuiColorEditFlags flags)
	{
		return ImGui::ColorPicker4(label.c_str(), col, flags);
	}

	bool kGuiManager::colorButton(string descId, vec4 col, ImGuiColorEditFlags flags, vec2 size)
	{
		return ImGui::ColorButton(descId.c_str(), ImVec4(col.r, col.g, col.b, col.a), flags, ImVec2(size.x, size.y));
	}

	void kGuiManager::setColorEditOptions(ImGuiColorEditFlags flags)
	{
		ImGui::SetColorEditOptions(flags);
	}

	// ---- Combo / List / Selectable ----

	bool kGuiManager::combo(string label, int *currentItem, std::vector<string> items, int popupMaxHeightInItems)
	{
		std::vector<const char *> cItems;
		cItems.reserve(items.size());
		for (const auto &s : items)
			cItems.push_back(s.c_str());
		return ImGui::Combo(label.c_str(), currentItem, cItems.data(), (int)cItems.size(), popupMaxHeightInItems);
	}

	bool kGuiManager::listBox(string label, int *currentItem, std::vector<string> items, int heightInItems)
	{
		std::vector<const char *> cItems;
		cItems.reserve(items.size());
		for (const auto &s : items)
			cItems.push_back(s.c_str());
		return ImGui::ListBox(label.c_str(), currentItem, cItems.data(), (int)cItems.size(), heightInItems);
	}

	bool kGuiManager::selectable(string label, bool selected, ImGuiSelectableFlags flags, vec2 size)
	{
		return ImGui::Selectable(label.c_str(), selected, flags, ImVec2(size.x, size.y));
	}

	bool kGuiManager::selectable(string label, bool *pSelected, ImGuiSelectableFlags flags, vec2 size)
	{
		return ImGui::Selectable(label.c_str(), pSelected, flags, ImVec2(size.x, size.y));
	}

	// ---- Tree / Collapsing ----

	bool kGuiManager::treeStart(string label)
	{
		return ImGui::TreeNode(label.c_str());
	}

	bool kGuiManager::treeStartEx(string id, string label, ImGuiTreeNodeFlags flags)
	{
		if (label.empty())
			return ImGui::TreeNodeEx(id.c_str(), flags);
		return ImGui::TreeNodeEx(id.c_str(), flags, label.c_str());
	}

	void kGuiManager::treeEnd()
	{
		ImGui::TreePop();
	}

	void kGuiManager::treePop()
	{
		ImGui::TreePop();
	}

	bool kGuiManager::collapsingHeader(string label, ImGuiTreeNodeFlags flags)
	{
		return ImGui::CollapsingHeader(label.c_str(), flags);
	}

	bool kGuiManager::collapsingHeader(string label, bool *visible, ImGuiTreeNodeFlags flags)
	{
		return ImGui::CollapsingHeader(label.c_str(), visible, flags);
	}

	void kGuiManager::setNextItemOpen(bool isOpen, ImGuiCond cond)
	{
		ImGui::SetNextItemOpen(isOpen, cond);
	}

	// ---- Image ----

	void kGuiManager::image(GLuint textureId, vec2 size, vec2 uv0, vec2 uv1)
	{
		ImGui::Image((ImTextureID)(intptr_t)textureId, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
	}

	bool kGuiManager::imageButton(string id, GLuint textureId, vec2 size, vec2 uv0, vec2 uv1)
	{
		return ImGui::ImageButton(id.c_str(), (ImTextureID)(intptr_t)textureId, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
	}

	// ---- Progress ----

	void kGuiManager::progressBar(float fraction, vec2 size, string overlay)
	{
		ImGui::ProgressBar(fraction, ImVec2(size.x, size.y), overlay.empty() ? nullptr : overlay.c_str());
	}

	// ---- Tooltip ----

	void kGuiManager::setItemTooltip(string text)
	{
		ImGui::SetItemTooltip(text.c_str());
	}

	void kGuiManager::beginTooltip()
	{
		ImGui::BeginTooltip();
	}

	void kGuiManager::endTooltip()
	{
		ImGui::EndTooltip();
	}

	// ---- Popup ----

	void kGuiManager::openPopup(string id, ImGuiPopupFlags flags)
	{
		ImGui::OpenPopup(id.c_str(), flags);
	}

	bool kGuiManager::popupStart(string id, ImGuiWindowFlags flags)
	{
		return ImGui::BeginPopup(id.c_str(), flags);
	}

	void kGuiManager::popupEnd()
	{
		ImGui::EndPopup();
	}

	bool kGuiManager::popupModal(string name, bool *open, ImGuiWindowFlags flags)
	{
		return ImGui::BeginPopupModal(name.c_str(), open, flags);
	}

	bool kGuiManager::popupContextItemStart(string id, ImGuiPopupFlags flags)
	{
		return ImGui::BeginPopupContextItem(id.empty() ? nullptr : id.c_str(), flags);
	}

	bool kGuiManager::popupContextWindowStart(string id, ImGuiPopupFlags flags)
	{
		return ImGui::BeginPopupContextWindow(id.empty() ? nullptr : id.c_str(), flags);
	}

	bool kGuiManager::isPopupOpen(string id, ImGuiPopupFlags flags)
	{
		return ImGui::IsPopupOpen(id.c_str(), flags);
	}

	void kGuiManager::closeCurrentPopup()
	{
		ImGui::CloseCurrentPopup();
	}

	// ---- Tab Bar ----

	bool kGuiManager::tabBarStart(string id, ImGuiTabBarFlags flags)
	{
		return ImGui::BeginTabBar(id.c_str(), flags);
	}

	void kGuiManager::tabBarEnd()
	{
		ImGui::EndTabBar();
	}

	bool kGuiManager::tabItemStart(string label, bool *open, ImGuiTabItemFlags flags)
	{
		return ImGui::BeginTabItem(label.c_str(), open, flags);
	}

	void kGuiManager::tabItemEnd()
	{
		ImGui::EndTabItem();
	}

	void kGuiManager::setTabItemClosed(string tabOrDockedWindowLabel)
	{
		ImGui::SetTabItemClosed(tabOrDockedWindowLabel.c_str());
	}

	// ---- Tables ----

	bool kGuiManager::tableStart(string id, int columns, ImGuiTableFlags flags, vec2 outerSize, float innerWidth)
	{
		return ImGui::BeginTable(id.c_str(), columns, flags, ImVec2(outerSize.x, outerSize.y), innerWidth);
	}

	void kGuiManager::tableEnd()
	{
		ImGui::EndTable();
	}

	bool kGuiManager::tableNextColumn()
	{
		return ImGui::TableNextColumn();
	}

	bool kGuiManager::tableSetColumnIndex(int columnN)
	{
		return ImGui::TableSetColumnIndex(columnN);
	}

	void kGuiManager::tableNextRow(ImGuiTableRowFlags rowFlags, float minRowHeight)
	{
		ImGui::TableNextRow(rowFlags, minRowHeight);
	}

	void kGuiManager::tableSetupColumn(string label, ImGuiTableColumnFlags flags, float initWidthOrWeight)
	{
		ImGui::TableSetupColumn(label.c_str(), flags, initWidthOrWeight);
	}

	void kGuiManager::tableSetupScrollFreeze(int cols, int rows)
	{
		ImGui::TableSetupScrollFreeze(cols, rows);
	}

	void kGuiManager::tableHeadersRow()
	{
		ImGui::TableHeadersRow();
	}

	void kGuiManager::tableHeader(string label)
	{
		ImGui::TableHeader(label.c_str());
	}

	// ---- Scroll ----

	float kGuiManager::getScrollX()
	{
		return ImGui::GetScrollX();
	}

	float kGuiManager::getScrollY()
	{
		return ImGui::GetScrollY();
	}

	void kGuiManager::setScrollX(float scrollX)
	{
		ImGui::SetScrollX(scrollX);
	}

	void kGuiManager::setScrollY(float scrollY)
	{
		ImGui::SetScrollY(scrollY);
	}

	float kGuiManager::getScrollMaxX()
	{
		return ImGui::GetScrollMaxX();
	}

	float kGuiManager::getScrollMaxY()
	{
		return ImGui::GetScrollMaxY();
	}

	void kGuiManager::setScrollHereX(float centerXRatio)
	{
		ImGui::SetScrollHereX(centerXRatio);
	}

	void kGuiManager::setScrollHereY(float centerYRatio)
	{
		ImGui::SetScrollHereY(centerYRatio);
	}

	// ---- Item Queries ----

	bool kGuiManager::isItemHovered(ImGuiHoveredFlags flags)
	{
		return ImGui::IsItemHovered(flags);
	}

	bool kGuiManager::isItemActive()
	{
		return ImGui::IsItemActive();
	}

	bool kGuiManager::isItemFocused()
	{
		return ImGui::IsItemFocused();
	}

	bool kGuiManager::isItemClicked(ImGuiMouseButton mouseButton)
	{
		return ImGui::IsItemClicked(mouseButton);
	}

	bool kGuiManager::isItemVisible()
	{
		return ImGui::IsItemVisible();
	}

	bool kGuiManager::isItemEdited()
	{
		return ImGui::IsItemEdited();
	}

	bool kGuiManager::isItemActivated()
	{
		return ImGui::IsItemActivated();
	}

	bool kGuiManager::isItemDeactivated()
	{
		return ImGui::IsItemDeactivated();
	}

	bool kGuiManager::isItemDeactivatedAfterEdit()
	{
		return ImGui::IsItemDeactivatedAfterEdit();
	}

	bool kGuiManager::isItemToggledOpen()
	{
		return ImGui::IsItemToggledOpen();
	}

	bool kGuiManager::isAnyItemHovered()
	{
		return ImGui::IsAnyItemHovered();
	}

	bool kGuiManager::isAnyItemActive()
	{
		return ImGui::IsAnyItemActive();
	}

	bool kGuiManager::isAnyItemFocused()
	{
		return ImGui::IsAnyItemFocused();
	}

	vec2 kGuiManager::getItemRectMin()
	{
		ImVec2 v = ImGui::GetItemRectMin();
		return vec2(v.x, v.y);
	}

	vec2 kGuiManager::getItemRectMax()
	{
		ImVec2 v = ImGui::GetItemRectMax();
		return vec2(v.x, v.y);
	}

	vec2 kGuiManager::getItemRectSize()
	{
		ImVec2 v = ImGui::GetItemRectSize();
		return vec2(v.x, v.y);
	}

	// ---- Mouse ----

	bool kGuiManager::isMouseDown(ImGuiMouseButton button)
	{
		return ImGui::IsMouseDown(button);
	}

	bool kGuiManager::isMouseClicked(ImGuiMouseButton button, bool repeat)
	{
		return ImGui::IsMouseClicked(button, repeat);
	}

	bool kGuiManager::isMouseReleased(ImGuiMouseButton button)
	{
		return ImGui::IsMouseReleased(button);
	}

	bool kGuiManager::isMouseDoubleClicked(ImGuiMouseButton button)
	{
		return ImGui::IsMouseDoubleClicked(button);
	}

	bool kGuiManager::isMouseHoveringRect(vec2 rMin, vec2 rMax, bool clip)
	{
		return ImGui::IsMouseHoveringRect(ImVec2(rMin.x, rMin.y), ImVec2(rMax.x, rMax.y), clip);
	}

	vec2 kGuiManager::getMousePos()
	{
		ImVec2 p = ImGui::GetMousePos();
		return vec2(p.x, p.y);
	}

	vec2 kGuiManager::getMouseDelta()
	{
		ImVec2 d = ImGui::GetIO().MouseDelta;
		return vec2(d.x, d.y);
	}

	float kGuiManager::getMouseWheel()
	{
		return ImGui::GetIO().MouseWheel;
	}

	void kGuiManager::destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}
}
