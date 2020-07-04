# iot-connect-example-psoc-pub

Avnet IoTConnect SDK example - publisher client on Cypress PSoC 6 BLE Pioneer Kit (CY8CKIT_062_WIFI_BT)

## Install

You should install mbed cli first. Refer to: [mbed cli install and setup](https://os.mbed.com/docs/mbed-os/v6.0/build-tools/install-and-set-up.html)


## Build

```bash
$ git clone https://github.com/ahnniu/iot-connect-example-psoc-pub.git pub
$ cd pub
$ mbed config root .
$ mbed target CY8CKIT_062_WIFI_BT
$ mbed toolchain GCC_ARM
$ mbed compile
```