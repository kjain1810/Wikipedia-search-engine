echo "Ensure all wikidump XMLs are in this folder and no other XML is here"
rm -rf files
echo "Beginning preprocessing..."
python3 index.py
echo "Preprocessing finished!"
echo "Beginning indexing"
g++ -std=c++17 createDB.cpp -o run -pthread -lsqlite3
./run ./files
rm -rf files
rm run