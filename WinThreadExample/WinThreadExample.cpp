#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <Windows.h>

DWORD WINAPI Worker(LPVOID thread_params) {
	auto int_param = (int)thread_params;
	printf("Hello world!!! This is thread %d.\n", int_param);
	return 0;
}

struct worker_for_params {
	int thread_id, start_idx, end_idx, step;
};

DWORD WINAPI Worker_For(LPVOID thread_params) {
	auto for_params = (worker_for_params *)thread_params;
	// printf("Hello world!!! This is thread %d.\n", for_params->thread_id);
	for (int i = for_params->start_idx; i < for_params->end_idx; i += for_params->step) {
		printf("Iteration %d processed by thread %d\n", i, for_params->thread_id );
	}
	return 0;
}

struct worker_matmul_params {
	int thread_id, start_idx, end_idx, step;
	double *
};

DWORD WINAPI Worker_MatMul(LPVOID thread_params) {
	auto for_params = (worker_for_params *)thread_params;
	// printf("Hello world!!! This is thread %d.\n", for_params->thread_id);
	for (int i = for_params->start_idx; i < for_params->end_idx; i += for_params->step) {
		printf("Iteration %d processed by thread %d\n", i, for_params->thread_id );
	}
	return 0;
}

int main()
{
	printf("Please enter the number of elements to process:\n");
	int num_els;
	scanf("%d", &num_els);

	printf("Processing %d elements.\n", num_els);

	int num_threads = 8;
	HANDLE* thread_handles = new HANDLE[num_threads];
	worker_for_params* thread_params = new worker_for_params[num_threads];

	for (int i = 0; i < num_threads; i++) {
#ifdef BLOCK
		thread_params[i].thread_id = i;
		thread_params[i].start_idx = i * num_els / num_threads;
		thread_params[i].end_idx = (i + 1) * num_els / num_threads;
		thread_params[i].step = 1;
#else
		thread_params[i].thread_id = i;
		thread_params[i].start_idx = i;
		thread_params[i].end_idx = num_els;
		thread_params[i].step = num_threads;
#endif
		thread_handles[i] = CreateThread(NULL, 0, Worker_For, (LPVOID) &thread_params[i], 0, NULL);
	}

	printf("Before work has been done!\n");

	WaitForMultipleObjects(num_threads, thread_handles, true, INFINITE);
	printf("Work has been done!\n");

	return 0;
}

int main_02()
{
	int num_threads = 8;
	HANDLE* thread_handles = new HANDLE[num_threads];

	for (int i = 0; i < num_threads; i++) {
		thread_handles[i] = CreateThread(NULL, 0, Worker, (LPVOID) i, 0, NULL);
	}

	printf("Before work has been done!\n");

	WaitForMultipleObjects(num_threads, thread_handles, true, INFINITE);
	printf("Work has been done!\n");

	return 0;
}

int main_01()
{
	HANDLE thread_handle = CreateThread(NULL, 0, Worker, NULL, 0, NULL);

	printf("Before work has been done!\n");

	if (thread_handle) {
		WaitForSingleObject(thread_handle, INFINITE);
		printf("Work has been done!\n");
	}
	else {
		printf("Unable to create worker thread\n");
	}

	return 0;
}
