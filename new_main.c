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
typedef int buffer_item;
buffer_item START_NUMBER = 0;
buffer_item buffer[BUFFER_SIZE];
int insertPointer = 0, removePointer = 0;


int requested_seat_id;
/*Define mutex and semaphores */
pthread_mutex_t mutex;
sem_t empty;
sem_t full;


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

    
  

    pthread_mutex_init(&mutex, NULL);
    /* sem full is initialized as 0 */
	sem_init(&full, 0, 0);
	/* sem empty is initialized as BUFFER_SIZE */
	sem_init(&empty, 0, BUFFER_SIZE);


    pthread_t client_ids[atoi(NUMBER_CLIENTS)];
    pthread_t teller_ids[3];
    struct client_info *client_ptr = NULL;
    struct teller_info *teller_ptr = NULL;
    client_ptr = client_array;
    teller_ptr  = teller_array;
     //create client threads
    for(int j = 0; j < 3; j++){
      
		pthread_create(&teller_ids[j], NULL, &teller,(void*)teller_ptr );
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
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
	sem_destroy(&empty);
	return 0;
}

void *client(void *param){
	
    struct client_info *client_ptr_my =  (struct client_info*)param;

    pthread_mutex_lock(&mutex);
        
        printf("%s request seat %d\n", client_ptr_my->client_name,client_ptr_my->request_seat_id);
    
    pthread_mutex_unlock(&mutex);
    
	pthread_exit(NULL);
}

void *teller(void *param){
	
    struct teller_info *teller_ptr =  (struct teller_info*)param;
   
	// pthread_mutex_lock(&mutex);
	
    //     printf("%s has arrived.\n",teller_ptr->teller_name);
        
    //     //printf("%d",service_time);
    // pthread_mutex_unlock(&mutex);

    // pthread_mutex_lock(&mutex);
	
        
    // pthread_mutex_unlock(&mutex);
    
	pthread_exit(NULL);
}