#ifndef __FF_HLK_LD2450_H__
#define __FF_HLK_LD2450_H__
#include "Arduino.h"

class FF_HLK_LD2450 {
  private:
    typedef struct RadarTarget {
        int16_t x;
        int16_t y;
        int16_t speed;
        uint16_t resolution;
    } RadarTarget_t;

    RadarTarget_t radarTargets[3];
    
  public:
    // Methods
    void begin(int bitrate);
    bool refreshRadarData();

    // Getters for the first target
    int16_t getTargetX(int index){return radarTargets[index].x;}
    int16_t getTargetY(int index){return radarTargets[index].y;}
    int16_t getTargetSpeed(int index){return radarTargets[index].speed;}
    uint16_t getTargetResolution(int index){return radarTargets[index].resolution;}
};
#endif
