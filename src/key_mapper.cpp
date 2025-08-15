#include "key_mapper.h"
#include <fstream>
#include <iostream>
#include <vector>

KeyMapper::KeyMapper() : octaveShift_(0) {
}

KeyMapper::~KeyMapper() {
}

bool KeyMapper::loadKeyMapFromFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << u8"无法打开键位映射文件: " << filePath << std::endl;
            return false;
        }

        nlohmann::json config;
        file >> config;
        file.close();

        return parseConfig(config);
    } catch (const std::exception& e) {
        std::cerr << u8"加载键位映射错误: " << e.what() << std::endl;
        return false;
    }
}

std::string KeyMapper::getKeyFromMidiNote(int midiNote) const {
    // 应用八度移调
    int shiftedNote = midiNote + (octaveShift_ * 12);

    // 将MIDI音符转换为音名
    std::string noteName = midiNoteToName(shiftedNote);

    // 查找映射
    auto it = noteToKeyMap_.find(noteName);
    if (it != noteToKeyMap_.end()) {
        return it->second;
    }

    return "";
}

void KeyMapper::setOctaveShift(int shift) {
    octaveShift_ = shift;
}

int KeyMapper::getOctaveShift() const {
    return octaveShift_;
}

bool KeyMapper::parseConfig(const nlohmann::json& config) {
    try {
        // 清除现有映射
        noteToKeyMap_.clear();

        // 解析音符键位映射
        if (config.contains("midi_key_map")) {
            const auto& keyMap = config["midi_key_map"];
            for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
                try {
                    std::string noteName = it.key();
                    std::string key = it.value();
                    noteToKeyMap_[noteName] = key;
                } catch (const std::exception& ) {
                    std::cerr << u8"键位映射中的无效条目: " << it.key() << u8" -> " << it.value() << std::endl;
                    continue;
                }
            }
        }

        // 解析八度移调
        if (config.contains("octave_shift")) {
            octaveShift_ = config["octave_shift"];
        }

        std::cout << u8"已加载 " << noteToKeyMap_.size() << u8" 个音符键位映射" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << u8"解析键位映射配置错误: " << e.what() << std::endl;
        return false;
    }
}

std::string KeyMapper::midiNoteToName(int midiNote) const {
    // MIDI音符到音名的映射
    const std::vector<std::string> noteNames = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    // 计算八度和音名索引
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;

    // 返回音名+八度
    return noteNames[noteIndex] + std::to_string(octave);
}