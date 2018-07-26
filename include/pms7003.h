#include <stdio.h>
#include <stdbool.h>

#ifndef PMS7003_H
#define PMS7003_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum pms7003_mode {
    ACTIVE = 1,
    PASSIVE = 2
};

unsigned char* pms7003_find_frame(unsigned char* frame, size_t frame_len);
bool pms7003_validate_checksum(unsigned char* frame);
void pms7003_set_mode(int uartno, enum pms7003_mode mode);
void pms7003_sleep(int uartno);
void pms7003_wakeup(int uartno);
void pms7003_request_read(int uartno);
unsigned long pms7003_get_pm1_0(unsigned char* frame);
unsigned long pms7003_get_pm2_5(unsigned char* frame);
unsigned long pms7003_get_pm10_0(unsigned char* frame);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PMS7003_H */