#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "the2.h"

/*
 *  returns the order of the letter in the alphabet
 *  if given char is not an ascii letter, returns 0
 */
int letter_order(char c) {
    if (c < 91 && c > 64) {
        return c - 64;
    } else if (c < 123 && c > 96) {
        return c - 96;
    }
    return 0;
}
/*
 * Prints the connections of given user.
 */
void print_connections(User *user) {
    Connection *connection;
    int i;
    for (i = 0; i < BUCKET_SIZE; i++) {
        printf("Bucket %d: ", i);
        if (user->connections[i] != NULL) {
            connection = user->connections[i];
            do {
                printf("[user1: %s, user2: %s, connection_time: %ld] -> ", connection->user1->name,
                       connection->user2->name, connection->connection_time);

            } while ((connection = connection->next_connection) != NULL);
        }
        printf("NULL\n");


    }
}

/*
 * Returns the hash code of the user.
 */
unsigned long hash_code(User *user) {
    unsigned long hash = 0;
    int i = 0;

    while ((*user).name[i] != 0) {
        hash += letter_order((*user).name[i]) * pow(HASH_KEY, i);
        i++;
    }

    return hash;
}

/*
 * Returns the User with given id from environment.
 */
User *get_user(Environment environment, int id) {
    /* TODO: Implement this function. */
    int j;
	for(j = 0 ; environment.users[j] != NULL ; j++){
		if(environment.users[j]->id == id) return environment.users[j] ;
		
	}
    return NULL;
}

/*
 * Connects two user and registers the related connection objects to both users' connection hash tables.
 */
void helplazim(Environment environment, int id1, int id2, long connection_time){
	User* first = get_user(environment, id1) ;
   	User* second = get_user(environment, id2) ;
    	unsigned long hash_second_to_use = hash_code(second) % BUCKET_SIZE ;
   	
	Connection* head1 = first -> connections[ hash_second_to_use ] ;
	Connection* con_first = (Connection*)(malloc(sizeof(Connection))) ;
	con_first -> user1 = first ;
	con_first -> user2 = second ;
	con_first -> connection_time = connection_time ;
	

        if (first -> connections[ hash_second_to_use ] == NULL){
		first -> connections[ hash_second_to_use ] = con_first;
		con_first -> prev_connection = NULL ;
		con_first -> next_connection = NULL ;
	}
	else{
		if(head1->next_connection == NULL)
		{
			if(connection_time > head1->connection_time){
				head1 -> next_connection = con_first;
				con_first -> prev_connection = head1;
				con_first -> next_connection = NULL ;
			}
			else{
				con_first -> next_connection = head1 ;
				head1 = con_first ;
				con_first -> prev_connection = NULL ;
			}
		}
		else{
		for(; head1->next_connection != NULL && connection_time > head1->connection_time ; head1=head1->next_connection);
		
			if(head1->next_connection == NULL && connection_time > head1->connection_time){
				head1->next_connection = con_first;
				
				con_first -> prev_connection = head1;
				con_first -> next_connection = NULL ;
			}
			else {
			head1 -> prev_connection -> next_connection = con_first ;
			con_first -> prev_connection = head1 -> prev_connection;
			con_first -> next_connection = head1;
			head1 -> prev_connection = con_first;
	}
		}}

}
/*
 * Connects two user and registers the related connection objects to both users' connection hash tables.
 */
void connect_users(Environment environment, int id1, int id2, long connection_time) {
    /* TODO: Implement this function. */
    if(get_connection( environment,id1, id2) == NULL) {
        
    
	helplazim(environment ,id1 ,id2 , connection_time);
	helplazim(environment ,id2 ,id1 , connection_time);
    }
	   

	
	
}

/*
 * Rstrip the given string.
 */

char *rstrip(char *string) {
  char *end;

  while(isspace((unsigned char)*string)){
    string++;
  } 

  if(*string == 0) {  
    return string;
  }

  end = string + strlen(string) - 1;
  while(end > string && isspace((unsigned char)*end)){ 
    end--;
  }

  end[1] = '\0';

  return string;
}


/*
 * Creates a new environment with the information in the given file that contains users
 * with connections and returns it.
 */
