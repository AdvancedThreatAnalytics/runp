// runp.cpp : Run a binary payload in a wrapper, for easier analysis in debuggers.
// Created by Kristen Steffen for Critical Start

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

int ReadPayload(HANDLE payload_handle, char* &payload_buffer){
    /* Returns a pointer to the byte array containing the payload. */
    int payload_size = GetFileSize(payload_handle, NULL);
    DWORD bytes_read = 0;
    char *buffer = new char[payload_size + 1];
    if(
        ReadFile(
            payload_handle, buffer, payload_size, &bytes_read, NULL
        )
    ){
        // Ensure null termination.
        buffer[bytes_read] = '\0';
        payload_buffer = buffer;
        return bytes_read;
    }
    return -1;
}

void *CopyToMemory(char *payload_buffer, int buffer_size){
    /* Copy the payload into executable memory. */
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

int main(int argc, char *argv[]){
    /* Execute a user-provided payload. */

    // Check for correct number of arguments.
    if(argc < 2){
        printf("Usage: %s <payload_path>", argv[0]);
        return 1;
    }

    // Open the payload file.
    HANDLE payload_handle = OpenPayload(argv[1]);
    if(payload_handle == INVALID_HANDLE_VALUE){
        printf("Unable to open payload. Please check the path.");
        return 1;
    }

    // Read payload into memory.
    char *payload_buffer;
    int payload_size = ReadPayload(payload_handle, payload_buffer);
    CloseHandle(payload_handle);

    // Ensure payload is valid.
    switch (payload_size)
    {
    case 0: // Payload is empty.
        printf("Payload is empty.");
        return 1;
    case -1: // ReadFile failed.
        printf("Unable to read payload. Please check file permissions.");
        return 1;
    default: // Payload is valid.
        break;
    }

    void *function_pointer = CopyToMemory(payload_buffer, payload_size);

    // Run the payload.
    RunPayload(function_pointer);
    return 0;
}
