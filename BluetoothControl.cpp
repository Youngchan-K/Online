#include "BluetoothControl.h"

bdaddr_t bdaddr_any = {0, 0, 0, 0, 0, 0};
bdaddr_t bdaddr_local = {0, 0, 0, 0xff, 0xff, 0xff};

// 목적 : UUID setting
// 개요 : 블루투스 포트 사용할 수 있도록 UUID 설정
// parameter : rfcomm_channel - 연결할 PORT의 개수
sdp_session_t* BluetoothControl::register_service(uint8_t rfcomm_channel)
{
    uint32_t service_uuid_int[] = {0x01110000, 0x00100000, 0x80000080, 0xFB349B5F};
    const char *name = "JCart Bluetooth Control";
    const char *company = "EVAR";
    const char *description = "Electric car Charge Service";
    
    // RFCOMM 프로토콜 하부에서 Transport protocol로 L2CAP 프로토콜이 작동
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

    // General service ID
    sdp_uuid128_create(&service_uuid, &service_uuid_int);
    sdp_set_service_id(&record, service_uuid);

    char str_uuid[256] = "";
    sdp_uuid2strn(&service_uuid, str_uuid, 256);
    printf("Registering UUID %s\n", str_uuid);

    // Service class
    sdp_uuid16_create(&service_class_uuid, SERIAL_PORT_SVCLASS_ID);
    service_class_list = sdp_list_append(0, &service_class_uuid);
    sdp_set_service_classes(&record, service_class_list);

    // Bluetooth profile information
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    profile_list = sdp_list_append(0, &profile);
    sdp_set_profile_descs(&record, profile_list);

    // Make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups(&record, root_list);

    // L2CAP information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append(0, &l2cap_uuid);
    proto_list = sdp_list_append(0, l2cap_list);

    // Register the RFCOMM channel for RFCOMM sockets
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
    sdp_list_append(rfcomm_list, channel);
    sdp_list_append(proto_list, rfcomm_list);

    access_proto_list = sdp_list_append(0, proto_list);
    sdp_set_access_protos(&record, access_proto_list);

    // Service name, provider(company), and description
    sdp_set_info_attr(&record, name, company, description);

    // Connect to the local SDP server
    session = sdp_connect(&bdaddr_any, &bdaddr_local, SDP_RETRY_IF_BUSY);

    // Register the service record
    sdp_record_register(session, &record, 0);

    // Cleanup
    sdp_data_free(channel);
    sdp_list_free(l2cap_list, 0);
    sdp_list_free(rfcomm_list, 0);
    sdp_list_free(root_list, 0);
    sdp_list_free(access_proto_list, 0);
    sdp_list_free(service_class_list, 0);
    sdp_list_free(profile_list, 0);

    return session;
}

// Docking.cpp 코드 이용 -> go_straight & turn_cart

// 목적 : 카트 이동
// 개요 : 카트를 원하는 거리 만큼 직선 이동
// parameter : distance - 이동할 거리(mm)
int BluetoothControl::go_straight(double distance)
{
    unsigned long cnt = 0, cnt1 = 0;
    usleep(50);
    this->com->auto_set_vw(distance * 10, 0, 0, 0);

    while (com->ID.moveCheck != 1)
    {
	//printf("cnt : %d\n", cnt); 
        cnt++;
        if (cnt == 0xfffffff)
        {
            this->com->auto_set_vw(distance * 10, 0, 0, 0);
            cnt1++;
            cnt = 0;
            puts("cnt initialize");
            if (cnt1 == 10)
            {
                puts("turn cart Error");
                exit(-1);
            }
        }
    }
    com->ID.moveCheck = 0;
    puts("moveCheck received");
    while (com->check != AUTO);
    com->check = WAIT;
    printf("result dist : %f mm\tresult ang : %f deg\n", (double)(com->ID.motorDistance / 10), (double)(com->ID.motorOmega / 10));
    return 0;
}

// 목적 : 카트 회전
// 개요 : 카트를 원하는 각도만큼 제자리 회전
// parameter : theta - 회전할 각도(deg)
int BluetoothControl::turn_cart(double theta)
{
    unsigned long cnt = 0, cnt1 = 0;
    usleep(50);
    this->com->auto_set_vw(0, theta * 10, 0, 0);

    while (com->ID.moveCheck != 1)
    {
        cnt++;
        if (cnt == 0xfffffff)
        {
            this->com->auto_set_vw(0, theta * 10, 0, 0);
            cnt1++;
            cnt = 0;
            puts("cnt initialize");
            if (cnt1 == 10)
            {
                puts("turn cart Error");
                exit(-1);
            }
        }
    }
    com->ID.moveCheck = 0;
    puts("moveCheck received");
    while (com->check != AUTO);
    com->check = WAIT;
    printf("result dist : %f mm\tresult ang : %f deg\n", (double)(com->ID.motorDistance / 10), (double)(com->ID.motorOmega / 10));
    return 0;
}

// 목적 : Bluetooth 서버 생성하여 스마트폰과 연결
// 개요 : 스마트폰에서 입력한 값에 따라 카트 원격 제어
// 안드로이드만 Service 가능 (아이폰 X)
int BluetoothControl::run()
{
    printf("Bluetooth Connection Start\n");
    sdp_session_t* session = register_service(PORT);

    struct sockaddr_rc Local;
    struct sockaddr_rc Remote;

    char input[1024];
    int BTsocket, client, read_data;
    socklen_t socket_len = sizeof(Remote);

    // Socket 생성
    BTsocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Local 설정
    Local.rc_family = AF_BLUETOOTH;
    Local.rc_bdaddr = bdaddr_any;
    Local.rc_channel = (uint8_t) PORT; // RFCOMM Channel의 개수는 최대 30까지

    // Bind
    if (bind(BTsocket, (struct sockaddr *)&Local, sizeof(Local)) < 0)
    {
        printf("Bind Failed\n");
    }
    else
    {
        printf("Bind Success\n");
    }

    // Listen - 클라이언트 접속 대기
    if (listen(BTsocket, PORT) < 0) // 연결 시도하는 클라이언트 최대 개수
    {
        printf("Listen Failed\n");

    }
    else
    {
        printf("Listen for connection...\n");
    }

    
    // Accept - 클라이언트 연결 요청 수락
    client = accept(BTsocket, (struct sockaddr *)&Remote, &socket_len);

    if (client < 0)
    {
        printf("Accept Failed\n");
    }
    else
    {
        printf("Accept Success!\n");
    }
            
    // Read data - 클라이언트로부터 데이터 수신
    do
    {
        memset(input, 0, sizeof(input));
        read_data = read(client, input, sizeof(input));

        if(read_data > 0) 
        {
            printf("Received Data : %s\n", input);
            string str_input = (string)input;
            
            // 입력값에 따라 카트 10mm씩 이동
            if(str_input == "F")
            {
                printf("Go Straight\n");
                go_straight(10);
            }
            else if(str_input == "B")
            {
                printf("Go Back\n");
                go_straight(-10);
            }

            // 입력값에 따라 카트 1도씩 회전
            else if (str_input == "L")
            {
                printf("Turn Left\n");
                turn_cart(1);
            }
            else if (str_input == "R")
            {
                printf("Turn Right\n");
                turn_cart(-1);
            }

            else if (str_input == "S")
            {
                printf("Stop\n");
            }
        }
    } while (read_data > 0); 


    // Close socket and connection
    close(client);
    close(BTsocket);
    sdp_close(session);
    
    printf("연결 종료");

    return 0;
}