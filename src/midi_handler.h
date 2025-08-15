#pragma once
#include <RtMidi.h>
#include <vector>
#include <functional>

class MidiHandler {
public:
    MidiHandler();
    ~MidiHandler();

    // 初始化MIDI输入
    bool initialize();

    // 关闭MIDI输入
    void shutdown();

    // 设置MIDI消息回调函数
    void setMidiCallback(std::function<void(double, const std::vector<unsigned char>&)> callback);

    // 获取可用的MIDI设备列表
    std::vector<std::string> getAvailableDevices();

    // 选择MIDI设备
    bool selectDevice(unsigned int deviceId);

private:
    RtMidiIn* midiIn_;
    std::function<void(double, const std::vector<unsigned char>&)> midiCallback_;

    // MIDI消息处理函数
    static void midiMessageCallback(double deltaTime, std::vector<unsigned char>* message,
                                    void* userData);
};