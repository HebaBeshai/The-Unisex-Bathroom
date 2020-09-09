#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


#include "./ezipc.h"
void quitHandler();
int *total_num_males = 0;
int *total_num_females = 0;

int main(){
    SETUP();
//declarations
    int nStalls;
    int time_of_Arrival;   //hold the input from user temporarily
    char gender; //hold the character input from user
    int current_Num_People = 0;
    int current_Array_Size = 3;
    int time = 0; //the maximum time point of anyone
    
    int *a = (int *)malloc(3* sizeof(int));                            /*every line of the input file, the time they arrive and their gender--
																	    the gender is stored as a number-- positive is female and negative is male is stored here
                                                                        3 is a random number because we do not know the length of the file yet
											                            for example 1F becomes a 1 in the array
											                            a 3M becomes a negative 3 in the array, instead of having two arrays*/
                                                                                                                //printf("HAHAHAHA");
    printf("what is the number of stalls?"); //prints what is the number of
    fscanf(stdin, "%d", &nStalls);  //read one integer as nStalls
    FILE* file = fopen("unisex.txt", "r"); //opens the file unisex.txt
    
    

    while (fscanf(file, "%d %c", &time_of_Arrival, &gender) != EOF){                 // reads the gender and the entry time from the file
        if(current_Num_People >= current_Array_Size){  //creates room to hold more people
            current_Array_Size = current_Array_Size*2; //update array size
            a  = (int *)realloc(a,current_Array_Size* sizeof(int));  //make array bigger
        }

        if (gender == 'M'){ //if the gender if male then...
            time_of_Arrival = time_of_Arrival*-1;  //negative means males
           
        }
        

        a[current_Num_People] = time_of_Arrival;     //stores the entry time in the array
         current_Num_People++;             //each time you read a line need to incrememnt num of people
    } //end of while
    



    int mutex = SEMAPHORE(SEM_BIN, 1);

    //SETUP();
    signal(SIGINT, quitHandler);
 
    int *female_in_bathroom = SHARED_MEMORY(sizeof(int));  //current
    int *male_in_bathroom = SHARED_MEMORY(sizeof(int));
    int *waitingQ = SHARED_MEMORY(sizeof(int));
     
    *total_num_males = SHARED_MEMORY(sizeof(int));  //total
    *total_num_females = SHARED_MEMORY(sizeof(int));
    
    *female_in_bathroom = 0; //current
    *male_in_bathroom = 0;
    *waitingQ = 0;
    *total_num_males = 0; //total
    *total_num_females = 0;
    
    printf("HAHAHAHA\n");

    for(int i = 0; i < current_Num_People; i++){ //they need to fork and determine whether they are parent or child
       

            //what is this persons time point
            time_of_Arrival = a[i];
            
                
                 if(time_of_Arrival < 0){   //less than zero, male
				 printf("check 1");
                time = time_of_Arrival*-1; //time equals the absolute value of temp
                
                
                
                pid_t pid = fork(); //child process for male
                
                if (pid == 0){  //inside the child process
                printf("user M # %d, arrives at %d, enters bathroom\n", i, time);
                printf("number of users inside of bathroom is %d\n", *total_num_males); //**total_num_males
                    sleep((unsigned int)time);

                    
                    int istrue=0;
                    while (female_in_bathroom > 0 || *total_num_males == nStalls) {
                      
                        if(istrue != 1){
                        *waitingQ++;
                        istrue = 1;
                        }
                      
                        
                        sleep(0);
                        
                    }
                    *total_num_males++; //total number
                    
                     
                    P(mutex);
                    male_in_bathroom++;
                    V(mutex);
                    
                    
                    
                    printf("user M # %d is using the stall\n ", current_Num_People);  
                    time = (i *3) %4+1;  //time for bathing
                    sleep(time);
                    
                    
                   
                    
                    P(mutex);
                    *male_in_bathroom--;
                    V(mutex);
                   
                    printf("number of users inside bathroom is %d\n", *male_in_bathroom);
                    
                    exit(0);
                 
                 
                    
                }
            }
            else{
                time = gender;    //else time equals temp
                
                
                
                pid_t pid = fork(); //child process for female
                *total_num_females++;
                if (pid == 0){  //inside the child process
                
                printf("number of users inside of bathroom is %d\n", *total_num_females); //*total_num_females
                
                
                    sleep((unsigned int)time);
                    
                   
                    int istrue=0;
                    while (male_in_bathroom > 0 || *total_num_females == nStalls) {
                      
                        if(istrue != 1){
                        *waitingQ++;
                        istrue = 1;
                        }
                        
                        
                        sleep(0);
                        
                    }
                    *total_num_females++; //total number
                    
                    printf("user F # %d is waiting, the size of the waiting line %d \n ", current_Num_People, *waitingQ);
                    
                     P(mutex);
                     female_in_bathroom++;
                     V(mutex);
                    
                    
                    printf("user F # %d is using the stall\n ", current_Num_People);
                    time = (i *3) %4+1;  //time for bathing
                    sleep(time);
                    
                    printf("user F # %d is done using the stall and leaves bathroom\n ", current_Num_People);
                    printf("number of users inside of bathroom is %d\n", *female_in_bathroom);

                    P(mutex);
                    female_in_bathroom--;
                    V(mutex);
                    
                  
                    exit(0);

                }
            }

        }

}//end of main

void quitHandler() {
    printf("\n%d females used bathroom\n", *total_num_females);
    printf("%d males used bathroom\n", *total_num_males);
    printf("\n%d the end\n");
    fflush(stdout);
    exit(0);

}



