all:
	g++ -o CounterTest CounterTest.cpp -lgtest -lgtest_main -pthread
	
test:
	make -j 4 && ./CounterTest && rm CounterTest
