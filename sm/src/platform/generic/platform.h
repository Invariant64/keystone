
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

// No special data needed for default platform
struct platform_enclave_data{

};

// Enclave configuration
#define ENCL_MAX                16
#define ENCLAVE_REGIONS_MAX     8

// SM configuration
#define SMM_BASE                0x80000000
#define SMM_SIZE                0x200000

// PMP configuration
#define PMP_N_REG               8
#define PMP_MAX_N_REGION        16

// CPU configuration
#define MAX_HARTS               16

// Initialization functions
void sm_copy_key(void);

typedef unsigned char byte;

// Sanctum header fields in DRAM
extern byte sanctum_dev_public_key[32];
extern byte sanctum_dev_secret_key[64];
// unsigned int sanctum_sm_size = 0x1ff000;
extern byte sanctum_sm_hash[64];
extern byte sanctum_sm_public_key[32];
extern byte sanctum_sm_secret_key[64];
extern byte sanctum_sm_signature[64];

#endif // _PLATFORM_H_

