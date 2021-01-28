#include "BluetoothConnection.h"

bdaddr_t bdaddr_any = {0, 0, 0, 0, 0, 0};
bdaddr_t bdaddr_local = {0, 0, 0, 0xff, 0xff, 0xff};

// 블루투스 COM(직렬) 포트 사용할 수 있도록 UUID 설정
sdp_session_t* register_service(uint8_t rfcomm_channel)
{
    uint32_t service_uuid_int[] = {0x01110000, 0x00100000, 0x80000080, 0xFB349B5F};
    const char *name = "JCart Bluetooth Control";
    const char *company = "EVAR";
    const char *description = "Electric car Charge Service";
    
    // RFCOMM 프로토콜 하부에는 L2CAP 프로토콜이 Transport protocol로 작동
    // RFCOMM을 사용하여 블루투스 통신을 하면 L2CAP 프로토콜도 사용됨
    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, service_uuid, service_class_uuid;
    sdp_list_t *l2cap_list = 0,
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0,
               *access_proto_list = 0,
               *service_class_list = 0,
               *profile_list = 0;
    sdp_data_t *channel = 0;
    sdp_profile_desc_t profile;
    sdp_record_t record = {0};
    sdp_session_t *session = 0;

    // set the general service ID
    sdp_uuid128_create(&service_uuid, &service_uuid_int);
    sdp_set_service_id(&record, service_uuid);

    char str[256] = "";
    sdp_uuid2strn(&service_uuid, str, 256);
    printf("Registering UUID %s\n", str);

    // set the service class
    sdp_uuid16_create(&service_class_uuid, SERIAL_PORT_SVCLASS_ID);
    service_class_list = sdp_list_append(0, &service_class_uuid);
    sdp_set_service_classes(&record, service_class_list);

    // set the Bluetooth profile information
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    profile_list = sdp_list_append(0, &profile);
    sdp_set_profile_descs(&record, profile_list);

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( &record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // register the RFCOMM channel for RFCOMM sockets
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
    sdp_list_append(rfcomm_list, channel);
    sdp_list_append(proto_list, rfcomm_list);

    access_proto_list = sdp_list_append(0, proto_list);
    sdp_set_access_protos(&record, access_proto_list);

    // set the service name, provider(company), and description
    sdp_set_info_attr(&record, name, company, description);

    // connect to the local SDP server, register the service record,
    session = sdp_connect(&bdaddr_any, &bdaddr_local, SDP_RETRY_IF_BUSY);
    sdp_record_register(session, &record, 0);

    // cleanup
    sdp_data_free(channel);
    sdp_list_free(l2cap_list, 0);
    sdp_list_free(rfcomm_list, 0);
    sdp_list_free(root_list, 0);
    sdp_list_free(access_proto_list, 0);
    sdp_list_free(service_class_list, 0);
    sdp_list_free(profile_list, 0);

    return session;
}


int main(int argc, char *argv[])
{
    cout << "Bluetooth Connection Start" << endl;
    int port = 5;
    sdp_session_t* session = register_service(port)
    
    struct sockaddr_rc Local;
    struct sockaddr_rc Remote;

    char input[1024];
    int BTsocket, client, read_data;
    socklen_t socket_len = sizeof(Remote);

    // Socket 생성
    BTsocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Local 설정
    bdaddr_t tmp;

    Local.rc_family = AF_BLUETOOTH;
    Local.rc_bdaddr = bdaddr_any;
    Local.rc_channel = (uint8_t) port; // RFCOMM Channel의 개수는 최대 30까지

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

    while(1)
    {
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
        read_data = read(client, input, sizeof(input));

        if(read_data > 0) 
        {
            cout << "Received Data :" << input << endl;
        }
        else
        {
            cout << "데이터 수신 실패" << endl;
        }
    }


    // Close connection
    close(client);
    close(BTsocket);

    sdp_close(session);
    
    return 0;
}