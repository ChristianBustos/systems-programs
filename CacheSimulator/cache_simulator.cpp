#include <iostream>
#include <iomanip>
#include <cctype>
using namespace std;

const int memorySize = 32768; //32kb
const int cacheLineSize = 16; //16bytes
const int cacheSize = 2048; //2kb
const int associativity = 2; //2way set
const int sets = cacheSize / (cacheLineSize * associativity); //64 sets
const int intsPerLine = cacheLineSize / 4; //4 ints for every cache line

int bitsOffset = 0; //counter of bits needed for offset within cache line
int bitsIndex = 0; //counter of bits to identify the set

struct cacheLine {
	int data[intsPerLine]; //store 4 integers, 16bytes in total
	bool valid; //indicate if line has valid data
	bool dirty; //indicate if the line has been modified but not yet saved to memory
	int tag; //used to identify memory block
	int usedLast; //timestamp to determine LRU
};

//memory and cache declaration
cacheLine cache[sets][associativity]; //64 sets, 2ways each = 128 total cache lines
int memory[memorySize / 4]; //8192 ints for main memory 
int usedLastCounter; //counter for LRU 

//calculate number of bits for offset and index
void calcBitWidths() {
	int temp = cacheLineSize;
	while (temp >>= 1) bitsOffset++; //divide by 2, increment if result is nonzero - count how many times we can divide by 2
	temp = sets;
	while (temp >>= 1) bitsIndex++; //divide by 2, increment if result is nonzero - count how many times we can divide by 2
}

//deconstruct address into offset,index, and tag
void addressParse(int address, int& tag, int& index, int& offset) {
	offset = address & ((1 << bitsOffset) - 1); //lower 4 bits = offset (0-15)
	index = (address >> bitsOffset) & ((1 << bitsIndex) - 1); //next 6 bits = set index (0-63)
	tag = (address >> (bitsOffset + bitsIndex)) & 0xFFFF; //remaining upper 6 bits = tag (0-63)
}

//initalize memory and cache to -1 and invalid
void initialize() {
	usedLastCounter = 0;
	for (int i = 0; i < (memorySize / 4); ++i)
		memory[i] = -1;

	for (int i = 0; i < sets; ++i) {
		for (int j = 0; j < associativity; ++j) {
			cache[i][j].valid = false;
			cache[i][j].dirty = false;
			cache[i][j].tag = -1;
			cache[i][j].usedLast = 0;
			for (int k = 0; k < intsPerLine; ++k)
				cache[i][j].data[k] = -1;
		}
	}
}

//check if tag is in cache set
int locateCacheLine(int tag, int index) {
	for (int way = 0; way < associativity; ++way) {
		if (cache[index][way].valid && cache[index][way].tag == tag)
			return way; //cache hit, tag found
	}
	return -1; //cache miss, tag not found
}

//choose the least recently used line to evict
int selectTarget(int index) {
	for (int way = 0; way < associativity; ++way) {
		if (!cache[index][way].valid)
			return way;
	}
	return (cache[index][0].usedLast > cache[index][1].usedLast) ? 1 : 0;
}


//check if target is dirty, if so write it back to memory
void writeBack(int index, int way) {
	if (cache[index][way].dirty) {
		int memoryAddress = ((cache[index][way].tag << (bitsOffset + bitsIndex)) | (index << bitsOffset));
		for (int i = 0; i < intsPerLine; ++i)
			memory[(memoryAddress >> 2) + i] = cache[index][way].data[i];
	}
}

//load block from memory into cache when a miss occurs
void getBlock(int address, int tag, int index, int way) {
	int baseAddress = address & ~((1 << bitsOffset) - 1); //find starting address of block
	for (int i = 0; i < intsPerLine; ++i)
		cache[index][way].data[i] = memory[(baseAddress >> 2) + i]; //load cache with 4 ints from memory

	cache[index][way].tag = tag;
	cache[index][way].valid = true; //now active and usable
	cache[index][way].dirty = false; //directly from memory, no changes yet
	cache[index][way].usedLast = usedLastCounter++;
}

//access memory to read or write (option A)
void accessMemory(char rw, int address, int data = 0) {
	//address input validation
	if (address < 0 || address >= memorySize) {
		cout << "Input address " << address << " invalid" << endl;
		return; 
	}

	int tag, index, offset;
	addressParse(address, tag, index, offset);
	int way = locateCacheLine(tag, index);

	//if cache miss, choose which line to evict, write back if dirty, load new block into cache
	if (way == -1) {
		way = selectTarget(index);
		writeBack(index, way);
		getBlock(address, tag, index, way); 
	}
	else {
		cache[index][way].usedLast = usedLastCounter++;
	} //increment LRU
	int intOffset = (offset >> 2); // divide offset by 4

	if (toupper(rw) == 'R') {
		int value = cache[index][way].data[intOffset]; //just read dont print
	}
	else if (toupper(rw) == 'W') {
		cache[index][way].data[intOffset] = data; //write data to cache and mark dirty
		cache[index][way].dirty = true;
	}
}

//just display memory and cache contents for address (option B)
void display(int address) {
	if (address < 0 || address >= memorySize) {
		cout << "Input address " << address << " invalid" << endl;
		return;
	}

	int tag, index, offset;
	addressParse(address, tag, index, offset);

	cout << dec << "Address: " << address << " ";
	cout << "memory:" << hex << setw(8) << setfill('0') << memory[address >> 2] << " ";

	for (int way = 0; way < associativity; ++way) {
		if (cache[index][way].valid)
			cout << hex << setw(8) << setfill('0') << cache[index][way].data[offset >> 2] << " ";
		else
			cout << "-1 ";
	}

	for (int way = 0; way < associativity; ++way) {
		if (cache[index][way].valid)
			cout << "1 " << (cache[index][way].dirty ? 1 : 0) << " ";
		else
			cout << "0 0 ";
	}

	cout << endl;
}



int main() {
	calcBitWidths(); //dynamically determine offset and index bits
	initialize(); //clear memory and cache

	while (true) {
		char userInput;
		cout << "\nEnter a command A or B: ";
		cin >> userInput;

		//input commmand validation
		if (toupper(userInput) != 'A' && toupper(userInput) != 'B') {
			cout << "Command must be A, a, B, or b" << endl;
			continue;
		}

		int address;
		cout << "Enter the address: ";
		cin >> address;

		if (toupper(userInput) == 'B' && address == -1)
			break; //b-1 ends the program

		if (address < 0 || address >= memorySize) {
			cout << "Input address " << address << " invalid" << endl;
			continue; //input address validation
		}

		if (toupper(userInput) == 'A') {
			char readWrite;
			cout << "Enter R to read memory or W to write memory: ";
			cin >> readWrite;

			//read write input validation
			if (toupper(readWrite) != 'R' && toupper(readWrite) != 'W') {
				cout << "Request type must be R or W" << endl;
				continue;
			}

			//align address to start of cache line, nearest multiple of 4
			int sourceAddress = address;
			address = (address / 4) * 4;
			if (address != sourceAddress)
				cout << "Setting address to next lower multiple of 4" << endl;

			//write data if W, read data if R
			if (toupper(readWrite) == 'W') {
				int data;
				cout << "Enter integer data to be written: ";
				cin >> data;
				accessMemory(readWrite, address, data);
			}
			else {
				accessMemory(readWrite, address);
			}
		}
		//display address and data associated
		else if (toupper(userInput) == 'B') {
			display(address);
		}
	}
	cout << "Done." << endl;
	return 0;
}
