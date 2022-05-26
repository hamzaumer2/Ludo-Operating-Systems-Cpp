//200789 Hamza Umer Farooq
//201947 Muhammad Sarib Hanif
//BSCS - 4 - B
//OS SEMESTER PROJECT

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <algorithm>   // FOR RANDOM_SHUFFLE ( USED ON LINE 121)//ISSUE(STARVATION)----------------------------------------------------------------------------------


using namespace std;

//SHARED VARIABLES
sem_t sem, sem1, sem2;


//
pthread_t tid1, tid2;



int rolling_dice() {
    srand(time(NULL));
    return rand() % 6 + 1;
}


int play_pieces = 4;

struct play_piece {
    
    int value, x, y;
    bool open, home, stop, win;
    char sym;

    play_piece() {
        value = -1;
        x = 0, y = 0;
        stop = 0;
        win = 0;
        open = 0;
        sym = '!';
    }

    play_piece(int v, int x1, int y1, bool o, bool s, bool w, char sy) {
        value = v;
        x = x1;
	    y = y1;
        stop = s;
        win = w;
        open = o;
        sym = sy;
    }

};

//each player has 1-4 play_pieces


struct player {

    play_piece *tok;
    int hitRate, withoutsixturns;
    bool is_win, inGame;

    player() {

        hitRate = 1;
        is_win = 0;
        inGame = 1;
        withoutsixturns = 0;
    }

    player(char symbol, int noOfplay_pieces) {

        tok = new play_piece[noOfplay_pieces];
        for (int i = 0; i < noOfplay_pieces; i++) {
            tok[i].sym = symbol;
        }

        hitRate = 1;
        is_win = 0;
        inGame = 1;
        withoutsixturns = 0;
    }
}; player p1, p2, p3, p4;


void *MasterThread(void *attr);

void* playerthread(void* attr);

void *killPlayer(void *attr);


//////////////////////////////////////////////////////
void movement_path(int distance, play_piece* t);



//-------------------------------------
//INTERFACED WORK

void draw_frame();

void gridder(int, int);
//-------------------------------------


int main(void) {
    cout << "\n\nOS LAB PROJECT";
    cout << "\n200789 - HAMZA UMER FAROOQ";
    cout << "\n201947 - SARIB HANIF";
    cout << "\nBSCS - 4 - B";



    do {
        cout << "\nEnter the number of play_pieces for each player between 1 to 4 :";
        cin >> play_pieces;
    } while (play_pieces < 1 || play_pieces > 4);

    //creating four players
    player temp('&', play_pieces), temp1('%', play_pieces), temp2('#', play_pieces), temp3('@', play_pieces);

    p1 = temp;
    p2 = temp1;
    p3 = temp2;
    p4 = temp3;

    //creating an array for random turns
    int arr[4] = {1, 2, 3, 4};
    random_shuffle(arr, arr + 4);
    while (true) {
        sem_init(&sem, 0, 1);//INITIALISING SEMAPHORE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //ISSUE: STARVATION!!!------------------------------------------------------------------------------------------------------------------------------
        //random_shuffle(arr, arr + 4);//random values for different player selection
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        pthread_t tid[4];

        //THREAD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        for (int i = 0; i < 4; i++) {
            pthread_create(&tid[i], NULL, &playerthread, &arr[i]);
            pthread_join(tid[i], NULL);
        }
    }
    return 0;
}



void *MasterThread(void *attr) {

    int temp = (*(int *) attr);
    player *tempPlayer;
    //checking player
    switch (temp) {
    case 1: {
        tempPlayer = &p1;
        break;
    }
    case 2: {
        tempPlayer = &p2;
        break;
    }
    case 3: {
        tempPlayer = &p3;
        break;
    }
    case 4: {
        tempPlayer = &p4;
        break;
    }
    default: {
        cout << "\nInvalid Player!!!";
        break;
    }

    }

    sem_wait(&sem2);//LOCKING SEMAPHORE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < play_pieces; i++) {
        //checing home path condition and hit rate
        if (tempPlayer->hitRate > 0 && tempPlayer->tok[i].value >= 50) {
            tempPlayer->tok[i].home = 1;
        } else {
            tempPlayer->tok[i].home = 0;

        }

    }
    if (tempPlayer->withoutsixturns >= 10) {//IF PLAYER DOESNT GET SIX IN 10 TURNS THEN HE IS OUT
        tempPlayer->inGame = 0;
    }
    bool notwinflag = 0;
    for (int i = 0; i < play_pieces; i++) {
        if (!tempPlayer->tok[i].win) {
            notwinflag = 1;
        }
    }

    if (notwinflag == 0) {
        tempPlayer->is_win = 1;
    }


    sem_post(&sem2);//UNLOCING SEMAPHORE///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    pthread_exit(NULL);

}


