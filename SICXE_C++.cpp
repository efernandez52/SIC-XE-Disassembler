# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <sstream>
# include <iomanip> 
# include <cctype>
# include <queue>
using namespace std;
string extractHexValue(const string& input)
    {
        size_t start = input.find("X'");
        size_t end = input.find("'", start + 2);
        return input.substr(start + 2, end - start - 2);
    }
struct SymbolEntry1 
    {
        string symbol;
    };
struct SymbolEntry2
    {
        string name;
        string litconst;
        int length;
        string address;

    };
string getRegisterValue(const string& input) 
    {  
    const static string registers[] = // registers for format 2 instructions 
		{
			"A", "X", "L", "B", "S", "T", "F", "PC", "SW"
		};

	const static string registerVals[] = 
		{
			"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
		};	
        char firstChar = input[0];
        char secondChar = input[1];

        string result;

        if (secondChar == '0') // if the second character is 0 return the corresponding value from regsters
            {
                int index = firstChar - '0';
                result = registers[index];
        
            } 
        else // if the second character is not 0 return both values separated by a comma
            {
                int index = firstChar - '0';
                result = registers[index];
                result += ",";
                index = secondChar - '0';
                result += registers[index];

            }
            return result;
    }


string decimalToHexString(int decimalVal)// this function turns decimal into a hex string of 4 characters
 {
    stringstream ss;
    ss << hex << decimalVal; 
    string LOCCTRString = ss.str();
    for (char &c : LOCCTRString) 
    {
        c = toupper(c);
    }
	int minLength = 4;// this makes sure the location counter is exactly four characters if it is not it adds zeros
    if (LOCCTRString.length() < minLength) 
	{
        LOCCTRString = string(minLength - LOCCTRString.length(), '0') + LOCCTRString;
    }
    return LOCCTRString;

}

unsigned int hexStringToDecimal(const string& tempString) // this functions takes a hex string and turns it into decimal 
	{
        size_t pos;
        unsigned int result = stoul(tempString, &pos, 16);
        return result;
    } 

string xbp(char heCh) // this function determines xbp
{
int hexValue;
/*because of formatting issues this process is in most of my functions and it turns my input char into an integer so that i am able to mask it */

        if (isdigit(heCh))
                {
                        hexValue = heCh - '0'; // isdigit this one checks if it is a regular number and not a letter
                }
        else if (isalpha(heCh))
                {
                        hexValue = toupper(heCh) - 'A' + 10; // this subtracts A from an upercase hex character and adds 10 so we get an integer example B - A = 1  +10 = 11 and infact B = 11
                }


    // Extract the middle two bits (bit 2 and bit 1)
  	 bool bit2 = (hexValue & 0x04) != 0; // this masks all value but mot the b bit
       	 bool bit1 = (hexValue & 0x02) != 0; // this masks all the values but not the p bit
      		if (bit2 && !bit1)
		 	{
                         	return "base";  // 'b' = 1 condition
                 	}
	 	else if (!bit2 && bit1)
		 	{
    				return "pc";  // 'p' = 1 condition
        	 	}
	 	else if (bit2 && bit1)	 
		 	{
          			return "invalid"; // 'b' = 1 'p' = 1 condition
                 	}	
	 	else if (!bit2 && !bit1) 
		 	{
            			return "absolute";   // Neither 'b' nor 'p'
               	 	}
       
 	     return " ";
        
}

