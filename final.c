// Pat Myron and Anna McMahon
#include <stdio.h>
#include <math.h>
#include "gfxModified.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// structs
typedef struct person {   // dont forget typdef
    char name[20];
    float x;
    float y; //position

    float vx; //velocity
    float vy;

    int r;  //radius

    int boundaries[2];
    bool onGround;
    int lives;
    int color[3];
    int wins;
    struct person *next;
} Person_type;
typedef struct ball {
    float x;
    float y; //position

    float vx; //velocity
    float vy;

    int r;  //radius



} Ball_type;

// declarations must go below structs so that the type thing will work
// function declarations
void drawPlayer(Person_type);

void moveBall(Ball_type *, Person_type *, Person_type *);

void jump(Person_type *pPlayer, Ball_type *pBall);

void validatePlayerMovement(Person_type *);

void updatePlayerScoreBoard(Person_type);

bool gameOver(Person_type *, Person_type *);

void drawBall(Ball_type ball);

void movePlayer(Person_type *, Person_type *, int c, bool keys[6]);

void calculateBallsDirection(Ball_type *Ball, Person_type *Player);

void checkForCollisions(Ball_type *, Person_type *);

void drawNet();

void bringPlayerToGround(Person_type *pPlayer);

void resetGame(Person_type *, Person_type *, Ball_type *);

void getNames(char playerNames[2][20]);

float calculateAngleBetweenCenters(float playerx, float playery, float ballx, float bally);

void getNames2(char playerNames[20], int colors[3]);

void makeNode(Person_type **, Person_type *);

void displayData(Person_type *, Person_type *, Ball_type *, Person_type *, bool *quit);

int optionsMenu();

void drawPlayerKeysAtStartScreen();

void newPlayers(Person_type *, Person_type *, Ball_type *);

Person_type *playerExists(Person_type *, Person_type *);

void actOnOptions(Person_type *, Person_type *, Ball_type *, Person_type *, bool *quit);

int wd = 1200, ht = 700;
bool keys[6];  //global for my sanity

int main() {
    // initialize variables and structs
    int c;
    Person_type *head = NULL, *p1, *p2, *p11, *p22;

    //variables used to set the values for players
    int vx = 0, vy = 0, lives = 3, colors1[3] = {0, 255, 0}, colors2[3] = {0, 0, 255}, wins = 0, radius = 50;
    bool onground = true;

    bool rquit = false, *quit = &rquit;

    // initializes two player structs and one ball struct
    Person_type player1 = {" ", wd / 4, ht, vx, vy, radius, {0, wd / 2}, onground, lives, {0, 255, 0}, wins,
                           p1}, *pPlayer1;
    Person_type player2 = {" ", 3 * wd / 4, ht, vx, vy, radius, {wd / 2, wd}, onground, lives, {0, 0, 255}, wins,
                           p2}, *pPlayer2;
    Ball_type ball = {wd / 4, ht / 2, 0, 10, 10}, *pBall;
    pPlayer1 = &player1;
    pPlayer2 = &player2;
    pBall = &ball;
    bool escapeKeyPressed = false; //escape key terminates game while playing
    int i, j;

    for (i = 0; i < 6; i++) {
        keys[i] = 0;   //initializing array of keys to false
    }
    gfx_open(wd, ht, "My window");
    getNames2(player1.name, player1.color);
    getNames2(player2.name, player2.color);

    while (1) {
        if (gfx_event_waiting2()) {

            escapeKeyPressed = gfx_keyPress(keys);
            if (escapeKeyPressed) return 0;
            movePlayer(pPlayer1, pPlayer2, c, keys);
            validatePlayerMovement(pPlayer1);
            validatePlayerMovement(pPlayer2);
        }
        moveBall(pBall, pPlayer1, pPlayer2);
        drawBall(*pBall);

        bringPlayerToGround(pPlayer1);
        bringPlayerToGround(pPlayer2);

        drawPlayer(*pPlayer1);
        drawPlayer(*pPlayer2);

        updatePlayerScoreBoard(player1);
        updatePlayerScoreBoard(player2);

        //make net
        drawNet();

        if (gameOver(pPlayer1, pPlayer2)) {

            p11 = (playerExists(head, pPlayer1));
            p22 = (playerExists(head, pPlayer2));
            if (p11 == NULL)
                makeNode(&head, pPlayer1);

            else if (pPlayer1->lives > 0)
                p11->wins += 1;


            if (p22 == NULL) {

                makeNode(&head, pPlayer2);
            } else if (pPlayer2->lives > 0) {
                p22->wins += 1;
            }

            if ((*quit) == true)//(actOnOptions(pPlayer1, pPlayer2, pBall, head)==0)
                return 0;
            int act;
            actOnOptions(pPlayer1, pPlayer2, pBall, head, quit);
            if ((*quit) == true)//(actOnOptions(pPlayer1, pPlayer2, pBall, head)==0)
                return 0;
        }
        gfx_flush();
        usleep(9000);
    }//end while
}//end main

