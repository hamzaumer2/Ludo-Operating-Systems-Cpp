The above project has been completed. Let me know if you find any issues

***********************************************************************************************************************************

# Operating-Systems
Ludo Game. Operating Systems Project
 
 ***********************************************************************************************************************************
 How To Run:
 
 cd ~/Desktop/OS-Project
 
g++ ludo.cpp -pthread
 
 ./a.out
 
 ************************************************************************************************************************************
 
 
 Ludo is a traditional game consisting of 2-4 Players competing agaisnt each other for who takes all their tokens to home first
 
 Passing Parameters to each thread:

The parent thread will create the worker threads, passing each worker the location that it must check in the grid. This step will require passing several parameters to each thread.

A struct is made to create a data structure for each thread, which includes row and column where a thread must begin validating



struct token {

    int value, x, y;
    bool open, home, stop, win;
    char sym;
    token() {//CONSTRUCTORS
        value = -1;
        x = 0;
        y = 0;
        stop = 0;
        win = 0;
        open = 0;
        sym = '!';
    }
    token(int v, int x1, int y1, bool o, bool s, bool w, char sy) {
        value = v;
        x = x1;
        y = y1;
        stop = s;
        win = w;
        open = o;
        sym = sy;
    }
};


A player thread is made, which has the data for hits, turns with sixes, and scoreboard level
 
![image](https://user-images.githubusercontent.com/71342062/169860257-e94fa5c5-cad1-4069-9c44-1507caed862b.png)

--------------------------------------------------------------------------------------------------------------------------------------------


Running the Code

![image](https://user-images.githubusercontent.com/71342062/169882406-414544ff-9e4e-49a8-aa1d-25ebc2af3c8e.png)

______________________________________________________________________________________________________________________________________________

Dice Movement:

![image](https://user-images.githubusercontent.com/71342062/169882610-3844aedf-9007-4324-ab6d-e29d757f6a19.png)


