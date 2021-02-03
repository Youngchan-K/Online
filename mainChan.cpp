#include "BluetoothControl.h"

/*
리눅스 환경에서 라이브러리를 불러오지 못할 수도 있음

1. 블루투스 라이브러리 설치
sudo apt-get install libbluetooth-dev

2. SDP Server 활성화
컴파일&실행 : g++ -o Bluetooth mainChan.cpp ATSCom.cpp BluetoothConnection.cpp -lbluetooth -lpthread -> ./Bluetooth
Segmentation Fault가 발생할 수 있다.

sudo nano /etc/systemd/system/dbus-org.bluez.service
ExecStart=/usr/lib/bluetooth/bluetoothd 뒤에 '(앞에 1칸 띄어쓰기)--compat 추가'

sudo nano ~/.bashrc 입력 후, 파일 끝에 sudo chmod 777 /var/run/sdp 추가
*/

int main()
{
    ATSCom atscom;
    BluetoothControl BC;
    

    // 자동모드를 하기 위해 보드에 입력해주는 값
    stateData1 = 5 | serialCom.ID.LEDState << 4; // or num | 0x60;
    stateData2 = serialCom.ID.emergencyStop | serialCom.ID.callStaff << 1 | serialCom.ID.charingCableConnect << 2 | serialCom.ID.chareState << 3 | serialCom.ID.haptic << 4 | serialCom.ID.stationDocking << 6;
	atscom.set_state(stateData1, stateData2);   

    BC.run();

    return 0;
}