// this calls options menu and uses the return to decide what to do next
void actOnOptions(Person_type *pPlayer1, Person_type *pPlayer2, Ball_type *pBall, Person_type *head, bool *quit) {
    int opt = optionsMenu();
    if (opt == 1)
        resetGame(pPlayer1, pPlayer2, pBall);
    else if (opt == 2) {
        displayData(pPlayer1, pPlayer2, pBall, head, quit);
    } else if (opt == 3) {
        *quit = true;
    } else
        newPlayers(pPlayer1, pPlayer2, pBall);
}

// this iterates through the linked list and looks to see if a player already exists, and then returns a pointer that player if it finds it or null otherwise
Person_type *playerExists(Person_type *p, Person_type *

new ){
int found = 0;  // a flag to check if a person with a matching name has been founs
while (!
found &&((p)
!= NULL)) {
if (
strcmp((p)
->name,new->name)==0){
found = 1;
}
else
(p) = (p)->
next;      // advance pointer
}
return
p;
}

// this provides an interface for the user to enter in two players at the begining of the game, and new players each time 'n' is selected from the menu
void getNames2(char playerNames[20], int colors[3]) {
    static int x = 1;
    int j;
    for (j = 0; j < 20; j++) {
        playerNames[j] = ' ';
    }
    int i = 0, c, k = 1;
    char str1[60], str2[60];
    for (j = 0; j < 60; j++) {
        str1[j] = ' ';
        str2[j] = ' ';
    }
    gfx_color(colors[0], colors[1], colors[2]);
    gfx_fill_rectangle(wd / 2 - 150, ht / 2 - 30, 300, 60);
    drawPlayerKeysAtStartScreen();
    drawPlayerKeysAtStartScreen(); // called twice to fix very strange error
    gfx_color(0, 0, 0);
    gfx_flush();
    if (x > 0)
        gfx_text((wd / 2) - 100, ht / 2, "Player 1 name:");
    else
        gfx_text((wd / 2) - 100, ht / 2, "Player 2 name:");
    gfx_flush();
    c = 0;
    i = 0;

    while (c != 13) { //enter
        if (gfx_event_waiting()) {
            c = gfx_wait();
            if (c == 8) {//backspace
                playerNames[i - 1] = ' ';
                i--;
            }
            if (c == 13) {//enter
                continue;
            }
            if (i == 20) {
                continue;
            }
            if (c != 8) {
                playerNames[i] = c;
                i++;
            }
            gfx_clear();
            gfx_color(colors[0], colors[1], colors[2]);
            gfx_fill_rectangle(wd / 2 - 150, ht / 2 - 30, 300, 60);

            drawPlayerKeysAtStartScreen();

            gfx_color(0, 0, 0);
            if (x > 0) {
                snprintf(str1, 36, "Player 1 name: %s ", playerNames);
                gfx_text((wd / 2) - 100, ht / 2, str1);
            } else {
                snprintf(str2, 36, "Player 2 name: %s ", playerNames);
                gfx_text((wd / 2) - 100, ht / 2, str2);
            }
            gfx_flush();
            usleep(1000);
        }
    }
    x = -x;
}

// this resets the game each time another game begins (ie after the user chooses 'p' or after the user changes players)
void resetGame(Person_type *pPlayer1, Person_type *pPlayer2, Ball_type *pBall) {
    pPlayer1->lives = 3;
    pPlayer2->lives = 3;
    pPlayer1->x = wd / 4;
    pPlayer2->x = 3 * wd / 4;
    pBall->x = wd / 4;
    pBall->y = ht / 2;
    pBall->vy = 10;
    pBall->vx = 0;
}

// this displays the first screen again which allows the user to switch players in the game and th
void newPlayers(Person_type *pPlayer1, Person_type *pPlayer2, Ball_type *pBall) {
    gfx_clear();
    getNames2(pPlayer1->name, pPlayer1->color);
    getNames2(pPlayer2->name, pPlayer1->color);
    pPlayer1->wins = 0;
    pPlayer2->wins = 0;
    resetGame(pPlayer1, pPlayer2, pBall);


}

