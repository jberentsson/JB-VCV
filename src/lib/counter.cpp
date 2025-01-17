#include <stdlib.h>

class Counter {
	private:
		int size = 0;
		int i = 0;

	public:
		void main(int n){
			size = n;
		}

		int increment(){
            i++;

            if (i > (size - 1)){
                i = 0;
            }

            return i;
        }

        int decrement(){
            i--;

            if (i < 0){
                i = size - i - 2;
            }

            return i;
        }

		int index(){
			return i;
		}

		void setIndex(int x){
            i = x;
		};

        int getSize(){
            return size;
        }
};