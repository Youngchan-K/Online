#include "BluetoothConnection.h"

int main(int argc, char *argv[])
{
    struct sockaddr_rc Local;
    struct sockaddr_rc Remote;

    char buffer[1024];
    int BTsocket, client, read_data;
    socklen_t socket_len = sizeof(Remote);

    // Socket 생성
    BTsocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Local 설정
    Local.rc_family = AF_BLUETOOTH;
    Local.rc_bdaddr = *BDADDR_ANY;
    Local.rc_channel = (uint8_t) 1; // RFCOMM Channel의 개수는 최대 30까지

    // Bind 
    if (bind(BTsocket, (struct sockaddr *)&Local, sizeof(Local)) < 0)
    {
        cout << "Bind Failed" << endl;
    }
    else
    {
        cout << "Bind Success" << endl;
    }

    // Listen
    if (listen(BTsocket, 5) < 0) // 연결 시도하는 클라이언트 최대 5개
    {
        cout << "Listen Failed" << endl;
    }
    else
    {
        cout << "Listen for connection" << endl;
    }

    // Accept - 클라이언트 연결 요청 수락
    client = accept(BTsocket, (struct sockaddr *)&Remote, &socket_len);

    if (client < 0)
    {
        cout << "Accept Failed" << endl;
    }
    else
    {
        cout << "Accept Success" << endl;
    }
    
    // Read data - 클라이언트로부터 데이터 수신
    read_data = read(client, buffer, sizeof(buffer));

    if(read_data > 0) 
    {
        cout << "Received Data :" << buffer << endl;
    }
    else
    {
        cout << "데이터 수신 실패" << endl;
    }

    // Close connection
    close(client);

    // Close socket
    close(BTsocket);
    
    return 0;
}