// draws the net
void drawNet() {
    gfx_color(255, 255, 255);
    gfx_line(wd / 2, ht, wd / 2, ht - 120);
}

// draws a player (half circle filled in)
void drawPlayer(Person_type player) {
    int j;
    float leftEP, rightEP, height; //used to draw the player filled in
    int numLines = 80;
    float part = M_PI / 2 / numLines;
    float parteye = 2 * M_PI / numLines;//contolls filling in the eye
    //sets color to that of player
    gfx_color(player.color[0], player.color[1], player.color[2]);
    for (j = 0; j < numLines; j++) {
        leftEP = player.x + cos(M_PI + part * j) * player.r;
        rightEP = player.x + cos(2 * M_PI - part * j) * player.r;
        height = player.y + sin(M_PI + part * j) * player.r;
        gfx_line(leftEP, height, rightEP, height);
    }
    // the below makes the eye on the player
    gfx_color(255, 255, 255);
    for (j = 0; j < 20; j++) {
        leftEP = player.x + 10 + cos(M_PI + parteye * j) * player.r / 8;
        rightEP = player.x + 10 + cos(2 * M_PI - parteye * j) * player.r / 8;
        height = player.y - 25 + sin(M_PI + parteye * j) * player.r / 8;
        gfx_line(leftEP, height, rightEP, height);
    }
}

// this updates the position and velocity of the ball based
void moveBall(Ball_type *pBall, Person_type *pPlayer1, Person_type *pPlayer2) {
    Person_type *pPlayer;
    //check which side of the board the ball is on, only the player on the same side of the board is a parameter in checkForCollisions
    pPlayer = ((pBall->x) < (wd / 2)) ? pPlayer1 : pPlayer2;
    //checkForCollisions checks to see if the ball is colliding with anything and changes the balls velocity accordingly
    checkForCollisions(pBall, pPlayer);
    // adds the change in x/y to the ball
    (pBall->vy) += .08;// mimics gravity
    (pBall->x) = (pBall->x) + (pBall->vx);
    (pBall->y) = (pBall->y) + (pBall->vy);
    gfx_clear();
}

// check the surroundings of the ball and update the location accordingly, change the velocity if the ball "hits" anything. (ie net,boundary, edge, of the screen)
void checkForCollisions(Ball_type *pBall, Person_type *pPlayer) {
    float touchingDist, playerBallDist;
    bool playerDeath = false;

    if ((pBall->x) < (pBall->r) || (pBall->x) > wd - (pBall->r))  // side collisions
        pBall->vx = -(pBall->vx);
    if ((pBall->y) < (pBall->r))   //top collisions
        pBall->vy = -(pBall->vy);
    if ((pBall->y) > ht - (pBall->r)) {   //ground collisions
        pBall->vy = -(pBall->vy);
        playerDeath = true;
    }
    // decrements player lives
    if (playerDeath) {
        (pPlayer->lives)--;
        gfx_color(255, 0, 0);
        pBall->y = ht / 2;              // putting it above him
        pBall->x = pPlayer->x;  // putting it above him
        pBall->vx = 0;                 // so it falls straight on him
        pBall->vy = 10;        // maintain same total velocity
        gfx_text(pPlayer->x, (pPlayer->y) - 100, "-1");
        if ((pPlayer->lives) > 0)
            drawBall(*pBall); // moved this if statement so ball is drawn where its about to fall
        gfx_flush();
        usleep(1000000);
        pPlayer->x = pBall->x; // to really make sure it hits him
        pPlayer->vx = 0; // to really make sure it hits him

    }
    /*
     (pPlayer->lives)--;
     gfx_color(255,0,0);
     
     pBall->y = 20;              // putting it above him
     pBall->x = pPlayer->x;  // putting it above him
     pBall->vx = 0;                 // so it falls straight on him
     gfx_text(pPlayer->x,(pPlayer->y)-100,"-1");
     drawBall(*pBall);
     gfx_flush();
     usleep(1000000);     // waiting a second
     */


    // checks for collisions with net
    if (((pBall->y) > (ht - 150)) && (pBall->x) + (pBall->r) > (wd / 2) && (pBall->x) - (pBall->r) < (wd / 2)) {
        (pBall->vx) = -(pBall->vx);// because the net is vertical wall
    }
    // distance between the centers of the ball and the player
    playerBallDist = pow((pPlayer->y - (pBall->y)), 2) + pow((pPlayer->x - (pBall->x)), 2);
    // minimum distance player and ball centers can be w/out touching
    touchingDist = pow(pPlayer->r + (pBall->r), 2);
    // checks to see of there is a collision with a player
    if ((playerBallDist) < (touchingDist)) {
        calculateBallsDirection(pBall, pPlayer);
    }
}