string determineFormat3or4(const string& tempOpcode)// this function determines format 3 or 4
	{
    	if (tempOpcode.length() <= 2)
 		{
			string format = "-1";
       		 return format ;  
   		}

    	char hexDigit = tempOpcode[2]; // for example say tempOpcode contains 050 tempOpcode[2] is 0 in format3/4 instructions the third hex digit in the objcode determines the xbpe flags 

       	int hexValue;

    	if (isdigit(hexDigit))
	        {
        	hexValue = hexDigit - '0';
    		}

	 else if (isalpha(hexDigit)) 
	        {
        	hexValue = toupper(hexDigit) - 'A' + 10;
   	 	}
 	else 
		{
            string format = "-1";
       		 return format ; 
   	 	}

     	if (hexValue % 2 == 0) // in my own research i denoted that only in format3/4 if the third hex digit in obj code is odd this means it is format 4 if it is even or = 0 then this means it is format 3
			{
			string format = "3";
        	return format;
    		} 
	else
 			{
        	string format = "4";
       		 return format ; 
    		}
	}


string oatBitmask( char heCh) // this function will mask evrything except for n and i bits
{
	int hexValue;

    	if (isdigit(heCh))
    		{
        		hexValue = heCh - '0';
    		}
    	else if (isalpha(heCh))
    		{
        		hexValue = toupper(heCh) - 'A' + 10;
    		}
    

    	hexValue &= 0x03; // Apply the bitmask to the lowest 2 bits
        // this bitmask takes the singular hexvalue and keeps only lowest 2 bits for example if hexValue is C then this will output 00x0 since the bits for C are bx1100 and for 3 bx0011 this means 3 masks the bits where it has 0 giving us bx0000
    	string result;

    	if (hexValue == 0x02) //if you look close at n and i bits you will notice that in a valid sicxe instructions if ni = bx10 this is 2 in hex , 10 also means indirect and etc.
    		{
        		result = "@"; // @ means indirect
    		}
    	else if (hexValue == 0x01)
    		{
        		result = "#"; // # means immediate
    		}
    	else // this case was originally (hexValue == 0x03) however we must assume all instructions are valid even though a 00 in sicxe would indicate invalid because 00 is only for regular sic
    		{
        		result = "";// simple has nothing attached to operand 
    		}
                     
        return result;
}


string processBitmask(const string& hexString)
{
	 char heCh = hexString[1]; 
    	 int hexValue;

    	if (isdigit(heCh))
    		{
        		hexValue = heCh - '0';
    		}
    	else if (isalpha(heCh))
    		{
        		hexValue = toupper(heCh) - 'A' + 10;
    		}
    	else
    		{
                	return "00";
    		}

        hexValue &= 0xFC; // as discussed in class this should be our bitmask to dtermine opcode only if we have determined obj code to not be format 2 
	char maskedChar;
        if (hexValue < 10) // this is just like using isdigit 
    		{
        		maskedChar = hexValue + '0';
    		}
    	else
    		{
        		maskedChar = hexValue - 10 + 'A';
    		}

        string result = hexString;
    	result[1] = maskedChar;

    	return result;
}


int processCheckifFormat2(const string& tempOpcode, const string ops[], const bool format2[], size_t arraySize)
{
	for (size_t i = 0; i < arraySize; i++) // in this forloop i check to see if my current tempopcode is part of the ops the forloop iterates over the size of the ops array which is also the size of our oter data structures
		{
		if (tempOpcode == ops[i] && format2[i]) // if the opcode is found in the ops array then we have to check if it is a format 2 instruction if it is True then it returns the position i this will be used to determinemnemonic
			{
			return i;
			}
		} 


	return -1;
}


