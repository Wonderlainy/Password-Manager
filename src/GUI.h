#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <random>
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

std::string getRandomString(unsigned int lenght);
std::vector<std::string> split(const std::string& line, char delimiter);

struct Entry {
    std::string site;
    std::string email;
    std::string username;
    std::string password;
    std::string description;
};

struct GUI {

    std::vector<Entry> passwords;
    unsigned int selected;

    std::string siteString;
    std::string emailString;
    std::string usernameString;
    std::string passwordString;
    std::string descriptionString;

    bool addMode;
    bool editMode;      bool editing;
    bool deleteMode;    bool sureToDelete;
    bool emailMode;
    bool usernameMode;
    bool passwordMode;

    bool kernelModified; bool saveChanges;

    void setFlagsFalse();
    void emptyStrings();

    void loadKernel();
    void saveKernel();

    void action();

    void init();
    void draw();
};