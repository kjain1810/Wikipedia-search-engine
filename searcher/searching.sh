rm stemmedtext.txt
echo "ENSURE WORDS TO SEARCH ARE IN searchtext.txt!!"
python3 searcher.py
g++ -std=c++17 searcher.cpp -o run -pthread -lsqlite3
./run
rm run stemmedtext.txt