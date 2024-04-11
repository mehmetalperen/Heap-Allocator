// Mehmet Nadi 56102231
// Khoi Pham 91404433
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define HEAP_SIZE 127

void set_header(unsigned char *heap, int header_idx, int block_size, bool allocated)
{
    heap[header_idx] = block_size << 1;
    if (allocated)
    {
        heap[header_idx] = heap[header_idx] | 0b00000001;
    }
    else
    {
        heap[header_idx] = heap[header_idx] & 0b11111110;
    }
}

void heap_init(unsigned char *heap)
{
    memset(heap, 0, HEAP_SIZE);
    set_header(heap, 0, HEAP_SIZE, false);
}

int malloc_block(unsigned char *heap, int payload_size)
{

    int request_block_size = payload_size + 1; // +1 for header
    int best_block = -1;
    unsigned char best_block_size = HEAP_SIZE + 1;
    unsigned char current_block = 0;

    while (current_block < HEAP_SIZE)
    {
        unsigned char current_header = heap[current_block];
        unsigned char current_block_size = current_header >> 1;
        if (current_block_size == 0) {
            printf("ERROR: Header corrupted\n");
            exit(0);
        }
        unsigned char is_curr_block_allocated = current_header & 0b00000001;
        if (!is_curr_block_allocated && current_block_size >= request_block_size)
        {
            if (current_block_size < best_block_size)
            {
                best_block = current_block;
                best_block_size = current_block_size;
            }
        }
        current_block += current_block_size;
    }

    if (best_block != -1)
    {
        set_header(heap, best_block, request_block_size, true);

        int remaining_space = best_block_size - request_block_size;
        if (remaining_space >= 2) // if remaining space is enough for a new block: at least 2 bytes 1 for header, 1 for payload
        {
            set_header(heap, best_block + request_block_size, remaining_space, false); // set header for the split part
        }
        else
        {
            set_header(heap, best_block, best_block_size, true); // adjust the block size to include the remaining space if it's too small to form a new block
        }
        return best_block + 1; // return address of payload
    }

    return -1;
}
void free_block(unsigned char *heap, int payload_address)
{
    int header_idx = payload_address - 1;
    unsigned char current_header = heap[header_idx];
    int current_block_size = current_header >> 1;

    set_header(heap, header_idx, current_block_size, false); // false means free

    int next_block_idx = header_idx + current_block_size;

    /*
    HANDLE EDGE CASE:
    letws say you have a block like this: 1 1 1
    free middle one you have this: 1 0 1
    then free  last one, you have this 1 0 0
    then free first one you have this: 00 0 but they should be merged you shouldve got sometihng like 000
    keep merging until  you either hit an allocated block or end of the heap */
    while (next_block_idx < HEAP_SIZE)
    {
        unsigned char next_header = heap[next_block_idx];
        int next_block_size = next_header >> 1;
        bool is_next_block_allocated = next_header & 0b00000001;

        if (!is_next_block_allocated)
        {
            current_block_size += next_block_size;
            set_header(heap, header_idx, current_block_size, false);
            next_block_idx += next_block_size;
        }
        else
        {
            break; // stop coalessing if neighboor block is allocated
        }
    }
}

void write_mem(unsigned char *heap, int start_address, char *data)
{
    int i = 0;
    while (data[i] != '\0' && (start_address + i) < HEAP_SIZE)
    {
        heap[start_address + i] = data[i];
        i++;
    }
}