// draw the ball filled in
void drawBall(Ball_type ball) {
    int j;
    float leftEP, rightEP, height;
    int numLines = 160;
    float part = 2 * M_PI / numLines;
    for (j = 0; j < numLines; j++) {
        leftEP = ball.x + cos(M_PI + part * j) * ball.r;
        rightEP = ball.x + cos(2 * M_PI - part * j) * ball.r;
        height = ball.y + sin(M_PI + part * j) * ball.r;
        gfx_line(leftEP, height, rightEP, height);
    }
}

// this checks to see if the player is off the ground, if it is off the ground this function takes one step iterating through the jumping motion. this function is seperate from all the other player movements because it is the only player action not controlled by a key input
void bringPlayerToGround(Person_type *pPlayer) {
    //check if player is on the ground (done jumping)
    if (pPlayer->y > ht) {
        pPlayer->vy = 0;
        pPlayer->y = ht;
        pPlayer->onGround = true;
    }
    // check to see if player is jumping
    if (!(pPlayer->onGround)) {
        pPlayer->y += pPlayer->vy;
        (pPlayer->vy) += .05;
    }
}

// after a key/ keys are pressed by the player/ players this function makes sure the key press will not send the player outside of the boundary
void validatePlayerMovement(Person_type *pPlayer) {

    //check right bound for the player
    if ((pPlayer->x) > ((pPlayer->boundaries[1]) - (pPlayer->r))) {
        pPlayer->x = (pPlayer->boundaries[1] - pPlayer->r);
    }
    // check left bound for the player
    if ((pPlayer->x) < (pPlayer->boundaries[0] + pPlayer->r)) {
        pPlayer->x = (pPlayer->boundaries[0] + pPlayer->r);
    }

}//end func

// prints the player's current number of lives at the top of the window
void updatePlayerScoreBoard(Person_type player) {
    char str[40];
    sprintf(str, "%s: %d", player.name, player.lives);
    gfx_color(player.color[0], player.color[1], player.color[2]);
    gfx_text(player.boundaries[0] + 40, 50, str);
}


// if one of the players has 0 lives this function displays the winner, waits for a keypress to return to the menu, and returns true
bool gameOver(Person_type *pPlayer1, Person_type *pPlayer2) {
    char str[100], c = 0;

    if (pPlayer2->lives <= 0 || pPlayer1->lives <= 0) {

        while (c != 'c') {
            if (pPlayer1->lives <= 0) {
                sprintf(str, "GAMEOVER The winner is:%s ", pPlayer2->name);
                (pPlayer2->wins) += 1;
                gfx_text((wd / 2) - 100, ht / 2 - 200, str);
            } else if (pPlayer2->lives <= 0) {
                sprintf(str, "GAMEOVER The winner is: %s", pPlayer1->name);
                (pPlayer1->wins) += 1;
                gfx_text((wd / 2) - 100, ht / 2 - 200, str);
            }
            gfx_text((wd / 2) - 100, ht / 2 - 150, "press 'c' to see the Menu Options");
            if (gfx_event_waiting()) {
                c = gfx_wait();
            }
        }// end while
        return true;
    }
    return false; //game isnt over, both players >1 lives
}//end func


// returns the number inputed by the user on the menu the return value is used by actOnOptions to decide what to do next
int optionsMenu() {
    int c;
    char str[100];
    bool letterPress = false;
    gfx_clear();
    gfx_text((wd / 2) - 100, ht / 2 - 150, "Menu Options:");
    gfx_text((wd / 2) - 120, ht / 2 - 100, "play slime volley(p) ");
    gfx_text((wd / 2) - 120, ht / 2 - 50, "see player stats(s) ");
    gfx_text((wd / 2) - 120, ht / 2, "change players(n)");
    gfx_text((wd / 2) - 120, ht / 2 + 50, "quit the game(q) ");

    while (1) {
        if (gfx_event_waiting()) {
            c = gfx_wait();
            switch (c) {
                case 'n':
                    return 4;
                    break;
                case 'p':
                    return 1;
                    break;

                case 's':
                    return 2;
                    break;

                case 'q':
                    return 3;
                    break;

            }//end switch
        }//end if
        gfx_flush();
    }//end while
}

