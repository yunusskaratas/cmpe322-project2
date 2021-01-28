/*
* This program provides a possible solution for producer-consumer/bounded buffer problem using mutex and semaphore.
*
* How to compile: gcc producer_consumer.c -o producer_consumer.o -lpthread
* How to run: ./producer_consumer.o num_of_producers num_of_consumers
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h> 
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sched.h>
#define STRING_SIZE 10
#define LINESIZE 100

struct client_info {

char client_name[20];  
int arrival_time;
int service_time;
int request_seat_id;  
pthread_t thread_id; 
};

struct teller_info {

char teller_name[15];  

};


/* Size of the buffer for queue */
#define BUFFER_SIZE 300 
#define RESERVATION_SIZE 250

struct client_info buffer[BUFFER_SIZE];
int reservation[RESERVATION_SIZE];
int insertPointer = 0, removePointer = 0;
int availability[3];
int capacity=0;
int requested_seat_id;
int client_number=0;
int reserved_client_number = 0 ;
/*Define mutex and semaphores */

sem_t rw_mutex;
sem_t availability_lock;
sem_t empty;
sem_t full;
sem_t reserve_lock;
sem_t A_LOCK;
FILE *outputfile;

/* Function definitions that will be used by threads*/
void *teller(void *param);
void *client(void *param);

int main(int argc, char *argv[]){
	
	char line[LINESIZE];
    const char s[2] = ",";  
    char theather_name[100];
    char NUMBER_CLIENTS[100];
	FILE *myfile = fopen (argv[1], "r" );
    outputfile = fopen(argv[2],"w");
    fgets(theather_name, sizeof(theather_name), myfile); 
    theather_name[strcspn(theather_name, "\r\n")] = 0;
    
    printf("Welcome to the Sync-Ticket!\n");
    fprintf(outputfile,"Welcome to the Sync-Ticket!\n");
    if(strcmp(theather_name,"OdaTiyatrosu") == 0){
        capacity = 60;
    }else if(strcmp(theather_name,"UskudarStudyoSahne") == 0){
        capacity = 80;
    }else if(strcmp(theather_name,"KucukSahne") == 0){
        capacity = 200;
    }else{
        printf("theather name does not match!\n");
    }


    fgets(NUMBER_CLIENTS, sizeof(NUMBER_CLIENTS), myfile); 
   client_number = atoi(NUMBER_CLIENTS);
    struct client_info client_array[atoi(NUMBER_CLIENTS)];
    struct teller_info teller_array[3];
    
    strcpy( teller_array[0].teller_name, "Teller A");
    strcpy( teller_array[1].teller_name, "Teller B");
    strcpy( teller_array[2].teller_name, "Teller C");
   
    
    

    for(int i=0;i<atoi(NUMBER_CLIENTS);i++){
        
        fgets(line, sizeof(line), myfile);  
        strcpy( client_array[i].client_name, strtok(line, s));
        
        client_array[i].arrival_time= atoi(strtok(NULL, s));
        client_array[i].service_time= atoi(strtok(NULL, s));
        client_array[i].request_seat_id= atoi(strtok(NULL, s));
        //printf("%s : %d \n",client_array[i].client_name,client_array[i].arrival_time);
    }
    // read completed

    
    sem_init(&availability_lock, 0, 3);
    sem_init(&rw_mutex, 0, 1);
    sem_init(&A_LOCK, 0, 1);
    
    sem_init(&reserve_lock, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);


    pthread_t client_ids[atoi(NUMBER_CLIENTS)];
    pthread_t teller_ids[3];
    struct client_info *client_ptr = NULL;
    struct teller_info *teller_ptr = NULL;
    client_ptr = client_array;
    teller_ptr  = teller_array;
     //create teller threads
    for(int j = 0; j < 3; j++){
     
		pthread_create(&teller_ids[j], NULL, &teller,NULL );
        printf("%s has arrived.\n", teller_ptr->teller_name);
        fprintf(outputfile,"%s has arrived.\n", teller_ptr->teller_name);
        teller_ptr++;
       
	}
    //create client threads
    for(int j = 0; j < atoi(NUMBER_CLIENTS); j++){
      
		pthread_create(&client_ids[j], NULL, &client,(void*)client_ptr );
        
        client_ptr++;
	}




    //wait main thread till the end
    for(int j = 0; j < atoi(NUMBER_CLIENTS); j++){
		pthread_join(client_ids[j], NULL);
	}
    for(int j = 0; j < 3; j++){
		pthread_join(teller_ids[j], NULL);
	}
    sem_destroy(&rw_mutex);
    sem_destroy(&reserve_lock);
    sem_destroy(&availability_lock);
    sem_destroy(&full);
	sem_destroy(&empty);
    sem_destroy(&A_LOCK);
    printf("All clients received service.\n");
    fprintf(outputfile,"All clients received service.\n");
    fclose(outputfile);
    return 0;
}

