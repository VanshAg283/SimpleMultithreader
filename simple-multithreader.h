#include <iostream>
#include <list>
#include <stdlib.h>
#include <cstring>
#include <functional>
#include <pthread.h>
#include <sys/time.h>

int user_main(int argc, char **argv);
double tot_time = 0;

struct ThreadData
{
  int low;
  int high;
  std::function<void(int)> lambda;
};

void *threadFunc(void *arg)
{
  if (arg == NULL)
  {
    std::cerr << "Error: NULL argument passed to threadFunc\n";
    return NULL;
  }
  ThreadData *data = static_cast<ThreadData *>(arg);
  int thread_id = *(int *)arg;
  int start = data->low;
  int end = data->high;
  for (int j = start; j < end; ++j)
  {
    data->lambda(j);
  }
  pthread_exit(NULL);
}

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads)
{
  if (numThreads <= 0) {
      std::cerr << "Error: numThreads must be greater than 0\n";
      return;
  }
  if (high < low) {
      std::cerr << "Error: high must be greater than or equal to low\n";
      return;
  }
  if (!lambda) {
      std::cerr << "Error: lambda function is null\n";
      return;
  }
  pthread_t threads[numThreads];
  ThreadData data[numThreads];
  int range = high - low;
  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int i = 0; i < numThreads; ++i)
  {
    data[i].low = low + i * (range / numThreads);
    data[i].high = data[i].low + (range / numThreads);
    data[i].lambda = lambda;
    if ((data[i].high > high))
    {
      data[i].high = high;
    }
    if (pthread_create(&threads[i], NULL, threadFunc, &data[i]) != 0) {
        std::cerr << "Error: Failed to create thread " << i << "\n";
        return;
    }
  }

  for (int i = 0; i < numThreads; ++i)
  {
    if (pthread_join(threads[i], NULL) != 0) {
        std::cerr << "Error: Failed to join thread " << i << "\n";
        return;
    }
  }

  gettimeofday(&end, NULL);
  double diff = ((end.tv_sec - start.tv_sec) * 1000) + ((end.tv_usec - start.tv_usec) / 1000.0);
  std::cout << "Execution time: " << diff << " ms\n";
  tot_time += diff;
}

struct ThreadData2
{
  int low1;
  int high1;
  int low2;
  int high2;
  std::function<void(int, int)> lambda;
};

void *threadFunc2(void *arg)
{
  if (arg == NULL)
  {
    std::cerr << "Error: NULL argument passed to threadFunc\n";
    return NULL;
  }
  ThreadData2 *data = (ThreadData2 *)(arg);
  int thread_id = *(int *)arg;
  int start1 = data->low1;
  int end1 = data->high1;
  int start2 = data->low2;
  int end2 = data->high2;
  for (int j = start1; j < end1; ++j)
  {
    for (int k = start2; k < end2; ++k)
    {
      data->lambda(j, k);
    }
  }
  pthread_exit(NULL);
}

void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads)
{
  if (numThreads <= 0) {
      std::cerr << "Error: numThreads must be greater than 0\n";
      return;
  }
  if (high1 < low1 || high2 < low2) {
      std::cerr << "Error: high must be greater than or equal to low\n";
      return;
  }
  if (!lambda) {
      std::cerr << "Error: lambda function is null\n";
      return;
  }
  pthread_t threads[numThreads];
  ThreadData2 data[numThreads];
  int range1 = high1 - low1;
  int range2 = high2 - low2;
  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int i = 0; i < numThreads; ++i)
  {
    data[i].low1 = low1 + i * (range1 / numThreads);
    data[i].high1 = data[i].low1 + (range1 / numThreads);
    data[i].low2 = low2;
    data[i].high2 = high2;
    data[i].lambda = lambda;
    if ((data[i].high1 > high1))
    {
      data[i].high1 = high1;
    }
    if (pthread_create(&threads[i], NULL, threadFunc2,(void*)&data[i]) != 0) {
        std::cerr << "Error: Failed to create thread " << i << "\n";
        return;
    };
  }

  for (int i = 0; i < numThreads; ++i)
  {
    if (pthread_join(threads[i], NULL) != 0) {
        std::cerr << "Error: Failed to join thread " << i << "\n";
        return;
    }
  }

  gettimeofday(&end, NULL);
  double diff = ((end.tv_sec - start.tv_sec) * 1000) + ((end.tv_usec - start.tv_usec) / 1000.0);
  std::cout << "Execution time: " << diff << " ms\n";
  tot_time += diff;
}

void demonstration(std::function<void()> &&lambda)
{
  if (!lambda)
  {
    std::cerr << "Error! Lambda function is null\n";
    return;
  }
  lambda();
}

int main(int argc, char **argv)
{
  int x = 5, y = 1;

  auto lambda1 = [x, &y](void)
  {
    y = 5;
    std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
  };
  demonstration(lambda1);

  int rc = user_main(argc, argv);

  auto lambda2 = []()
  {
    std::cout << "====== Hope you enjoyed CSE231(A) ======\n";
    std::cout << "Total execution time: " << tot_time << " ms\n";
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main
