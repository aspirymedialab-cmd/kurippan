#pragma once
#include <array>
#include <cmath>
#include <cstdint>

struct MPEVoice {
    bool active = false;
    int noteNumber = -1;
    int channel = -1;
    
    // Expressive Dimensions
    float pitchBend = 0.0f;  // Glide / Pitch (-1.0 to 1.0)
    float pressure  = 0.0f;  // Press / Aftertouch (0.0 to 1.0)
    float timbre    = 0.0f;  // Slide / CC74 (0.0 to 1.0)
    float velocity  = 0.0f;  // Initial Strike (0.0 to 1.0)
};

class MPEPolyVoiceAllocator {
public:
    static constexpr int MAX_VOICES = 16;
    
    void processMidiMessage(uint8_t status, uint8_t data1, uint8_t data2) {
        uint8_t msgType = status & 0xF0;
        uint8_t channel = status & 0x0F;

        // Channel 1 is typically Global MPE Master Channel
        if (channel == 0) {
            handleGlobalCC(data1, data2);
            return;
        }

        switch (msgType) {
            case 0x90: // Note On
                if (data2 > 0) allocateVoice(channel, data1, data2 / 127.0f);
                else freeVoice(channel, data1);
                break;
                
            case 0x80: // Note Off
                freeVoice(channel, data1);
                break;
                
            case 0xE0: // Pitch Bend (14-bit)
                {
                    int rawValue = (data2 << 7) | data1;
                    float normalized = (rawValue - 8192) / 8192.0f;
                    updatePitchBend(channel, normalized);
                }
                break;
                
            case 0xD0: // Channel Pressure (Pressure/Press)
                updatePressure(channel, data1 / 127.0f);
                break;
                
            case 0xB0: // Control Change (Slide/Timbre = CC 74)
                if (data1 == 74) {
                    updateTimbre(channel, data2 / 127.0f);
                }
                break;
        }
    }

    const std::array<MPEVoice, MAX_VOICES>& getVoices() const { return voices; }

private:
    std::array<MPEVoice, MAX_VOICES> voices;

    void allocateVoice(int channel, int note, float vel) {
        for (auto& v : voices) {
            if (!v.active) {
                v.active = true;
                v.channel = channel;
                v.noteNumber = note;
                v.velocity = vel;
                v.pitchBend = 0.0f;
                v.pressure = 0.0f;
                v.timbre = 0.5f; // Default center
                break;
            }
        }
    }

    void freeVoice(int channel, int note) {
        for (auto& v : voices) {
            if (v.active && v.channel == channel && v.noteNumber == note) {
                v.active = false;
                v.noteNumber = -1;
            }
        }
    }

    void updatePitchBend(int channel, float val) {
        for (auto& v : voices) if (v.active && v.channel == channel) v.pitchBend = val;
    }
    
    void updatePressure(int channel, float val) {
        for (auto& v : voices) if (v.active && v.channel == channel) v.pressure = val;
    }

    void updateTimbre(int channel, float val) {
        for (auto& v : voices) if (v.active && v.channel == channel) v.timbre = val;
    }

    void handleGlobalCC(uint8_t cc, uint8_t val) {
        // Handle global MPE zone configuration
    }
};
