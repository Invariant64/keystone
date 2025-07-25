#include "results.h"
#include "functions.h"
#include "../engines/mmap.h"
#include "../engines/mmap_lat.h"
#include "../engines/mem_lat.h"
#include "../engines/page_fault.h"
#include "parser.h"

int main(int argc, char *argv[])
{

    //example1,这个能跑
    static int membench_argc_hardcoded = 9;
    static char *membench_argv_hardcoded[] = {
        (char *)"./membench",
        (char *)"-runtime=5",
        (char *)"-dir=/dev/zero",
        (char *)"-size=4M",//原来是3G
        (char *)"-copysize=4K",
        (char *)"-mode=randread",
        (char *)"-engine=mmap",
        (char *)"-map_pop=0",//原来是 =1 ,为了能跑改成0
        (char *)"-map_shared=0",
        NULL
    };

    // 因为runtime的mmap只支持MAP_ANONYMOUS和MAP_PRIVATE，所以map_pop=1 map_shared=1会报错
    // //example2
    // static int membench_argc_hardcoded = 5;
    // static char *membench_argv_hardcoded[] = {
    //     (char *)"./membench",
    //     (char *)"-dir=/mnt/mem/file",
    //     (char *)"-map_pop=1",
    //     (char *)"-iter=1",
    //     (char *)"-map_shared=1",
    //     NULL
    // };

    //example3
    // 因为runtime的mmap只支持MAP_ANONYMOUS和MAP_PRIVATE，所以map_shared=1会报错
    // static int membench_argc_hardcoded = 13;
    // static char *membench_argv_hardcoded[] = {
    //     (char *)"./membench",
    //     (char *)"-runtime=100",
    //     (char *)"-engine=mmap_lat",
    //     (char *)"-runtime=50",
    //     (char *)"-dir=/mnt/mem/file",
    //     (char *)"-size=4K",
    //     (char *)"-copysize=4B",
    //     (char *)"-mode=randread",
    //     (char *)"-engine=mmap",
    //     (char *)"-map_pop=0",
    //     (char *)"-map_shared=1",
    //     (char *)"-cpy_iter=1000",
    //     (char *)"-iter=1",
    //     NULL
    // };

    argc = membench_argc_hardcoded; // Assign to the existing local variable
    argv = membench_argv_hardcoded; // Assign to the existing local variable

    Arguments args = {0, "", 0, 0, 0, 0, -1, 1, 0, 0, 1};

    parse(&args, argc, argv);
    switch (args.engine)
    {
    case 0:
        mmap_engine(&args);
        break;
    case 1:
        mmap_lat_engine(&args);
        break;
    case 2:
        mem_lat_engine(&args);
        break;
    case 3:
        page_fault_lat_engine(&args);
        break;
    default:
        LOG(ERROR, EINVAL, "Engine");
        break;
    }

    if (strlen(args.path) > 0)
        free_tok(args.path);

    return 0;
}