int realloc_block(unsigned char *heap, int curr_payload_address, int new_payload_size)
{
    int curr_header_address = curr_payload_address - 1;
    int new_block_size = new_payload_size + 1;
    unsigned char curr_header_val = heap[curr_header_address];
    int curr_block_size = curr_header_val >> 1;

    if (new_block_size < curr_block_size - 1) // Truncate and split
    {
        int excess_block_size = curr_block_size - new_block_size;
        int excess_header_address = curr_header_address + new_block_size;
        set_header(heap, excess_header_address, excess_block_size, true);
        free_block(heap, excess_header_address + 1);
        // set_header(heap, excess_header_address, excess_block_size, false);

        set_header(heap, curr_header_address, new_block_size, true);
        return curr_payload_address;
    }
    else if (new_block_size == curr_block_size - 1)
    {
        printf("No need to truncate since remainder block is too small (zero payload size)\n");
        return curr_payload_address;
    }
    else if (new_block_size == curr_block_size)
    {
        printf("No need to realloc since new payload size is the same as old.\n");
        return curr_payload_address;
    }
    else if (new_block_size > curr_block_size)
    {
        int adjacent_free_size = 0;
        int next_header_address = curr_header_address + curr_block_size;
        unsigned char next_header_value = heap[next_header_address];
        int next_block_size = next_header_value >> 1;
        int next_block_is_allocated = next_header_value & 0b00000001;
        while (!next_block_is_allocated && next_header_address < HEAP_SIZE && new_block_size > adjacent_free_size + curr_block_size)
        {
            if (next_block_size == 0) {
                printf("ERROR: Header corrupted\n");
                exit(0);
            }
            adjacent_free_size += next_block_size;

            next_header_address = next_header_address + next_block_size;
            next_header_value = heap[next_header_address];
            next_block_size = next_header_value >> 1;
            next_block_is_allocated = next_header_value & 0b00000001;
        }

        if (new_block_size == curr_block_size + adjacent_free_size
                || new_block_size == curr_block_size + adjacent_free_size - 1) // Expand fully
        {
            set_header(heap, curr_header_address, curr_block_size + adjacent_free_size, true);
            return curr_payload_address;
        }
        else if (new_block_size < curr_block_size + adjacent_free_size - 1) // Expand partly and split
        {
            int excess_block_size = curr_block_size + adjacent_free_size - new_block_size;
            int excess_header_address = curr_header_address + new_block_size;
            set_header(heap, excess_header_address, excess_block_size, false);
            set_header(heap, curr_header_address, new_block_size, true);
            return curr_payload_address;
        }
        else // Find a new block
        {
            int new_payload_address = malloc_block(heap, new_payload_size);
            if (new_payload_address == -1)
            {
                printf("Couldn't find enough space to realloc\n");
                return -1;
            }
            else
            {
                int curr_payload_size = curr_block_size - 1;
                memcpy(&heap[new_payload_address], &heap[curr_payload_address], curr_payload_size*sizeof(unsigned char));
                free_block(heap, curr_payload_address);
                return new_payload_address;
            }
        }
    }
}

void print_block_list(unsigned char *heap)
{
    unsigned char current_block = 0;
    while (current_block < HEAP_SIZE)
    {
        unsigned char current_header = heap[current_block];
        unsigned char current_block_size = current_header >> 1;
        unsigned char is_curr_block_allocated = current_header & 0b00000001;

        if (is_curr_block_allocated)
        {
            printf("%d, %d, allocated\n", current_block + 1, current_block_size - 1);
        }
        else
        {
            printf("%d, %d, free\n", current_block + 1, current_block_size - 1);
        }

        current_block = current_block + current_block_size;

        if (current_block_size == 0) {
            printf("ERROR: Header corrupted\n");
            exit(0);
        }
        if (current_block_size == 1) {
            printf("WARNING: Payload size ZERO\n");
        }
    }
}

int main(int argc, char *argv[])
{
    unsigned char heap[HEAP_SIZE];
    heap_init(heap);

    char command[10];
    while (1)
    {
        printf("> ");
        scanf("%s", command);
        if (strcmp(command, "quit") == 0)
        {
            return 0;
        }
        else if (strcmp(command, "malloc") == 0)
        {
            int payload_size = 0;
            scanf("%d", &payload_size);
            int header = malloc_block(heap, payload_size);

            if (header == -1)
            {
                printf("No suitable block found\n");
            }
            else
            {
                printf("%d\n", header);
            }
        }
        else if (strcmp(command, "realloc") == 0)
        {
            int curr_payload_address;
            int new_payload_size;
            scanf("%d %d", &curr_payload_address, &new_payload_size);
            int new_payload_address = realloc_block(heap, curr_payload_address, new_payload_size);
            printf("%d\n", new_payload_address);
        }
        else if (strcmp(command, "free") == 0)
        {
            int payload_address = 0;
            scanf("%d", &payload_address);
            free_block(heap, payload_address);
        }
        else if (strcmp(command, "blocklist") == 0)
        {
            print_block_list(heap);
        }
        else if (strcmp(command, "writemem") == 0)
        {
            int start_address;
            char data[HEAP_SIZE];
            scanf("%d %s", &start_address, data);

            write_mem(heap, start_address, data);
        }
        else if (strcmp(command, "printmem") == 0)
        {
            int start_location = 0;
            int print_length = 0;
            scanf("%d %d", &start_location, &print_length);
            for (int i = start_location; i < start_location + print_length; i++)
            {
                printf("%x ", heap[i]);
            }
            printf("\n");
        }
        else
        {
            printf("Invalid command\n");
        }
    }
}