#pragma once
#include <string>
#include <unordered_map>

class KeyEmulator {
public:
    KeyEmulator();
    ~KeyEmulator();

    // 初始化键盘模拟器
    bool initialize();

    // 模拟按下键盘按键
    bool pressKey(const std::string& key);

    // 模拟释放键盘按键
    bool releaseKey(const std::string& key);

    // 模拟按下并释放键盘按键（点击）
    bool clickKey(const std::string& key, int pressDurationMs = 50);

    // 关闭键盘模拟器
    void shutdown();

private:
    // 底层键盘事件发送函数
    bool sendKeyEvent(const std::string& key, bool isPress);

    // 检查键是否有效
    bool isValidKey(const std::string& key);

    // Windows平台特定的键盘扫描码映射
    std::unordered_map<std::string, int> keyToScanCodeMap_;

    // 初始化键盘扫描码映射
    void initializeScanCodeMap();
};