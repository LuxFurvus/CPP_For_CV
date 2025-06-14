clear; g++ -std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra test.cpp -o test && ./test; rm test

clear; g++ -std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra TextCleanerByMarkerTests.cpp TextCleanerByMarker.cpp -lgtest -lgtest_main -pthread -o test && ./test; rm test

clear; g++ -std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra main.cpp -o main && ./main; rm main