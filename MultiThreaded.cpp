//g++ -o thread Olin.cpp -lpthread
//./thread out.txt
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <fstream>
using namespace std;

int n=10000;    //Global integer varable that represent the 10000 numbers
int randnum[10000], sorted[10000], L2Min[100], top[5];  //Global arrays randnum for the 10000 random numbers, sorted for the sorted 10000 numbers, L2Min for the mins in each level 2 thread, top for the 5 mins one from each level 1 thread
void copy(int from, int to, int arr[]){ //function for copying the array used for sorting the 10000 random numbers
    for(int i=from;i<to;i++){   //Loop for going from and to of 10000 random numbers, from the start index, to the end index
    arr[i] = randnum[i];    //Putting the numbers into the sorted array
    }
}
void* L2(void* data){   //Function for the 100 Level 2 threads that finds the min from 100 random numbers
    long st;    //Long varable st used to put the void pointer into
    st=(long)data;  //Dereference void pointer and put it into st
    int index=st/100;   //Create an index number by dividing st by 100. This works because all the data comming in will be in increments of 100
    int end=st+100; //Create an end varable that is used for the loop to be the last number
    L2Min[index]=randnum[st];   //Set the index of L2Min to the index st of randnum
    for(int k=st;k<end;k++){ //Loop for going through each random number within st to end
        if(L2Min[index]>randnum[k]){    //If statement for getting a min for each Level 2 thread
            L2Min[index]=randnum[k];    //To change the number in L2Min if it meets something smaller
        }
    }  
    pthread_exit(NULL); //Exit the thread
}
void* L1(void* data){   //Function for the level 1 threads with 2000 random numbers each
    long start; //Long varable st used to put the void pointer into
    start=(long)data;   //Dereference void pointer and put it into start
    int from=start/100; //Create a from integer varable for the loop to start from. I divided start by 100 because the numbers that are going to come in are 0,2000,4000,6000,8000 and there are 5 Level 1 threads and each of them will be checking 20 mins from the L2Min
    int to=from+20; //Create a to integer varable for the loop to end. I added 20 to from because i am only checking 20 nums fro each of the level 1 threads
    int index=start/2000;   //Create an index int varable for top array. I divided start by 2000 because the numbers that are going to come in are 0,2000,4000,6000,8000 and when I divide that by 2000 I get 0,1,2,3,4 which are the numbers that I needed for each of the 5 threads I made
    pthread_t tids[20]; //Create an array of 20 threads for each thread from level 1
    pthread_attr_t attr;    //Create thread attributes 
    pthread_attr_init(&attr);   //Set the attributes to defalt values
    long s=start;   //Create a long varable called s
    for(int x=0;x<20;x++){  //Loop for creating 20 threads
        pthread_create(&tids[x], &attr, L2, (void *)s); //Create 20 threads for each level 1 thread
        s+=100; //Adding 100 to s because for the 20 threads each of them need to sort 100 random varables
    } 
    for(int z=0;z<20;z++){  //Loop for joining the 20 threads from each level 1 thread
        pthread_join(tids[z], NULL);    //Join the 20 threads in level 2
    } 
    top[index]=L2Min[from]; //Set top to L2Min 
    for(int c=from;c<to;c++){   //Loop for going through 20 mins out of 100 from L2Min
        if(top[index]>L2Min[c]){    //If statement for finding a number smaller than the one in top
            top[index]=L2Min[c];    //If the smaller min was found change the one in top
        }
    }
    pthread_exit(NULL); //Exit the thread
}

int main(int argc,char* argv[]){
    ofstream outFile(argv[1]);  //Create a file writer outFile
    int grand, line=0;  //Create integer varable grand and line
    time_t t;   //Create time
    srand((unsigned)time(&t)); // Init(seed) randnum generator
    for(int l=0;l<n;l++){ //generate random num
        randnum[l]=rand();  //Get a random number from srand for each of the index of array randnum
    }
    copy(0, n,sorted);  //Call copy function for sorting 
    for (int i = 0; i < n-1; i++){  //Loop for outer loop bubble sort
        for (int j = 0; j < n-i-1; j++) {   //Loop for inner loop bubble sort
            if(sorted[j] > sorted[j+1]){    //Checking if the number is bigger than the one before it 
                int temp=sorted[j]; //Create a temp for the data
                sorted[j]=sorted[j+1];  //Set the current to next
                sorted[j+1]=temp;   //Set next to temp
            }
        }    
    }
    outFile << "Sorted Min: " << sorted[0]<< endl;  //Print out the sorted Min to outFile 
    outFile << "Sorted Max: " << sorted[n-1]<< endl;    //Print out rhe sorted Max to outFile
    pthread_t tids[5];  //Create an aray of 5 threads for level 1
    pthread_attr_t attr;    //Create thread attributes 
    pthread_attr_init(&attr);   //Set the attributes to defalt values
    long start=0;   //Create a long varable called start
    for(int x=0;x<5;x++){   //Loop for creating 5 level 1 threads
        pthread_create(&tids[x], &attr, L1, (void*)start);  //Create the level 1 threads
        start+=2000;    //Add 2000 to start because the ranges I need is 0,2000,4000,6000,8000 (10000/5=2000)
    } 
    for(int z=0;z<5;z++){   //Loop for joining the 5 threads
        pthread_join(tids[z], NULL);    //Join the 5 threads in level 1
    } 
    outFile << "100 Min: " << endl; //Print out the 100 mins from level 2 in L2Min array to outFile
    for(int y=0;y<100;y++){ //Loop for going through 100 mins in the arrat L2Min
        outFile << L2Min[y] << " ";  //Print the mins to outFile
        line++; //Line increace by one 
        if(line==10){   //Chech if the there are 10 numbers on the line
            outFile << endl;    //Go to the next line in outFile
            line=0; //Reset line back to zero
        }
    }
    outFile << "Top 5 Min: " << endl;   //Print the top 5 mins to outFile
    grand=top[0];   //Set index 0 of top to grand min
    for(int t=0;t<5;t++){   //Loop for going through all 5 mins in array top
        outFile << top[t] << " ";  //Print the top 5 mins to outFile
        if(grand>top[t]){   //Check to see if the min in grand is the smalles of the top 5 mins
            grand=top[t];   //If it was not the smallest change it 
        }
    }
    outFile << endl;    //Start on a new line
    outFile << "Grand Min: " << grand << endl;  //Print the grand min to outFile
    outFile.close();    //Close outFile end of code
}
