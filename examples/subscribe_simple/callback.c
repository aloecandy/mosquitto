#include <stdlib.h>
#include <stdio.h>
#include "mosquitto.h"
#include <string.h>
#include <time.h>

FILE *fp;

char fileAddr[256];
time_t current_time;
struct tm *struct_time;
struct tm *prev_struct_time;

int createFile(char* filename){
	strcat(strcpy(fileAddr, getenv("HOME")), "/");
	strcat(fileAddr, filename);
	strcat(fileAddr,".csv");
	fp=fopen(fileAddr,"w");
	if(fp==NULL){
        printf("Failed to open %s",fileAddr);
		return -1;
    }
	else{
		fprintf(fp, "topic,time,dust[ug/m^3]\n");
		fclose(fp);
		return 1;
	}
}
int on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg)
{
	time(&current_time);
	struct_time=localtime(&current_time);
	if(prev_struct_time==NULL ||prev_struct_time->tm_mday!=struct_time->tm_mday){
		prev_struct_time=struct_time;
		char tmp[20];
		sprintf(tmp,"dust_%02d%02d%02d",struct_time->tm_year%100,struct_time->tm_mon,struct_time->tm_mday);
		createFile(tmp);
	}
	
	printf("%s,%2d%2d%2d,%s\n", msg->topic,
	  struct_time->tm_hour,struct_time->tm_min,struct_time->tm_sec,
	  (const char *)msg->payload);
	fp=fopen(fileAddr,"a");
	if(fp==NULL){
        printf("Failed to open %s",fileAddr);
		return -1;
    }
	else{
		fprintf(fp,"%s,%02d%02d%02d,%s\n", msg->topic,
			struct_time->tm_hour,struct_time->tm_min,struct_time->tm_sec,
			(const char *)msg->payload);
		fclose(fp);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	
	int rc;
	
	mosquitto_lib_init();

	rc = mosquitto_subscribe_callback(
			on_message, NULL,
			"dust/#", 0,
			"ethernetcard.dlinkddns.com", 1883,
			NULL, 60, true,
			NULL, NULL,
			NULL, NULL);

	if(rc){
		printf("Error: %s\n", mosquitto_strerror(rc));
	}
	
	mosquitto_lib_cleanup();
		
	return rc;
}

