#include "GUI.h"

void CopyToClipboard(const std::string& str) {
    // Open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard." << std::endl;
        return;
    }
    // Empty the clipboard
    if (!EmptyClipboard()) {
        std::cerr << "Failed to empty clipboard." << std::endl;
        CloseClipboard();
        return;
    }
    // Allocate global memory for the string
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, str.size() + 1);
    if (!hGlobal) {
        std::cerr << "Failed to allocate global memory." << std::endl;
        CloseClipboard();
        return;
    }
    // Copy the string to the allocated memory
    memcpy(GlobalLock(hGlobal), str.c_str(), str.size() + 1);
    GlobalUnlock(hGlobal);
    // Set the clipboard data
    if (!SetClipboardData(CF_TEXT, hGlobal)) {
        std::cerr << "Failed to set clipboard data." << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }
    // Close the clipboard
    CloseClipboard();
}

std::string GetClipboardText() {
    // Try to open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard." << std::endl;
        return "";
    }
    // Check if clipboard contains text
    if (!IsClipboardFormatAvailable(CF_TEXT)) {
        std::cerr << "Clipboard does not contain text." << std::endl;
        CloseClipboard();
        return "";
    }
    // Get the clipboard data handle
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        std::cerr << "Failed to get clipboard data." << std::endl;
        CloseClipboard();
        return "";
    }
    // Lock the handle to get the actual text pointer
    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        std::cerr << "Failed to lock global memory." << std::endl;
        CloseClipboard();
        return "";
    }
    // Copy the text to a std::string
    std::string text(pszText);
    // Unlock the memory
    GlobalUnlock(hData);
    // Close the clipboard
    CloseClipboard();
    return text;
}