///
// function for the motion of the play_pieces
///
void movement_path(int distance, play_piece *t) {
    int attr;

    //checking players play_pieces

    switch (t->sym) {
    case '&': {
        attr = 1;
        break;
    }
    case '%': {
        attr = 2;
    }
    case '#': {
        attr = 3;
    }
    case '@': {
        attr = 4;
    }
    }

    

    for (int i = 0; i < distance; i++) {
        sem_init(&sem2, 0, 1);
        //ceating master thread
        pthread_create(&tid2, NULL, &MasterThread, &attr);
        pthread_join(tid2, NULL);
        t->value++;
        //checking if not home and then assingning respective positions to play_pieces
        if (!t->home) {

            if (t->y > 9 && t->x == 6) {
                t->y--;
            } else if (t->y == 9 && t->x == 6) {
                t->y = 8;
                t->x = 5;
            } else if (t->y == 8 && t->x > 0 && t->x <= 5) {
                t->x--;
            } else if (t->x == 0 && t->y > 6) {
                t->y--;
            } else if (t->y == 6 && t->x < 5) {
                t->x++;
            } else if (t->x == 5 && t->y == 6) {
                t->y = 5;
                t->x = 6;
            } else if (t->y > 0 && t->x == 6) {
                t->y--;
            } else if (t->y == 0 && t->x < 8) {
                t->x++;
            } else if (t->y < 5 && t->x == 8) {
                t->y++;
            } else if (t->x == 8 && t->y == 5) {
                t->y = 6;
                t->x = 9;
            } else if (t->x < 14 && t->y == 6) {
                t->x++;
            } else if (t->x == 14 && t->y < 8) {
                t->y++;
            } else if (t->x > 9 && t->y == 8) {
                t->x--;
            } else if (t->x == 9 && t->y == 8) {
                t->y = 9;
                t->x = 8;
            } else if (t->x == 8 && t->y < 14) {
                t->y++;
            } else if (t->x > 6 && t->y == 14) {
                t->x--;
            }
            if (t->value > 51) {
                t->value = (t->value % 52);
            }
        } 
        
        else {
            switch (t->sym) {
            case '&': {
                t->x++;
                break;
            }
            case '#': {
                t->y++;
                break;
            }
            case '%': {
                t->y--;
                break;
            }
            case '@': {
                t->x--;
                break;
            }
            default: {
                cout << "\nPlayer Doesnt Exist";
                break;
            }
            }
        }
    }
}

///
// draw the whole game scenarion or grid as well as play_pieces
///


int x[4][3];

