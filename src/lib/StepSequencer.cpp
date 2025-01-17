#include <cstddef>

class Sequencer {
	int index = 0;
	int length;
	bool direction = false;
	double cv[];

	void main(int n){
		length = n; 
		cv[n] = {0};
	}

	bool setValue(int n, double val){
		if (length > n){
			cv[n] = val;
			return true;  
		}

		return false;
	}

	double getCurrentValue(int n){
		return cv[n];
	}

	int nextStep(){
		if (length % index == 0){
			index = 0;
			return index;
		} else {
			return index++;
		}
	}

	int previousStep(){
		if (length % index == length){
			index = length;
			return index;
		} else {
			return index--;
		}
	}

	int changeDirection(){
		direction = !direction;
		return direction;
	}

	void reset(){
		index = 0;
	}
};