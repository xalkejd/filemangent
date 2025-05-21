#pragma once

#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace fs = std::filesystem;

struct MenuItem {
    float x, y, w, h;
    std::string index;
};
std::vector<MenuItem> menuTOOL;
std::vector<MenuItem> menuHistory;


static int winWidth = 800;
static int winHeight = 600;

const int ICON_SIZE = 64;
const int PADDING = 20;
const int TOOLBAR_H = 40;
const int SEARCHBAR_H = 30;
const int ROW_EXTRA = 15;
const size_t MAX_NAME_LEN = 11;

enum class MenuType { NONE, FILE, HOME, VIEW };


enum ContextType { DELETE_ONLY, CREATE_ONLY };
ContextType menuContextType;


enum class MenuAction {
    CREATE_FILE,
    CREATE_FOLDER,
    OPEN_PAGE,
    PASTE,
    COPY,
    MOVE,
    ERASE,
    BIG,
    MEDIUM,
    SMALL,
};
MenuAction actionFromLabel(const std::string& label) {
    if (label == "Crea nuovo file")        return MenuAction::CREATE_FILE;
    if (label == "Crea nuova cartella")   return MenuAction::CREATE_FOLDER;
    if (label == "Apri nuova pagina")      return MenuAction::OPEN_PAGE;
    if (label == "Incolla")                return MenuAction::PASTE;
    if (label == "Copia")                  return MenuAction::COPY;
    if (label == "Sposta in")              return MenuAction::MOVE;
    if (label == "Elimina")                return MenuAction::ERASE;
    if (label == "Icone grandi")           return MenuAction::BIG;
    if (label == "Icone medie")           return MenuAction::MEDIUM;
    if (label == "Icone piccole")          return MenuAction::SMALL;
}

class Main {
public:
    Main() {}
    virtual ~Main() = default;
    static bool showRightClickMenu;
    static double rightClickX, rightClickY;
    static MenuType menuAperto;
    static fs::path currentPath;
    static std::vector<fs::directory_entry> entries;
    static std::string searchQuery;
    static bool searchActive;
    static std::vector<std::string> searchHistory;
    virtual  void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    using IconDrawer = std::function<void(float, float, float)>;
    static std::unordered_map<std::string, IconDrawer> iconDrawers;

    static void drawRect(float x, float y, float w, float h, float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
        glEnd();
    }

