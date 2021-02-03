#include "ATSCom.h"
#include "BluetoothControl.h"

/*
1. 블루투스 라이브러리 설치
- 리눅스 환경에서 라이브러리를 불러오지 못할 수도 있음
- sudo apt-get install libbluetooth-dev

2. SDP Server 활성화
- 컴파일&실행 : g++ -o Bluetooth mainChan.cpp ATSCom.cpp BluetoothConnection.cpp -lbluetooth -lpthread -> ./Bluetooth
- Segmentation Fault가 발생할 수 있다.

- sudo nano /etc/systemd/system/dbus-org.bluez.service
- ExecStart=/usr/lib/bluetooth/bluetoothd 뒤에 '(앞에 1칸 띄어쓰기)--compat 추가'

- sudo nano ~/.bashrc 입력 후, 파일 끝에 sudo chmod 777 /var/run/sdp 추가
*/

int main()
{
    ATSCom atscom("/dev/ttyS1", 115200, 0, 1);
    BluetoothControl BC(&atscom);
    
    uint8_t BTdata1 = 0;
    uint8_t BTdata2 = 0;

    // 자동모드를 하기 위해 보드에 입력해주는 값
    BTdata1 &= 0xF0; // or num | 0x60;
    BTdata1 |= 6;
	atscom.set_state(BTdata1, BTdata2);

    // 블루투스 원격 제어 시작
    BC.run();

    return 0;
}