void *client(void *param){
	
    struct client_info *client_ptr_my =  (struct client_info*)param;
    client_ptr_my->thread_id = pthread_self();
    //printf("thread id is : %u\n",(unsigned int)client_ptr_my->thread_id);
    sleep(client_ptr_my->arrival_time);
    
    // client needs to produce and get to the queue
    sem_wait(&empty);
    sem_wait(&rw_mutex);

    // Insert reservation seat request to the buffer
    buffer[insertPointer] = *client_ptr_my;
    insertPointer = (insertPointer + 1) % BUFFER_SIZE;
    //printf("%s request %d \n", client_ptr_my->client_name, client_ptr_my->request_seat_id);
    
    //remove lock to be used from any teller
    sem_post(&rw_mutex);
	sem_post(&full);
    

	
}

void *teller(void *param){
	
    //struct teller_info *teller_ptr_my =  (struct teller_info*)param;
    
    struct client_info served_client;
    int reserved_teller = -1;
    while(1){
        if(client_number == reserved_client_number ){
            pthread_exit(NULL);
           
        }

        sem_wait(&availability_lock);
        //teller_ptr_my->available = false;
         
        sem_wait(&full);
        sem_wait(&rw_mutex);
        //printf("availability : ");
        //for(int i = 0; i<3; i++){
        //    printf("%d ", availability[i]);
        //}
        //printf("\n");
        sem_wait(&A_LOCK);
        if(availability[0]==0){
            
            
            served_client = buffer[removePointer];
            removePointer = (removePointer + 1) % BUFFER_SIZE;
            reserved_teller = 0;
            availability[0]=1;
            sem_post(&A_LOCK);
        }
        else if(availability[1]==0){
   
            served_client  = buffer[removePointer];
            removePointer = (removePointer + 1) % BUFFER_SIZE;
           reserved_teller = 1;
           availability[1]=1;
            sem_post(&A_LOCK);
        }
        else if(availability[2]==0){
            
            served_client = buffer[removePointer];
            removePointer = (removePointer + 1) % BUFFER_SIZE;
            reserved_teller = 2;
            availability[2]=1;
            sem_post(&A_LOCK);
        }
        sem_post(&rw_mutex);
        sem_post(&empty);

       
        
        //printf("%d handle the request of %s \n",reserved_teller+1,served_client.client_name );
        //printf("reservation seat id: %d \n",served_client.request_seat_id);

        int successful = 0;
        sem_wait(&reserve_lock);
        if(reservation[served_client.request_seat_id]==0){
            successful = served_client.request_seat_id;
        }
        else{
            int i;
            for(i = 1; i< capacity; i++){
                if(reservation[i]==0){
                    
                    successful = i;                   
                    break;
                }
            }
        }
        //printf("thread name : %s with id : %u canceled\n", served_client.client_name,(unsigned int)served_client.thread_id);
        pthread_cancel(served_client.thread_id);
        reserved_client_number = reserved_client_number +1;
        //printf("reserved client number : %d \n", reserved_client_number);
         char* teller_name;
        if(reserved_teller == 0){
            teller_name = "Teller A";
        }
        else if(reserved_teller == 1){
            teller_name = "Teller B";
        }
        else if(reserved_teller == 2){
            teller_name = "Teller C";
        }
    
       
        if(successful>0){
           
            
            reservation[successful] = 1; // make it reserved 
            sem_post(&reserve_lock);
            sleep(served_client.service_time);
             availability[reserved_teller] = 0;
             sem_post(&availability_lock);
            //Client2 requests seat 1, reserves seat 1. Signed by Teller B.
           
            printf("%s requests seat %d, reserves seat %d. Signed by %s.\n",served_client.client_name,served_client.request_seat_id,successful,teller_name);
            fprintf(outputfile, "%s requests seat %d, reserves seat %d. Signed by %s.\n",served_client.client_name,served_client.request_seat_id,successful,teller_name );

            //printf("reserved seat :  %d  for client : %s , signed by: %d \n", successful,served_client.client_name,reserved_teller+1);
            

        }
        else{
            //Hermione123 requests seat 2, reserves None. Signed by Teller A.
            printf("%s requests seat %d, reserves seat None. Signed by %s.\n",served_client.client_name,served_client.request_seat_id,teller_name);
            fprintf(outputfile,"%s requests seat %d, reserves seat None. Signed by %s.\n",served_client.client_name,served_client.request_seat_id,teller_name);
            
             sem_post(&reserve_lock);
              availability[reserved_teller] = 0;
             sem_post(&availability_lock);
        }

        
        

        

    

    }
    //sem_post(&availability_lock);
	
}