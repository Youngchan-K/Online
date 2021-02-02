/********************************************************************************
작성자 : chan
목적 : 카트와 스마트폰을 Bluetooth로 연결하고, 스마트폰 입력값에 따라 카트 이동 및 회전
사용방식 : Bluetooth를 사용할 수 있도록 UUID를 설정하여 카트와 연결
중요사항 : Bluetooth를 이용하여 socket으로 통신하기 위해서 UUID 반드시 필요
*********************************************************************************/

#ifndef BLUETOOTHCONTROL
#define BLUETOOTHCONTROL

#include "ATSCom.h"
#include <iostream>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

using namespace std;

#define PORT 1

class BluetoothControl
{
private:
    ATSCom *com;

    sdp_session_t* register_service(uint8_t rfcomm_channel);

    int gostraight(double distance);
    int turncart(double theta);
    int stopbrake();

public:
    int run();
};

#endif