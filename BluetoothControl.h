#ifndef BLUETOOTHCONTROL
#define BLUETOOTHCONTROL

#define PORT 5

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 리눅스 환경에서 해당 라이브러리를 불러오지 못할 수도 있음
// sudo apt-get install libbluetooth-dev 설치 필요
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include "../ATS_Com/ATSCom.h"

using namespace std;

class BluetoothControl
{
private:
    int turn_cart(double theta);
    
    int go_straight(double distance);
}

#endif