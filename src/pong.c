#include <stdio.h>

#define HEIGHT 25
#define WIDTH 80

void draw_screen(int x0, int y0, int xl, int yl, int xr, int yr);


int main() {
    int x0 = (HEIGHT + 1)/2, y0 = (WIDTH + 1)/2;
    int xr = WIDTH -1, yr = (HEIGHT + 1)/2; 
    int xl = 2, yl = (HEIGHT + 1)/2;
    draw_screen(x0, y0, xl, yl, xr, yr);
    return 0;
}

void draw_screen(int x0, int y0, int xl, int yl, int xr, int yr){
    printf("\033[0d\033[2J");
    for (int i = 1; i <= HEIGHT; i++){
        for (int j = 1; j <= WIDTH; j++){
            if (i == 1 || i == HEIGHT) printf("-"); else  
            if (j == 1 || j == WIDTH) printf("|"); else 
            if (i == x0 && j == y0 ) printf("O"); else
            if ((i == xl && j == yl)  (i == xr && j == yr)  (i == xl  && j == yl -1)  (i == xr && j == yr - 1)  (i == xl && j == yl + 1) || (i == xr && j == yr + 1)) printf("XXX"); else
            printf(" ");

    
        } printf("\n");
    }
}
