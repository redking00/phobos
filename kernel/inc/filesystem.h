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

typedef struct directoryentry_t {
    uint8_t     type;
    uint8_t     name[116];
    uint32_t    sector;
    uint32_t    size;
}DIRECTORYENTRY;


typedef struct directorysector_t {
    uint32_t    up_sector;
    uint32_t    up_index;
    uint32_t    down_sector;
    DIRECTORYENTRY entry[4];
}DIRECTORYSECTOR;

typedef struct datasector_t{
    uint32_t    up_sector;
    uint32_t    up_index;
    uint32_t    down_sector;
    uint8_t     data[500];
}DATASECTOR;