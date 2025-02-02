#include <stdio.h>

#define HEIGHT 25
#define WIDTH 80
#define VX 1
#define VY 1

void draw_screen(int x0, int y0, int xl, int yl, int xr, int yr, int score_l, int score_r);
int control_rocketl(char c, int x0, int y0, int xl, int yl, int xr, int yr);
int control_rocketr(char c, int x0, int y0, int xl, int yl, int xr, int yr);
int move_ball_x(int x0, int y0, int xl, int yl, int xr, int yr);
int move_ball_y(int x0, int y0, int xl, int yl, int xr, int yr);
int check_l(int x0);
int check_r(int x0);


int main() {
    char c;
    int mv_ball_x = VX;
    int mv_ball_y = VY;
    int score_l = 0, score_r = 0;

    int x0 = (WIDTH + 1) / 2, y0 = (HEIGHT + 1) / 2;
    int xr = WIDTH - 1, yr = (HEIGHT + 1) / 2; 
    int xl = 2, yl = (HEIGHT + 1)/2;
    draw_screen(x0, y0, xl, yl, xr, yr, score_l, score_r);
    while ((c = getchar()) != 'q')
    {
        getchar();
        putchar(c);
        putchar('\n');
        yl = control_rocketl(c, x0, y0, xl, yl, xr, yr);
        yr = control_rocketr(c, x0, y0, xl, yl, xr, yr);
        
        mv_ball_x = move_ball_x(x0, y0, xl, yl, xr, yr) * mv_ball_x ;
        x0 = x0 + mv_ball_x;
        //printf("%d %d\n", x0, mv_ball_x);
        mv_ball_y = move_ball_y(x0, y0, xl, yl, xr, yr) * mv_ball_y ;
        y0 = y0 + mv_ball_y;
        if (check_l(x0)) score_l++;
        if (check_r(x0)) score_r++;
       // printf("%d %d\n", y0, mv_ball_y);

        //printf("%c %d %d %d %d %d %d\n", c, x0, y0, xl, yl, xr, yr);
        draw_screen(x0, y0, xl, yl, xr, yr, score_l, score_r);
      //int control_rocket(char c, int x0, int y0, int xl, int yl, int xr, int yr);  
    }
    
    
    return 0;
}

int check_l(int x0) {
    if (x0 == 1) return 1; else return 0;
}

int check_r(int x0) {
    if (x0 == WIDTH-1) return 1; else return 0;
}

int check_r(int x0);


int move_ball_x(int x0, int y0, int xl, int yl, int xr, int yr) {
    if (x0 == 2 || x0 == WIDTH - 1 || (x0 == xl + 1 &&  (y0 == yl || \
    y0 == yl - 1 || y0 == yl + 1) ) || (x0 == xr - 1 &&  (y0 == yr || \
    y0 == yr - 1 || y0 == yr + 1) )) return -1; else return 1;}
    
int move_ball_y(int x0, int y0, int xl, int yl, int xr, int yr) {if (y0 == 2 || y0 == HEIGHT - 1 ) return -1; else return 1;}

int control_rocketl(char c, int x0, int y0, int xl, int yl, int xr, int yr) {
    if (c == 'z' && yl + 2 < HEIGHT) yl++; else if (c == 'a' && yl - 3 > 0) yl--; 
    return yl;
}

int control_rocketr(char c, int x0, int y0, int xl, int yl, int xr, int yr) {
    if (c == 'm' && yr + 2 < HEIGHT) yr++;  else if (c == 'k' && yr - 3 > 0) yr--; 
    return yr;
}


void draw_screen(int x0, int y0, int xl, int yl, int xr, int yr, int score_l, int score_r){
    //printf("\033[0d\033[2J");
    for (int i = 1; i <= HEIGHT; i++){
        for (int j = 1; j <= WIDTH; j++){
            if (i == 1 || i == HEIGHT) printf("-"); else  
            if (j == 1 || j == WIDTH) printf("|"); else 
            if (j == x0 && i == y0 ) printf("o"); else
            if (j == xl && i == yl) printf("L"); else
            if (j == xr && i == yr) printf("R"); else
            if (j == xl  && i == yl -1) printf("L"); else
            if (j == xr && i == yr - 1)  printf("R"); else
            if (j == xl && i == yl + 1) printf("L"); else
            if (j == xr && i == yr + 1) printf("R"); else
            if (j == 4 && i == 4) printf("%d", score_l); else
            if (j == WIDTH - 4 && i == 4) printf("%d", score_r); else
            printf(" ");

    
        } printf("\n");
    }
}