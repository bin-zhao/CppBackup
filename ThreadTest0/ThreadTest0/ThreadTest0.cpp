// ThreadTest0.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <future>
#include <functional>
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "ThreadTest0.h"

using namespace TestUtil;

#if 0
using namespace std;

void ThreadRoutine(){
	cout << "thread: \n";			// 使用endl的输出为 thread: thread: \n
}

class ThreadRoutineClass{
public:
	void threadRoutine(){
		cout << "thread class: \n";
	}
};

int test(int argc, _TCHAR* argv[])
{
	thread thread0(ThreadRoutine);
	thread thread1(ThreadRoutine);
	thread thread2(&ThreadRoutineClass::threadRoutine, ThreadRoutineClass());		// 这个有点复杂，需要类实例

	thread0.join();
	thread1.join();
	thread2.join();
	cout << "main: " << endl;

	return 0;
}
#endif


#if 0
using namespace std;

mutex						g_mutex;
condition_variable			g_cv;

void ThreadRoutine(){
	unique_lock<mutex>		localMutex(g_mutex);
	g_cv.wait(localMutex);								// 等待信号

	cout << "thread\n";
}

int test(int argc, _TCHAR* argv[])
{
	thread thread0(ThreadRoutine);						// 立即执行线程
	// a1

// 	this_thread::sleep_for(chrono::seconds(1));			// suspend 1s
	g_cv.notify_one();									// 继续留在本线程，并不会切换到thread0

	for (long i = 0; i < 1000; i++){					// 在此期间会切换到thread0！！！
		cout << "main " << i << "\n";
	}

// 	thread0.join();										// suspend自己，等待指定线程，放在a1处会线程死锁

	cout << "main" << endl;

	return 0;
}
#endif


#if 0
class ThreadClass{
public:

	void threadRoutine(std::future<std::string>& future){
		std::cout << "thread: " << future.get() << "\n";			// future.get()会等待数据
	}
};

int test(int argc, _TCHAR* argv[]){
	std::promise<std::string> promise;
	std::future<std::string> future				= promise.get_future();

	ThreadClass obj;
	std::thread thread0(&ThreadClass::threadRoutine, obj, std::ref(future));
	// a1
	promise.set_value("hello, world!");

	thread0.join();													// 把join放到a1处会产生deadlock
	std::cout << "main" << "\n";

	return 0;
}

#endif


#if 0
class ThreadClass{
public:

	void threadRoutine(std::shared_future<std::string>& future){
		std::cout << "thread: " << future.get() << "\n";			// future.get()会等待数据
// 		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
};

int test(int argc, _TCHAR* argv[]){
	std::promise<std::string> promise;
	std::shared_future<std::string> future(promise.get_future());

	const int threadsNum		= 10;
	std::thread threads[threadsNum];
	ThreadClass obj;

	for (int i = 0; i < threadsNum; i++){
		threads[i]				= std::thread(&ThreadClass::threadRoutine, std::ref(obj), std::ref(future));
	}
	
// 	std::this_thread::sleep_for(std::chrono::seconds(5));
	promise.set_value("hello, world!");

	for (auto&& thread : threads){
		thread.join();
	}
	
	std::cout << "main" << "\n";

	return 0;
}

#endif


#if 0
class ThreadClass{
public:

	void threadRoutine(int index){
		std::this_thread::sleep_for(std::chrono::microseconds(1000000));
		std::call_once(m_onceFlag, std::bind(&ThreadClass::callOnce, this, index));
		std::cout << "thread " << index << '\n';
	}


private:

	void callOnce(int index){
		std::cout << "call once " << index << '\n';
	}


private:

	std::once_flag			m_onceFlag;
};


void test(int argc, _TCHAR* argv[]){
	const int threadNum			= 10;
	std::thread threads[threadNum];

	ThreadClass obj;
	for (int i = 0; i < threadNum; i++){
		threads[i]				= std::thread(&ThreadClass::threadRoutine, std::ref(obj), i);
	}

	for (auto&& thread : threads){
		thread.join();
	}
}

#endif


#if 0
class ThreadClass{
public:

	int threadRoutine(int maxIndex) const{
		int total				= 0;

		for (int i = 0; i < maxIndex; i++){
			total				+= i;
		}

		return total;
	}
};


int test(int argc, _TCHAR* argv[]){
	ThreadClass obj;

	// 用cref时，类函数必须是const的
	std::future<int> future			= std::async(std::bind(&ThreadClass::threadRoutine, std::cref(obj), 100));

	std::cout << "future " << future.get() << std::endl;

	return 0;
}

#endif


#if 0
int ThreadRoutine(const int& index){
// 	index++;

	return 0;
}


int test(int argc, _TCHAR* argv[]){
	int index				= 10;

// 	std::thread thread0(ThreadRoutine, index);
// 	std::thread thread0(ThreadRoutine, std::ref(index));
	std::thread thread0(ThreadRoutine, std::cref(index));

	thread0.join();
	std::cout << "index: " << index << std::endl;

	return 0;
}

#endif


#if 0
std::mutex				g_mutexPrint;

void Print(const int& index, const std::thread::id& threadID){
// 	std::unique_lock<std::mutex> lock(g_mutexPrint);
	std::lock_guard<std::mutex> lock(g_mutexPrint);

	std::cout << "thread " << index << ", " << threadID << std::endl;
}


void ThreadRoutine(const int& index){
	std::this_thread::sleep_for(std::chrono::seconds(1));
	Print(index, std::this_thread::get_id());
}


int test(int argc, _TCHAR* argv[]){
	const int threadCount		= 10;
	std::vector<std::thread*> threads;

	for (int i = 0; i < threadCount; i++){
		threads.push_back(new std::thread(ThreadRoutine, i));
	}

	for (auto&& thread : threads){
		thread->join();
	}

	for (auto&& thread : threads){
		delete thread;
		thread		= nullptr;
	}

	std::cout << "main " << std::this_thread::get_id() << std::endl;

	return 0;
}

#endif


#if 1
static std::mutex				__mutex;
static std::list<int>			__list;

void ThreadRoutine(int index){
	for (int i = 0; i < 10; i++){
// 		if (true){
// 		}
//		std::unique_lock<std::mutex> guard(__mutex);
		int value = index * 10 + i;
		__list.push_back(value);
		if (value % 3 == 0){
			__list.remove(value);
		}
// 		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


int ThreadTest0::test(int argc, _TCHAR* argv[]){
	const int threadCount			= 10;
	
	std::vector<std::thread*> threads;

	for (int i = 0; i < threadCount; i++){
		threads.push_back(new std::thread(ThreadRoutine, i));
	}

	for (auto&& thread : threads){
		thread->join();
	}

	for (auto&& thread : threads){
		delete thread;
		thread = nullptr;
	}

	int count			= 0;
	for (auto&& value : __list){
		std::cout << value << " ";
		count++;
		if (count % 10 == 0){
			std::cout << "\n";
		}
	}

	if (count % 10 != 0){
		std::cout << std::endl;
	}

	return 0;
}

#endif


/*
 * 1. join call in main method usually at the end of it for wait another thread execute over.
 * 2. std::cout can be block in multithread program!
*/

int _tmain(int argc, _TCHAR* argv[])
{
	new ThreadTest0;

	Test::getInstance()->test(argc, argv);
	system("pause");
	return 0;
}