#include "mgos.h"
#include "mgos_gpio.h"
#include "pms7003.h"

#define PMS7003_FRAME_LEN 32
#define PMS7003_FRAME_START1 0x42
#define PMS7003_FRAME_START2 0x4D

#define PMS7003_PM_1_0_H    10
#define PMS7003_PM_1_0_L    11
#define PMS7003_PM_2_5_H    12
#define PMS7003_PM_2_5_L    13
#define PMS7003_PM_10_0_H   14
#define PMS7003_PM_10_0_L   15

#define PMS7003_CMD_LEN 7

const unsigned char passive_mode_cmd[] =  { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
const unsigned char active_mode_cmd[] =   { 0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71 };
const unsigned char request_read_cmd[] =  { 0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71 };
const unsigned char sleep_cmd[] =         { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
const unsigned char wakeup_cmd[] =        { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };

unsigned char* pms7003_find_frame(unsigned char* frame, size_t frame_len) {
  size_t i;
  if (frame_len > 0) {
    LOG(LL_VERBOSE_DEBUG, ("PMS7003: frame[0] = 0x%02x\r\n", frame[0]));
  }
  for (i = 1; i < frame_len; i++) {
    LOG(LL_VERBOSE_DEBUG, ("PMS7003: frame[%d] = 0x%02x\r\n", i, frame[0]));
    if ((frame[i-1] == PMS7003_FRAME_START1) && (frame[i] == PMS7003_FRAME_START2)) {
      if ((frame_len - i + 1) >= PMS7003_FRAME_LEN) {
          return &frame[i-1];
      }
      LOG(LL_DEBUG, ("PMS7003: Too small frame length: %d\r\n", frame_len));  
    }
  }
  LOG(LL_DEBUG, ("PMS7003: frame not found\r\n"));
  return NULL;
}

bool pms7003_validate_checksum(unsigned char* frame) {
  size_t i;
  unsigned long valid_checksum = (frame[PMS7003_FRAME_LEN-2] << 8) + frame[PMS7003_FRAME_LEN - 1];
  unsigned long checksum = 0;

  for (i = 0; i < PMS7003_FRAME_LEN - 2; i++) {
    checksum += frame[i];
  }
  LOG(LL_DEBUG, ("PMS7003: calculated checksum: 0x%04lx\r\n", checksum));
  LOG(LL_DEBUG, ("PMS7003:      valid checksum: 0x%04lx\r\n", valid_checksum));
  return checksum == valid_checksum;
}

void pms7003_set_mode(int uartno, enum pms7003_mode mode) {   
  if (mode == ACTIVE) {
      mgos_uart_write(uartno, active_mode_cmd, PMS7003_CMD_LEN);
  }
  else if (mode == PASSIVE) {
      mgos_uart_write(uartno, passive_mode_cmd, PMS7003_CMD_LEN);
  }
  else {
      LOG(LL_ERROR, ("Invalid PMS7003 mode. Valid values: (ACTIVE, PASSIVE)\r\n"));
  }
}

void pms7003_sleep(int uartno) {
  mgos_uart_write(uartno, sleep_cmd, PMS7003_CMD_LEN);
}

void pms7003_wakeup(int uartno) {
  mgos_uart_write(uartno, wakeup_cmd, PMS7003_CMD_LEN);
}

void pms7003_request_read(int uartno) {
  mgos_uart_write(uartno, request_read_cmd, PMS7003_CMD_LEN);
}

unsigned long pms7003_get_pm1_0(unsigned char* frame) {
  return (frame[PMS7003_PM_1_0_H] << 8) + frame[PMS7003_PM_1_0_L];
}

unsigned long pms7003_get_pm2_5(unsigned char* frame) {
  return (frame[PMS7003_PM_2_5_H] << 8) + frame[PMS7003_PM_2_5_L];
}

unsigned long pms7003_get_pm10_0(unsigned char* frame) {
  return (frame[PMS7003_PM_10_0_H] << 8) + frame[PMS7003_PM_10_0_L];
}