std::string getRandomString(unsigned int lenght) {
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:<=>?@[\\]^_`{|}~";
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    // Define the distribution for integers
    std::uniform_int_distribution<> dis_int(0, chars.size() - 1); // Range
    std::string rs = "";
    for(unsigned int i = 0; i < lenght; i++) {
        rs += chars[dis_int(gen)];
    }
    return rs;
}

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void GUI::loadKernel() {
    std::filesystem::path filePath = "kernel";
    if (std::filesystem::exists(filePath)) {
        system("gpg --yes --decrypt --output file kernel");

        std::ifstream file("file");
        if (!file.is_open()) {
            std::cerr << "Failed to open kernel file" << std::endl;
            return;
        }

        std::string line;
        while (getline(file, line)) {
            std::vector<std::string> tokens = split(line, ';');
            Entry entry;
            entry.site = tokens[0];
            entry.email = tokens[1];
            entry.username = tokens[2];
            entry.password = tokens[3];
            entry.description = tokens[4];
            passwords.push_back(entry);
        }

        file.close();

        system("del file");
    }
}

void GUI::saveKernel() {
    std::ofstream file;
    file.open("file");

    // Check if the file was successfully opened
    if (!file) {
        std::cerr << "Failed to create the kernel file." << std::endl;
        return;
    }

    // Write data to the file
    for(unsigned int i = 0; i < passwords.size(); i++) {
        file << passwords[i].site + ";" + passwords[i].email + ";" + passwords[i].username + ";" + passwords[i].password + ";" + passwords[i].description + ";\n";
    }

    // Close the file
    file.close();

    system("gpg --yes --symmetric --cipher-algo AES256 --output kernel file");
    system("del file");
}

void GUI::init() {
    setFlagsFalse();
    loadKernel();
    kernelModified = false;
    editing = false;
    sureToDelete = false;
    saveChanges = false;
}

void GUI::setFlagsFalse() {
    addMode = editMode = deleteMode = emailMode = usernameMode = passwordMode = false;
}

void GUI::emptyStrings() {
    siteString = emailString = usernameString = passwordString = descriptionString = "";
}

void GUI::action() {
    if(editMode) {
        editing = true;
        kernelModified = true;
    }
    if(deleteMode) {
        sureToDelete = true;
    }
    if(emailMode) {
        CopyToClipboard(passwords[selected].email);
    }
    if(usernameMode) {
        CopyToClipboard(passwords[selected].username);
    }
    if(passwordMode) {
        CopyToClipboard(passwords[selected].password);
    }
}

void GUI::draw() {
    ImGui::Begin("Passwords");
    if(ImGui::Button("Add")) {
        setFlagsFalse();
        addMode = !addMode;
        emptyStrings();
    }
    ImGui::SameLine();
    if(ImGui::Button("Edit")) {
        setFlagsFalse();
        editMode = !editMode;
    }
    ImGui::SameLine();
    if(ImGui::Button("Delete")) {
        setFlagsFalse();
        deleteMode = !deleteMode;
    }
    ImGui::SameLine();
    if(ImGui::Button("Copy Email")) {
        setFlagsFalse();
        emailMode = !emailMode;
    }
    ImGui::SameLine();
    if(ImGui::Button("Copy Username")) {
        setFlagsFalse();
        usernameMode = !usernameMode;
    }
    ImGui::SameLine();
    if(ImGui::Button("Copy Password")) {
        setFlagsFalse();
        passwordMode = !passwordMode;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    for(unsigned int i = 0; i < passwords.size(); i++) {
        if(editMode || deleteMode || emailMode || usernameMode || passwordMode) {
            if(ImGui::Button(std::to_string(i).c_str())) {
                selected = i;
                action();
            }
            ImGui::SameLine();
        }
        ImGui::Text((" - " + passwords[i].site).c_str());
    }

    if(addMode) {
        ImGui::Begin("Add");
        ImGui::Text("Site:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##0", &siteString);
        ImGui::Text("Email:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##1", &emailString);
        ImGui::Text("Username:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##2", &usernameString);
        ImGui::Text("Password:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##3", &passwordString);
        ImGui::SameLine();
        if(ImGui::Button("Generate")) {
            passwordString = getRandomString(20);
        }
        ImGui::SameLine();
        if(ImGui::Button("Copy")) {
            CopyToClipboard(passwordString);
        }
        ImGui::SameLine();
        if(ImGui::Button("Paste")) {
            passwordString = GetClipboardText();
        }
        ImGui::Text("Description:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##4", &descriptionString);
        if(ImGui::Button("Add")) {
            Entry entry;
            entry.site = siteString;
            entry.email = emailString;
            entry.username = usernameString;
            entry.password = passwordString;
            entry.description = descriptionString;
            passwords.push_back(entry);
            kernelModified = true;
            setFlagsFalse();
        }
        ImGui::SameLine();
        if(ImGui::Button("Close")) {
            setFlagsFalse();
        }
        ImGui::End();
    }

    if(editing) {
        ImGui::Begin("Edit");
        ImGui::Text("Site:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##0", &passwords[selected].site);
        ImGui::Text("Email:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##1", &passwords[selected].email);
        ImGui::Text("Username:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##2", &passwords[selected].username);
        ImGui::Text("Password:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##3", &passwords[selected].password);
        ImGui::Text("Description:"); ImGui::SameLine(); ImGui::SetNextItemWidth(200); ImGui::InputText("##4", &passwords[selected].description);
        if(ImGui::Button("Close")) {
            editing = false;
        }
        ImGui::End();
    }

    if(sureToDelete) {
        ImGui::Begin("Delete");
        ImGui::Text("Are you sure that you want to delete this entry?");
        if(ImGui::Button("Yes")) {
            passwords.erase(passwords.begin() + selected);
            sureToDelete = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("No")) {
            sureToDelete = false;
        }
        ImGui::End();
    }

    if(saveChanges) {
        ImGui::Begin("Save changes");
        ImGui::Text("Exit saving changes?");
        if(ImGui::Button("Yes")) {
            saveKernel();
            kernelModified = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Discard")) {
            kernelModified = false;
        }
        ImGui::End();
    }

    if(kernelModified) {
        ImGui::Begin("Kernel modified");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unsaved changes");
        if(ImGui::Button("Save")) {
            saveKernel();
            kernelModified = false;
        }
        ImGui::End();
    }

    ImGui::End();
}