void *killPlayer(void *attr) {
    int temp = (*(int *) attr);
    player *tempPlayer;
    switch (temp) {
    case 1: {
        tempPlayer = &p1;
        break;
    }
    case 2: {
        tempPlayer = &p2;
        break;
    }
    case 3: {
        tempPlayer = &p3;
        break;
    }
    case 4: {
        tempPlayer = &p4;
        break;
    }
    }

    if (tempPlayer->inGame && !tempPlayer->is_win) {
        sem_wait(&sem1);//LOCKING SEMAPHORE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        for (int i = 0; i < play_pieces; i++) {
            if (tempPlayer->tok[i].sym == '&') {
                for (int j = 0; j < play_pieces; j++) {
                    if (tempPlayer->tok[i].x == p3.tok[j].x && 
                        tempPlayer->tok[i].y == p3.tok[j].y && 
                        p3.tok[j].open == 1 && p3.tok[j].stop != 1) {
                        
                        cout << "Player 1 killed Player 3" << endl;
                        tempPlayer->hitRate++;
                        p3.tok[j].value = -1;
                        p3.tok[j].open = 0;
                        p3.tok[j].home = 0;
                        p3.tok[j].x = 8;
                        p3.tok[j].y = 1;
                    }
                    if (tempPlayer->tok[i].x == p4.tok[j].x && 
                        tempPlayer->tok[i].y == p4.tok[j].y &&
                        p4.tok[j].open == 1 && p4.tok[j].stop != 1) {
                        
                        cout << "Player 1 killed Player 4" << endl;
                        tempPlayer->hitRate++;
                        p4.tok[j].value = -1;
                        p4.tok[j].home = 0;
                        p4.tok[j].open = 0;
                        p4.tok[j].x = 13;
                        p4.tok[j].y = 8;
                    }
                    if (tempPlayer->tok[i].x == p2.tok[j].x && 
                        tempPlayer->tok[i].y == p2.tok[j].y &&
                        p2.tok[j].open == 1 && p2.tok[j].stop != 1) {
                        
                        cout << "Player 1 killed Player 2" << endl;
                        tempPlayer->hitRate++;
                        p2.tok[j].value = -1;
                        p2.tok[j].home = 0;
                        p2.tok[j].open = 0;
                        p2.tok[j].x = 6;
                        p2.tok[j].y = 13;
                    }
                }
            } else if (tempPlayer->tok[i].sym == '%') {
                for (int j = 0; j < play_pieces; j++) {
                    if (tempPlayer->tok[i].x == p1.tok[j].x && 
                        tempPlayer->tok[i].y == p1.tok[j].y &&
                        p1.tok[j].open == 1 && p1.tok[j].stop != 1) {
                        
                        cout << "Player 2 killed Player 1" << endl;
                        tempPlayer->hitRate++;
                        p1.tok[j].value = -1;
                        p1.tok[j].home = 0;
                        p1.tok[j].open = 0;
                        p1.tok[j].x = 1;
                        p1.tok[j].y = 6;
                    }
                    if (tempPlayer->tok[i].x == p3.tok[j].x && 
                        tempPlayer->tok[i].y == p3.tok[j].y &&
                        p3.tok[j].open == 1 && p3.tok[j].stop != 1) {
                        
                        cout << "Player 2 killed Player 3" << endl;
                        tempPlayer->hitRate++;
                        p3.tok[j].value = -1;
                        p3.tok[j].home = 0;
                        p3.tok[j].open = 0;
                        p3.tok[j].x = 8;
                        p3.tok[j].y = 1;
                    }
                    if (tempPlayer->tok[i].x == p4.tok[j].x && 
                        tempPlayer->tok[i].y == p4.tok[j].y &&
                        p4.tok[j].open == 1 && p4.tok[j].stop != 1) {
                        
                        cout << "Player 2 killed Player 4" << endl;
                        tempPlayer->hitRate++;
                        p4.tok[j].value = -1;
                        p4.tok[j].home = 0;
                        p4.tok[j].open = 0;
                        p4.tok[j].x = 13;
                        p4.tok[j].y = 8;
                    }
                }
            } else if (tempPlayer->tok[i].sym == '#') {
                for (int j = 0; j < play_pieces; j++) {
                    if (tempPlayer->tok[i].x == p4.tok[j].x && 
                        tempPlayer->tok[i].y == p4.tok[j].y &&
                        p4.tok[j].open == 1 && p4.tok[j].stop != 1) {
                        
                        cout << "Player 3 killed Player 4" << endl;
                        tempPlayer->hitRate++;
                        p4.tok[j].value = -1;
                        p4.tok[j].home = 0;
                        p4.tok[j].open = 0;
                        p4.tok[j].x = 13;
                        p4.tok[j].y = 8;
                    }
                    if (tempPlayer->tok[i].x == p2.tok[j].x && 
                        tempPlayer->tok[i].y == p2.tok[j].y &&
                        p2.tok[j].open == 1 && p2.tok[j].stop != 1) {
                        
                        cout << "Player 3 killed Player 2" << endl;
                        tempPlayer->hitRate++;
                        p2.tok[j].value = -1;
                        p2.tok[j].home = 0;
                        p2.tok[j].open = 0;
                        p2.tok[j].x = 6;
                        p2.tok[j].y = 13;
                    }
                    if (tempPlayer->tok[i].x == p1.tok[j].x && 
                        tempPlayer->tok[i].y == p1.tok[j].y &&
                        p1.tok[j].open == 1 && p1.tok[j].stop != 1) {
                        
                        cout << "Player 3 killed Player 1" << endl;
                        tempPlayer->hitRate++;
                        p1.tok[j].value = -1;
                        p1.tok[j].home = 0;
                        p1.tok[j].open = 0;
                        p1.tok[j].x = 1;
                        p1.tok[j].y = 6;
                    }
                }
            } else if (tempPlayer->tok[i].sym == '@') {
                for (int j = 0; j < play_pieces; j++) {
                    if (tempPlayer->tok[i].x == p2.tok[j].x && 
                        tempPlayer->tok[i].y == p2.tok[j].y &&
                        p2.tok[j].open == 1 && p2.tok[j].stop != 1) {
                        
                        cout << "Player 4 killed Player 2" << endl;
                        tempPlayer->hitRate++;
                        p2.tok[j].value = -1;
                        p2.tok[j].home = 0;
                        p2.tok[j].open = 0;
                        p2.tok[j].x = 6;
                        p2.tok[j].y = 13;
                    }
                    if (tempPlayer->tok[i].x == p1.tok[j].x && 
                        tempPlayer->tok[i].y == p1.tok[j].y &&
                        p1.tok[j].open == 1 && p1.tok[j].stop != 1) {
                        
                        cout << "Player 4 killed Player 1" << endl;
                        tempPlayer->hitRate++;
                        p1.tok[j].value = -1;
                        p1.tok[j].home = 0;
                        p1.tok[j].open = 0;
                        p1.tok[j].x = 1;
                        p1.tok[j].y = 6;
                    }
                    if (tempPlayer->tok[i].x == p3.tok[j].x && 
                        tempPlayer->tok[i].y == p3.tok[j].y &&
                        p3.tok[j].open == 1 && p3.tok[j].stop != 1) {
                        
                        cout << "Player 4 killed Player 3" << endl;
                        tempPlayer->hitRate++;
                        p3.tok[j].value = -1;
                        p3.tok[j].open = 0;
                        p3.tok[j].home = 0;
                        p3.tok[j].x = 8;
                        p3.tok[j].y = 1;
                    }
                }
            }
        }
        sem_post(&sem1);//UNLOCKING SEMAPHORE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    pthread_exit(NULL);
}

