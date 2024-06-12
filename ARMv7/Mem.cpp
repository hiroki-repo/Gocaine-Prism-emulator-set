#include "Mem.h"
#include <cstdint>
#include <cstdlib>

__declspec(dllexport) Mem::Mem(IO *_io)
{
    io  = _io;
    mem = (uint8_t *)malloc(sizeof(uint8_t) * size);
    for (int i = 0; i < size; i++) {
        mem[i] = 0;
    }
    mem_byte     = mem;
    mem_halfword = reinterpret_cast<uint16_t *>(mem);
    mem_word     = reinterpret_cast<uint32_t *>(mem);
}
__declspec(dllexport) Mem::Mem(typeofmemaccess* prm_0, uint32_t prm_1) {
    memaccess = prm_0;
    memoryaccesssize = prm_1;
}
Mem::~Mem()
{
    if (mem != nullptr) { free(mem); }
}
void Mem::init()
{
    reset();
}
void Mem::reset()
{
    if (mem != nullptr) { return; }
    for (int64_t i = 0; i < size; i++) {
        mem[i] = 0;
    }
}

uint8_t Mem::ld_byte(uint32_t addr)
{
    if (memaccess != nullptr) {
        return memaccess(addr, 0, 1 | (0 << 4));
    }
    int64_t rval = io->check_portl(addr);
    if (rval != -555)
        return rval;

    return mem_byte[addr];
};
uint16_t Mem::ld_halfword(uint32_t addr)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 1) {
            return memaccess(addr, 0, 1 | (1 << 4));
        }
        else {
            return (memaccess(addr, 0, 1 | (0 << 4)) << (8 * 0)) | (memaccess(addr + 1, 0, 1 | (0 << 4)) << (8 * 1));
        }
    }
    uint64_t addru = addr;
    int64_t  rval  = io->check_portl(addr);
    if (rval != -555)
        return rval;

    if (addr & 1)
        throw "ld_halfword: alignment error!";

    if (10485760 <= (addru >> 1))
        return io->ld_data(addr);
    else
        return mem_halfword[addru >> 1];
};
uint32_t Mem::ld_word(uint32_t addr)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 2) {
            return memaccess(addr, 0, 1 | (2 << 4));
        }
        else if (memoryaccesssize >= 1) {
            return (memaccess(addr, 0, 1 | (1 << 4)) << (16 * 0)) | (memaccess(addr + 2, 0, 1 | (1 << 4)) << (16 * 1));
        }
        else {
            return (memaccess(addr, 0, 1 | (0 << 4)) << (8 * 0)) | (memaccess(addr + 1, 0, 1 | (0 << 4)) << (8 * 1)) | (memaccess(addr + 2, 0, 1 | (0 << 4)) << (8 * 2)) | (memaccess(addr + 3, 0, 1 | (0 << 4)) << (8 * 3));
        }
    }
    uint64_t addru = addr;
    int64_t  rval  = io->check_portl(addr);
    if (rval != -555)
        return rval;

    if (addr & 3)
        throw "Unaligned ld_word: ";

    if (5242880 <= (addru >> 2))
        return io->ld_data(addr);
    else
        return mem_word[addru >> 2];
};

