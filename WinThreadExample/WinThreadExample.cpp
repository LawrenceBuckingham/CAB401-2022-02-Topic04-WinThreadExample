#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <omp.h>

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
	int thread_id, start_idx, end_idx, step, N;
	double* a, *b, *c;
};

DWORD WINAPI Worker_MatMul(LPVOID thread_params) {
	auto params = (worker_matmul_params*)thread_params;
	auto a = params->a;
	auto b = params->b;
	auto c = params->c;
	auto N = params->N;

	for (int i = params->start_idx; i < params->end_idx; i += params->step) {
		for (int j = 0; j < N; j++) {
			double t = 0;
			for (int k = 0; k < N; k++) {
				t += a[i * N + k] * b[k * N + j];
			}
			c[i * N + j] = t;
		}
	}
	return 0;
}

void matmul(double* a, double* b, double* c, int N) {
	for (int i = 0; i < N; i ++) {
		for (int j = 0; j < N; j++) {
			double t = 0;
			for (int k = 0; k < N; k++) {
				t += a[i * N + k] * b[k * N + j];
			}
			c[i * N + j] = t;
		}
	}
}

void check_equal( double * expeccted, double * acctual, int N) {
	for (int i = 0; i < N * N; i++ ) {
		if (expeccted[i] != acctual[i]) {
			printf("Error at (%d, %d)\n", i / N, i % N);
			return;
		}
	}
}

int main()
{
	printf("Please enter N:\n");
	int N;
	scanf("%d", &N);
	printf("Processing matrices N = %d.\n", N);

	double* a = new double[N * N];
	double* b = new double[N * N];
	double* c_orig = new double[N * N];
	double* c = new double[N * N];

	srand(12);

	for (int i = 0; i < N * N; i++) {
		a[i] = 100.0 * rand() / RAND_MAX;
		b[i] = 100.0 * rand() / RAND_MAX;
		c[i] = 0;
		c_orig[i] = 0;
	}

	printf("starting matmul at %f\n", omp_get_wtime());
	matmul(a, b, c_orig, N);
	printf("ended    matmul at %f\n", omp_get_wtime());

	printf("starting matmul par at %f\n", omp_get_wtime());
	int num_threads = 8;
	HANDLE* thread_handles = new HANDLE[num_threads];
	worker_matmul_params* thread_params = new worker_matmul_params[num_threads];

	for (int i = 0; i < num_threads; i++) {
#ifndef BLOCK
		thread_params[i].thread_id = i;
		thread_params[i].start_idx = i * N / num_threads;
		thread_params[i].end_idx = (i + 1) * N / num_threads;
		thread_params[i].step = 1;
		thread_params[i].N = N;
		thread_params[i].a = a;
		thread_params[i].b = b;
		thread_params[i].c = c;
#else
		thread_params[i].thread_id = i;
		thread_params[i].start_idx = i;
		thread_params[i].end_idx = num_els;
		thread_params[i].step = num_threads;
#endif
		thread_handles[i] = CreateThread(NULL, 0, Worker_MatMul, (LPVOID) &thread_params[i], 0, NULL);
	}

	WaitForMultipleObjects(num_threads, thread_handles, true, INFINITE);
	printf("ended    matmul par at %f\n", omp_get_wtime());

	check_equal(c_orig, c, N);

	return 0;
}

int main_03()
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
