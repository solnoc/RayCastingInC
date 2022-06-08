#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


#define TC_NRM  "\x1B[0m"
#define TC_RED  "\x1B[1;31m"
#define TC_GRN  "\x1B[1;32m"
#define TC_YEL  "\x1B[1;33m"
#define TC_BLU  "\x1B[1;34m"
#define TC_MAG  "\x1B[1;35m"
#define TC_CYN  "\x1B[1;36m"
#define TC_WHT  "\x1B[1;37m"

#define TC_B_NRM  "\x1B[0m"
#define TC_B_RED  "\x1B[0;31m"
#define TC_B_GRN  "\x1B[0;32m"
#define TC_B_YEL  "\x1B[0;33m"
#define TC_B_BLU  "\x1B[0;34m"
#define TC_B_MAG  "\x1B[0;35m"
#define TC_B_CYN  "\x1B[0;36m"
#define TC_B_WHT  "\x1B[0;37m"

#define TC_BG_NRM "\x1B[40m"
#define TC_BG_RED "\x1B[41m"
#define TC_BG_GRN "\x1B[42m"
#define TC_BG_YEL "\x1B[43m"
#define TC_BG_BLU "\x1B[44m"
#define TC_BG_MAG "\x1B[45m"
#define TC_BG_CYN "\x1B[46m"
#define TC_BG_WHT "\x1B[47m"


int **buffer;
char ***buffer_color;
void draw_pixel(int i, int j, char* color, int block_type)
{
    char block[7];
    bool draw = false;
    switch(block_type)
    {
        case 0:
            if(buffer[i][j] != 0 || strcmp(buffer_color[i][j],color))
            {
                strncpy(block, "\u2588", 5);
                buffer[i][j] = 0;
                strcpy(buffer_color[i][j], color);
                draw = true;
            }
            break;
        case 1:
            if(buffer[i][j] != 1 || strcmp(buffer_color[i][j],color))
            {
                strncpy(block, "\u2593", 5);
                buffer[i][j] = 1;
                strcpy(buffer_color[i][j], color);
                draw = true;
            }
            break;
        case 2:
            if(buffer[i][j] != 2 || strcmp(buffer_color[i][j],color))
            {
                strncpy(block, "\u2592", 5);
                buffer[i][j] = 2;
                strcpy(buffer_color[i][j], color);
                draw = true;
            }
            break;
        default:
            draw = true;
            strncpy(block, "\u2588", 5);
            break;
    }
    if(draw)
    {
        printf("\033[%d;%dH", i, j*2);
        printf("%s%s",color,block);
        printf("\033[%d;%dH", i, j*2+1);
        printf("%s%s",color,block);
    }
}


int height;
int width;

struct texture{
    char ***encoding;
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

    textura.encoding = malloc(sizeof(char**) * textura.height);
    for(int i=0;i<textura.height;i++)
    {
        textura.encoding[i] = malloc(sizeof(char*) * textura.width);
        for(int j=0;j<width;j++)
        {
            textura.encoding[i][j] = malloc(sizeof(char) * 13);
        }
    }
    for(int i=0;i<textura.height;i++)
    {
        for(int j=0;j<textura.width;j++)
        {
            fscanf(file, "%s",textura.encoding[i][j]);
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

    buffer = malloc(sizeof(int*) * height);
    for(int i=0;i<height;i++)
    {
        buffer[i] = malloc(sizeof(int) * width);
    }
    buffer_color = malloc(sizeof(char**) * height);
    for(int i=0;i<height;i++)
    {
        buffer_color[i] = malloc(sizeof(char*) * width);
    }
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            buffer_color[i][j] = malloc(sizeof(char) * 13);
        }
    }
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            buffer[i][j] = -1;
            strcpy(buffer_color[i][j],"TC_GRN");
        }
    }

    struct texture wall = upload_texture("wall.tex");
    
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            draw_pixel(i,j,wall.encoding[i % wall.height][j % wall.width],0);
        }
    }

    return 0;
}