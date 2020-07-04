#ifndef __DEVICES_SETTINGS_H__
#define __DEVICES_SETTINGS_H__


#define TESTING_LED3_NAME "LED 3"
#define TESTING_LED3_ID "led3"
#define TESTING_LED3_PWD "SharedAccessSignature sr=poc-iotconnect-iothub-eu.azure-devices.net%2Fdevices%2F617449FD62B649E7AD1050DB4E95096E-led3&sig=bLYUC%2FaG1zKnajvmqtGwzvONL58mLs9oZzQ5YPl68Ss%3D&se=1625032925"


#define TESTING_LED2_NAME "LED 2"
#define TESTING_LED2_ID "led2"
#define TESTING_LED2_PWD "1234"

#define TESTING_LED2_CLIENT_CERT_PEM                                       \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "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\r\n" \
    "-----END CERTIFICATE-----\r\n"


#define TESTING_LED2_CLIENT_PRIVATE_KEY                                    \
    "-----BEGIN RSA PRIVATE KEY-----\r\n"                                  \
    "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\r\n"     \
    "-----END RSA PRIVATE KEY-----\r\n"

#define TESTING_AZ_LED1_NAME "Azure LED 1"
#define TESTING_AZ_LED1_ID "az-led1"
#define TESTING_AZ_LED1_PWD "SharedAccessSignature sr=psoc.azure-devices.net%2Fdevices%2Faz-led1&sig=rdpqq8XnFX9k3GcNPi43tRT2VOwTyD8yBjEmRE8mI50%3D&se=1597445777"

#endif