//
//INDIVIDUAL PLAYER THREADS
//TRACKS ALL THE DARA OF PLAYER INCLUDING IF THEY GOT 3 SIXES CONSECUTIVE, AND THE VALUES TO WHICH TO MOVE THE PLAYER
//
//

void *playerthread(void *attr) {

    int temp = (*(int *) attr);

    //checking players
    player *tempPlayer;
    switch (temp) {
    case 1: {
        tempPlayer = &p1;
        break;
    }
    case 2: {
        tempPlayer = &p2;
        break;
    }
    case 3: {
        tempPlayer = &p3;
        break;
    }
    case 4: {
        tempPlayer = &p4;
        break;
    }
    default: {
        cout << "\nINVALID ENTRY";
        break;
    }

   }
    
    sem_wait(&sem);
    //initializing dice value for all the players to 0
    for (int i = 0; i < 3; i++) {
        x[temp - 1][i] = 0;
    }
    bool threesix = true;
    int tempdice = 0;
    for (int i = 0; i < 3; i++) {
        x[temp - 1][i] = rolling_dice();// assiging dice value
        cout << "\nDice Value of Player: " << temp << " is: " << x[temp - 1][i] << endl;//priinting dice value
        //wait(NULL);
        sleep(1);
        
        //checking without six turns condition and incrementing value of variable withoutsixturns
        if (x[temp - 1][i] != 6) {
            tempPlayer->withoutsixturns++;
        } else {
            tempPlayer->withoutsixturns = 0;
        }

        if (x[temp - 1][i] != 6) {
            threesix = false;
            break;
        }

    }
    cout << "\n----------------------------------------------------------------------";

    if (threesix == true) {//checking 3 consecutive sixes condition
        for (int i = 0; i < 3; i++) {
            x[temp - 1][i] = 0;//assigning zero on 3 consecutive sixes
        }
    }

    for (int j = 0; j < 3 && x[temp - 1][j] > 0; j++) {
        int n = 1;
        bool flag = false;
        for (int i = 0; i < play_pieces; i++) {
            if (tempPlayer->tok[i].open == 1) {
                flag = true;
            }
        }
        if (flag || x[temp - 1][j] == 6) {
            cout << "\nEnter which play_piece you want to move for player: " << temp << " :";
            cin >> n;

            while (n > play_pieces || n < 1) {
                cout << "Enter value from 1 to " << play_pieces << ": ";
                cin >> n;
            }
            while (((!tempPlayer->tok[n - 1].open) && x[temp - 1][j] != 6) || (tempPlayer->tok[n - 1].win)) {
                cout << "Please Enter value of opened play_pieces: ";
                cin >> n;
            }
            bool winflag = 0;
            bool notmoveflag = 0;
            while (tempPlayer->tok[n - 1].value + x[temp - 1][j] > 56) {
                cout << "Illegal Move!!!..." << endl;

                for (int k = 0; k < play_pieces; k++) {
                    if (k != n - 1) {
                        if (!tempPlayer->tok[k].win && tempPlayer->tok[k].open) {
                            winflag = 1;
                        }
                        if (x[temp - 1][j] == 6 && !tempPlayer->tok[k].open) {
                            winflag = 1;
                        }
                    }
                }
                if (winflag) {
                    cout << "Enter Another play_piece: " << endl;
                    cin >> n;
                } else {
                    notmoveflag = 1;
                    break;
                }
            }


            if (tempPlayer->tok[n - 1].open && !notmoveflag) {
                movement_path(x[temp - 1][j], &tempPlayer->tok[n - 1]);
            } else if (x[temp - 1][j] == 6) {

                tempPlayer->tok[n - 1].value = 0;
                tempPlayer->tok[n - 1].open = 1;
                
                switch (tempPlayer->tok[n - 1].sym) {
                case '&': {
                    tempPlayer->tok[n - 1].x = 1;
                    tempPlayer->tok[n - 1].y = 6;
                    break;
                }
                case '%': {
                    tempPlayer->tok[n - 1].x = 6;
                    tempPlayer->tok[n - 1].y = 13;
                    break;
                }
                case '#': {
                    tempPlayer->tok[n - 1].x = 8;
                    tempPlayer->tok[n - 1].y = 1;
                    break;
                }
                case '@': {
                    tempPlayer->tok[n - 1].x = 13;
                    tempPlayer->tok[n - 1].y = 8;
                    break;
                }
                default: {
                    cout << "\nIllegal";
                    break;
                }
                }

                

            }
            //safe points conditions
            if (tempPlayer->tok[n - 1].value == 0 || tempPlayer->tok[n - 1].value == 8 ||
                tempPlayer->tok[n - 1].value == 13 || tempPlayer->tok[n - 1].value == 21 ||
                tempPlayer->tok[n - 1].value == 26 || tempPlayer->tok[n - 1].value == 34 ||
                tempPlayer->tok[n - 1].value == 39 || tempPlayer->tok[n - 1].value == 47) {
                tempPlayer->tok[n - 1].stop = 1;
            } else {
                tempPlayer->tok[n - 1].stop = 0;
            }
        }

        for (int i = 0; i < play_pieces; i++)
            if ((tempPlayer->tok[i].value) == -1) {
                cout << "\nplay_piece: " << i + 1 << " of player: " << temp << " is in HOME" << endl;
            }
            else {
                cout << "\nplay_piece: " << i + 1 << " of player: " << temp << " is at position: " << (tempPlayer->tok[i].value) << endl;
            }
        cout << endl;
        cout << endl;
        sem_init(&sem1, 0, 1);
        pthread_create(&tid1, NULL, &killPlayer, &temp);
        pthread_join(tid1, NULL);

        
        draw_frame();
        //system("CLS");

    }
    sem_post(&sem);
    pthread_exit(NULL);
}



