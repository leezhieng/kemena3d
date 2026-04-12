#ifndef KGUIMANAGER_H
#define KGUIMANAGER_H

#include "kexport.h"

#include "kdatatype.h"
#include "krenderer.h"
#include "kwindow.h"

#include <SDL3/SDL.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

namespace kemena
{
	class KEMENA3D_API kGuiManager
	{
	public:
		kGuiManager();
		virtual ~kGuiManager();

		void init(kRenderer *newRenderer);
		void processEvent(kSystemEvent event);

		void loadDefaultFontFromResource(string resourceName);

		void canvasStart();
		void canvasEnd();

		// ---- Window ----
		void windowStart(string title, bool *open = nullptr, ImGuiWindowFlags flags = 0);
		void windowEnd();
		vec2 getWindowSize();
		vec2 getWindowPos();
		void setNextWindowSize(vec2 size, ImGuiCond cond = 0);
		void setNextWindowPos(vec2 pos, ImGuiCond cond = 0, vec2 pivot = vec2(0, 0));
		void setNextWindowContentSize(vec2 size);
		void setNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);
		void setNextWindowFocus();
		void setNextWindowBgAlpha(float alpha);
		bool isWindowFocused(ImGuiFocusedFlags flags = 0);
		bool isWindowHovered(ImGuiHoveredFlags flags = 0);

		// ---- Dockspace ----
		void dockSpaceStart(string name);
		void dockSpaceEnd();

		// ---- Menu ----
		bool menuBar();
		void menuBarEnd();
		bool menu(string text);
		void menuEnd();
		bool menuItem(string text, string shortcut = "", bool selected = false, bool enabled = true);
		bool menuItem(string text, string shortcut, bool *selected, bool enabled = true);

		// ---- Layout ----
		void groupStart();
		void groupEnd();
		void sameLine(float offsetFromStartX = 0.0f, float spacing = -1.0f);
		void spacing();
		void separator();
		void separatorText(string text);
		void newLine();
		void indent(float indentW = 0.0f);
		void unindent(float indentW = 0.0f);
		void dummy(vec2 size);
		void setCursorPos(vec2 pos);
		void setCursorPosX(float x);
		void setCursorPosY(float y);
		vec2 getCursorPos();
		float getCursorPosX();
		float getCursorPosY();
		vec2 getCursorScreenPos();
		vec2 getContentRegionAvail();
		void setNextItemWidth(float itemWidth);
		void pushItemWidth(float itemWidth);
		void popItemWidth();
		float calcItemWidth();
		void pushTextWrapPos(float wrapLocalPosX = 0.0f);
		void popTextWrapPos();

		// ---- ID Stack ----
		void pushId(string id);
		void pushId(int id);
		void pushId(const void *ptr);
		void popId();

		// ---- Style ----
		void pushStyleColor(ImGuiCol idx, vec4 color);
		void pushStyleColor(ImGuiCol idx, ImU32 color);
		void popStyleColor(int count = 1);
		void pushStyleVar(ImGuiStyleVar idx, float val);
		void pushStyleVar(ImGuiStyleVar idx, vec2 val);
		void popStyleVar(int count = 1);
		void pushFont(ImFont *font);
		void popFont();

		// ---- Text ----
		void text(string text);
		void textColored(vec4 color, string text);
		void textDisabled(string text);
		void textWrapped(string text);
		void labelText(string label, string text);
		void bulletText(string text);
		void bullet();

		// ---- Buttons ----
		bool button(string text, ivec2 size = ivec2(0, 0));
		bool smallButton(string text);
		bool invisibleButton(string id, vec2 size, ImGuiButtonFlags flags = 0);
		bool arrowButton(string id, ImGuiDir dir);
		bool radioButton(string label, bool active);
		bool radioButton(string label, int *v, int vButton);

		// ---- Checkbox ----
		bool checkbox(string text, bool *output);
		bool checkboxFlags(string label, int *flags, int flagsValue);
		bool checkboxFlags(string label, unsigned int *flags, unsigned int flagsValue);

		// ---- Input Text ----
		bool inputText(string label, string &value, size_t maxLength = 256, ImGuiInputTextFlags flags = 0);
		bool inputTextMultiline(string label, string &value, size_t maxLength = 1024, vec2 size = vec2(0, 0), ImGuiInputTextFlags flags = 0);
		bool inputTextWithHint(string label, string hint, string &value, size_t maxLength = 256, ImGuiInputTextFlags flags = 0);

