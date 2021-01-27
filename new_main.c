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

#define STRING_SIZE 10
#define LINESIZE 100

struct client_info {

char client_name[20];  
int arrival_time;
int service_time;
int request_seat_id;   
};

struct teller_info {

char teller_name[15];  
bool available; 
};


/* Size of the buffer for queue */
#define BUFFER_SIZE 300 
#define RESERVATION_SIZE 60

struct client_info buffer[BUFFER_SIZE];
bool reservation[RESERVATION_SIZE] = {true};
int insertPointer = 0, removePointer = 0;


int requested_seat_id;
/*Define mutex and semaphores */

sem_t rw_mutex;
sem_t availability_lock;
sem_t empty;
sem_t full;
sem_t reserve_lock;

/* Function definitions that will be used by threads*/
void *teller(void *param);
void *client(void *param);

int main(int argc, char *argv[]){
	
	char line[LINESIZE];
    const char s[2] = ",";  
    char theather_name[100];
    char NUMBER_CLIENTS[100];
	FILE *myfile = fopen ("configuration_file.txt", "r" );
    
    fgets(theather_name, sizeof(theather_name), myfile); 
    theather_name[strcspn(theather_name, "\r\n")] = 0;
    int capacity = 0;
    
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
   
    struct client_info client_array[atoi(NUMBER_CLIENTS)];
    struct teller_info teller_array[3];
    
    strcpy( teller_array[0].teller_name, "Teller A");
    strcpy( teller_array[1].teller_name, "Teller B");
    strcpy( teller_array[2].teller_name, "Teller C");
   
    for(int i =0 ; i<3;i++){
        teller_array[i].available=true;
    }
    

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
      
		pthread_create(&teller_ids[j], NULL, &teller,(void*)teller_ptr );
        printf("%s is available\n", teller_ptr->teller_name);
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
	return 0;
}

void *client(void *param){
	
    struct client_info *client_ptr_my =  (struct client_info*)param;
    sleep(client_ptr_my->arrival_time);
    
    // client needs to produce and get to the queue
    sem_wait(&empty);
    sem_wait(&rw_mutex);

    // Insert reservation seat request to the buffer
    buffer[insertPointer] = *client_ptr_my;
    insertPointer = (insertPointer + 1) % BUFFER_SIZE;
    printf("%s request %d \n", client_ptr_my->client_name, client_ptr_my->request_seat_id);
    
    //remove lock to be used from any teller
    sem_post(&rw_mutex);
	sem_post(&full);
    

	pthread_exit(NULL);
}

void *teller(void *param){
	
    struct teller_info *teller_ptr_my =  (struct teller_info*)param;
    sleep(25);

        sem_wait(&availability_lock);

        
        teller_ptr_my->available = false;
        
        sem_wait(&full);
        sem_wait(&rw_mutex);
        // Get request
        struct client_info served_client = buffer[removePointer];
        removePointer = (removePointer + 1) % BUFFER_SIZE;
        
        
        //try to reserve the seat 
        sem_wait(&reserve_lock);
        bool succesful_request = false;
        if(reservation[served_client.request_seat_id]){
            reservation[served_client.request_seat_id] = false; // make it reserved
            succesful_request= true;
            sleep(served_client.service_time);
            printf("%s get request from %s \n", teller_ptr_my->teller_name,served_client.client_name);
            printf("requested seat is available with id: %d\n", served_client.request_seat_id );
        }
        else{
            for(int i = 1; i< RESERVATION_SIZE; i++){
                if(reservation[i]){
                    reservation[i] = false; // reserve from front
                    succesful_request= true;
                    break;
                }
            }
            sleep(served_client.service_time);
            printf("%s get request from %s \n", teller_ptr_my->teller_name,served_client.client_name);
            printf("requested seat is not available, another seat is given\n");
        }
        if(!succesful_request){
            printf("%s get request from %s \n", teller_ptr_my->teller_name,served_client.client_name);
            printf("not a succesful request, no available seat exist\n");
        }
        
        
        sem_post(&reserve_lock);



        pthread_mutex_unlock(&rw_mutex);
        sem_post(&empty);

        sem_post(&availability_lock);

    

    
	pthread_exit(NULL);
}