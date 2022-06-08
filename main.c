#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
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

int getch(void) {
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return(c);
}
int **buffer;
char ***buffer_color;
void draw_pixel(int i, int j, char color[], int block_type)
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

void sort_corners(float **array, int n)
{
    for(int i=0;i<n-1;i++)
    {
        for(int j=i+1;j<n;j++)
        {
            if(array[i][0] > array[j][0])
            {
                float aux[2] = {array[i][0], array[i][1]};
                array[i][0] = array[j][0];
                array[i][1] = array[j][1];
                array[j][0] = aux[0];
                array[j][1] = aux[1];
            }
        }
    }
}

int width;
int height;
char map[20][20];
long double playerAngle;
float px;
float py;
float fov;
clock_t begin_time;
long double fElapsedTime;
void Initialize()
{
    begin_time = clock();
    fElapsedTime = 1.0f;
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
    strncpy(map[0] , "####################", 20);
    strncpy(map[1] , "#                  #", 20);
    strncpy(map[2] , "#                  #", 20);
    strncpy(map[3] , "#   # #            #", 20);
    strncpy(map[4] , "#   # #            #", 20);
    strncpy(map[5] , "#   # #            #", 20);
    strncpy(map[6] , "#   # #######      #", 20);
    strncpy(map[7] , "#   #       #      #", 20);
    strncpy(map[8] , "#   ####### #      #", 20);
    strncpy(map[9] , "#         # #      #", 20);
    strncpy(map[10], "#                  #", 20);
    strncpy(map[11], "#                  #", 20);
    strncpy(map[12], "#                  #", 20);
    strncpy(map[13], "#                  #", 20);
    strncpy(map[14], "#                  #", 20);
    strncpy(map[15], "#                  #", 20);
    strncpy(map[16], "#                  #", 20);
    strncpy(map[17], "#                  #", 20);
    strncpy(map[18], "#                  #", 20);
    strncpy(map[19], "####################", 20);

    playerAngle = 0.0f;
    px = 4.0f;
    py = 1.0f;
    fov = 3.14159f/4.0f;
}
void UpdateAndDraw()
{
    clock_t end_time = clock();
    double FPS = 1.0f / fElapsedTime;
    if(FPS < 60.0f)
    {
        
        begin_time = end_time;
        int key;
        key = getch();
        if (key == 'a')
            playerAngle -= 1.0f * fElapsedTime;
        else if(key == 'd')
            playerAngle += 1.0f * fElapsedTime;
        else if(key == 'w')
        {
            float y_step = cos(playerAngle) * 4.0f * fElapsedTime;
            float x_step = sin(playerAngle) * 4.0f * fElapsedTime;
            py += y_step;
            px += x_step;
            if(map[(int)py][(int)px] == '#')
            {
                py -= y_step;
                px -= x_step;
            }
        }
        else if(key == 's')
        {
            float y_step = cos(playerAngle) * 4.0f * fElapsedTime;
            float x_step = sin(playerAngle) * 4.0f * fElapsedTime;
            py -= y_step;
            px -= x_step;
            if(map[(int)py][(int)px] == '#')
            {
                py += y_step;
                px += x_step;
            }
        }
        if(playerAngle > 3.14*2)
        {
            playerAngle -= 3.14*2;
        }
        else if(playerAngle < -3.14*2)
        {
            playerAngle += 3.14*2;
        }
        for(float i=0;i<width;i++)
        {
            long double current_angle = playerAngle - (fov/2.0f) + fov*(i/width);
            float rx = px;
            float ry = py;
            int rx_int = rx;
            int ry_int = ry;
            float eyeX = sin(current_angle);
            float eyeY = cos(current_angle);
            float d = 0;

            bool hitWall = false;
            bool hitCorner = false;
            while(!hitWall && rx <= 19 && ry <= 19 && rx>=0 && ry>=0)
            {
                d += 0.05;
                rx = px + eyeX * d;
                ry = py + eyeY * d;
                rx_int = rx;
                ry_int = ry;
                if(map[ry_int][rx_int] == '#')
                {
                    hitWall = true;
                    float **corners;
                    corners = malloc(sizeof(float) * 4);
                    for(int i=0;i<4;i++)
                    {
                        corners[i] = malloc(sizeof(float) * 2);
                    }
                    for(int cy=0;cy<=1;cy++)
                    {
                        for(int cx=0;cx<=1;cx++)
                        {
                            float vx = rx_int + cx - px;
                            float vy = ry_int + cy - py;
                            float dis = sqrt(vx*vx + vy*vy);
                            float cos_rays = (vx*eyeX + vy*eyeY) / dis;
                            corners[cy * 2 + cx][0] = dis;
                            corners[cy * 2 + cx][1] = cos_rays;
                        }
                    }
                    sort_corners(corners,4);
                    if(acos(corners[0][1]) < 0.01f) 
                    {
                        hitCorner = true;
                    } 
                    else if(acos(corners[1][1]) < 0.005f)
                    {
                        hitCorner = true;
                    }

                }
            }
            int celling = (height/2.0f) - height/d;
            int floor = height - celling;
            for(int j=0;j<height;j++)
            {
                if(j<celling)
                {
                    draw_pixel(j,i,TC_BLU,0);
                }
                else if(j>= celling && j<floor)
                {
                    if(d < 5.0f)
                        draw_pixel(j,i,TC_YEL,0 + hitCorner);
                    else if(d < 10.0f)
                        draw_pixel(j,i,TC_YEL,1 + hitCorner);
                    else
                        draw_pixel(j,i,TC_YEL,2);
                }
                else
                {
                    float poz = 1.0f - ((float)j - height/2.0f) / ((float)height/2.0f);
                    if(poz < 0.60f)
                        draw_pixel(j,i,TC_GRN,0);
                    else if(poz < 0.80f)
                        draw_pixel(j,i,TC_GRN,1);
                    else
                        draw_pixel(j,i,TC_GRN,2);
                }
            } 
        }
        fElapsedTime = (long double)(end_time - begin_time)/CLOCKS_PER_SEC;
    }
    else
    {
        fElapsedTime += (long double)(end_time - begin_time)/CLOCKS_PER_SEC;
    }
    begin_time = end_time;
}
void Destroy()
{

}

int main ()
{
    Initialize();
    while(true)
    {
        UpdateAndDraw();
    }
    return 0;
}
