#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 리눅스 환경에서 sudo apt-get install libbluetooth-dev 설치 필요
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>



using namespace std;