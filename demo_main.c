/* includes */
#include <stdio.h>
#include <stdlib.h>
/* includes do kernel do FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//Include math.h para geração aleatória do item
#include <math.h>

#define N 5

int sharedNumber = 0;
int sharedArray[5] = {0,0,0,0,0};
int count = 0;

//Declara os 3 semáforos
xQueueHandle xCountingSemaphoreFull;
xQueueHandle xCountingSemaphoreEmpty;
xQueueHandle xCountingSemaphoreMutex;


/*  Protótipos das tasks                                      */

void taskProdutor(void *pvParameters);
void taskConsumidor(void *pvParameters);

static int produce_item(){
	return rand();
}

static int consume_item(int item){		
	//consome item
}

int insert_item(int item, char* sender){

	sharedArray[count] = item;

	printf("-------------------------------------------------------- \n\n");
	printf("%s - Item inserido - %d itens no buffer\n", sender, ++count);
}

int remove_item(){

	int item = sharedArray[count-1];
	sharedArray[count-1] = 0;

	printf("-------------------------------------------------------- \n\n");
	printf("Consumidor - Item consumido - %d itens no buffer \n", --count);

	return item;
}

void taskProdutor(void *pvParameters){

	int item;
	
	char *sender = (char *)pvParameters;

	for(;;){
	
		item = produce_item();
		xSemaphoreTake( xCountingSemaphoreEmpty, portMAX_DELAY );
		xSemaphoreTake( xCountingSemaphoreMutex, portMAX_DELAY );

		insert_item(item, sender);

		xSemaphoreGive( xCountingSemaphoreMutex);
		xSemaphoreGive( xCountingSemaphoreFull);

		Sleep(600);
	}
}

void taskConsumidor(void *pvParameters){

	int item;

	for(;;){

		xSemaphoreTake(xCountingSemaphoreFull, portMAX_DELAY);
		xSemaphoreTake(xCountingSemaphoreMutex, portMAX_DELAY);

		item = remove_item();

		xSemaphoreGive( xCountingSemaphoreMutex);
		xSemaphoreGive( xCountingSemaphoreEmpty);

		consume_item(item);
		Sleep(600);
	}
}

void taskVazia(void * p){

	for (;;)
	{
		//Nao faz nada
	}
}


void demo_main( void )
{
	//Cria 3 semáforos, full, empty e o mutex
	xCountingSemaphoreFull  = xSemaphoreCreateCounting(5, 0);
	xCountingSemaphoreEmpty = xSemaphoreCreateCounting(5, 5);
	xCountingSemaphoreMutex = xSemaphoreCreateCounting(1, 1);

	//Cria uma task que não faz nada, somente para ser escalonada
	xTaskCreate( taskVazia, "Task vazia", 1000, NULL, 1, NULL );

	//Instancia um produtor
	xTaskCreate( taskProdutor, "Produtor1", 1000, (void *)"Produtor", 1, NULL );
	
	//Instancia outro produtor
	xTaskCreate( taskProdutor, "Produtor2", 1000, (void *)"Produtor2", 1, NULL );

	//Instancia o consumidor
	xTaskCreate( taskConsumidor, "Consumidor", 1000, NULL, 1, NULL );

	/* Inicia o escalonador de tarefas */
	vTaskStartScheduler();

	for( ;; );
}