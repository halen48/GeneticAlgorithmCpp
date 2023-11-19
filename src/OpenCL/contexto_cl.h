
#ifndef CONTEXTO_CL_H
#define CONTEXTO_CL_H

#include <CL/opencl.h>
#include <math.h>
#include "../ga.h"
#define CL_ARQ "cycle.cl"
#define KERNEL_FUNC "cycle"

void Contexto_OpenCL(vector<chromosome> * population){

	/* Data and buffers */
	chromosome * old_pop_host, * new_pop_host;
	cl_mem old_pop_device, new_pop_device;

	/* Estruturas do OpenCL */
	cl_context context;
	cl_program program;
	cl_kernel kernel;
	cl_command_queue queue;
	cl_device_id device;
	cl_platform_id platform;
	
	

	/* Verifica quais drives há disponiveis no Computador (plataforma) */
	clGetPlatformIDs(3, &platform, NULL);
	/* Seleciona o dispositivo relacionado a plataforma (dispositivo) */
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 3, &device, NULL);

	/* Local onde o OpenCL ira rodar*/
	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL/*Tratamento de Erro*/);

	// Initialize vectors on host
	old_pop_host = (chromosome*)(malloc(sizeof(chromosome)*population->size()));

	for (int i = 0; i < population->size(); i++) {
		old_pop_host[i] = (*population)[i];
	}

	/* Abrindo o arquivo */
	FILE *program_handle;
	char *program_buffer;
	size_t program_size;

	{
		program_handle = fopen(CL_ARQ, "r");
		fseek(program_handle, 0, SEEK_END);
		program_size = ftell(program_handle);
		rewind(program_handle);
		program_buffer = (char*)malloc(program_size + 1);
		fread(program_buffer, sizeof(char), program_size, program_handle);
		program_buffer[program_size] = '\0';
		fclose(program_handle);
	}

	/* Cria a rotina de instrucoes a partir do arquivo */
	program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, NULL/*Tratamento de Erro*/);

	/* Compila e constroi o programa para ser rodado */
	if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) != CL_SUCCESS){
		printf("Erro no programa: %s\n", program_buffer);
		system("PAUSE");
		exit(1);
	}

	free(program_buffer);

	/* Cria o kernel */
	kernel = clCreateKernel(program, KERNEL_FUNC, NULL);

	/* Cria os Buffers para a soma */
	old_pop_device = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(chromosome)*population->size(), NULL, NULL);
	new_pop_device = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(chromosome)*population->size(), NULL, NULL);

	/* Cria a fila do programa */
	queue = clCreateCommandQueue(context, device, 0, NULL/*Tratamento de Erro*/);

	/* Aloca os valores do dispositivo. Algo equivalente a 'dispositivo' = 'host', mas 'dispositivo' possui um endereço de memoria unico*/
	clEnqueueWriteBuffer(queue, old_pop_device, CL_TRUE, 0, sizeof(chromosome)*population->size(), old_pop_host, 0, NULL, NULL);

	/* Passa os argumentos para o kernel */
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &old_pop_host);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &old_pop_device);

	/* Prepara o kernel para executar na fila */
	/* Dimensões da grade de trabalho */
	size_t globalSize, localSize;
	localSize = ceil(sizeof(chromosome)*population->size() / 8.0);

	clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL);

	/* sincronizacao das threads */
	clFinish(queue);

	/* equivalente a 'host' = 'device' */
	clEnqueueReadBuffer(queue, new_pop_device, CL_TRUE, 0, sizeof(chromosome)*population->size(), &new_pop_host, 0, NULL, NULL);

	/* Copia os valores */
	for (int i = 0; i < sizeof(chromosome)*population->size(); i++){
		(*population)[i] = (*new_pop_host)[i];
	}

	/* desaloca os recursos (Opcional, mas boa pratica)*/
	clReleaseMemObject(old_pop_device);
	clReleaseMemObject(new_pop_device);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

}
#endif
