#ifndef QUEACTIONS_H
#define QUEACTIONS_H


#include <queue>
#include <string>
#include <mutex>



using namespace std;

template <typename T>
class QueActions {
private:
	queue<T> q;
	mutex mtx;
public:
	QueActions() {}

	bool Add(T data) {
		lock_guard<mutex> lock(mtx);
		q.push(data);
        	return true;
	}

	long long GetSize() {
		return q.size();
	}

	T Get() {
		lock_guard<mutex> lock(mtx);
		T el = q.front();
        	q.pop();
	        return el;
	}
};

#endif