Environment *init_environment(char *user_file_name) {
    Environment *environment;
    User *user;
    FILE *fp;
    char *line = NULL;
    char *iter;
    char *sep = " ";
    size_t len = 0;
    size_t read;
    int id, id1, id2;
    long timestamp;
    char *name;
    User* user1, *user2;

    int u_count = 0;

    environment = malloc(sizeof(Environment));
    environment->users = malloc(sizeof(User*));
    environment->users[0] = NULL;

    fp = fopen(user_file_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        line = rstrip(line);
        if (strcmp(line, "#Users#") == 0) {
            continue;
        } else if (strcmp(line, "#Connections#") == 0) {
            break;
        }
        user = malloc(sizeof(User));
        iter = strtok(line, sep);
        id = atoi(iter);
        iter = strtok(NULL, sep);
        name = malloc(sizeof(char)*strlen(iter));
        strcpy(name, iter);
        user->id=id;
        user->name=name;
        environment->users = realloc(environment->users, sizeof(User*)*(u_count+2));
        environment->users[u_count] = user;
        environment->users[u_count+1] = NULL;
        u_count++;
    }
    
    /* Uncomment after connect_users function implemented. */
    while ((read = getline(&line, &len, fp)) != -1) {
        line = rstrip(line);
        iter = strtok(line, sep);
        id1 = atoi(iter);
        iter = strtok(NULL, sep);
        id2 = atoi(iter);
        iter = strtok(NULL, sep);
        timestamp = atol(iter);
        connect_users(*environment, id1, id2, timestamp);

    }

    return environment;

}

/*
 * Returns the connection between users with id1 and id2. The connection
 * is the one stored in user1's hash table. If there is no such a connection returns
 * NULL.
 */
Connection *get_connection(Environment environment, int id1, int id2) {
    /* TODO: Implement this function. */
        User* first = get_user(environment, id1) ;
   	User* second = get_user(environment, id2) ;
	unsigned long hash_second_to_use = hash_code(second) % BUCKET_SIZE ;
	Connection* head1 = first -> connections[ hash_second_to_use ] ;
	 if (first -> connections[ hash_second_to_use ] == NULL){
		return NULL;
	}
	else {
		if(head1->next_connection == NULL ){
			if(head1->user2 == second) return head1;
			else return NULL;
			}
		else{

			for(;head1->next_connection != NULL && head1->user2 != second ; head1=head1->next_connection);
		if( head1->user2 == second) return head1;
		else return NULL;
}
	}
   
    return NULL;
}

/*
 * Remove connection from the given user. Additionally, finds the other user and removes related connection
 * from her/his table also.
 */
void help_remove(Environment environment, int id1, int id2){
    User* first = get_user(environment, id1) ;
   	User* second = get_user(environment, id2) ;
	unsigned long hash_second_to_use = hash_code(second) % BUCKET_SIZE ;
	Connection* temp = get_connection(environment,  id1,  id2);
	Connection* prev = NULL ;	
	Connection* next = NULL ;
	if(temp == NULL) {}
	else if(temp -> next_connection == NULL) {
	   
	    if(temp->prev_connection != NULL) {
	        temp -> prev_connection -> next_connection = NULL ;
	       
	    }
	    else first -> connections[ hash_second_to_use ] = NULL;
		free(temp);		
	}
	else if(temp -> prev_connection == NULL) {
		if(temp->next_connection != NULL){
		    temp -> next_connection -> prev_connection = NULL ;
		    first -> connections[ hash_second_to_use ] = temp -> next_connection;
		}
		 else first -> connections[ hash_second_to_use ] = NULL;
		free(temp);
		}
	else{
		temp -> prev_connection -> next_connection = temp -> next_connection ;
		temp -> next_connection -> prev_connection = temp -> prev_connection ;
		free(temp);
}

}
void remove_connection(Environment environment, int id1, int id2) {
    /* TODO: Implement this function. */
	if(get_connection( environment, id1, id2)!=NULL){
	help_remove(environment ,id1 ,id2);
	help_remove(environment ,id2 ,id1);
	}
	
}

/*
 * Returns dynamic User array which contains common connections. The last element
 * of the array is NULL, which is mandatory for detecting the end.
 */
User **get_common_connections(Environment environment, User *user1, User *user2) {
    /* TODO: Implement this function. */
    User** env =  environment.users ;
    Connection* common1 = NULL ;	
	Connection* common2 = NULL ;
	User** gungor = (User**)malloc(sizeof(User*));
	int i = 1,k = 0;
    for(k; env[k]!=NULL && (env[k]!=user1||env[k]!=user2);k++){
        common1 = get_connection(environment, user1-> id,env[k] -> id) ;
        common2 = get_connection(environment, user2-> id,env[k] -> id) ;
        if(common1 != NULL && common2 != NULL){
            i++;
            gungor = (User**)realloc((void*)gungor,i*sizeof(User*));
            gungor[i-2] = env[k] ;
           
        }
    }
    gungor[i-1] = NULL;
    return gungor ;
    
    
}



