#ifndef _H_MEM
#define _H_MEM
#include "IO.h"
#include <cstdint>

typedef uint32_t typeofmemaccess(uint32_t, uint32_t, uint32_t);

class Mem {

  public:
    IO      *io   = nullptr;
    uint32_t size = 20 * 1024 * 1024;

  private:
    typeofmemaccess* memaccess;
    uint32_t memoryaccesssize = 0;
    uint8_t  *mem          = nullptr;
    uint8_t  *mem_byte     = nullptr;
    uint16_t *mem_halfword = nullptr;
    uint32_t *mem_word     = nullptr;

  public:
    __declspec(dllexport) Mem(IO *_io);
    __declspec(dllexport) Mem(typeofmemaccess* prm_0, uint32_t prm_1);
    ~Mem();
    void init();
    void reset();

    uint8_t  ld_byte(uint32_t addr);
    uint16_t ld_halfword(uint32_t addr);
    uint32_t ld_word(uint32_t addr);

    void st_byte(uint32_t addr, uint8_t onebyte);
    void st_halfword(uint32_t addr, uint16_t halfword);
    void st_word(uint32_t addr, uint32_t halfword);

    uint8_t  ld_byte_fast(uint32_t addr);
    uint16_t ld_halfword_fast(uint32_t addr);
    uint32_t ld_word_fast(uint32_t addr);
    void     st_word_unaligned(uint32_t addr, uint32_t word);

    void st_byte_fast(uint32_t addr, uint8_t onebyte);
    void st_halfword_fast(uint32_t addr, uint16_t halfword);
    void st_word_fast(uint32_t addr, uint32_t halfword);
};
#endif
