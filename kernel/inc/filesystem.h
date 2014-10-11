#pragma pack(1)

typedef struct bootsector_t {
    uint8_t  jmp[3];
    uint8_t  volumeid[256];
    uint32_t sut_first_sector; //sector usage table start
    uint32_t sut_size;         //sector usage table size in sectors
    uint8_t  trash[243];
    uint8_t  signature[2];
} BOOTSECTOR;