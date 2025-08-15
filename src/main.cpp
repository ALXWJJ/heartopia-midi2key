#include <iostream>
#include <string>
#include <windows.h> 
#include <thread> // 用于std::this_thread::sleep_for
#include <fstream> // 用于文件存在性检查

#include "midi_handler.h"
#include "key_mapper.h"
#include "key_emulator.h"

// 全局变量，用于存储当前按下的键，防止重复按键
std::unordered_map<std::string, bool> currentlyPressedKeys;

// MIDI消息处理函数
void handleMidiMessage(double deltaTime, const std::vector<unsigned char>& message,
                       KeyMapper& keyMapper, KeyEmulator& keyEmulator) {
    if (message.size() < 3) {
        return;
    }

    // 解析MIDI消息类型
    unsigned char messageType = message[0] & 0xF0;
    unsigned char midiNote = message[1];
    unsigned char velocity = message[2];

    // 获取对应的键盘按键
    std::string key = keyMapper.getKeyFromMidiNote(midiNote);
    if (key.empty()) {
        return;
    }

    // 处理音符按下事件
    if (messageType == 0x90 && velocity > 0) {
        if (!currentlyPressedKeys[key]) {
            std::cout << u8"音符按下: " << keyMapper.midiNoteToName(midiNote) << u8" -> 按键: " << key << std::endl;
            keyEmulator.pressKey(key);
            currentlyPressedKeys[key] = true;
        }
    }
    // 处理音符释放事件
    else if ((messageType == 0x80) || (messageType == 0x90 && velocity == 0)) {
        if (currentlyPressedKeys[key]) {
            std::cout << u8"音符释放: " << keyMapper.midiNoteToName(midiNote) << u8" -> 按键: " << key << std::endl;
            keyEmulator.releaseKey(key);
            currentlyPressedKeys[key] = false;
        }
    }
}

// 检查是否以管理员权限运行
bool IsRunAsAdmin() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, cbSize, &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

// 请求管理员权限重启程序
void RequestAdminRights() {
    wchar_t szPath[MAX_PATH];
    if (GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
        SHELLEXECUTEINFOW sei = {
            sizeof(sei),
            SEE_MASK_NOCLOSEPROCESS,
            NULL,
            L"runas",
            szPath,
            NULL,
            NULL,
            SW_SHOWNORMAL
        };
        ShellExecuteExW(&sei);
    }
}

int main(int argc, char* argv[]) {
    // 切换控制台编码为UTF-8
    system("chcp 65001 > nul");
    
    // 检查是否以管理员权限运行
    if (!IsRunAsAdmin()) {
        std::cout << u8"正在请求管理员权限..." << std::endl;
        RequestAdminRights();
        return 0;
    }

    std::cout << u8"Heartopia MIDI 2 KEY" << std::endl;
    std::cout << "======================================" << std::endl;

    // 初始化键盘模拟器
    KeyEmulator keyEmulator;
    if (!keyEmulator.initialize()) {
        std::cerr << u8"无法初始化键盘模拟器" << std::endl;
        return 1;
    }

    // 初始化键盘映射器
    KeyMapper keyMapper;

    // 获取可执行文件路径
    char exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::string exeDir = std::string(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\"));

    // 构建配置文件路径
    std::string configPath;
    if (argc > 1) {
        configPath = argv[1];
        std::cout << u8"使用命令行参数指定的配置文件: " << configPath << std::endl;

        // 检查指定的配置文件是否存在
        std::ifstream file(configPath);
        if (!file.good()) {
            // 如果不存在，尝试在可执行文件目录下查找
            std::string fileName = configPath.substr(configPath.find_last_of("\\/") + 1);
            std::string fallbackPath = exeDir + "\\" + fileName;
            //std::cout << u8"指定的配置文件不存在，尝试使用: " << fallbackPath << std::endl;
            configPath = fallbackPath;
        }
    } else {
        configPath = exeDir + "\\heartopia_piano.json";
        std::cout << u8"加载默认配置: heartopia_piano.json" << std::endl;
    }

    if (!keyMapper.loadKeyMapFromFile(configPath)) {
        std::cerr << u8"无法从文件加载键盘映射" << std::endl;
        return 1;
    }

    // 初始化MIDI处理
    MidiHandler midiHandler;
    if (!midiHandler.initialize()) {
        std::cerr << u8"无法初始化MIDI处理器" << std::endl;
        return 1;
    }

    // 设置MIDI消息回调
    midiHandler.setMidiCallback([&](double deltaTime, const std::vector<unsigned char>& message) {
        handleMidiMessage(deltaTime, message, keyMapper, keyEmulator);
    });

    // 获取可用MIDI设备列表
    std::vector<std::string> availableDevices = midiHandler.getAvailableDevices();
    unsigned int deviceId;

    if (availableDevices.size() == 1) {
        // 只有一个设备，自动选择
        deviceId = 0;
        //std::cout << u8"自动选择的MIDI设备: " << availableDevices[0] << std::endl;
    } else {
        // 让用户选择MIDI设备
        std::cout << u8"可用的MIDI设备:" << std::endl;
        for (unsigned int i = 0; i < availableDevices.size(); ++i) {
            std::cout << "  " << i << ": " << availableDevices[i] << std::endl;
        }
        std::cout << u8"请输入MIDI设备ID: ";
        std::cin >> deviceId;
    }

    if (!midiHandler.selectDevice(deviceId)) {
        std::cerr << u8"无法选择MIDI设备" << std::endl;
        return 1;
    }

    std::cout << u8"正在监听MIDI输入... (按Ctrl+C退出)" << std::endl;

    // 无限循环，保持程序运行
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 清理资源 (注意：由于无限循环，以下代码实际上不会执行)
    midiHandler.shutdown();
    keyEmulator.shutdown();

    std::cout << u8"程序已成功退出" << std::endl;
    return 0;
}