//
//DRAWING GRID FOR LUDO
void draw_frame() {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {


            switch (play_pieces) {


            case 1:// grid for 1 play_piece+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++==
            {
                if (i == 4 && j == 1 && !p1.tok[0].open)
                    cout << "  &  ";

                else if (i == 10 && j == 1 && !p2.tok[0].open)
                    cout << "  %  ";
                
                else if (i == 1 && j == 10 && !p3.tok[0].open)
                    cout << "  #  ";
                
                else if (i == 10 && j == 10 && !p4.tok[0].open)
                    cout << "  @  ";



                else if (i == p1.tok[0].y && j == p1.tok[0].x && p1.tok[0].open)
                    cout << "  &  ";

                else if (i == p2.tok[0].y && j == p2.tok[0].x && p2.tok[0].open)
                    cout << "  %  ";

                else if (i == p3.tok[0].y && j == p3.tok[0].x && p3.tok[0].open)
                    cout << "  #  ";

                else if (i == p4.tok[0].y && j == p4.tok[0].x && p4.tok[0].open)
                    cout << "  @  ";


                else {
                    gridder(i, j);
                }

                break;

            }



            case 2://grid for 2 play_pieces++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            {
                if (i == 4 && j == 1 && !p1.tok[0].open)
                    cout << "  &  ";
                else if (i == 4 && j == 4 && !p1.tok[1].open)
                    cout << "  &  ";
                
                else if (i == 10 && j == 1 && !p2.tok[0].open)
                    cout << "  %  ";
                else if (i == 10 && j == 4 && !p2.tok[1].open)
                    cout << "  %  ";
                
                else if (i == 1 && j == 10 && !p3.tok[0].open)
                    cout << "  #  ";
                else if (i == 1 && j == 13 && !p3.tok[1].open)
                    cout << "  #  ";

                else if (i == 10 && j == 10 && !p4.tok[0].open)
                    cout << "  @  ";
                else if (i == 10 && j == 13 && !p4.tok[1].open)
                    cout << "  @  ";


                else if (i == p1.tok[0].y && j == p1.tok[0].x && p1.tok[0].open)
                    cout << "  &  ";
                else if (i == p1.tok[1].y && j == p1.tok[1].x && p1.tok[1].open)
                    cout << "  &  ";

                else if (i == p2.tok[0].y && j == p2.tok[0].x && p2.tok[0].open)
                    cout << "  %  ";
                else if (i == p2.tok[1].y && j == p2.tok[1].x && p2.tok[1].open)
                    cout << "  %  ";

                else if (i == p3.tok[0].y && j == p3.tok[0].x && p3.tok[0].open)
                    cout << "  #  ";
                else if (i == p3.tok[1].y && j == p3.tok[1].x && p3.tok[1].open)
                    cout << "  #  ";

                else if (i == p4.tok[0].y && j == p4.tok[0].x && p4.tok[0].open)
                    cout << "  @  ";
                else if (i == p4.tok[1].y && j == p4.tok[1].x && p4.tok[1].open)
                    cout << "  @  ";
                
                else {
                    gridder(i, j);
                }
                break;
            }




            case 3://grid for 3 play_pieces+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            {
                if (i == 4 && j == 1 && !p1.tok[0].open)
                    cout << "  &  ";
                else if (i == 4 && j == 4 && !p1.tok[1].open)
                    cout << "  &  ";
                else if (i == 1 && j == 4 && !p1.tok[2].open)
                    cout << "  &  ";
                
                else if (i == 10 && j == 1 && !p2.tok[0].open)
                    cout << "  %  ";
                else if (i == 10 && j == 4 && !p2.tok[1].open)
                    cout << "  %  ";
                else if (i == 13 && j == 1 && !p2.tok[2].open)
                    cout << "  %  ";

                else if (i == 1 && j == 10 && !p3.tok[0].open)
                    cout << "  #  ";
                else if (i == 1 && j == 13 && !p3.tok[1].open)
                    cout << "  #  ";
                else if (i == 4 && j == 10 && !p3.tok[2].open)
                    cout << "  #  ";

                else if (i == 10 && j == 10 && !p4.tok[0].open)
                    cout << "  @  ";
                else if (i == 10 && j == 13 && !p4.tok[1].open)
                    cout << "  @  ";
                else if (i == 13 && j == 10 && !p4.tok[2].open)
                    cout << "  @  ";

                
                else if (i == p1.tok[0].y && j == p1.tok[0].x && p1.tok[0].open)
                    cout << "  &  ";
                else if (i == p1.tok[1].y && j == p1.tok[1].x && p1.tok[1].open)
                    cout << "  &  ";
                else if (i == p1.tok[2].y && j == p1.tok[2].x && p1.tok[2].open)
                    cout << "  &  ";

                else if (i == p2.tok[0].y && j == p2.tok[0].x && p2.tok[0].open)
                    cout << "  %  ";
                else if (i == p2.tok[1].y && j == p2.tok[1].x && p2.tok[1].open)
                    cout << "  %  ";
                else if (i == p2.tok[2].y && j == p2.tok[2].x && p2.tok[2].open)
                    cout << "  %  ";

                else if (i == p3.tok[0].y && j == p3.tok[0].x && p3.tok[0].open)
                    cout << "  #  ";
                else if (i == p3.tok[1].y && j == p3.tok[1].x && p3.tok[1].open)
                    cout << "  #  ";
                else if (i == p3.tok[2].y && j == p3.tok[2].x && p3.tok[2].open)
                    cout << "  #  ";

                else if (i == p4.tok[0].y && j == p4.tok[0].x && p4.tok[0].open)
                    cout << "  @  ";
                else if (i == p4.tok[1].y && j == p4.tok[1].x && p4.tok[1].open)
                    cout << "  @  ";
                else if (i == p4.tok[2].y && j == p4.tok[2].x && p4.tok[2].open)
                    cout << "  @  ";


                else {
                    gridder(i, j);
                }

                break;
            }


            //4 play_pieceS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            case 4: {
                if (i == 4 && j == 1 && !p1.tok[0].open)
                    cout << "  &  ";
                else if (i == 4 && j == 4 && !p1.tok[1].open)
                    cout << "  &  ";
                else if (i == 1 && j == 4 && !p1.tok[2].open)
                    cout << "  &  ";
                else if (i == 1 && j == 1 && !p1.tok[3].open)
                    cout << "  &  ";

                else if (i == 10 && j == 1 && !p2.tok[0].open)
                    cout << "  %  ";
                else if (i == 10 && j == 4 && !p2.tok[1].open)
                    cout << "  %  ";
                else if (i == 13 && j == 1 && !p2.tok[2].open)
                    cout << "  %  ";
                else if (i == 13 && j == 4 && !p2.tok[3].open)
                    cout << "  %  ";
                
                else if (i == 1 && j == 10 && !p3.tok[0].open)
                    cout << "  #  ";
                else if (i == 1 && j == 13 && !p3.tok[1].open)
                    cout << "  #  ";
                else if (i == 4 && j == 10 && !p3.tok[2].open)
                    cout << "  #  ";
                else if (i == 4 && j == 13 && !p3.tok[3].open)
                    cout << "  #  ";
                
                else if (i == 10 && j == 10 && !p4.tok[0].open)
                    cout << "  @  ";
                else if (i == 10 && j == 13 && !p4.tok[1].open)
                    cout << "  @  ";
                else if (i == 13 && j == 10 && !p4.tok[2].open)
                    cout << "  @  ";
                else if (i == 13 && j == 13 && !p4.tok[3].open)
                    cout << "  @  ";
                
                
                else if (i == p1.tok[0].y && j == p1.tok[0].x && p1.tok[0].open)
                    cout << "  &  ";
                else if (i == p1.tok[1].y && j == p1.tok[1].x && p1.tok[1].open)
                    cout << "  &  ";
                else if (i == p1.tok[2].y && j == p1.tok[2].x && p1.tok[2].open)
                    cout << "  &  ";
                else if (i == p1.tok[3].y && j == p1.tok[3].x && p1.tok[3].open)
                    cout << "  &  ";
                
                else if (i == p2.tok[0].y && j == p2.tok[0].x && p2.tok[0].open)
                    cout << "  %  ";
                else if (i == p2.tok[1].y && j == p2.tok[1].x && p2.tok[1].open)
                    cout << "  %  ";
                else if (i == p2.tok[2].y && j == p2.tok[2].x && p2.tok[2].open)
                    cout << "  %  ";
                else if (i == p2.tok[3].y && j == p2.tok[3].x && p2.tok[3].open)
                    cout << "  %  ";
                
                else if (i == p3.tok[0].y && j == p3.tok[0].x && p3.tok[0].open)
                    cout << "  #  ";
                else if (i == p3.tok[1].y && j == p3.tok[1].x && p3.tok[1].open)
                    cout << "  #  ";
                else if (i == p3.tok[2].y && j == p3.tok[2].x && p3.tok[2].open)
                    cout << "  #  ";
                else if (i == p3.tok[3].y && j == p3.tok[3].x && p3.tok[3].open)
                    cout << "  #  ";
                
                else if (i == p4.tok[0].y && j == p4.tok[0].x && p4.tok[0].open)
                    cout << "  @  ";
                else if (i == p4.tok[1].y && j == p4.tok[1].x && p4.tok[1].open)
                    cout << "  @  ";
                else if (i == p4.tok[2].y && j == p4.tok[2].x && p4.tok[2].open)
                    cout << "  @  ";
                else if (i == p4.tok[3].y && j == p4.tok[3].x && p4.tok[3].open)
                    cout << "  @  ";

                else {
                    gridder(i, j);
                }

                break;
            }
            }

        }
        cout << endl << endl;
    }

}