// this takes in an array of six bools that are set to true if they have been pressed, it updates the players postions accordingly. Their intended moves are checked in validatePlayerMove (keeps the player on its side of the board)
void movePlayer(Person_type *pPlayer1, Person_type *pPlayer2, int c, bool keys[6]) {
    //checks keys[] bool array
    if (keys[1] == 1) {  // player1 up key pressed
        if (pPlayer1->onGround) {
            pPlayer1->onGround = false;
            pPlayer1->vy = -3;
        }
    }
    if (keys[0] == 1) {  // player1 left key pressed
        pPlayer1->x -= 20;
    }
    if (keys[2] == 1) {  // player1 right key pressed
        pPlayer1->x += 20;
    }
    if (keys[4] == 1) {  // player2 up key pressed
        if (pPlayer2->onGround) {
            pPlayer2->onGround = false;
            pPlayer2->vy = -3;
        }
    }
    if (keys[3] == 1) {  // player2 left key pressed
        pPlayer2->x -= 20;
    }
    if (keys[5] == 1) {  // player2 right key pressed
        pPlayer2->x += 20;
    }
    gfx_flush();
}// end function

// at the end of each game if a player does not already exist in the linked list then a new node is created with their information
void makeNode(Person_type **head, Person_type *ppPlayer) {
    Person_type *p;
    p = (Person_type *) malloc(sizeof(Person_type));
    p->lives = ppPlayer->lives;
    if (ppPlayer->lives > 0)
        p->wins = 1;
    else
        p->wins = 0;
    strcpy(p->name, ppPlayer->name);
    p->next = *head;
    *head = p;
}

// when the user enters 's' from the menu options this function is called to display the statistics of each player in the linked list
void displayData(Person_type *pPlayer1, Person_type *pPlayer2, Ball_type *pBall, Person_type *head, bool *quit) {
    int d = 0, c;
    char str[40];
    gfx_clear();
    gfx_text(wd / 2 - 70, 1 * ht / 3 + 30, "Player Stats:");
    gfx_text(wd / 2 - 100, 1 * ht / 3 + 80, "Player name         Wins");
    while (head) {
        sprintf(str, "%s %d", head->name, head->wins);
        gfx_text(wd / 2 - 100, 1 * ht / 3 + 110 + d, str);
        head = head->next;
        d += 15;
    }
    int act; // player keyboard action
    do {
        gfx_text(wd / 2 - 125, 1 * ht / 3 + d + 140, "Press 'y' to return to the Menu Options");
        if (gfx_event_waiting()) {
            c = gfx_wait();
            if (c == 'y') {
                actOnOptions(pPlayer1, pPlayer2, pBall, head, quit);
                gfx_clear();
            }
        }
    } while (c != 'y');
    gfx_clear();
}

void calculateBallsDirection(Ball_type *Ball, Person_type *Player) { //changes balls velocity using physics
    float fullBallVelocity;
    fullBallVelocity = 10; //the balls full velocity is set to ten after it hits anything as opposed to its actual current velocity because the game dwindles when the ball moves slower and slower
    // quadrant headed towards if ball was at origin
    /*
     int q = -1;
     if(Ball -> vx > 0 && Ball -> vy > 0)
     q=1;
     if(Ball -> vx < 0 && Ball -> vy > 0)
     q=2;
     if(Ball -> vx < 0 && Ball -> vy < 0)
     q=3;
     if(Ball -> vx > 0 && Ball -> vy < 0)
     q=4;
     */
    /*
     float vslope;
     if(  Ball -> vx  ==0 )
     vslope=0;
     else
     vslope = Ball -> vy / Ball -> vx ;
     */
    float slopeBetweenCenters;
    float centersDiffX, centersDiffY;
    centersDiffX = Ball->x - Player->x;
    centersDiffY = Ball->y - Player->y;
    if (abs(centersDiffX) < 0.00001)
        centersDiffX = 0.00001;  //dont divide by zero
    slopeBetweenCenters = centersDiffY / centersDiffX;
    int quadrantHit;
    quadrantHit = -1; // quadrant of player hit
    // 1 = top right, 2 = top left, 3 = bottom left, 4 = bottom right
    if (centersDiffX > 0 && centersDiffY > 0)
        quadrantHit = 1;
    if (centersDiffX < 0 && centersDiffY > 0)
        quadrantHit = 2;
    if (centersDiffX < 0 && centersDiffY < 0)
        quadrantHit = 3;
    if (centersDiffX > 0 && centersDiffY < 0)
        quadrantHit = 4;
    if (centersDiffX == 0 && centersDiffY < 0) { // BOTTOM
        Ball->vx = 0;
        Ball->vy = fullBallVelocity;
        return;
        // quadrantHit=5;
    }
    if (centersDiffX == 0 && centersDiffY > 0) { // TOP
        Ball->vx = 0;
        Ball->vy = fullBallVelocity;
        return;
        // quadrantHit=6;
    }
    if (centersDiffX < 0 && centersDiffY == 0) { // LEFT
        Ball->vx = fullBallVelocity;
        Ball->vy = 0;
        return;
        // quadrantHit=7;
    }
    if (centersDiffX > 0 && centersDiffY == 0) { // RIGHT
        Ball->vx = fullBallVelocity;
        Ball->vy = 0;
        return;
        // quadrantHit=8;
    }
    float angleBetweenCenters;
    angleBetweenCenters = atan(slopeBetweenCenters);
    if (quadrantHit == 2 || quadrantHit == 3)
        angleBetweenCenters += 3.1415926; // fixing restricted range of atan
    Ball->vx = fullBallVelocity * cos(angleBetweenCenters);
    Ball->vy = fullBallVelocity * sin(angleBetweenCenters);
}//end func