void Mem::st_byte(uint32_t addr, uint8_t onebyte)
{
    if (memaccess != nullptr) {
        memaccess(addr, onebyte, 0 | (0 << 4));
        return;
    }
    if (-555 != io->check_ports(addr, onebyte)) {
        return;
    }

    mem_byte[addr] = onebyte;
};
void Mem::st_halfword(uint32_t addr, uint16_t halfword)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 1) {
            memaccess(addr, halfword, 0 | (1 << 4));
        }
        else {
            memaccess(addr, (halfword >> (8 * 0)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 1, (halfword >> (8 * 1)) & 0xFF, 0 | (0 << 4));
        }
        return;
    }
    if (-555 != io->check_ports(addr, halfword)) {
        return;
    }

    if (addr & 1)
        throw "st_halfword: alignment error!";

    uint64_t addru = addr;
    if (10485760 <= (addru >> 1))
        io->st_data(addr, halfword);
    else
        mem_halfword[addr >> 1] = halfword;
};
void Mem::st_word(uint32_t addr, uint32_t word)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 2) {
            memaccess(addr, word, 0 | (2 << 4));
        }
        else if (memoryaccesssize >= 1) {
            memaccess(addr, (word >> (16 * 0)) & 0xFFFF, 0 | (1 << 4));
            memaccess(addr + 2, (word >> (16 * 1)) & 0xFFFF, 0 | (1 << 4));
        }
        else {
            memaccess(addr, (word >> (8 * 0)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 1, (word >> (8 * 1)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 2, (word >> (8 * 2)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 3, (word >> (8 * 3)) & 0xFF, 0 | (0 << 4));
        }
        return;
    }
    if (-555 != io->check_ports(addr, word)) {
        return;
    }

    uint64_t addru = addr;
    if (5242880 <= (addru >> 2))
        io->st_data(addr, word);
    else
        mem_word[addru >> 2] = word;
};

void Mem::st_word_unaligned(uint32_t addr, uint32_t word)
{
    if (-555 != io->check_ports(addr, word)) {
        return;
    }
    uint64_t addru = addr;
    uint64_t wordu = word;

    auto align = addr & 3;
    if (align == 0) {
        mem_word[addru >> 2] = word;
    } else if (align == 2) {
        st_halfword(addr, word & 0xffff);
        st_halfword(addr + 2, wordu >> 16);
    } else {
        st_byte(addr, word & 0xff);
        st_byte(addr + 1, (wordu >> 8) & 0xff);
        st_byte(addr + 2, (wordu >> 16) & 0xff);
        st_byte(addr + 3, wordu >> 24);
    }
};
uint8_t Mem::ld_byte_fast(uint32_t addr)
{
    if (memaccess != nullptr) {
        return memaccess(addr, 0, 1 | (0 << 4));
    }
    return mem_byte[addr];
};
uint16_t Mem::ld_halfword_fast(uint32_t addr)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 1) {
            return memaccess(addr, 0, 1 | (1 << 4));
        }
        else {
            return (memaccess(addr, 0, 1 | (0 << 4)) << (8 * 0)) | (memaccess(addr + 1, 0, 1 | (0 << 4)) << (8 * 1));
        }
    }
    return mem_halfword[addr >> 1];
};
uint32_t Mem::ld_word_fast(uint32_t addr)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 2) {
            return memaccess(addr, 0, 1 | (2 << 4));
        }
        else if (memoryaccesssize >= 1) {
            return (memaccess(addr, 0, 1 | (1 << 4)) << (16 * 0)) | (memaccess(addr + 2, 0, 1 | (1 << 4)) << (16 * 1));
        }
        else {
            return (memaccess(addr, 0, 1 | (0 << 4)) << (8 * 0)) | (memaccess(addr + 1, 0, 1 | (0 << 4)) << (8 * 1)) | (memaccess(addr + 2, 0, 1 | (0 << 4)) << (8 * 2)) | (memaccess(addr + 3, 0, 1 | (0 << 4)) << (8 * 3));
        }
    }
    uint64_t addru = addr;
    return mem_word[addru >> 2];
};
void Mem::st_byte_fast(uint32_t addr, uint8_t onebyte)
{
    if (memaccess != nullptr) {
        memaccess(addr, onebyte, 0 | (0 << 4));
        return;
    }
    mem_byte[addr] = onebyte;
};
void Mem::st_halfword_fast(uint32_t addr, uint16_t halfword)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 1) {
            memaccess(addr, halfword, 0 | (1 << 4));
        }
        else {
            memaccess(addr, (halfword >> (8 * 0)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 1, (halfword >> (8 * 1)) & 0xFF, 0 | (0 << 4));
        }
        return;
    }
    mem_halfword[addr >> 1] = halfword;
};
void Mem::st_word_fast(uint32_t addr, uint32_t word)
{
    if (memaccess != nullptr) {
        if (memoryaccesssize >= 2) {
            memaccess(addr, word, 0 | (2 << 4));
        }
        else if (memoryaccesssize >= 1) {
            memaccess(addr, (word >> (16 * 0)) & 0xFFFF, 0 | (1 << 4));
            memaccess(addr + 2, (word >> (16 * 1)) & 0xFFFF, 0 | (1 << 4));
        }
        else {
            memaccess(addr, (word >> (8 * 0)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 1, (word >> (8 * 1)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 2, (word >> (8 * 2)) & 0xFF, 0 | (0 << 4));
            memaccess(addr + 3, (word >> (8 * 3)) & 0xFF, 0 | (0 << 4));
        }
        return;
    }
    uint64_t addru       = addr;
    mem_word[addru >> 2] = word;
};
