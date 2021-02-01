#include "App/BluetoothControl.h"

// 컴파일&실행 : g++ -o Bluetooth mainChan.cpp BluetoothConnection.cpp -lbluetooth -> ./Bluetooth
// Segmentation Fault가 발생할 수 있음
/*
sudo nano /etc/systemd/system/dbus-org.bluez.service
ExecStart=/usr/lib/bluetooth/bluetoothd 뒤에 '(앞에 1칸 띄어쓰기)--compat 추가'

sudo nano ~/.bashrc 입력 후, 파일 끝에 sudo chmod 777 /var/run/sdp 추가
*/

int main()
{
    BluetoothControl BC;
    
    BC.run();

    return 0;
}