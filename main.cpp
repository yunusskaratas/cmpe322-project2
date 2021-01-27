#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sstream>
#include <vector>
using namespace std;
typedef int buffer_item;

void *client(void *param);


int main(int argc, char *argv[]){
	

	string input_file;
	string theater_name;
	int NUMBER_CLIENTS;
    input_file = argv[1];
   
    ifstream in_file;
    ofstream out_file;

    in_file.open(input_file);
    
    in_file>> theater_name;
	in_file >> NUMBER_CLIENTS;
    cout << theater_name<< endl;
	cout  << NUMBER_CLIENTS << endl;
	pthread_t client_ids[NUMBER_CLIENTS];
	
	for(int i = 0; i<NUMBER_CLIENTS ;i++){
		string str;
		in_file >> str;
		vector<string> v;
	
		stringstream ss(str);
	
		while (ss.good()) {
			string substr;
			getline(ss, substr, ',');
			v.push_back(substr);
		}
	
		pthread_create(&client_ids[i], NULL, &client, &v);
		
	}
	
	for(int i = 0; i<NUMBER_CLIENTS ;i++){

		pthread_join(client_ids[i], NULL);
	}
	
	
	// pthread_mutex_init(&mutex, NULL);
	// /* sem full is initialized as 0 */
	// sem_init(&full, 0, 0);
	// /* sem empty is initialized as BUFFER_SIZE */
	// sem_init(&empty, 0, BUFFER_SIZE);

	// producerThreads = atoi(argv[1]);
	// pthread_t pid[producerThreads];

	// consumerThreads = atoi(argv[2]);
	// pthread_t cid[producerThreads];

	// /* Create producer and consumer threads according to the command line arguments */
	// for(i = 0; i < producerThreads; i++){
	// 	pthread_create(&pid[i], NULL, &producer, NULL);
	// }

	// for(j = 0; j < consumerThreads; j++){
	// 	pthread_create(&cid[j], NULL, &consumer, NULL);
	// }

	// /* Join threads */
	// for(int i = 0; i < producerThreads; i++) {
	//     pthread_join(pid[i], NULL);
	// }
	// for(int j = 0; j < consumerThreads; j++) {
	//     pthread_join(cid[j], NULL);
	// }

	// /* Delete mutex and destroy semaphores */
	// pthread_mutex_destroy(&mutex);
	// sem_destroy(&full);
	// sem_destroy(&empty);
	// return 0;
    return 0;
}

void *client(void *param){
	buffer_item item;
	 vector<string> info = *(vector<string> *) param;
	 sleep(2);
	
	
	cout << info[0] << endl;
	
	
	pthread_exit(NULL);
}