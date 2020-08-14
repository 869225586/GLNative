//
// Created by edz on 2020/8/14.
//

#ifndef NATIVE_PLAYSTATUS_H
#define NATIVE_PLAYSTATUS_H


class PlayStatus {
public:
    bool exit = false;
    bool load = true;
    bool seek = false;
    bool pause = false;
public:
    PlayStatus();
    ~PlayStatus();

};


#endif //NATIVE_PLAYSTATUS_H
