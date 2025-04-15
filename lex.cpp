#include "lex.h"
#include<stack>

using namespace std;

// Map of all Tokens
map<Token, string> tokensToString {
    // Reserved Words
    {IF, "IF"}, {ELSE, "ELSE"}, {PRINT, "PRINT"}, {INTEGER, "INTEGER"}, {REAL, "REAL"}, {CHARACTER, "CHARACTER"}, {END, "END"}, {THEN, "THEN"}, {PROGRAM, "PROGRAM"}, {LEN, "LEN"}, 
    // Identifier
    {IDENT, "IDENT"},
    // Integer, Real, String, Boolean Constant 
    {ICONST, "ICONST"}, {RCONST, "RCONST"}, {SCONST, "SCONST"}, {BCONST, "BCONST"},
    // Operators 
    {PLUS, "PLUS"}, {MINUS, "MINUS"}, {MULT, "MULT"}, {DIV, "DIV"}, {ASSOP, "ASSOP"}, {EQ, "EQ"}, {POW, "POW"}, {GTHAN, "GTHAN"}, {LTHAN, "LTHAN"}, {CAT, "CAT"},
    // Delimiters
    {COMMA, "COMMA"}, {LPAREN, "LPAREN"}, {RPAREN, "RPAREN"}, {DOT, "DOT"}, {DCOLON, "DCOLON"}, {DEF, "DEF"}, 
    // Error
    {ERR, "ERR"},
    // EOF Token
    {DONE, "DONE"}
};

// Possible Lexeme States
enum lexState {
    startState, identState, stringState, intState, realState, comState
};

// Map of Reserved Words for id_or_kw
map<string, Token> reservedWords {
    {"PROGRAM", PROGRAM}, {"END", END}, {"ELSE", ELSE}, {"IF", IF}, {"INTEGER", INTEGER}, {"REAL", REAL}, {"CHARACTER", CHARACTER}, {"THEN", THEN}, {"PRINT", PRINT}, {"LEN", LEN}
};

// Determine if a Lexeme is part of the reserved words list and return the Lexeme
LexItem id_or_kw(const string& lexeme, int linenum) {
    string upper = "";
    for (char ch : lexeme) {
        upper += toupper(ch);
    }
    Token tok; 
    map<string, Token>::iterator reservedWordSearch = reservedWords.find(upper);
    if (reservedWordSearch == reservedWords.end()) {
        tok = IDENT; 
    } else {
        tok = reservedWords[upper];
    }
    return LexItem(tok, lexeme, linenum);
}

// Operator function for Tokens
ostream& operator<<(ostream& out, const LexItem& tok) {
    for (int i = IF; i <= LEN; i++) {
        if (tok.GetToken() == static_cast<Token>(i)) {
            return out << tokensToString[tok.GetToken()];
        }
    }
    for (int i = PLUS; i <= CAT; i++) {
        if (tok.GetToken() == static_cast<Token>(i)) {
            return out << tokensToString[tok.GetToken()];
        }
    }
    for (int i = COMMA; i <= DEF; i++) {
        if (tok.GetToken() == static_cast<Token>(i)) {
            return out << tokensToString[tok.GetToken()];
        }
    }
    switch (tok.GetToken()) {
        case IDENT: 
            return out << "IDENT: \'" << tok.GetLexeme() << "\'";
            break;
        case ICONST: 
            return out << "ICONST: (" << tok.GetLexeme() << ")";
            break;
        case RCONST:
            return out << "RCONST: (" << tok.GetLexeme() << ")";
            break;
        case SCONST:
            return out << "SCONST: \"" << tok.GetLexeme() << "\"";
            break;
        default: 
            return out << "Error in line " << (tok.GetLinenum() + 1) << ": Unrecognized Lexeme {" << tok.GetLexeme() << "}";
            break; 
    }
}

// Iterator through each element in the array and separate the lexemes and determine their type
LexItem getNextToken(istream& in, int& linenum) {
    stack<char> stringERR;
    lexState state = startState; 
    string lexeme;
    Token tok = ERR;
    char c;
    while (in.get(c)) {
        switch (state) {
            case startState: 
                if (c == '\n') {
                    linenum++;
                }
                if (isspace(c)) {
                    continue;
                }
                lexeme = c;
                if (isalpha(c) || c == '_') {
                    state = identState;
                } else if (isdigit(c)) {
                    state = intState;
                } else if (c == '!') {
                    state = comState;
                } else if (c == '\'' || c == '\"') {
                    stringERR.push(c);
                    lexeme = "";
                    state = stringState;
                } else if (c == '.') {
                    if (isdigit(in.peek())) {
                        state = realState;
                    } else {
                        return LexItem(DOT, lexeme, linenum);
                    } 
                } else {
                    tok = ERR;
                    switch (c) {
                        case '+':
                            tok = PLUS;
                            break;
                        case '-':
                            tok = MINUS;
                            break;
                        case '/':
                            if (in.peek() == '/') {
                                in.get(c);
                                lexeme += c;
                                tok = CAT;
                            } else {
                                tok = DIV;
                            }
                            break;
                        case '=': 
                            if (in.peek() == '=') {
                                in.get(c);
                                lexeme += c;
                                tok = EQ;
                            } else {
                                tok = ASSOP;
                            }
                            break;
                        case '<':
                            tok = LTHAN;
                            break;
                        case '>':
                            tok = GTHAN;
                            break;
                        case ',':
                            tok = COMMA;
                            break;
                        case ')': 
                            tok = RPAREN;
                            break;
                        case '(':
                            tok = LPAREN;
                            break;
                        case '*':
                            if (in.peek() == ',') {
                                lexeme += in.peek();
                                tok = DEF;
                            } else if (in.peek() == '*') {
                                in.get(c);
                                lexeme += c;
                                tok = POW;
                            } else {
                                tok = MULT;
                            }
                            break;
                        case ':': 
                            if (in.peek() == ':') {
                                in.get(c);
                                lexeme += c;
                                tok = DCOLON;
                            }
                            break;
                    }
                    return LexItem(tok, lexeme, linenum);
                }
                break;
            case identState:
                if (isalnum(c) || c == '_') {
                    lexeme += c;
                } else {
                    in.putback(c);
                    state = startState;
                    return id_or_kw(lexeme, linenum);
                }
                break;
            case intState:
                if (isdigit(c)) {
                    lexeme += c;
                } else if (c == '.') {
                    lexeme += c;
                    state = realState;
                }
                else {
                    in.putback(c);
                    state = startState;
                    return LexItem(ICONST, lexeme, linenum);
                }
                break;
            case realState:
                if (isdigit(c)) {
                    lexeme += c;
                } else if (c == '.') {
                    lexeme += c; 
                    return LexItem(ERR, lexeme, linenum);
                } else {
                    in.putback(c);
                    state = startState;
                    return LexItem(RCONST, lexeme, linenum);
                }
                break;
            case comState:
                if (c == '\n') {
                    linenum++;
                    state = startState;
                }
                break;
            case stringState:
                if (stringERR.top() == c) {
                    stringERR.pop();
                    state = startState;
                    return LexItem(SCONST, lexeme, linenum);
                } else if (c == '\n') {
                    lexeme = stringERR.top() + lexeme;
                    stringERR.pop();
                    return LexItem(ERR, lexeme, linenum);
                } 
                lexeme += c;
                break;
            }
        }
        if (in.eof()) {
            return LexItem(DONE, "", linenum);
        }
    return LexItem(ERR, lexeme, linenum);
}
