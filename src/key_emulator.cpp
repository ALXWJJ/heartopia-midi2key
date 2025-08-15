#include "key_emulator.h"
#include <Windows.h>
#include <unordered_map>
#include <iostream>
#include <thread>

KeyEmulator::KeyEmulator() {
    initializeScanCodeMap();
}

KeyEmulator::~KeyEmulator() {
    shutdown();
}

bool KeyEmulator::initialize() {
    // Windows平台不需要特别的初始化
    //std::cout << u8"键盘模拟器已初始化" << std::endl;
    return true;
}

bool KeyEmulator::pressKey(const std::string& key) {
    if (!isValidKey(key)) {
        std::cerr << u8"无效的按键: " << key << std::endl;
        return false;
    }

    return sendKeyEvent(key, true);
}

bool KeyEmulator::releaseKey(const std::string& key) {
    if (!isValidKey(key)) {
        std::cerr << "Invalid key: " << key << std::endl;
        return false;
    }

    return sendKeyEvent(key, false);
}

bool KeyEmulator::clickKey(const std::string& key, int pressDurationMs) {
    if (!pressKey(key)) {
        return false;
    }

    // 等待指定的按键持续时间
    std::this_thread::sleep_for(std::chrono::milliseconds(pressDurationMs));

    return releaseKey(key);
}

void KeyEmulator::shutdown() {
    // Windows平台不需要特别的关闭操作
    std::cout << u8"键盘模拟器已关闭" << std::endl;
}

bool KeyEmulator::sendKeyEvent(const std::string& key, bool isPress) {
    auto it = keyToScanCodeMap_.find(key);
    if (it == keyToScanCodeMap_.end()) {
        std::cerr << u8"在扫描码映射中未找到按键: " << key << std::endl;
        return false;
    }

    int scanCode = it->second;

    // 构造INPUT结构
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = scanCode;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;

    if (!isPress) {
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
    }

    // 发送键盘事件
    UINT result = SendInput(1, &input, sizeof(INPUT));
    return result == 1;
}

bool KeyEmulator::isValidKey(const std::string& key) {
    return keyToScanCodeMap_.find(key) != keyToScanCodeMap_.end();
}

void KeyEmulator::initializeScanCodeMap() {
    // 清空映射表
    keyToScanCodeMap_.clear();
    // 字母键 (小写和大写)
    keyToScanCodeMap_["a"] = 0x1E;
    keyToScanCodeMap_["A"] = 0x1E;
    keyToScanCodeMap_["b"] = 0x30;
    keyToScanCodeMap_["B"] = 0x30;
    keyToScanCodeMap_["c"] = 0x2E;
    keyToScanCodeMap_["C"] = 0x2E;
    keyToScanCodeMap_["d"] = 0x20;
    keyToScanCodeMap_["D"] = 0x20;
    keyToScanCodeMap_["e"] = 0x12;
    keyToScanCodeMap_["E"] = 0x12;
    keyToScanCodeMap_["f"] = 0x21;
    keyToScanCodeMap_["F"] = 0x21;
    keyToScanCodeMap_["g"] = 0x22;
    keyToScanCodeMap_["G"] = 0x22;
    keyToScanCodeMap_["h"] = 0x23;
    keyToScanCodeMap_["H"] = 0x23;
    keyToScanCodeMap_["i"] = 0x17;
    keyToScanCodeMap_["I"] = 0x17;
    keyToScanCodeMap_["j"] = 0x24;
    keyToScanCodeMap_["J"] = 0x24;
    keyToScanCodeMap_["k"] = 0x25;
    keyToScanCodeMap_["K"] = 0x25;
    keyToScanCodeMap_["l"] = 0x26;
    keyToScanCodeMap_["L"] = 0x26;
    keyToScanCodeMap_["m"] = 0x32;
    keyToScanCodeMap_["M"] = 0x32;
    keyToScanCodeMap_["n"] = 0x31;
    keyToScanCodeMap_["N"] = 0x31;
    keyToScanCodeMap_["o"] = 0x18;
    keyToScanCodeMap_["O"] = 0x18;
    keyToScanCodeMap_["p"] = 0x19;
    keyToScanCodeMap_["P"] = 0x19;
    keyToScanCodeMap_["q"] = 0x10;
    keyToScanCodeMap_["Q"] = 0x10;
    keyToScanCodeMap_["r"] = 0x13;
    keyToScanCodeMap_["R"] = 0x13;
    keyToScanCodeMap_["s"] = 0x1F;
    keyToScanCodeMap_["S"] = 0x1F;
    keyToScanCodeMap_["t"] = 0x14;
    keyToScanCodeMap_["T"] = 0x14;
    keyToScanCodeMap_["u"] = 0x16;
    keyToScanCodeMap_["U"] = 0x16;
    keyToScanCodeMap_["v"] = 0x2F;
    keyToScanCodeMap_["V"] = 0x2F;
    keyToScanCodeMap_["w"] = 0x11;
    keyToScanCodeMap_["W"] = 0x11;
    keyToScanCodeMap_["x"] = 0x2D;
    keyToScanCodeMap_["X"] = 0x2D;
    keyToScanCodeMap_["y"] = 0x15;
    keyToScanCodeMap_["Y"] = 0x15;
    keyToScanCodeMap_["z"] = 0x2C;
    keyToScanCodeMap_["Z"] = 0x2C;

    // 数字键
    keyToScanCodeMap_["1"] = 0x02;
    keyToScanCodeMap_["2"] = 0x03;
    keyToScanCodeMap_["3"] = 0x04;
    keyToScanCodeMap_["4"] = 0x05;
    keyToScanCodeMap_["5"] = 0x06;
    keyToScanCodeMap_["6"] = 0x07;
    keyToScanCodeMap_["7"] = 0x08;
    keyToScanCodeMap_["8"] = 0x09;
    keyToScanCodeMap_["9"] = 0x0A;
    keyToScanCodeMap_["0"] = 0x0B;

    // 符号键
    keyToScanCodeMap_[","] = 0x33;
    keyToScanCodeMap_["."] = 0x34;
    keyToScanCodeMap_[";"] = 0x27;
    keyToScanCodeMap_["/"] = 0x35;
    keyToScanCodeMap_["-"] = 0x0C;
    keyToScanCodeMap_["["] = 0x1A;
    keyToScanCodeMap_["="] = 0x0D;
    keyToScanCodeMap_["]"] = 0x1B;

    keyToScanCodeMap_["`"] = 0x29;
    keyToScanCodeMap_["\\"] = 0x2B;
    keyToScanCodeMap_["'"] = 0x28;
    
}