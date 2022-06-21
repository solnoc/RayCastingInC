#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

enum e_color{ NRM,RED,GRN,YEL,BLU,MAG,CYN,WHY };
enum e_blockType{LIGHT, SHADED, DARK};

enum e_blockType **buffer;
enum e_color **buffer_color;
void draw_pixel(int i, int j, enum e_color color, enum e_blockType block_type)
{
    if(buffer[i][j] != block_type || buffer_color[i][j] != color)
    {
        buffer[i][j] = block_type;
        buffer_color[i][j] = color;

        char* drawColor = malloc(sizeof(char) * 13);
        char* block = malloc(sizeof(char) * 7);
        switch (color)
        {
        case NRM:
            strcpy(drawColor, "\x1B[0m");
            break;
        case RED:
            strcpy(drawColor, "\x1B[1;31m");
            break;
        case GRN:
            strcpy(drawColor, "\x1B[1;32m");
            break;
        case YEL:
            strcpy(drawColor, "\x1B[1;33m");
            break;
        case BLU:
            strcpy(drawColor, "\x1B[1;34m");
            break;
        case MAG:
            strcpy(drawColor, "\x1B[1;35m");
            break;
        case CYN:
            strcpy(drawColor, "\x1B[1;36m");
            break;
        case WHY:
            strcpy(drawColor, "\x1B[1;37m");
            break;
        default:
            strcpy(drawColor, "\x1B[1;37m");
            break;
        }
        switch (block_type)
        {
        case LIGHT:
            strcpy(block, "\u2588");
            break;
        case SHADED:
            strcpy(block, "\u2593");
            break;
        case DARK:
            strcpy(block, "\u2592");
            break;
        default:
            strcpy(block, "\u2592");
            break;
        }

        printf("\033[%d;%dH", i, j*2);
        printf("%s%s",drawColor,block);
        printf("\033[%d;%dH", i, j*2+1);
        printf("%s%s",drawColor,block);
    }
}

int height;
int width;

struct texture{
    enum e_color** colorEncoding;
    enum e_blockType** blockTypeEncoding;
    int height;
    int width;
};

struct texture upload_texture(const char* filepath)
{
    struct texture textura;

    FILE* file = fopen(filepath,"r");
    char* line = malloc(sizeof(char) * 256);
    fgets(line, 256, file);

    char* token = strtok(line, " ");
    textura.height = atoi(token);
    token = strtok(NULL," ");
    textura.width = atoi(token);

    textura.colorEncoding = malloc(sizeof(enum e_color*) * textura.height);
    textura.blockTypeEncoding = malloc(sizeof(enum e_blockType*) * textura.height); 
    for(int i=0;i<textura.height;i++)
    {
        textura.colorEncoding[i] = malloc(sizeof(enum e_color) * textura.width);
        textura.blockTypeEncoding[i] = malloc(sizeof(enum e_blockType) * textura.width);
    }
    for(int i=0;i<textura.height;i++)
    {
        for(int j=0;j<textura.width;j++)
        {
            fscanf(file, "%u",&textura.colorEncoding[i][j]);
            fscanf(file, "%u",&textura.blockTypeEncoding[i][j]);
        }
    }

    fclose(file);

    return textura;
}

int main()
{
    struct winsize ws;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
    height = ws.ws_row;
    width = ws.ws_col/2;

    buffer = malloc(sizeof(enum e_blockType*) * height);
    for(int i=0;i<height;i++)
    {
        buffer[i] = malloc(sizeof(enum e_blockType) * width);
    }
    buffer_color = malloc(sizeof(enum e_color*) * height);
    for(int i=0;i<height;i++)
    {
        buffer_color[i] = malloc(sizeof(enum e_color) * width);
    }
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            buffer[i][j] = DARK;
            buffer_color[i][j] = WHY;
        }
    }
    
    struct texture wall = upload_texture("wall.txt");
    
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            draw_pixel(i,j,wall.colorEncoding[i % wall.height][j % wall.width],wall.blockTypeEncoding[i % wall.height][j % wall.width]);
        }
    }

    return 0;
}