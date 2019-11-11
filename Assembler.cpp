/* Assembler code fragment */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>
using namespace std;

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
string translate(string, string, string, string, string, int);
int bintodec(string);
string label_str[100], opcode_str[100], arg0_str[100], arg1_str[100], arg2_str[100];
int address = 0;
int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    if (argc != 3)
    {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];
    //read assembly instruction
    inFilePtr = fopen(inFileString, "r");
    // std::cout<<inFilePtr<<std::endl;
    if (inFilePtr == NULL)
    {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    // outFilePtr = fopen(outFileString, "w");
    // if (outFilePtr == NULL)
    // {
    //     printf("error in opening %s\n", outFileString);
    //     exit(1);
    // }
    //Write to Show.txt
    ofstream myfile(outFileString);

    char line[MAXLINELENGTH];
    int count = 0;
    while (fgets(line, MAXLINELENGTH, inFilePtr))
    {
        //                                    ในหนึ่งบรรทัดlabelจะมีlengthไม่เกิน 6 (space 6 ครั้ง)
        if (line[0] == ' ')
        {
            sscanf(line, "%[      ] %s %s %s %s", label,
                   opcode, arg0, arg1, arg2);
        }
        else
        {
            sscanf(line, "%s %s %s %s %s", label,
                   opcode, arg0, arg1, arg2);
        }
        label_str[count] = label;
        opcode_str[count] = opcode;
        arg0_str[count] = arg0;
        arg1_str[count] = arg1;
        arg2_str[count] = arg2;
        printf("(address :%d) %s", address, line);
        address++;
        count++;
    }
    int addr = 0;
    string result[100];
    int sum[100];
    fclose(inFilePtr);
    if (myfile.is_open())
    {
        for (int i = 0; i < address; i++)
        {

            result[i] = translate(label_str[i], opcode_str[i], arg0_str[i], arg1_str[i], arg2_str[i], addr);
            //ถ้าerror เมื่อประกาศopcodeที่นอกเหนือจากที่กำหนด
            if (result[i] == "error")
            {
                cout << "error :" << opcode_str[i] << " is not found\n";
                exit(1);
            }
            else
            {
                if (opcode_str[i] == ".fill")
                {
                    //Do nothing
                    sum[i] = atoi(result[i].c_str());
                }
                else
                {
                    sum[i] = bintodec(result[i]);
                }
            }
            //print answer to Show.txt file
            myfile << "(address " << i << "): " << sum[i] << endl;
            addr++;
        }
        myfile.close();
        exit(0);
    }
    else
    {
        cout << "Unable to openfile\n";
    }
    return (0);
}

int bintodec(string str)
{
    int sum = 0, minus = 1;
    for (int i = 0; i < 32; i++)
    {
        int a = str[i] - '0';
        sum += a * pow(2, 32 - minus);
        minus++;
    }
    return sum;
}

string dectobin(string str)
{
    switch (atoi(str.c_str()))
    {
    case 1:
        return "001";
        break;
    case 2:
        return "010";
        break;
    case 3:
        return "011";
        break;
    case 4:
        return "100";
        break;
    case 5:
        return "101";
        break;
    case 6:
        return "110";
        break;
    case 7:
        return "111";
        break;

    default:
        return "000";
        break;
    }
}

string twoComplement(string str)
{
    int n = str.length();
    char tmp[16], onesComp[16], twosComp[16];

    strcpy(tmp, str.c_str());
    int i, carry = 1;
    /* Find ones complement of the binary number */
    for (i = 0; i < 16; i++)
    {
        if (tmp[i] == '1')
        {
            onesComp[i] = '0';
        }
        else if (tmp[i] == '0')
        {
            onesComp[i] = '1';
        }
    }
    onesComp[16] = '\0';

    /*
     * Add 1 to the ones complement
     */
    for (i = 16 - 1; i >= 0; i--)
    {
        if (onesComp[i] == '1' && carry == 1)
        {
            twosComp[i] = '0';
        }
        else if (onesComp[i] == '0' && carry == 1)
        {
            twosComp[i] = '1';
            carry = 0;
        }
        else
        {
            twosComp[i] = onesComp[i];
        }
    }
    twosComp[16] = '\0';
    return twosComp;
}