// this draws the key display so the user knows which keys to use
void drawPlayerKeysAtStartScreen() {
    gfx_text(wd / 2 - 300, ht / 2 + 100, "Player 1 keys:");
    gfx_text(wd / 2 + 200, ht / 2 + 100, "Player 2 keys:");

    gfx_rectangle(wd / 2 - 300 + 50, ht / 2 + 150, 40,
                  40); // strange error where this one doesnt draw first function call
    // so i call function twice 
    gfx_rectangle(wd / 2 - 300, ht / 2 + 200, 40, 40);
    gfx_rectangle(wd / 2 - 300 + 100, ht / 2 + 200, 40, 40);
    gfx_rectangle(wd / 2 + 200 + 50, ht / 2 + 150, 40, 40);
    gfx_rectangle(wd / 2 + 200, ht / 2 + 200, 40, 40);
    gfx_rectangle(wd / 2 + 200 + 100, ht / 2 + 200, 40, 40);

    gfx_text(wd / 2 - 300 + 50 + 20, ht / 2 + 150 + 20, "W");
    gfx_text(wd / 2 - 300 + 20, ht / 2 + 200 + 20, "A");
    gfx_text(wd / 2 - 300 + 100 + 20, ht / 2 + 200 + 20, "D");
    //gfx_text(wd/2+200+50 +20, ht/2+150 +20, "UP");
    //gfx_text(wd/2+200 +20, ht/2+200 +20, "LEFT");   
    //gfx_text(wd/2+200+100 +20, ht/2+200 +20, "RIGHT");

    gfx_line(wd / 2 + 200 + 50 + 20, ht / 2 + 150 + 5, wd / 2 + 200 + 50 + 20, ht / 2 + 150 + 35);
    gfx_triangle(wd / 2 + 200 + 50 + 20, ht / 2 + 150 + 5, wd / 2 + 200 + 50 + 20 - 5, ht / 2 + 150 + 5 + 5,
                 wd / 2 + 200 + 50 + 20 + 5, ht / 2 + 150 + 5 + 5);
    gfx_line(wd / 2 + 200 + 20 + 15, ht / 2 + 200 + 20, wd / 2 + 200 + 20 - 15, ht / 2 + 200 + 20);
    gfx_triangle(wd / 2 + 200 + 20 - 15, ht / 2 + 200 + 20, wd / 2 + 200 + 20 - 15 + 5, ht / 2 + 200 + 20 + 5,
                 wd / 2 + 200 + 20 - 15 + 5, ht / 2 + 200 + 20 - 5);
    gfx_line(wd / 2 + 200 + 100 + 20 + 15, ht / 2 + 200 + 20, wd / 2 + 200 + 100 + 20 - 15, ht / 2 + 200 + 20);
    gfx_triangle(wd / 2 + 200 + 100 + 20 + 15, ht / 2 + 200 + 20, wd / 2 + 200 + 100 + 20 + 15 - 5,
                 ht / 2 + 200 + 20 + 5, wd / 2 + 200 + 100 + 20 + 15 - 5, ht / 2 + 200 + 20 - 5);
}
