#ifndef _H_IO
#define _H_IO
#include "IRQ.h"
#include "dtimer.h"
#include "UART.h"
#include <sys/types.h>

class IO {

  public:
    IRQ    *gic    = nullptr;
    DTimer *timer0 = nullptr;
    UART   *uart0  = nullptr;

    uint32_t *port_data;

  public:
    IO(IRQ *_gic, DTimer *_timer0, UART *_uart0);
    ~IO();

    uint8_t  ld_byte(int addr);
    uint16_t ld_halfword(int addr);
    uint32_t ld_word(int addr);

    uint8_t  st_byte(int addr, uint8_t onebyte);
    uint16_t st_halfword(int addr, uint16_t halfword);
    uint32_t st_word(int addr, uint32_t word);
    int       check_ports(int addr, int data);
    int       check_portl(int addr);

    uint32_t ld_data(int addr);
    uint32_t st_data(uint32_t addr, uint32_t data);

  private:
};
#endif
