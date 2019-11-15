/* instruction-level simulator */
#include "Method.h"
#include <stdlib.h>
#include <stdio.h>
using namespace std;
#define NUMMEMORY 65536 /* maximum number of words in memory 16-bit instruction */
#define NUMREGS 8       /* number of machine registers มี register 8 ตัว */
#define MAXLINELENGTH 1000

typedef struct stateStruct
{ //ห้ามแก้ส่วนนี้
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory; //ตัวนับจำนวนบรรทัดในไฟล์ machine code
} stateType;

void printState(stateType *);
void setInitialize(stateStruct *);
int convertNum(int num);
string convertDec_Bi(int decimal);
string convert_negative(string str);
void action(string str, stateType *state);
int convert_dec(string str);
int offset_dec(string str);
string halt;
int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH]; //แถวในชุดคำสั่ง
    stateType state;
    stateType *stateX;
    //บรรทัดนี้ allocate memory สำหรับpointer ถ้าไม่ทำจะเกิด Segmentation fault (core dumped)
    stateX = (stateType *)(malloc(sizeof(stateType)));
    FILE *filePtr;
    // ใช้ 2 ตัว --> ตัวมัน กับ ตัวไฟล์ที่จะอ่านเข้ามา(Machine code จากข้อแรก )
    if (argc != 2)
    {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    //Initialize reg first
    setInitialize(stateX);

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++)
    {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1)
        {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
        //เช็คค่าใน memory ของ stateX
        stateX->mem[state.numMemory] = state.mem[state.numMemory];
        stateX->numMemory++;
    }
    string str;
    stateX->pc = 0;
    while (stateX->pc < state.numMemory)
    {
        //Call printState 1 ครั้ง ก่อน instruction executes
        printState(stateX);
        str = convertDec_Bi(state.mem[stateX->pc]);
        //    substr ตัวแรกคือเลือกตำแหน่งแรกที่จะเอา ตัวสองคือความยาวที่ต้องการจะใช้
        action(str, stateX);
        //ถ้าเครื่องมีการ halt เกิดขึ้นให้ printState แล้วก็จบการทำงาน
        if (halt == "halted")
        {
            //Call printState 1 ครั้ง ก่อน simulator exits.
            printState(stateX);
            cout<<"Amount of state.numMemory : "<<state.numMemory<<endl;
            cout << "Finish!!!!!!!!!!\n";
            exit(0);
        }
        //Go to next instruction
        stateX->pc++;
    }
    return (0);
}

void action(string str, stateType *stateX)
{
    //add instruction
    if (str.substr(7, 3) == "000")
    {
        //     destReg   =  regA + regB
        stateX->reg[convert_dec(str.substr(29, 3))] = stateX->reg[convert_dec(str.substr(10, 3))] + stateX->reg[convert_dec(str.substr(13, 3))];
    }
    //nand instruction
    else if (str.substr(7, 3) == "001")
    {
        // destReg = regA (nand) regB
        stateX->reg[convert_dec(str.substr(29, 3))] = !(stateX->reg[convert_dec(str.substr(10, 3))] & stateX->reg[convert_dec(str.substr(13, 3))]);
    }
    else if (str.substr(7, 3) == "010")
    {
        // regB = Memory[regA + offsetField]
        //cout<<"Enter\n";
        //cout<<offset_dec(str.substr(16, 16))<<"\t"<<convert_dec(str.substr(10, 3))<<endl;
        stateX->reg[convert_dec(str.substr(13, 3))] = stateX->mem[stateX->reg[convert_dec(str.substr(10, 3))] + offset_dec(str.substr(16, 16))];
    }
    else if (str.substr(7, 3) == "011")
    {
        //Memory[regA + offsetField] = regB
        stateX->mem[stateX->reg[convert_dec(str.substr(10, 3))] + offset_dec(str.substr(16, 16))] = stateX->reg[convert_dec(str.substr(13, 3))];
    }
    else if (str.substr(7, 3) == "100")
    {
        // regA == regB
        if (stateX->reg[convert_dec(str.substr(10, 3))] == stateX->reg[convert_dec(str.substr(13, 3))])
        {
            cout << offset_dec(str.substr(16, 16)) << endl;
            stateX->pc = stateX->pc + offset_dec(str.substr(16, 16));
        }
        else
        {
            //Do nothing
        }
    }
    else if (str.substr(7, 3) == "101")
    {
        if (convert_dec(str.substr(10, 3)) == convert_dec(str.substr(13, 3)))
        {
            // regA และ regB คือregister ตัวเดียวกัน ให้เก็บ PC+1 ก่อน
            stateX->reg[convert_dec(str.substr(10, 3))] = stateX->pc + 1;
                }
        else
        {
            //เก็บค่า pc+1 ใน regB
            stateX->reg[convert_dec(str.substr(13, 3))] = stateX->pc + 1;
            //กระโดดไปที่ address ที่ถูกเก็บไว้ในregA
            stateX->pc = stateX->reg[convert_dec(str.substr(10, 3))];
        }
    }
    else if (str.substr(7, 3) == "110")
    {
        halt = "halted";
        cout << " halt simulator!!!!!\n";
    }
    else if (str.substr(7, 3) == "111")
    {
        //Do nothing
    }
}

int offset_dec(string str)
{
    int sum = 0;
    int check;
    //if input is negative integer
    if (str[0] == '1')
    {
        str = twoComplement(str, 16);
        check = 1;
    }
    else
    {
        //Do nothing
    }
    for (int i = 0; i < 16; i++)
    {
        //แปลง char เป็น int
        sum += (str[i] - '0') * pow(2, 15 - i);
    }
    //return negative integer
    if (check == 1)
    {
        return 0 - sum;
    }
    //return positive integer
    else
    {
        return sum;
    }
}

int convert_dec(string str)
{
    if (str == "001")
    {
        return 1;
    }
    else if (str == "010")
    {
        return 2;
    }
    else if (str == "011")
    {
        return 3;
    }
    else if (str == "100")
    {
        return 4;
    }
    else if (str == "101")
    {
        return 5;
    }
    else if (str == "110")
    {
        return 6;
    }
    else if (str == "111")
    {
        return 7;
    }
    else
    {
        return 0;
    }
}

string convertDec_Bi(int dec)
{
    int tmp[32];
    string s;
    if (dec > 0)
    {
        for (int i = 32; i > 0; i--)
        {
            tmp[i] = dec % 2;
            dec /= 2;
            s = to_string(tmp[i]) + s;
        }
        return s;
    }
    else
    {
        string tmp = to_string(dec);
        string t = tmp.substr(1, tmp.length());
        t = convertDec_Bi(atoi(t.c_str()));
        t = twoComplement(t, 32);
        return t;
    }
}

void setInitialize(stateStruct *state)
{
    int i = 0;
    while (i < 8)
    {
        state->reg[i] = 0;
        i++;
    }
}

void printState(stateType *statePtr) //ห้ามแก้ส่วนนี้
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++)
    {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}
