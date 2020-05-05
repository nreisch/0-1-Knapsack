
all:
	g++ -std=c++11 createkn01.cpp -o createkn01
	g++ -std=c++11 bruteforce.cpp -o bruteforce
	g++ -std=c++11 dynpro.cpp -o dynpro
	g++ -std=c++11 backtrack.cpp -o backtrack

clean: backtrack dynpro bruteforce createkn01
	rm backtrack
	rm dynpro
	rm bruteforce
	rm createkn01