void gridder(int i, int j) {


                    if (j < 5 && i < 5)
                        cout << "     ";
                    else if (j < 6 && i == 5)
                        cout << " --- ";
                    else if (j == 5 && i < 6)
                        cout << "    |";
                    else if (j < 5 && i > 9)
                        cout << "     ";
                    else if (j < 6 && i == 9)
                        cout << " --- ";
                    else if (j == 5 && i > 8)
                        cout << "    |";
                    else if (j > 9 && i < 5)
                        cout << "     ";
                    else if (j > 8 && i == 5)
                        cout << " --- ";
                    else if (j == 9 && i < 6)
                        cout << "|     ";
                    else if (j > 9 && i > 9)
                        cout << "     ";
                    else if (j > 8 && i == 9)
                        cout << " --- ";
                    else if (j == 9 && i > 8)
                        cout << "|    ";

                    else if (i > 5 && j > 5 && j < 9 && i < 9)
                        cout << "  H  ";
                    else if (j == 1 && i == 6)
                        cout << "  A  ";
                    else if (j == 2 && i == 8)
                        cout << "  A  ";
                    else if (i == 7 && j > 0 && j < 6)
                        cout << "  A  ";
                    else if (j == 6 && i == 2)
                        cout << "  B  ";
                    else if (j == 8 && i == 1)
                        cout << "  B  ";
                    else if (j == 7 && i > 0 && i < 6)
                        cout << "  B  ";
                    else if (j == 6 && i == 13)
                        cout << "  C  ";
                    else if (j == 8 && i == 12)
                        cout << "  C  ";
                    else if (j == 7 && i > 8 && i < 14)
                        cout << "  C  ";
                    else if (j == 13 && i == 8)
                        cout << "  D  ";
                    else if (j == 12 && i == 6)
                        cout << "  D  ";
                    else if (i == 7 && j > 8 && j < 14)
                        cout << "  D  ";

                    else
                        cout << "  -  ";

}