		// ---- Input Scalar ----
		bool inputFloat(string label, float *v, float step = 0.0f, float stepFast = 0.0f, string format = "%.3f", ImGuiInputTextFlags flags = 0);
		bool inputFloat2(string label, float v[2], string format = "%.3f", ImGuiInputTextFlags flags = 0);
		bool inputFloat3(string label, float v[3], string format = "%.3f", ImGuiInputTextFlags flags = 0);
		bool inputFloat4(string label, float v[4], string format = "%.3f", ImGuiInputTextFlags flags = 0);
		bool inputInt(string label, int *v, int step = 1, int stepFast = 100, ImGuiInputTextFlags flags = 0);
		bool inputInt2(string label, int v[2], ImGuiInputTextFlags flags = 0);
		bool inputInt3(string label, int v[3], ImGuiInputTextFlags flags = 0);
		bool inputInt4(string label, int v[4], ImGuiInputTextFlags flags = 0);
		bool inputDouble(string label, double *v, double step = 0.0, double stepFast = 0.0, string format = "%.6f", ImGuiInputTextFlags flags = 0);

		// ---- Drag ----
		bool dragFloat(string label, float *v, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool dragFloat2(string label, float v[2], float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool dragFloat3(string label, float v[3], float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool dragFloat4(string label, float v[4], float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool dragInt(string label, int *v, float speed = 1.0f, int min = 0, int max = 0, string format = "%d", ImGuiSliderFlags flags = 0);
		bool dragInt2(string label, int v[2], float speed = 1.0f, int min = 0, int max = 0, string format = "%d", ImGuiSliderFlags flags = 0);
		bool dragInt3(string label, int v[3], float speed = 1.0f, int min = 0, int max = 0, string format = "%d", ImGuiSliderFlags flags = 0);
		bool dragInt4(string label, int v[4], float speed = 1.0f, int min = 0, int max = 0, string format = "%d", ImGuiSliderFlags flags = 0);
		bool dragFloatRange2(string label, float *vCurrentMin, float *vCurrentMax, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f", string formatMax = "", ImGuiSliderFlags flags = 0);
		bool dragIntRange2(string label, int *vCurrentMin, int *vCurrentMax, float speed = 1.0f, int min = 0, int max = 0, string format = "%d", string formatMax = "", ImGuiSliderFlags flags = 0);

		// ---- Slider ----
		bool sliderFloat(string label, float *v, float min, float max, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool sliderFloat2(string label, float v[2], float min, float max, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool sliderFloat3(string label, float v[3], float min, float max, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool sliderFloat4(string label, float v[4], float min, float max, string format = "%.3f", ImGuiSliderFlags flags = 0);
		bool sliderAngle(string label, float *vRad, float vDegreesMin = -360.0f, float vDegreesMax = 360.0f, string format = "%.0f deg", ImGuiSliderFlags flags = 0);
		bool sliderInt(string label, int *v, int min, int max, string format = "%d", ImGuiSliderFlags flags = 0);
		bool sliderInt2(string label, int v[2], int min, int max, string format = "%d", ImGuiSliderFlags flags = 0);
		bool sliderInt3(string label, int v[3], int min, int max, string format = "%d", ImGuiSliderFlags flags = 0);
		bool sliderInt4(string label, int v[4], int min, int max, string format = "%d", ImGuiSliderFlags flags = 0);

		// ---- Color ----
		bool colorEdit3(string label, float col[3], ImGuiColorEditFlags flags = 0);
		bool colorEdit4(string label, float col[4], ImGuiColorEditFlags flags = 0);
		bool colorPicker3(string label, float col[3], ImGuiColorEditFlags flags = 0);
		bool colorPicker4(string label, float col[4], ImGuiColorEditFlags flags = 0);
		bool colorButton(string descId, vec4 col, ImGuiColorEditFlags flags = 0, vec2 size = vec2(0, 0));
		void setColorEditOptions(ImGuiColorEditFlags flags);

		// ---- Combo / List / Selectable ----
		bool combo(string label, int *currentItem, std::vector<string> items, int popupMaxHeightInItems = -1);
		bool listBox(string label, int *currentItem, std::vector<string> items, int heightInItems = -1);
		bool selectable(string label, bool selected = false, ImGuiSelectableFlags flags = 0, vec2 size = vec2(0, 0));
		bool selectable(string label, bool *pSelected, ImGuiSelectableFlags flags = 0, vec2 size = vec2(0, 0));

		// ---- Tree / Collapsing ----
		bool treeStart(string label);
		bool treeStartEx(string id, string label = "", ImGuiTreeNodeFlags flags = 0);
		void treeEnd();
		void treePop();
		bool collapsingHeader(string label, ImGuiTreeNodeFlags flags = 0);
		bool collapsingHeader(string label, bool *visible, ImGuiTreeNodeFlags flags = 0);
		void setNextItemOpen(bool isOpen, ImGuiCond cond = 0);

		// ---- Image ----
		void image(GLuint textureId, vec2 size, vec2 uv0 = vec2(0, 0), vec2 uv1 = vec2(1, 1));
		bool imageButton(string id, GLuint textureId, vec2 size, vec2 uv0 = vec2(0, 0), vec2 uv1 = vec2(1, 1));

		// ---- Progress ----
		void progressBar(float fraction, vec2 size = vec2(-FLT_MIN, 0), string overlay = "");

		// ---- Tooltip ----
		void setItemTooltip(string text);
		void beginTooltip();
		void endTooltip();

		// ---- Popup ----
		void openPopup(string id, ImGuiPopupFlags flags = 0);
		bool popupStart(string id, ImGuiWindowFlags flags = 0);
		void popupEnd();
		bool popupModal(string name, bool *open = nullptr, ImGuiWindowFlags flags = 0);
		bool popupContextItemStart(string id = "", ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonRight);
		bool popupContextWindowStart(string id = "", ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonRight);
		bool isPopupOpen(string id, ImGuiPopupFlags flags = 0);
		void closeCurrentPopup();

		// ---- Tab Bar ----
		bool tabBarStart(string id, ImGuiTabBarFlags flags = 0);
		void tabBarEnd();
		bool tabItemStart(string label, bool *open = nullptr, ImGuiTabItemFlags flags = 0);
		void tabItemEnd();
		void setTabItemClosed(string tabOrDockedWindowLabel);

		// ---- Tables ----
		bool tableStart(string id, int columns, ImGuiTableFlags flags = 0, vec2 outerSize = vec2(0, 0), float innerWidth = 0.0f);
		void tableEnd();
		bool tableNextColumn();
		bool tableSetColumnIndex(int columnN);
		void tableNextRow(ImGuiTableRowFlags rowFlags = 0, float minRowHeight = 0.0f);
		void tableSetupColumn(string label, ImGuiTableColumnFlags flags = 0, float initWidthOrWeight = 0.0f);
		void tableSetupScrollFreeze(int cols, int rows);
		void tableHeadersRow();
		void tableHeader(string label);

		// ---- Scroll ----
		float getScrollX();
		float getScrollY();
		void setScrollX(float scrollX);
		void setScrollY(float scrollY);
		float getScrollMaxX();
		float getScrollMaxY();
		void setScrollHereX(float centerXRatio = 0.5f);
		void setScrollHereY(float centerYRatio = 0.5f);

		// ---- Item Queries ----
		bool isItemHovered(ImGuiHoveredFlags flags = 0);
		bool isItemActive();
		bool isItemFocused();
		bool isItemClicked(ImGuiMouseButton mouseButton = 0);
		bool isItemVisible();
		bool isItemEdited();
		bool isItemActivated();
		bool isItemDeactivated();
		bool isItemDeactivatedAfterEdit();
		bool isItemToggledOpen();
		bool isAnyItemHovered();
		bool isAnyItemActive();
		bool isAnyItemFocused();
		vec2 getItemRectMin();
		vec2 getItemRectMax();
		vec2 getItemRectSize();

		// ---- Mouse ----
		bool isMouseDown(ImGuiMouseButton button);
		bool isMouseClicked(ImGuiMouseButton button, bool repeat = false);
		bool isMouseReleased(ImGuiMouseButton button);
		bool isMouseDoubleClicked(ImGuiMouseButton button);
		bool isMouseHoveringRect(vec2 rMin, vec2 rMax, bool clip = true);
		vec2 getMousePos();
		vec2 getMouseDelta();
		float getMouseWheel();

		void destroy();

	protected:
	private:
		float mainScale;
		kRenderer *renderer;
	};
}

#endif // KGUIMANAGER_H
