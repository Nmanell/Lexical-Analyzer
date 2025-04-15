/* 
 * PA1
 * main.cpp
 * CS280
 * Author: Enmanuel Bueno
 */

#include <iostream> 
#include <fstream>
#include <set>
#include "lex.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc <= 1) {
        cout << "NO SPECIFIED INPUT FILE.\n";
        return 1;
    }
    
    string filename = argv[1];
    ifstream fin(filename.c_str());
    
    if (! fin) {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        return 1;
    }
    
    if (fin.peek() == EOF) {
        cout << "Empty File.\n";
        return 1;
    }
    
    bool argall = false;
    bool argint = false;
    bool argreal = false;
    bool argstr = false;
    bool argid = false;
    bool argkw = false;

    for (int i = 2; i < argc; i++) {
        string arg = argv[i]; 
        if (arg == "-all") {
            argall = true;
        } else if (arg == "-int") {
            argint = true;
        } else if (arg == "-real") {
            argreal = true;
        } else if (arg == "-str") {
            argstr = true;
        } else if (arg == "-id") {
            argid = true;
        } else if (arg == "-kw") {
            argkw = true;
        } else {
            if (argv[i][0] == '-') {
                cout << "UNRECOGNIZED FLAG {" << argv[i] << "}\n";
                return 1;
            } else {
                cout << "ONLY ONE FILE NAME IS ALLOWED.\n";
                return 1;
            }
        }
    }
    
    int linenum = 0;
    int tokenCount = 0;
    set<int> allInt;
    set<string> allStr;
    set<double> allReal;
    map<string, int> allid;
    set<Token> reservedWords = {PROGRAM, END, ELSE, IF, THEN, INTEGER, REAL, CHARACTER, PRINT, LEN};
    map<string, int> allkw;
    LexItem item = getNextToken(fin, linenum);
    
    while (item.GetToken() != ERR && item.GetToken() != DONE) {
        tokenCount++;
        if (argall) {
            cout << item << endl;
        } 
        if (item.GetToken() == ICONST) {
            allInt.insert(stoi(item.GetLexeme()));
        }
        if (item.GetToken() == RCONST) {
            allReal.insert(stod(item.GetLexeme()));
        }
        if (item.GetToken() == SCONST) {
            allStr.insert(item.GetLexeme());
        } 
        if (item.GetToken() == IDENT) {
            if (allid.find(item.GetLexeme()) == allid.end()) {
                allid[item.GetLexeme()] = 1;
            } else {
                allid[item.GetLexeme()]++;
            }
        }
        if (reservedWords.find(item.GetToken()) != reservedWords.end()) {
            allkw[item.GetLexeme()]++;
        }
        item = getNextToken(fin, linenum);
    }
    if (item.GetToken() == ERR) {
        cout << item << endl;
        return 1;
    }
    
    cout << endl << "Lines: " << item.GetLinenum() << endl << "Total Tokens: " << tokenCount << endl << "Identifiers: " << allid.size() << endl << "Integers: " << allInt.size() << endl << "Reals: " << allReal.size() << endl << "Strings: " << allStr.size() << endl;
    
    if (argid) {
        cout << "IDENTIFIERS:\n";
        map<string, int>::iterator idit;
        for (idit = allid.begin(); idit != allid.end(); idit++) {
            if (next(idit)  == allid.end()) {
                cout << idit->first << " (" << idit->second << ")";
            } else {
                cout << idit->first << " (" << idit->second << "), ";
            }
        }
        cout << endl;
    }
    if (argkw) {
        cout << "KEYWORDS:\n";
        map<string, int>::iterator kwit;
        for (kwit = allkw.begin(); kwit != allkw.end(); kwit++) {
            if (next(kwit) == allkw.end()) {
                cout << kwit->first << " (" << kwit->second << ")";
            } else {
                cout << kwit->first << " (" << kwit->second << "), ";
            }
        }
        cout << endl;
    }
    if (argint) {
        cout << "INTEGERS:\n";
        for (auto i = allInt.begin(); i != allInt.end(); ++i) {
            if (next(i) == allInt.end()) {
                cout << *i;
            } else {
                cout << *i << ", ";
            }
        }
        cout << endl;
    }
    if (argreal) {
        cout << "REALS:\n";
        for (auto r = allReal.begin(); r != allReal.end(); ++r) {
            if (next(r) == allReal.end()) {
                cout << *r;
            } else {
                cout << *r << ", ";
            }
        }
        cout << endl;
    }
    if (argstr) {
        cout << "STRINGS:\n";
        for (auto s = allStr.begin(); s != allStr.end(); ++s) {
            if (next(s) == allStr.end()) {
                cout << "\"" << *s << "\"";
            } else {
                cout << "\"" << *s << "\", ";
            }
        }
        cout << endl;
    }

    return 0;
}