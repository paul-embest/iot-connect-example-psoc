#include "mbed.h"
#include "mbed_trace.h"
#include "IoTConnectCommon.h"
#include "IoTConnectClient.h"
#include "NTPClient.h"
#include "entry_embest_settings.h"
#include "devices_settings.h"
#include "jsmn.h"
#include "RGBLed.h"

#define TRACE_GROUP  "main"

// Blinking rate in milliseconds
#define BLINKING_RATE_MS     500


void on_recv(MQTT::Message* msg);
int sync_rtc_time(NetworkInterface* net);
NetworkInterface* connect_network();
void do_change_color(const char* color);
void on_color_change(void* property);
void handle_btn_pressed();

static const char* colors[8] = {
    "red", "green", "blue","magenta",
    "cyan", "yellow", "white", "black"
};
static volatile bool is_publish = false;
NetworkInterface* network = NULL;
RGBLed led_rgb(LED_RED, LED_GREEN, LED_BLUE);
InterruptIn btn1(BUTTON1);
IoTConnectEntry embest(ENTRY_EMBEST_COMPANY_NAME, ENTRY_EMBEST_CPID);
IoTConnectEntry hub(ENTRY_PSOC_COMPANY_NAME, ENTRY_PSOC_CPID);

IoTConnectStringProperty psoc_led_color("color", "white");
IoTConnectDevice* psoc_led;
IoTConnectClient* client = NULL;

int main()
{
    int ret;

    mbed_trace_init();

    #ifdef MBED_MAJOR_VERSION
        printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif


    embest.set_mqtt(ENTRY_EMBEST_MQTT_SERVER_HOST_NAME,
                   ENTRY_EMBEST_MQTT_SERVER_PORT);

    hub.set_mqtt(ENTRY_PSOC_MQTT_SERVER_HOST_NAME, ENTRY_PSOC_MQTT_SERVER_PORT);

    // psoc_led = new IoTConnectDevice(TESTING_LED3_ID, TESTING_LED3_NAME, TESTING_LED3_PWD, &embest);
    psoc_led = new IoTConnectDevice(TESTING_AZ_LED1_ID, TESTING_AZ_LED1_NAME, TESTING_AZ_LED1_PWD, &hub);
    psoc_led->add(&psoc_led_color, on_color_change);

    // Connect to network
    network = connect_network();
    if (network == NULL) {
        return -1;
    }

    // sync the real time clock (RTC)
    ret = sync_rtc_time(network);
    if (ret != 0) {
        return ret;
    }

    client = new IoTConnectClient(network, psoc_led);

    ret = client->connect();
    if (ret != 0) {
        printf("Connect failed, exit\n");
        return -1;
    }
    ret = client->subscribe(MQTT::QOS0);
    if (ret != 0) {
        printf("Failed to subscribe\n");
        return -1;
    }
    ret = client->start_main_loop();
    if (ret != 0) {
        printf("Failed to start client thread main loop\n");
        return -1;
    }

    btn1.rise(handle_btn_pressed);
    printf("Register button1 event, press Button1 will pub a message\n");
    printf("Note: Since the pub topic is different with the sub topic, it won't change the led color\n");
    printf("Please use az iot hub monitor-events -n {iothub_name} to see if the pub is succussful.\n");

    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED2);

    while (true) {
        static int i = 0;
        if (is_publish) {
            is_publish = false;
            psoc_led_color.set_value(colors[i]);
            printf("Button Pressed, try to pub a message with a new color: %s\n", colors[i]);
            i = (i+1)%8;
            client->pub_props(MQTT::QOS0);
        }
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE_MS);
    }
}

NetworkInterface* connect_network() {
    NetworkInterface* net = NULL;

    printf("Opening network interface...\r\n");

    net = NetworkInterface::get_default_instance();
    if (!net) {
        printf("Unable to open network interface.\r\n");
        return NULL;
    }

    nsapi_error_t net_status = NSAPI_ERROR_NO_CONNECTION;
    while ((net_status = net->connect()) != NSAPI_ERROR_OK) {
        printf("Unable to connect to network (%d). Retrying...\r\n", net_status);
    }
    SocketAddress address;
    net->get_ip_address(&address);
    printf("Connected to the network successfully. IP address: %s\r\n", address.get_ip_address());

    printf("\n");

    return net;
}

int sync_rtc_time(NetworkInterface* net ) {

    const char* ntp_srv = "ntp.aliyun.com";
    if (net == NULL) {
        return -1;
    }

    NTPClient ntp(net);
    ntp.set_server(ntp_srv, 123);
    time_t now = ntp.get_timestamp();
    if (now <= 0) {
        printf("Failed to retrieve the time from %s:123\n", ntp_srv);
        return -1;
    }
    set_time(now);
    printf("Time is now %s", ctime(&now));

    return 0;
}

void do_change_color(const char* color) {

    if (color == NULL) {
        return;
    }

    if (strcmp(color, "red") == 0) {
        led_rgb.setColor(RGBLed::RED);
    } else if (strcmp(color, "green") == 0) {
        led_rgb.setColor(RGBLed::GREEN);
    } else if (strcmp(color, "blue") == 0) {
        led_rgb.setColor(RGBLed::BLUE);
    } else if (strcmp(color, "magenta") == 0) {
        led_rgb.setColor(RGBLed::MAGENTA);
    } else if (strcmp(color, "cyan") == 0) {
        led_rgb.setColor(RGBLed::CYAN);
    } else if (strcmp(color, "yellow") == 0) {
        led_rgb.setColor(RGBLed::YELLOW);
    }  else if (strcmp(color, "white") == 0) {
        led_rgb.setColor(RGBLed::WHITE);
    } else {
        led_rgb.setColor(RGBLed::BLACK);
    }
}

void on_color_change(void* property) {
    const char* color = NULL;
    const char* key = NULL;

    if (property == NULL) {
        return;
    }

    key = ((IoTConnectStringProperty*)property)->get_key();
    if (key == NULL) {
        return;
    }

    color = ((IoTConnectStringProperty*)property)->get_value();
    if (color == NULL) {
        return;
    }
    printf("prop[%s] recv new color:%s.\n", key, color);

    do_change_color(color);
}

void handle_btn_pressed() {
    is_publish = true;
}

void on_recv(MQTT::Message* msg) {
    printf("on_recv() called\n");
    if (msg) {
        printf("Recieved new Message#%d: %s\n", msg->id, msg->payload);
        free(msg->payload);
        free(msg);
    }
}