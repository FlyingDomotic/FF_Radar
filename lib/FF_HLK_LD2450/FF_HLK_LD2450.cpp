#include "FF_HLK_LD2450.h"

void FF_HLK_LD2450::begin(int bitrate) {
    Serial.begin(bitrate);
}

bool FF_HLK_LD2450::refreshRadarData() {
    if (Serial.available()) {
        byte rec_buf[80] = "";
        int len = Serial.readBytes(rec_buf, sizeof(rec_buf));

        for (int i = 0; i < len; i++) {
            if (rec_buf[i] == 0xAA && rec_buf[i+1] == 0xFF && rec_buf[i+2] == 0x03 && rec_buf[i+3] == 0x00 && rec_buf[i+28] == 0x55 && rec_buf[i+29] == 0xCC)  {
                int index = i+4;
                for (int targetCounter = 0; targetCounter < 3; targetCounter++) {
                    if (index+7 < len) {

                        RadarTarget target;

                        target.x = (int16_t)(rec_buf[index] | (rec_buf[index+1] << 8));
                        if (rec_buf[index+1] & 0x80) {
                            target.x -= 0x8000;
                        } else {
                            target.x = -target.x;
                        }

                        target.y = (int16_t)(rec_buf[index+2] | (rec_buf[index+3] << 8));
                        if (rec_buf[index+3] & 0x80) {
                            target.y -= 0x8000;
                        } else {
                            target.y = -target.y;
                        }

                        target.speed = (int16_t)(rec_buf[index+4] | (rec_buf[index+5] << 8));
                        if (rec_buf[index+5] & 0x80) {
                            target.speed -= 0x8000;
                        } else {
                            target.speed = -target.speed;

                        }
                        target.resolution = (uint16_t)(rec_buf[index+6] | (rec_buf[index+7] << 8));

                        radarTargets[targetCounter].x = target.x;
                        radarTargets[targetCounter].y = target.y;
                        radarTargets[targetCounter].speed = target.speed;
                        radarTargets[targetCounter].resolution = target.resolution;
                        index += 8;
                    }
                }
                i = index;
            }
        }
    return true;
    }
  return false;
}