int main(int argc, char** argv )
{



	if (argc != 3) //this is checking to make sure we have two arguments in command line
		{
			cerr << "Error: Please include input files" << endl;
			return 1;
		}
    const char* input_File1 = argv[1];
    const char* input_File1_1 = argv[1];
	const char* input_File2 = argv[2];
    const char* input_File2_2 = argv[2];
	ifstream inputFileStream1(input_File1);	
    ifstream inputFileStream1_1(input_File1_1);	// extra												 //this is the input file stream 
	ifstream inputFileStream2(input_File2);
	if (!inputFileStream1.is_open()) //this is making sure the file provided can be opened and read
		{
        	cerr << "Error: The file can not be opened" << endl;
        	return 1;
        }

	if (!inputFileStream2.is_open()) //this is making sure the file provided can be opened and read
		{
        	cerr << "Error: The .sym file can not be opened" << endl;
        	return 1;
        }	

	ofstream outputFileStream("out.lst");
	
	if (!outputFileStream.is_open()) // if the output file can not be made this prints an error
 	 	{
            cerr << "Error: The file can not be made" << endl; 
            return 1;
        }
	

	const static string ops[] = 
		{
        	"18", "58", "90", "40", "B4", "28",
        	"88", "A0", "24", "64", "9C", "C4",
        	"C0", "F4", "3C", "30", "34", "38",
        	"48", "00", "68", "50", "70", "08",
        	"6C", "74", "04", "D0", "20", "60",
        	"98", "C8", "44", "D8", "AC", "4C",
        	"A4", "A8", "F0", "EC", "0C", "78",
        	"54", "80", "D4", "14", "7C", "E8",
        	"84", "10", "1C", "5C", "94", "B0",
        	"E0", "F8", "2C", "B8", "DC"
    	};



	const static bool format2[] = 
		{
       	 	false,false,true,false,true,false,
        	false,true,false,false,true,false,
        	false,false,false,false,false,false,
        	false,false,false,false,false,false,
       	 	false,false,false,false,false,false,
        	true,false,false,false,true,false,
        	true,true,false,false,false,false,
        	false,false,false,false,false,false,
        	false,false,false,false,true,true,
        	false,false,false,true,false
   		};


	const static string mnemonics[] = 
		{
			"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
			"COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
			"FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
			"JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
			"LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
			"MULR", "NORM", "OR", "RD", "RMO", "RSUB",
			"SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
			"STCH", "STF", "STI", "STL","STS", "STSW",
			"STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
			"TD", "TIO", "TIX", "TIXR", "WD"
    	};

    	//for my formatting i decided to use vectors there is a vector for each assignment for ex OAT is vectorOAT etc..
	vector<string> vectorOpcode;

	vector<string> vectorTA;

	vector<string> vectorSym; 

	vector<string> vectorMnemonic;

	vector<string> vectorLocctr;

    queue<int> newLOCCTR;// this will help with first in first out for new LOCCTR

    queue<int> symLOCCTR;

    queue<string> qByte;

    queue<string> nameq;

    queue<string> litconstq;

    queue<int> lengthq;

	string displacement;

	string tempOpcode;

    string tempColumn;

	string headerName;

	string startAddress;

	string target;

	string hexValue;

	string Base;

	string TA;

	string tempMask;

    string line;

    string symbol;

	int baseDec;

	int LOCCTR; // this is my LOCCTR

	int tempLOCCTR;

	int tempint;

    int ctr = 0;

	size_t opsSize = sizeof(ops) / sizeof(ops[0]); //this is the size of the ops array which is also the size of our other two arrays format2 and mnemonic this will be useful in searching the arrays
	
	string format;

    while (getline(inputFileStream2, line)) // this file stream is for the symbol file 
        {  
            if(!line.empty() && line[0] != '-' && line[1]!= 'y' && ctr != 1)
                {
                istringstream iss1(line);
                SymbolEntry1 entry;
                iss1 >> entry.symbol;
                symbol = entry.symbol;
                ctr++;        
                }
            else if (!line.empty() && line[0] != '-' && line[0] != 'N'&& line[0] != 'S' && line[0] != 'F')
                {
                istringstream iss2(line);
                SymbolEntry2 entry;
                iss2 >> entry.name >> entry.litconst >> entry.length >> entry.address;
                nameq.push(entry.name);
                int A = hexStringToDecimal(entry.address);
                symLOCCTR.push(A);
                string val = extractHexValue(entry.litconst);
                qByte.push(val);
                litconstq.push(entry.litconst);
                lengthq.push(entry.length);
                }    
        }
    while (getline(inputFileStream1, tempColumn)) 
		{
        	if (!tempColumn.empty() && tempColumn[0] == 'H') 
			{
				startAddress = tempColumn.substr(9,4);
            	headerName = tempColumn.substr(1,6);
				vectorSym.push_back(headerName);
				vectorLocctr.push_back(startAddress);
				vectorMnemonic.push_back("START");
				vectorSym.push_back(symbol);
				vectorOpcode.push_back("");// we have no object code for the first line 
				LOCCTR = hexStringToDecimal(startAddress);// takes the starting address from hex and turns it into decimal so that i am able to increment the counter 
				if (LOCCTR == 0)
				{
					vectorTA.push_back("0");
				}
				else
				{
					vectorTA.push_back(startAddress);
				}

				break;
			}
        }
    while (getline(inputFileStream1_1,tempColumn))// this iterates through the text file first to get the starting adresses 
        {
            while(!tempColumn.empty() && tempColumn[0] == 'T')
                {
                    string getAddress = tempColumn.substr(3,4);
                    int decAddress = hexStringToDecimal(getAddress);
                    newLOCCTR.push(decAddress);
                    break;
                }
            
        }
    inputFileStream1_1.close();	
    vectorLocctr.push_back(startAddress);	
	newLOCCTR.pop();// we are popping the first item in the queue since we are assuming the first start address is already declared in the header
	while (getline(inputFileStream1, tempColumn)) //this is my most important piece of code this while loop will terminate when there are no more lines to read 
		{         
			int count = 9; // because our instructions start in column 10 i set this counter to 9 since this counter will be key in determining where we are in the line so position 9 [9] is actually the 10th character where we start our obj code
			while(!tempColumn.empty() && tempColumn[0] == 'T' ) // this makes sure that every line we read we only proocess if the first element is T in other words this makes sure we are only processing text records this while loop terminates once the string is empty
				{	
					string length = tempColumn.substr(7,2);// these next lines help with knowing the length of the text record 
					int objLength = hexStringToDecimal(length);	// they will help us decide when to change the address since diffferent text records have different starting addreses
                    startAddress = tempColumn.substr(3,4);
					tempLOCCTR = hexStringToDecimal(startAddress);
                    objLength += tempLOCCTR;// this helps me keep track of how long the obj length should be in relation to start address of the current text record
		  			if (count + 1 < tempColumn.size())// this is checking to make sure there is charactersto be processed since we must assume our input file contains only valid obj code then my code assumes that if there is a character after our current count there must be another object code to process 
                    	{
                        if ( !symLOCCTR.empty() && symLOCCTR.front() == LOCCTR)// the code after this handles the symbol file logic 
                                    {
                                        vectorMnemonic.push_back("BYTE");
                                        vectorTA.push_back(litconstq.front());
                                        vectorOpcode.push_back(qByte.front());
                                        vectorSym.push_back(nameq.front());
                                        LOCCTR += (lengthq.front()/2);// length is in half bytes
                                        count += lengthq.front();
                                        symLOCCTR.pop();
                                        litconstq.pop();
                                        qByte.pop();
                                        nameq.pop();
                                       lengthq.pop();
                                       string LOCCTRstring = decimalToHexString(LOCCTR);
									if(objLength > LOCCTR)
										{ 
											vectorLocctr.push_back(LOCCTRstring);	
										}
                                    else if (!newLOCCTR.empty() && objLength == LOCCTR)
                                        {
                                            LOCCTR = newLOCCTR.front();
                                            LOCCTRstring = decimalToHexString(LOCCTR);
                                            vectorLocctr.push_back(LOCCTRstring);
                                            newLOCCTR.pop();
                                        }
                                    else 
                                        {
                                            vectorLocctr.push_back(LOCCTRstring); // this part is to handle the case that there is no more text records after the current one    
                                        }  

                                    }
                            else
                            {
							tempOpcode += tempColumn[count]; // saves the string at in tempcolumn at our current count and appends it to tempOpcode
							count++;// increment count to save the next string
							tempOpcode += tempColumn[count];
							int i = processCheckifFormat2(tempOpcode, ops, format2, opsSize); // this function takes our current first two hex characters of obj code and check if they are in format 2 							
							if ( i != -1) // the previous function will return the position at which its contents held true and because our 3 arrays are mapped this helps us use i to get our mnemonic 
								{             //also the previous function returns -1 if the opcode was found in ops array but it was false in bool so not -1 means it is format 2
									format = "2";
                                   vectorSym.push_back("");
									LOCCTR +=2;
                                        
                            
									string LOCCTRstring = decimalToHexString(LOCCTR);
									if(objLength > LOCCTR)
										{ 
											vectorLocctr.push_back(LOCCTRstring);	
										}
                                    else if (!newLOCCTR.empty() && objLength == LOCCTR)
                                        {
                                            LOCCTR = newLOCCTR.front();
                                            LOCCTRstring = decimalToHexString(LOCCTR);
                                            vectorLocctr.push_back(LOCCTRstring);
                                            newLOCCTR.pop();
                                        }
                                    else 
                                        {
                                            vectorLocctr.push_back(LOCCTRstring); // this part is to handle the case that there is no more text records after the current one    
                                        }    
                                    
                                      		
									count++;
									tempOpcode += tempColumn[count];  //here we are appending again , most of my count++ and tempOpcode += tempColumn[count] is just keeping track of the object code
                                	count++;
									tempOpcode += tempColumn[count];
                                	vectorOpcode.push_back(tempOpcode);
                               		vectorMnemonic.push_back(mnemonics[i]); // as mentioned before i use the returned i value to find the corresponding mnemonic
									string temp = tempOpcode.substr(2,2);
                                    string reg = getRegisterValue(temp);
                                    vectorTA.push_back(reg);
                                    
	
								}

 							else // this only executes if i = -1 this just means that it is not format 2 therefore we must mask that second bit
								{
									tempMask = tempOpcode; 
									string maskedBit = processBitmask(tempMask); // this function calls the process bitmask and irts input is our tempOpcode if it was determined not to be format 2
									tempMask.clear(); // when i was coding i had alot of errors soo this is just to make sure i can still make use of the empty string
									char tempChar = tempOpcode[1]; // since we know it is not format 2 it must be format 3/4 have a 6bit opcode so we must take the second character of our obj code and mask the left two bits  so that we can analyze the n and i bits which are on the right side
									tempMask = oatBitmask(tempChar); //this function will return immediate indirect or simple
									count++;
									tempOpcode += tempColumn[count];
									tempChar = tempOpcode[2];
									string xbpResult = xbp(tempChar); //xbp will return the TAAM
									format = determineFormat3or4(tempOpcode);
									vectorSym.push_back("");
									string maybeBase;
									for (size_t y = 0; y < opsSize; y++)// this forloop pushes the mnemonic into the vector but modifies it if its format 4
										{
											if (maskedBit == ops[y]) // after we masked our bit we returned y and now this forloop searches our new op code in the mnemonics array
												{
													if (format == "3")
														{
															
															vectorMnemonic.push_back(mnemonics[y]);
															maybeBase = mnemonics[y];
															break;	
														}
													else if (format == "4")
														{
															vectorMnemonic.push_back("+" + mnemonics[y]);
															maybeBase = mnemonics[y];
															break;	
														}
													
												}
										}

									// i already talked about the count++ being a way to save my objcode
                                    if (maybeBase == "LDB")
										{
										vectorLocctr.push_back("");	
										}

									if (format == "3")
										{
                                           
									        LOCCTR +=3;
                                            
                                            
                                            string LOCCTRstring = decimalToHexString(LOCCTR);
                                            if(objLength > LOCCTR)
										        { 
											        vectorLocctr.push_back(LOCCTRstring);	
										        }
                                            else if (!newLOCCTR.empty() && objLength == LOCCTR)
                                                {
                                                    LOCCTR = newLOCCTR.front();
                                                    newLOCCTR.pop();
                                                    LOCCTRstring = decimalToHexString(LOCCTR);
                                                    vectorLocctr.push_back(LOCCTRstring);
                                                }
                                            else 
                                                {
                                                    vectorLocctr.push_back(LOCCTRstring); // this part is to handle the case that there is no more text records after the current one    
                                                }    
											count++;
											tempOpcode+= tempColumn.substr(count,3);
											count +=2;
											displacement = tempOpcode.substr(3,3);
											tempint = hexStringToDecimal(displacement);
								
											if (xbpResult == "base")
											{
												tempint += baseDec;
												TA = decimalToHexString(tempint);
												vectorTA.push_back(tempMask + TA);
											}
											else if (xbpResult == "pc")
											{
												if (tempint > 2047)// this displacement field of pc
													{
														tempint -= 4096;
													}

												
												tempint += LOCCTR;
												TA = decimalToHexString(tempint);
												vectorTA.push_back(tempMask + TA);
													
											}
											else
											{
                                                TA = decimalToHexString(tempint);
												vectorTA.push_back(tempMask + TA);	
											}
										  

										}	
									else if(format == "4")
										{
											
									        LOCCTR +=4;
                                           
                                            string LOCCTRstring = decimalToHexString(LOCCTR);
                                            if(objLength > LOCCTR)
										        { 
											        vectorLocctr.push_back(LOCCTRstring);	
										        }
                                            else if (!newLOCCTR.empty() && objLength == LOCCTR)
                                                {
                                                    LOCCTR = newLOCCTR.front();
                                                    newLOCCTR.pop();
                                                    LOCCTRstring = decimalToHexString(LOCCTR);
                                                    vectorLocctr.push_back(LOCCTRstring);
                                                }
                                            else 
                                                {
                                                    vectorLocctr.push_back(LOCCTRstring); // this part is to handle the case that there is no more text records after the current one    
                                                }    

											count++;
											tempOpcode += tempColumn.substr(count,5);
											count+=4;
											displacement = tempOpcode.substr(4,4);
											vectorTA.push_back(tempMask + displacement); 

										}
										
									vectorOpcode.push_back(tempOpcode); // we push the objcode to the vector
									if (maybeBase == "LDB")	
										{
											Base = tempOpcode.substr(4,4);
											baseDec = hexStringToDecimal(Base);// this will hold the contents of the base register in decimal
											vectorTA.push_back(Base);
											vectorMnemonic.push_back("BASE");
											vectorOpcode.push_back("");
                                            maybeBase = "empty";
										}
								
								}

							tempOpcode.clear();// clear obj code before starting over  and increment the counter so that we move on to the next object code
							count++;
                            }
						}
					else
		    			{
							break;
	        	 		}
				}
		}		
	    // this is how my program will output
        vectorMnemonic.push_back("END");
        vectorTA.push_back(headerName);
        vectorLocctr.pop_back();
        vectorLocctr.push_back("");
        vectorSym.push_back("");
        vectorOpcode.push_back("");
        //cout<<vectorTA.size()<< " "<< vectorLocctr.size()<<" "<< vectorMnemonic.size()<< " "<< vectorOpcode.size()<< " "<< vectorSym.size()<< endl;
    for(size_t x = 0; x < vectorTA.size(); x++)
    	{
            outputFileStream <<left<<setw(15)<<vectorLocctr[x]
		    <<left<<setw(15)<<vectorSym[x]
		    <<left<<setw(15)<<vectorMnemonic[x]
			<<left<<setw(15)<<vectorTA[x]
		    <<left<<setw(15)<<vectorOpcode[x]
		    << endl;
        }
	inputFileStream1.close();
	inputFileStream2.close();
	outputFileStream.close();
	return 0;
}
