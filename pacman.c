#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#ifdef _WIN32
#include <io.h>
#define access _access

#else
#include <unistd.h>
#endif
#include <string.h>

// Oyun haritası boyutları
#define ROWS 20
#define COLS 40

#define SSROWS 20
#define SSCOLS 40

int pacmanX = 1, pacmanY = 1; // Pac-Man'in başlangıç konumu
int score = 0;

// Harita ve oyun değişkenleri
char startGame[SSROWS][SSCOLS] = {
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "############# START GAME ##############",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
};


// Harita ve oyun değişkenleri
char endGameLost[SSROWS][SSCOLS] = {
    "#######################################",//0
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "############## GAME OVER ##############",//9
    "################# PUAN ################",//10
    "#################      ################",//11 //17
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
};

// Harita ve oyun değişkenleri
char endGameWin[SSROWS][SSCOLS] = {
     "#######################################",//0
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "############# WIN WIN WIN #############",//9
    "################# PUAN ################",//10
    "#################      ################",//11 //17
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
    "#######################################",
};

// Harita ve oyun değişkenleri
char map[ROWS][COLS] = {
    "#######################################",
    "#.................#...................#",
    "#.#####.####....##.#.#####.####.#####.#",
    "#.#...............#...............#...#",
    "#.#.########. ######.#####.....####.#.#",
    "#.#.#.............................#.#.#",
    "#.#.#.#######............########.#.#.#",
    "#.#.#.#.........................#.#.#.#",
    "#.#.#.#.#####.#####.#####.#####.#.#.#.#",
    "#.#.#.#.#.....................#.#.#.#.#",
    "#.#.#.#.#.#########.#########.#.#.#.#.#",
    "#.#.#.#.#.#.................#.#.#.#.#.#",
    "#.#.#.#.#.#.######.########.#.#.#.#.#.#",
    "#.#.#.#.#.#...............#.#.#.#.#.#.#",
    "#.#.#.#.#.####......#####.#.#.#.#.#.#.#",
    "#.#.#.#.#...............#.#.#.#.#.#.#.#",
    "#.#.#.#.##.........####.#.#.#.#.#.#.#.#",
    "#.#.#.#...............#.#.#.#.#.#.#.#.#",
    "#.#.#.#######......####.#.#.#.#.#.#.#.#",
    "#######################################",
};

// Haritayı çizdir
void draw_map() {
    system("clear"); 
    for (int i = 0; i < ROWS; i++) {
        printf("%s\n", map[i]);
    }
}

// Haritayı çizdir
void drawScreen(char screenarray[SSROWS][SSCOLS]) {
    system("clear"); 
    for (int i = 0; i < SSROWS; i++) {
        printf("%s\n", screenarray[i]);
    }
}

// Pac-Man hareketi
void move_pacman(char input) {
    int nextX = pacmanX, nextY = pacmanY;

    if (input == 'w') nextX--;       // Yukarı
    else if (input == 's') nextX++;  // Aşağı
    else if (input == 'a') nextY--;  // Sol
    else if (input == 'd') nextY++;  // Sağ

    // Geçerli bir alana hareket et
    if (map[nextX][nextY] == '.' || map[nextX][nextY] == ' ') {
        if (map[nextX][nextY] == '.') {
            score++; // Yemek yediğinde skor artır
        }
        map[pacmanX][pacmanY] = ' '; // Eski konum boşalt
        pacmanX = nextX;             // Yeni konuma geç
        pacmanY = nextY;
        map[pacmanX][pacmanY] = 'P'; // Pac-Man'i yeni konuma koy
    }
}


// Terminali ham moda geçiren ve geri döndüren yardımcı fonksiyonlar
void enable_raw_mode(struct termios *orig_termios) {
    struct termios raw = *orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); // Kanonik modu ve Echo'yu kapat
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}


int main() {

    struct termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios); // Orijinal terminal ayarlarını kaydet
    enable_raw_mode(&orig_termios);        // Terminali ham moda al

    drawScreen(startGame);
    sleep(2);

    map[pacmanX][pacmanY] = 'P'; // Pac-Man'i haritaya yerleştir
    char input;

    while (1) {
        draw_map(); // Haritayı çiz
        printf("Move (WASD, q to quit): ");
        input = getchar(); // Kullanıcıdan giriş al
        if (input == 'q') break; // 'q' ile oyundan çık

        move_pacman(input); // Hareket et
    }



    char scoreStr[5]; // Skorun string versiyonu
    char digits[5];   // Her bir karakteri saklayacağımız dizi

    // 1. Tamsayıyı string'e çevir
    sprintf(scoreStr, "%d", score);

    // 2. Her karakteri ayrı bir diziye ata
    int len = strlen(scoreStr); // Skorun uzunluğu
    for (int i = 0; i < len; i++) {
        endGameLost[11][19+i] = scoreStr[i];
        endGameWin[11][19+i] = scoreStr[i];
    }

    sleep(2);

    if(score<40){
        drawScreen(endGameLost);
    }else if(score>39){
        drawScreen(endGameWin);
    }

    return 0;
}