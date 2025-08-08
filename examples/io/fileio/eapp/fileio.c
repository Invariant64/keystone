#include <stdio.h>
#include <string.h>
#include <dirent.h>

int main() {
    const char *filename = "test.txt";
    const char *write_data = "Hello from Keystone EApp!";
    char read_buffer[256] = {0};
    FILE *fp;
    int success = 1;

    printf("EApp: Starting file I/O test.\n");

    // 添加：读取 123 文件并打印内容
    const char *read_filename = "123";
    fp = fopen(read_filename, "r");
    if (fp == NULL) {
        printf("EApp: Failed to open file \"%s\" for reading.\n", read_filename);
    } else {
        printf("EApp: Content of file \"%s\":\n", read_filename);
        while (fgets(read_buffer, sizeof(read_buffer), fp) != NULL) {
            printf("%s", read_buffer);
        }
        fclose(fp);
        printf("\nEApp: Finished reading file \"%s\".\n", read_filename);
    }

    // 尝试读取文件
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("EApp: Error opening file for reading: %s\n", filename);
        success = 0;
    } else {
        printf("EApp: Successfully opened %s for reading.\n", filename);
        if (fgets(read_buffer, sizeof(read_buffer), fp) == NULL) {
            printf("EApp: Error reading from file: %s\n", filename);
            success = 0;
        } else {
            printf("EApp: Successfully read data from %s: \"%s\"\n", filename, read_buffer);
            if (strcmp(write_data, read_buffer) != 0) {
                printf("EApp: Data mismatch! Expected \"%s\", got \"%s\"\n", write_data, read_buffer);
                success = 0;
            } else {
                printf("EApp: Data matched successfully.\n");
            }
        }
        fclose(fp);
    }

    // 尝试写入文件
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("EApp: Error opening file for writing: %s\n", filename);
        success = 0;
    } else {
        printf("EApp: Successfully opened %s for writing.\n", filename);
        if (fputs(write_data, fp) == EOF) {
            printf("EApp: Error writing to file: %s\n", filename);
            success = 0;
        } else {
            printf("EApp: Successfully wrote data to %s.\n", filename);
        }
        fclose(fp);
    }

    if (!success) {
        printf("EApp: Write operation failed. Exiting.\n");
        return 1;
    }

    // 尝试读取文件
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("EApp: Error opening file for reading: %s\n", filename);
        success = 0;
    } else {
        printf("EApp: Successfully opened %s for reading.\n", filename);
        if (fgets(read_buffer, sizeof(read_buffer), fp) == NULL) {
            printf("EApp: Error reading from file: %s\n", filename);
            success = 0;
        } else {
            printf("EApp: Successfully read data from %s: \"%s\"\n", filename, read_buffer);
            if (strcmp(write_data, read_buffer) != 0) {
                printf("EApp: Data mismatch! Expected \"%s\", got \"%s\"\n", write_data, read_buffer);
                success = 0;
            } else {
                printf("EApp: Data matched successfully.\n");
            }
        }
        fclose(fp);
    }

    if (success) {
        printf("EApp: File I/O test PASSED.\n");
        return 0;
    } else {
        printf("EApp: File I/O test FAILED.\n");
        return 1;
    }
}