string offSet(string str)
{
    int num = atoi(str.c_str());
    char tmp[16];
    if (num > 0)
    {
        string s = "";
        for (int i = 0; i < 16; i++)
        {
            tmp[i] = '0' + (num % 2);
            num /= 2;
            s = tmp[i] + s;
        }
        cout << "\n";

        return s;
    }
    else
    {
        string t;
        t = str.substr(1, 2);
        t = offSet(t);
        return twoComplement(t);
    }
}

bool checklabel(string str)
{
    int count = 0;
    for (int i = 0; i < address; i++)
    {
        if (!str.compare(label_str[i]))
        {
            //Do nothing
            i = address;
        }
        else
        {
            count++;
        }
    }
    count++;
    if (count > address)
    {
        cout << "error : " << str << " is undefine!\n";
        exit(1);
    }
    else
    {
        return 1;
    }
}

string translate(string label, string opcode, string arg0, string arg1, string arg2, int addr)
{
    string bit = "0000000";
    if (arg0 != "" && arg1 != "" && arg2 != "")
    {

        if (isNumber(strdup(arg0.c_str())) && isNumber(strdup(arg1.c_str())) && isNumber(strdup(arg2.c_str())))
        {
            //Do Nothing
        }
        else if (opcode == ".fill")
        {
            //Do Nothing
        }
        else
        {
            //เช็คก่อนว่า label ที่นำเข้ามามีการประกาศไว้ในคำสั่งหรือเปล่า
            if (checklabel(arg2))
            {
                //Do nothing
            }
            for (int i = 0; i < address; i++)
            {
                //เช็คหาว่าค่าstringมีค่าเป็นintเท่ากับเท่าไหร่
                if (opcode_str[i] == ".fill")
                {
                    if (!arg2.compare(label_str[i]))
                    {
                        arg2 = arg0_str[i];
                    }
                }

                else if (!arg2.compare(label_str[i]))
                {
                    cout << addr << " " << i << endl;
                    if (i < addr)
                    {
                        arg2 = to_string(i - addr + 1);
                    }
                    else
                    {
                        arg2 = to_string(i);
                    }
                }
            }
        }
    }
    if (opcode == "lw")
    {
        return bit + "010" + dectobin(arg0) + dectobin(arg1) + offSet(arg2);
    }
    else if (opcode == "sw")
    {
        return bit + "011" + dectobin(arg0) + dectobin(arg1) + offSet(arg2);
    }
    else if (opcode == "beq")
    {
        if (isNumber(strdup(arg2.c_str())))
        {
            return bit + "100" + dectobin(arg0) + dectobin(arg1) + offSet(arg2);
        }
    }
    else if (opcode == "add")
    {
        return bit + "000" + dectobin(arg0) + dectobin(arg1) + "0000000000000" + dectobin(arg2);
    }
    else if (opcode == "nand")
    {
        return bit + "001" + dectobin(arg0) + dectobin(arg1) + "0000000000000" + dectobin(arg2);
    }
    else if (opcode == "jalr")
    {
        return bit + "101" + dectobin(arg0) + dectobin(arg1) + "0000000000000000";
    }
    else if (opcode == "noop")
    {
        return bit + "111" + "0000000000000000000000";
    }
    else if (opcode == "halt")
    {
        return bit + "110" + "0000000000000000000000";
    }
    else if (opcode == ".fill")
    {
        if (isNumber(strdup(arg0.c_str())))
        {
            return arg0;
        }
        //ถ้า arg0 เป็นตัวอักษร

        else
        {
            int addr = 0;
            for (int i = 0; i < 100; i++)
            {
                if (!arg0.compare(label_str[i]))
                {
                    arg0 = to_string(addr);
                    i = 100;
                }
                addr++;
            }
            return arg0;
        }
    }
    //หากใช้opcodeที่นอกเหนือจากคำสั่ง
    else
    {
        return "error";
    }
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}