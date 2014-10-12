#pragma pack(1)

#define DELETED_ENTRY   0
#define DIRECTORY_ENTRY 1
#define FILE_ENTRY      2


typedef struct bootsector_t {
    uint8_t  jmp[3];
    uint8_t  fsid[9];          //PHOBOSFS
    uint8_t  volumeid[256];
    uint32_t sut_first_sector; //sector usage table start
    uint32_t sut_size;         //sector usage table size in sectors
    uint8_t  trash[234];
    uint8_t  signature[2];
} BOOTSECTOR;

typedef struct fsentry_t {
    uint8_t  type;
    uint8_t  name[103];
    uint32_t right_e_sector;
    uint8_t  right_e_index;
    uint32_t left_e_sector;
    uint8_t  left_e_index;
    uint32_t top_e_sector;
    uint8_t  top_e_index;
    uint32_t down_e_sector;
    uint8_t  down_e_index;
    uint32_t size_in_bytes;
}FSENTRY;


typedef struct entrysector_t {
    FSENTRY entry[4];
}ENTRYSECTOR;

typedef struct datasector_t {
    uint32_t top_e_sector;
    uint8_t  top_e_index;
    uint32_t down_e_sector;
    uint8_t  down_e_index;
    uint8_t  data[502];
}DATASECTOR;



