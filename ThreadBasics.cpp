//
//  ThreadBasics.cpp
//  
//
//  Created by Linda Marshall on 2016/02/06.
//
//
// Thread example, adapted from http://www.cplusplus.com/reference/thread/thread/
//
// Compile with: g++ -c -std=c++14 ThreadBasics.cpp
//


#include <iostream>       //  cout
#include <thread>         //  thread

using namespace std;

void foo()
{
    for(int i = 0; i<100;i++)
         cout << "Foo: " << i <<  endl;
}

void bar(int x)
{
    for(int i = x; i<100;i++)
         cout << "Bar: " << i <<  endl;
}

int main()
{
    thread threadArray[10];
    
    threadArray[0] =  thread(foo);     // spawn new thread that calls foo()
    threadArray[1] =  thread(bar,50);  // spawn new thread that calls bar(50)
    threadArray[2] =  thread(bar,25);  // spawn new thread that calls bar(50)
    threadArray[3] =  thread(foo);     // spawn new thread that calls foo()
    
     cout << "main and threads execute concurrently...\n";
    
    // synchronize threads:
   for (int i = 0; i < 4; i++) {
        threadArray[i].join();
    }
    
     cout << "threads completed.\n";
    
    return 0;
}