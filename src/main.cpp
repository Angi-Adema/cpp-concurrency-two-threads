#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace std;

// Create a mutex object to lock access to shared resources
mutex mtx;

// Initialize a counter
int counter = 0;

// Initialize a shared variable indicating whether incrementCounter is finished
bool incrementDone = false;

// Create a condition_variable to utilize notifying waiting threads of the state of
// shared resources
condition_variable cv;

// Create a method to handle incrementing the counter to 20
void incrementCounter() {
	// Print statement indicating the results of the increment
	cout << "Result of incrementing the counter:" << endl;

	// Use a for loop to increment the counter to 20
	for (int i = 1; i < 21; i++) {

		// Use lock_guard with the mutex to acquire the lock on the shared resource
		// and automatically release the lock
		lock_guard<mutex> lock(mtx);

		// Increment the counter by 1 each loop cycle
		counter++;

		// Print statement showing the result of counter each iteration
		cout << counter << " ";

		// End on a new line
		if (counter == 20) {
			cout << endl;
		}

	}

	// Since incrementDone is a shared variable, we must create a new lock to update its state
	// This is included in curly braces to support the scope of the lock_guard releasing the
	// the lock once complete
	{
	lock_guard<mutex> lock(mtx);
	incrementDone = true;
	}

	// Call notify_one() using the conditional_variable to notify waiting thread
	// resources are now available
	cv.notify_one();
}

// Create a method to decrement the counter back to 0
void decrementCounter() {

		// Since we are using a condition variable wait() we use unique_lock to lock
	    // the shared resource of counter
		unique_lock<mutex> lock(mtx);

		// Call wait() to put the second thread into wait mode, pass in the lock
		// allowing the condition_variable to manage the lock, temporarily unlocking the lock
		// until incrementDone comes back as true. When notify_one is called, it will
		// take the second thread out of waiting mode through the lambda function confirming
		// incrementDone holds a value of true and wait() locks the mutex again.
		cv.wait(lock, [] {
			return incrementDone;
		});

		// Since thread 2 is no longer waiting, we release the lock.
		lock.unlock();

		// Print statement indicating the results of the decrement
		cout << "\nResult of decrementing the counter:" << endl;

		// Use a for loop to decrement the counter variable
		for (int i = 20; i > 0; i--) {

			// Lock the shared resources using lock_guard to automatically release the lock
			lock_guard<mutex> lock(mtx);

			// Print out the result of the counter with each iteration
			cout << counter << " ";

			// Decrement the counter each iteration
			counter--;

	}
}

// Program begins with the main method creating the first thread
int main() {

	// Create two additional threads that begin executing one right after the other
	thread t1(incrementCounter);
	thread t2(decrementCounter);

	// Main thread pauses here while thread 1 executes, then thread 2 executes, finally
	// main thread continues at the completion of thread 2
	t1.join();
	t2.join();

	return 0;

}
