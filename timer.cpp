#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>
#include <signal.h>

#define secondsInHour 3600
#define secondsInMinute 60
#define increment 1

using namespace std;

//structure for timer//
struct Timer {
    int seconds;
    int minutes;
    int hours;
};

//math for the timer//
struct Timer get_time(int seconds)
{
    struct Timer t;
    
    t.hours = seconds / secondsInHour;

    int seconds_remaining = seconds % secondsInHour;
    t.minutes = seconds_remaining / secondsInMinute;
    t.seconds = seconds_remaining % secondsInMinute;

    return t;

};

//allows you to exit timer whenever//
void signal_handler(int signum)
{
   cout << "Interrupt signal (" << signum << ") received.\n";

   exit(0);
}

//main loop//
int main(void)
{
    signal(SIGINT,signal_handler);

    cout << "Timer\n";
    int countStop = 0; //intialize with a starting value//

    while (countStop <= 0)
    {
        cout << "Set Timer in seconds: ";
        cin >> countStop;
        
        //will loop around again if this condition fails//
        if (countStop <= 0)
        {
            cout << "Sorry Timer must be greater than 0" << endl;
        }

    }


    //timer loop updates the values every second//
    for (int t = 0; t <= countStop; t = t + increment)
    {
        struct Timer timer = get_time(t);
        cout << timer.hours << ':' << timer.minutes << ':' << timer.seconds << endl;
        
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    
    return 0;
}
