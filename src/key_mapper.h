#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

class KeyMapper {
public:
    KeyMapper();
    ~KeyMapper();

    // 从JSON文件加载音符键位映射
    bool loadKeyMapFromFile(const std::string& filePath);

    // 根据MIDI音符获取对应的键盘按键
    std::string getKeyFromMidiNote(int midiNote) const;

    // 将MIDI音符转换为音名
    std::string midiNoteToName(int midiNote) const;

    // 设置八度移调
    void setOctaveShift(int shift);

    // 获取当前八度移调
    int getOctaveShift() const;

private:
    std::unordered_map<std::string, std::string> noteToKeyMap_;
    int octaveShift_;

    // 解析JSON配置
    bool parseConfig(const nlohmann::json& config);
};
