# PMS7003 dust sensor library for Mongoose OS

## Overview

[PMS7003](https://botland.com.pl/index.php?controller=attachment&id_attachment=2182) (by Plantower) is a dust sensor which measures PM concentration (air polution).

<p align="center">
  <img src="https://s3-eu-west-1.amazonaws.com/github-an/pms7003.jpg">
</p>

The sensor can work in two modes:
- **active** (the default after power on) `pms7003_set_mode(UART_NO, ACTIVE)` , where measure is sent continuously with specified period of time (see the documentation for the details)
- **passive** `pms7003_set_mode(UART_NO, PASSIVE)`, where measure is sent only when it was requested `pms7003_request_read(UART_NO)`

Furthermore there is possible to put the sensor into sleep mode `pms7003_sleep(UART_NO)` and then wake it up `pms7003_wakeup(UART_NO)`. In a sleep mode a fan is disabled and the sensor consumes about 4 mA.

## Initialization

Since communication with the sensor is done via UART0 the debug messages are sent through UART1 (TX1 => GPIO2 on ESP8266). The library sets `debug.stdout_uart` and `debug.stderr_uart` on UART1.

Typical connection of the sensor.

<p align="center">
  <img src="https://s3-eu-west-1.amazonaws.com/github-an/pms7003_connection.png">
</p>

## Usage

```c
#include "mgos.h"
#include "mgos_uart.h"
#include "pms7003.h"

#define UART_NO 0

static void uart_dispatcher(int uart_no, void *arg) {
  static struct mbuf data = {0};

  assert(uart_no == UART_NO);

  // read UART data if available
  size_t available = mgos_uart_read_avail(uart_no);
  if (available == 0) return;

  mgos_uart_read_mbuf(uart_no, &data, available);

  // find PMS7003 frame 
  unsigned char *frame = pms7003_find_frame((unsigned char*)data.buf, data.len);
  if ((frame == NULL) || (!pms7003_validate_checksum(frame))) {
    LOG(LL_DEBUG, ("PMS7003: There is no frame or is invalid\r\n"));
    return;
  }

  // print PM measured results
  LOG(LL_INFO, ("-------------------------------------------\r\n"));
  LOG(LL_INFO, ("PMS7003 PM1_0: %ld\r\n", pms7003_get_pm1_0(frame)));
  LOG(LL_INFO, ("PMS7003 PM2_5: %ld\r\n", pms7003_get_pm2_5(frame)));
  LOG(LL_INFO, ("PMS7003 PM10_0: %ld\r\n", pms7003_get_pm10_0(frame)));

  mbuf_free(&data);
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  // configure UART - PMS7003 uses 9600bps
  struct mgos_uart_config ucfg;
  mgos_uart_config_set_defaults(UART_NO, &ucfg);
  ucfg.baud_rate = 9600;

  if (!mgos_uart_configure(UART_NO, &ucfg)) {
    return MGOS_APP_INIT_ERROR;
  }

  // register UART handler (when data received) and enable receiver
  mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);
  mgos_uart_set_rx_enabled(UART_NO, true);

  return MGOS_APP_INIT_SUCCESS;
}
```

In case of any issues increase the debug level and check debug logs.
For deep debugging set *debug_level* on **4** which shows also each value in a frame.
This can be done by adding the following lines to *mos.yml* file
```yaml
config_schema:
  - ["debug.level", 3]
```

The default baud rate of the debug port (UART1) is *115.2kbps*
