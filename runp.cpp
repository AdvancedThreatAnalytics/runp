// runp.cpp : Run a binary payload in a wrapper, for easier analysis in debuggers.
// Created by Christopher Steffen for Critical Start

#include <windows.h>
#include <fileapi.h>
#include <stdio.h>

HANDLE OpenPayload(char *path){
    /* Returns a handle to an opened payload file. */
    return CreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
}

char *ReadPayload(HANDLE payload_handle){
    /* Returns a pointer to the byte array containing the payload. */
    DWORD payload_size = GetFileSize(payload_handle, NULL);
    DWORD bytes_read = 0;
    char *payload_buffer = new char[payload_size + 1];
    if(
        ReadFile(
            payload_handle, payload_buffer, payload_size, &bytes_read, NULL
        )
    ){
        // Successfully read payload bytes.
        payload_buffer[bytes_read] = '\0';
        if(bytes_read > 0){
            // Payload contains data.
            return payload_buffer;
        }
        return "EMPTY";
    }
    return "ERR";
}

void *CopyToMemory(char *payload_buffer){
    /* Take the payload, copy it into memory. */
    DWORD buffer_size = strlen(payload_buffer) + 1;
    void *function_pointer = VirtualAlloc(
        0,
        buffer_size,
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE
    );
    memcpy(function_pointer, payload_buffer, buffer_size);
    return function_pointer;
}

void RunPayload(void *function_pointer){
    /* Execute the binary payload from memory. */
    printf("Executing payload!");
    ((void(*)())function_pointer)();
}

int main(int argc, char **argv){
    if(argc < 2){
        printf(
            "Usage: %s [path to payload]\nExecutes the specified binary payload.\n",
            argv[0]
        );
        return 0;
    }

    // Get a file handle on the payload.
    HANDLE payload_handle = OpenPayload(argv[1]);
    if(payload_handle != INVALID_HANDLE_VALUE){
        // Load the contents.
        char *payload_buffer = ReadPayload(payload_handle);
        if(strcmp(payload_buffer, "ERR") == 0){
            printf("Error: Could not read payload contents.");
        }else if(strcmp(payload_buffer, "EMPTY") == 0){
            printf("Error: Payload is empty.");
        }else{
            void *function_pointer = CopyToMemory(payload_buffer);
            RunPayload(function_pointer);
        }
    }else{
        // Payload failed to load.
        printf("Error: Could not open payload at path %s", argv[1]);
    }
    CloseHandle(payload_handle);
    return 0;
}
