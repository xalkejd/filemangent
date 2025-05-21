#include "main.h"

std::unordered_map<std::string, Main::IconDrawer> Main::iconDrawers = {
    { "parent", [](float x, float y, float s) { drawIconFolder(x, y, s); } },
    { "folder", [](float x, float y, float s) { drawIconFolder(x, y, s); } },
    { ".png",   [](float x, float y, float s) { drawIconImage(x, y, s); } },
    { ".jpg",   [](float x, float y, float s) { drawIconImage(x, y, s); } },
    { ".jpeg",  [](float x, float y, float s) { drawIconImage(x, y, s); } },
    { ".webp",  [](float x, float y, float s) { drawIconImage(x, y, s); } },
    { ".mp4",   [](float x, float y, float s) { drawIconVideo(x, y, s); } },
    { ".avi",   [](float x, float y, float s) { drawIconVideo(x, y, s); } },
    { ".mov",   [](float x, float y, float s) { drawIconVideo(x, y, s); } },
    { ".webm",   [](float x, float y, float s) { drawIconVideo(x, y, s); } },
    { ".txt",   [](float x, float y, float s) { drawIconText(x, y, s); } },
    { ".doc",   [](float x, float y, float s) { drawIconText(x, y, s); } },
    { ".pdf",   [](float x, float y, float s) { drawIconText(x, y, s); } }
};

Main app;

int Main::start(int argc, char** argv) {
    glutInit(&argc, argv);
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "File Manager GUI", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int width, int height) {
        Main* self = static_cast<Main*>(glfwGetWindowUserPointer(w));
        if (self) self->framebufferSizeCallback(w, width, height);
        });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
        Main* self = static_cast<Main*>(glfwGetWindowUserPointer(w));
        if (self) self->mouseButtonCallback(w, button, action, mods);
        });
    glfwSetCharCallback(window, [](GLFWwindow* w, unsigned int codepoint) {
        Main* self = static_cast<Main*>(glfwGetWindowUserPointer(w));
        if (self) self->characterCallback(w, codepoint);
        });
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int sc, int action, int mods) {
        Main* self = static_cast<Main*>(glfwGetWindowUserPointer(w));
        if (self) self->keyCallback(w, key, sc, action, mods);
        });

    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        framebufferSizeCallback(window, width, height);
    }

    updateDirectory(fs::current_path());

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawToolbar();
        drawSearchBar();
        if (!searchHistory.empty())
            drawMenuItems(searchHistory, 400, TOOLBAR_H + SEARCHBAR_H,menuHistory);
        drawFileGrid();
        drawContextMenu();
        drawTopMenus();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void Main::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double mx, my; 
    
    glfwGetCursorPos(window, &mx, &my);
    bool onEntry = false;

    float x0 = PADDING;
    float y0 = TOOLBAR_H + SEARCHBAR_H + PADDING;
    float colSpacing = ICON_SIZE + PADDING * 2;
    float rowSpacing = ICON_SIZE + PADDING * 2 + ROW_EXTRA;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        showRightClickMenu = false;
        if (my >= TOOLBAR_H && my <= TOOLBAR_H + SEARCHBAR_H) {
            searchActive = true;
        }
        else {
            auto it = std::find(searchHistory.begin(), searchHistory.end(), searchQuery);
            if (!searchQuery.empty() && searchActive && it == searchHistory.end()) {
                if (searchHistory.size() > 8) {
                    searchHistory.pop_back();
                }
                searchHistory.push_back(searchQuery);
            }
            searchActive = false;
            if (inside(mx, my, 20, 5, 60, TOOLBAR_H - 10))
                menuAperto = (menuAperto == MenuType::NONE ? MenuType::FILE : MenuType::NONE);
            else if (inside(mx, my, 100, 5, 80, TOOLBAR_H - 10))
                menuAperto = (menuAperto == MenuType::HOME ? MenuType::NONE : MenuType::HOME);
            else if (inside(mx, my, 200, 5, 120, TOOLBAR_H - 10))
                menuAperto = (menuAperto == MenuType::VIEW ? MenuType::NONE : MenuType::VIEW);
            else {
                for (size_t i = 0; i < entries.size(); ++i) {
                    if (inside(mx, my, x0, y0, ICON_SIZE, ICON_SIZE)) {
                            onEntry = true;
                        if (i == 0 && entries[i].path() == currentPath.parent_path())
                            updateDirectory(currentPath.parent_path());
                        else if (entries[i].is_directory()) {
                            std::string nome = entries[i].path().filename().string();
                            fs::path newPath = currentPath / nome;
                            updateDirectory(newPath);
                         
                        } break;
                    }
                    for(auto& reg: menuTOOL){
                        if (mx >= reg.x && mx <= reg.x + reg.w && my >= reg.y && my <= reg.y + reg.h){
                            actionFromLabel(reg.index);
                        }
                  }

                    for (auto& reg : menuHistory) {
                        if (mx >= reg.x && mx <= reg.x + reg.w && my >= reg.y && my <= reg.y + reg.h) {
                            
                        }
                    }
                    x0 += colSpacing;
                    if (x0 + ICON_SIZE + PADDING > winWidth) {
                        x0 = PADDING;
                        y0 += rowSpacing;
                    }
                }
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rightClickX = mx; rightClickY = my; showRightClickMenu = true;
        if (!onEntry) {
            float x = x0, y = y0;
            for (size_t i = 0; i < entries.size(); ++i) {
                if (inside(mx, my, x, y, ICON_SIZE, ICON_SIZE)) {
                    onEntry = true;
                    break;
                }
                x += colSpacing;
                if (x + ICON_SIZE + PADDING > winWidth) {
                    x = x0;
                    y += rowSpacing;
                }
            }
        }
        menuContextType = onEntry ? ContextType::DELETE_ONLY : ContextType::CREATE_ONLY;
    }

}

int main(int argc, char** argv) {return app.start(argc, argv);}