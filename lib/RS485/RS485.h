#pragma once

class RS485 {
    int addr;
public:
    void Reset() { addr = 0; }
    void GetCommand();
}