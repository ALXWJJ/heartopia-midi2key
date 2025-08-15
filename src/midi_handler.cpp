#include "midi_handler.h"
#include <iostream>

MidiHandler::MidiHandler() : midiIn_(nullptr) {
    try {
        midiIn_ = new RtMidiIn();
    } catch (RtMidiError& error) {
        error.printMessage();
        midiIn_ = nullptr;
    }
}

MidiHandler::~MidiHandler() {
    shutdown();
    if (midiIn_) {
        delete midiIn_;
        midiIn_ = nullptr;
    }
}

bool MidiHandler::initialize() {
    if (!midiIn_) {
        std::cerr << u8"MIDI输入未初始化" << std::endl;
        return false;
    }

    return true;
}

void MidiHandler::shutdown() {
    if (midiIn_ && midiIn_->isPortOpen()) {
        midiIn_->closePort();
    }
}

void MidiHandler::setMidiCallback(std::function<void(double, const std::vector<unsigned char>&)> callback) {
    midiCallback_ = callback;
}

std::vector<std::string> MidiHandler::getAvailableDevices() {
    std::vector<std::string> devices;
    if (!midiIn_) {
        return devices;
    }

    unsigned int nPorts = midiIn_->getPortCount();
    for (unsigned int i = 0; i < nPorts; ++i) {
        try {
            devices.push_back(midiIn_->getPortName(i));
        } catch (RtMidiError& error) {
            error.printMessage();
            break;
        }
    }

    return devices;
}

bool MidiHandler::selectDevice(unsigned int deviceId) {
    if (!midiIn_) {
        std::cerr << u8"MIDI输入未初始化" << std::endl;
        return false;
    }

    unsigned int nPorts = midiIn_->getPortCount();
    if (deviceId >= nPorts) {
        std::cerr << u8"无效的设备ID" << std::endl;
        return false;
    }

    try {
        midiIn_->closePort();
        midiIn_->openPort(deviceId);
        // 设置回调函数
        midiIn_->setCallback(&MidiHandler::midiMessageCallback, this);
        // 忽略 sysex, timing, and active sensing messages.
        midiIn_->ignoreTypes(true, true, true);
        std::cout << u8"已选择MIDI设备: " << midiIn_->getPortName(deviceId) << std::endl;
        return true;
    } catch (RtMidiError& error) {
        error.printMessage();
        return false;
    }
}

void MidiHandler::midiMessageCallback(double deltaTime, std::vector<unsigned char>* message,
                                      void* userData) {
    if (!message || message->empty()) {
        return;
    }

    MidiHandler* handler = static_cast<MidiHandler*>(userData);
    if (handler && handler->midiCallback_) {
        handler->midiCallback_(deltaTime, *message);
    }
}