    static void drawText(float x, float y, const std::string& text) {
        glColor3f(0, 0, 0);
        glRasterPos2f(x, y);
        for (char c : text)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    int start(int argc, char** argv);

private:

    static void drawIconFolder(float x, float y, float s) {
        drawRect(x, y, s, s * 0.7f, 1.0f, 0.9f, 0.3f);

        drawRect(x + s * 0.05f, y - s * 0.15f, s * 0.5f, s * 0.2f, 1.0f, 0.83f, 0.2f);
    }
    static void drawIconImage(float x, float y, float s) {
        drawRect(x, y, s, s, 0.4f, 0.8f, 0.4f);

        glColor3f(0.2f, 0.5f, 0.2f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x + s * 0.1f, y + s * 0.9f);
        glVertex2f(x + s * 0.5f, y + s * 0.4f);
        glVertex2f(x + s * 0.9f, y + s * 0.9f);
        glEnd();

        drawRect(x + s * 0.7f, y + s * 0.7f, s * 0.2f, s * 0.2f, 1.0f, 1.0f, 0.3f);
    }

    static void drawIconVideo(float x, float y, float s) {
        drawRect(x, y, s, s, 0.8f, 0.3f, 0.3f);
        glColor3f(1, 1, 1);
        glBegin(GL_TRIANGLES);
        glVertex2f(x + s * 0.4f, y + s * 0.3f);
        glVertex2f(x + s * 0.7f, y + s * 0.5f);
        glVertex2f(x + s * 0.4f, y + s * 0.7f);
        glEnd();
    }

    static void drawIconText(float x, float y, float s) {
        drawRect(x, y, s, s, 1.0f, 1.0f, 1.0f);
        glColor3f(0, 0, 0);
        for (int i = 0; i < 4; ++i) {
            float yy = y + s * (0.2f + i * 0.15f);
            glBegin(GL_LINES);
            glVertex2f(x + s * 0.1f, yy);
            glVertex2f(x + s * 0.9f, yy);
            glEnd();
        }
    }
    static void drawIconUnknown(float x, float y, float s) {
        drawRect(x, y, s, s, 0.6f, 0.6f, 0.6f);
        glColor3f(1, 1, 1);
        drawText(x + s * 0.4f, y + s * 0.6f, "?");
    }

    static void framebufferSizeCallback(GLFWwindow*, int width, int height) {
        winWidth = width;
        winHeight = height;
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    static void updateDirectory(const fs::path& path) {
        currentPath = path;
        entries.clear();
        if (path.has_parent_path())
            entries.emplace_back(path.parent_path());
        for (auto& e : fs::directory_iterator(path))
            entries.emplace_back(e);
    }



    static void drawIconTexture(GLuint id, float x, float y, float w, float h) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(1, 0); glVertex2f(x + w, y);
        glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
        glTexCoord2f(0, 1); glVertex2f(x, y + h);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    static bool inside(double mx, double my, float x, float y, float w, float h) {
        return mx >= x && mx <= x + w && my >= y && my <= y + h;
    }

    static void drawToolbar() {
        drawRect(0, 0, (float)winWidth, (float)TOOLBAR_H, 0.5f, 0.5f, 0.5f);
        drawText(20, 25, "File");
        drawText(100, 25, "Home");
        drawText(200, 25, "Visualizza");
    }

    static void drawSearchBar() {
        drawRect(0, TOOLBAR_H, (float)winWidth, (float)SEARCHBAR_H, 0.9f, 0.9f, 0.9f);
        drawText(10, TOOLBAR_H + 20, "Search: " + searchQuery);
    }

    static void drawFileGrid() {
        float x = PADDING;
        float y = TOOLBAR_H + SEARCHBAR_H + PADDING;
        float colSpacing = ICON_SIZE + PADDING * 2;
        float rowSpacing = ICON_SIZE + PADDING * 2 + ROW_EXTRA;

        for (size_t i = 0; i < entries.size(); ++i) {
            auto& e = entries[i];
            std::string key, name;
            if (i == 0 && e.path() == currentPath.parent_path()) {
                key = "parent";
                name = "...";
            }
            else {
                name = e.path().filename().string();
                if (!searchQuery.empty() && name.find(searchQuery) == std::string::npos)
                    continue;
                if (name.size() > MAX_NAME_LEN)
                    name = name.substr(0, MAX_NAME_LEN - 3) + "...";
                key = e.is_directory() ? "folder" : e.path().extension().string();
            }
            auto it = iconDrawers.find(key);
            if (it != iconDrawers.end()) {
                it->second(x, y, ICON_SIZE);
            }
            else {
                drawIconUnknown(x, y, ICON_SIZE);
            }

            drawText(x, y + ICON_SIZE + 15, name);

            x += colSpacing;
            if (x + ICON_SIZE + PADDING > winWidth) {
                x = PADDING;
                y += rowSpacing;
            }
        }
    }

    static void drawContextMenu() {
        if (!showRightClickMenu) return;
        drawRect(rightClickX, rightClickY, 180, 30, 0.8f, 0.8f, 0.8f);

        if (menuContextType == DELETE_ONLY) {
            drawText(rightClickX + 10, rightClickY + 20, "Elimina");
        }
        else {
            drawRect(rightClickX, rightClickY, 180, 60, 0.8f, 0.8f, 0.8f);
            drawText(rightClickX + 10, rightClickY + 20, "Crea nuova cartella");
            drawText(rightClickX + 10, rightClickY + 40, "Crea nuovo file");
        }
    }

    static void drawTopMenus() {
        float baseY = TOOLBAR_H + SEARCHBAR_H;
        switch (menuAperto) {
        case MenuType::FILE:
            drawMenuItems({ "Crea nuovo file","Crea nuova cartella" },
                20, baseY, menuTOOL);
            break;
        case MenuType::HOME:
            drawMenuItems({ "Copia","Incolla","Sposta in","Elimina" },
                100, baseY, menuTOOL);
            break;
        case MenuType::VIEW:
            drawMenuItems({ "Icone grandi","Icone medie ","Icone piccole" },
                200, baseY, menuTOOL);
            break;
        default: break;
        }
    }

    static void drawMenuItems(const std::vector<std::string>& items, float x, float y, std::vector<MenuItem>& outMenu) {
        float h = 30, w = 200;
        drawRect(x, y, w, h * items.size(), 0.9f, 0.9f, 0.9f);
        for (size_t i = 0; i < items.size(); ++i) {
            float iy = y + i * h;
            outMenu.push_back({ x,iy,w,h,items[i] });
            drawText(x + 10, y + (i + 1) * h - 10, items[i]);
        }
    }

    static void characterCallback(GLFWwindow*, unsigned int codepoint) {
        if (searchActive && codepoint >= 32 && codepoint <= 126)
            searchQuery += static_cast<char>(codepoint);
    }

    static void keyCallback(GLFWwindow*, int key, int, int action, int) {
        if (searchActive && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            if (key == GLFW_KEY_BACKSPACE && !searchQuery.empty())
                searchQuery.pop_back();
            else if (key == GLFW_KEY_ENTER) {
                searchActive = false;
                if (!searchQuery.empty()) {
                    auto it = std::find(searchHistory.begin(), searchHistory.end(), searchQuery);
                    if (it == searchHistory.end()) {
                        if (searchHistory.size() > 8) {
                            searchHistory.pop_back();
                        }
                        searchHistory.push_back(searchQuery);
                    }
                }
            }

        }
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
            searchActive = false;
    }
};

bool Main::showRightClickMenu = false;
double Main::rightClickX = 0;
double Main::rightClickY = 0;
MenuType Main::menuAperto = MenuType::NONE;
fs::path Main::currentPath;
std::vector<fs::directory_entry> Main::entries;
std::string Main::searchQuery;
bool Main::searchActive = false;
std::vector<std::string> Main::searchHistory;