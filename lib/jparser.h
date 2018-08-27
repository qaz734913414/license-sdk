/*************************************************************************
    > File Name: jparser.h
    > Author: yushaohua
    > Mail: llint@qq.com 
    > Created Time: Fri 15 Dec 2017 07:35:32 PM PST
 ************************************************************************/

#ifndef JPARSER_H__
#define JPARSER_H__

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

namespace tutils {
template<typename T>
T strToNum(const string& str)
{
    T t;
    istringstream iss(str);
    iss >> t;
    return t;
}

inline bool isWhiteSpace(char c)
{
    if(c == ' ' || c == '\t' || c == '\r' || c == '\n') return true;
    return false;
}

inline bool isDigit(char c)
{
    if((c>='0' && c<='9') || c == 'e' || c == 'E' 
            || c == '.' || c == '-')
        return true;
    return false;
}

inline bool isEscape(char c)
{
    return c == '"' || c == '\\' || c == 'u' || c == 'r' 
        || c == 'n' || c == 'b' || c == 't' || c == 'f';
}

inline bool isHex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' || c <= 'Z');
}

inline bool isValidNum(const string& s)
{
    string temp = s;
    if(temp.empty()) return false;

    if(temp[0] == '-')
    {
        temp = temp.substr(1);
        if(temp.empty()) return false;
    }

    if(temp[0] == '0')
    {
        temp = temp.substr(1);
    }
    else if(temp[0] >= '1' && temp[0] <= '9')
    {
        temp = temp.substr(1);
        while(!temp.empty() && temp[0] >= '0' && temp[0] <= '9')
        {
            temp = temp.substr(1);
        }
    }
    else
    {
        return false;
    }

    if(temp.length() >= 2 && temp[0] == '.' && temp[1] >= 0 && temp[1] <= '9')
    {
        temp = temp.substr(2);
        while(!temp.empty() && temp[0] >= '0' && temp[0] <= '9')
        {
            temp = temp.substr(1);
        }
    }

    if(temp.length() >= 2 && (temp[0] == 'e' || temp[0] == 'E'))
    {
        temp = temp.substr(1);
        if(temp[0] == '+' || temp[0] == '-')
        {
            temp = temp.substr(1);
            if(temp.empty()) return false;
        }
        while(!temp.empty() && temp[0] >= '0' && temp[0] <= '9')
        {
            temp = temp.substr(1);
        }
    }

    return temp.empty();
}

enum JParserRet
{
    JPARSER_OK = 0,
    JPARSER_ERROR,
};


class Token
{
public:
    enum TokenType
    {
        TT_ARRAY_START = 1,
        TT_ARRAY_END = 2,
        TT_OBJECT_START = 4,
        TT_OBJECT_END = 8,
        TT_NUM = 16,
        TT_STRING = 32,
        TT_BOOLEN = 64,
        TT_NULL = 128,
        TT_COMMA = 256,
        TT_COLON = 512,
        TT_DOC_END = 1024,
    };

    union Value
    {
        double num;
        char* str;
        bool b;
    };

    Token(TokenType tt) : type(tt) {}
    Token(TokenType tt, char* s) : type(tt) { val.str = s; }
    Token(TokenType tt, double d) : type(tt) { val.num = d; }
    Token(TokenType tt, bool b) : type(tt) { val.b = b; }

    ~Token() 
    {
        if(type == TT_STRING) free(val.str);
    }

    friend ostream& operator<<(ostream& os, const Token& t)
    {
        switch(t.type)
        {
            case TT_ARRAY_START:
                os << "[";
                break;
            case TT_ARRAY_END:
                os << "]";
                break;
            case TT_OBJECT_START:
                os << "{";
                break;
            case TT_OBJECT_END:
                os << "}";
                break;
            case TT_NUM:
                os << t.val.num;
                break;
            case TT_STRING:
                os << t.val.str;
                break;
            case TT_BOOLEN:
                if(t.val.b) os << "true";
                else os << "false";
                break;
            case TT_NULL:
                os << "null";
                break;
            case TT_COMMA:
                os << ",";
                break;
            case TT_COLON:
                os << ":";
                break;
            case TT_DOC_END:
                os << "------end-------";
                break;
            default:
                break;
        }
        return os;
    }

    TokenType getType() { return type; }
    Value getVal() { return val; }

private:
    TokenType type;
    Value val;

};

class Scanner
{
public:
    Scanner(const char* s, size_t n) : buff(s), pos(0), len(n) {}
    char next() 
    {
        if(pos >= len) return -1;
        return buff[pos++];
    }
    void backone()
    {
        if(pos>0) pos--;
    }
    bool isEnd() { return pos == len; }
private:
    const char* buff;
    size_t pos;
    size_t len;
};

class Tokenizer
{
public:
    Tokenizer(Scanner* scanner) : sc(scanner) {}

	~Tokenizer()
	{
		for(size_t i=0; i<tokens.size(); ++i)
		{
			delete tokens[i];
		}
	}

    JParserRet tokenzie()
    {
        Token* tok;
        do
        {
            tok = getToken();
            if(!tok) return JPARSER_ERROR;
            tokens.push_back(tok);
        } while(tok->getType() != Token::TT_DOC_END);
        return JPARSER_OK;
    }

    vector<Token*>& getTokens()
    {
        return tokens;
    }

    void debug()
    {
        for(size_t i=0; i<tokens.size(); ++i)
        {
            cout << "token: " << *tokens[i] << endl;
        }
    }

private:
    Token* getToken()
    {
        char c;
        while(1)
        {
            if(sc->isEnd())
            {
                return new Token(Token::TT_DOC_END);
            }
            c = sc->next();
            if(!isWhiteSpace(c))
            {
                break;
            }
        }
        switch(c)
        {
            case ',': return new Token(Token::TT_COMMA);
            case ':': return new Token(Token::TT_COLON);
            case '{': return new Token(Token::TT_OBJECT_START);
            case '}': return new Token(Token::TT_OBJECT_END);
            case '[': return new Token(Token::TT_ARRAY_START);
            case ']': return new Token(Token::TT_ARRAY_END);
            case 'n': return getNullToken();
            case 't': return getTrueToken();
            case 'f': return getFalseToken();
            case '"': return getString();
            case '-': return getNum(c);
        }
        if(c>='0' && c<='9') return getNum(c);
        return NULL;
    }

    Token* getNullToken()
    {
        if(sc->next() == 'u' && sc->next() == 'l' && sc->next() == 'l')
        {
            return new Token(Token::TT_NULL);
        }
        return NULL;
    }
    Token* getTrueToken()
    {
        if(sc->next() == 'r' && sc->next() == 'u' && sc->next() == 'e')
        {
            return new Token(Token::TT_BOOLEN, true);
        }
        return NULL;
    }
    Token* getFalseToken()
    {
        if(sc->next() == 'a' && sc->next() == 'l' && sc->next() == 's' && sc->next() == 'e')
        {
            return new Token(Token::TT_BOOLEN, false);
        }
        return NULL;
    }
    Token* getString()
    {
        string s;
        char c;
        do
        {
            c = sc->next();
            if(c == '\\')
            {
                c = sc->next();
                if(!isEscape(c)) return NULL;
                s.append(1, '\\');
                s.append(1, c);
                if(c == 'u')
                {
                    for(size_t i=0; i<4; ++i)
                    {
                        c = sc->next();
                        if(isHex(c)) s.append(1, c);
                        else return NULL;
                    }
                }
            }
            else if(c == '\r' || c == '\n')
            {
                return NULL;
            }
            else
            {
                s.append(1, c);
            }
        } while(c != '"');
        s = s.substr(0, s.length()-1);
        char* p = (char*)malloc(s.length());
        strcpy(p, s.c_str());
        return new Token(Token::TT_STRING, p);
    }
    Token* getNum(char ic)
    {
        string s;
        char c = ic;
        while(isDigit(c))
        {
            s.append(1, c);
            c = sc->next();
        }
        sc->backone();
        if(!isValidNum(s)) return NULL;
        return new Token(Token::TT_NUM, strToNum<double>(s));
    }

    vector<Token*> tokens;
    Scanner* sc;

    map<Token::TokenType, string> m;
};

class JValue
{
public:
    enum JValueType
    {
        JVT_OBJECT,
        JVT_ARRAY,
        JVT_NUM,
        JVT_STRING,
        JVT_BOOLEN,
        JVT_NULL,
    };

	JValue() { setHeapFlag(); }
    JValue(JValueType t) : jvt(t) { setHeapFlag(); }
    JValue(double n) : jvt(JVT_NUM), num(n) {setHeapFlag(); }
    JValue(int n) : jvt(JVT_NUM), num(n) {setHeapFlag(); }
    JValue(string s) : jvt(JVT_STRING), str(s) { setHeapFlag(); }
    JValue(char* s) : jvt(JVT_STRING), str(s) { setHeapFlag(); }
    JValue(bool b) : jvt(JVT_BOOLEN), b(b) { setHeapFlag(); }

	~JValue()
	{
		switch(jvt)
		{
			case JVT_OBJECT:
				for(map<string, JValue*>::iterator it=obj.begin(); it!=obj.end(); ++it)
				{
					if(it->second)// && it->second->isOnHeap()) 
                    {
                        delete it->second;
                        it->second = NULL;
                    }
				}
				break;
			case JVT_ARRAY:
				for(size_t i=0; i<arr.size(); ++i)
				{
					if(arr[i]) //&& arr[i]->isOnHeap()) 
                    {
                        delete arr[i];
                        arr[i] = NULL;
                    }
				}
				break;
			default:
				break;
		}
	}


	static void* operator new(size_t size)
	{
		//heapFlag = true;
		return ::operator new(size);
	}

    /*
	static void* operator new[](size_t size) throw(bad_alloc)
	{
		heapFlag = true;
		return ::operator new[](size);
	}
    */

    void addObject(const string& key, JValue* val) { obj[key] = val; }
    void addArray(JValue* val) { arr.push_back(val); }
	void setType(JValueType t) { jvt = t; }
    JValueType getType() { return jvt; }
    map<string, JValue*>& getObject() { return obj; }
    vector<JValue*> getArray() { return arr; }
    double getNum() { return num; }
    string& getStr() { return str; }
    bool getBoolen() { return b; }

    friend ostream& operator<<(ostream& os, const JValue& jv)
    {
        switch(jv.jvt)
        {
            case JVT_OBJECT:
                {
                os << "{";
                for(map<string, JValue*>::const_iterator it=jv.obj.begin(); ;)
                {
                    os << "\"" << it->first << "\"" << ":" << *(it->second);
                    if(++it == jv.obj.end()) break;
                    else os << ",";
                }
                os << "}";
                break;
                }
            case JVT_ARRAY:
                os << "[";
                for(size_t i=0; i<jv.arr.size(); ++i)
                {
                    os << *(jv.arr[i]);
                    if(i < jv.arr.size()-1) os << ",";
                }
                os << "]";
                break;
            case JVT_NUM:
                os << jv.num;
                break;
            case JVT_STRING:
                os << "\"" << jv.str << "\"";
                break;
            case JVT_BOOLEN:
                if(jv.b) os << "\"true\"";
                else os << "\"false\"";
                break;
            case JVT_NULL:
                os << "null";
                break;
            default:
                break;
        }
        return os;
    }

	//static bool heapFlag;

private:
	bool onHeap;

    JValueType jvt;
    map<string, JValue*> obj;
    vector<JValue*> arr;
    double num;
    string str;
    bool b;

	bool isOnHeap() { return onHeap; }
	void setHeapFlag()
	{
	/*
		onHeap = heapFlag;
		heapFlag = false;
		*/
	}

};

class Parser
{
public:
    enum ParserStatus
    {
        ST_OBJ_KEY,
        ST_OBJ_VALUE,
    };

    Parser(const vector<Token*>& ts, JValue* v) 
        : tokens(ts)
		, valOut(v)
        , pos(0)
        , len(ts.size())
        , status(ST_OBJ_KEY)
        , expectToken(0)
    {}

    JParserRet parse()
    {
        if(len == 0) return JPARSER_ERROR;
        Token::TokenType type = tokens[pos]->getType();
        switch(type)
        {
            case Token::TT_OBJECT_START:
                expectToken = Token::TT_STRING | Token::TT_OBJECT_END;
                status = ST_OBJ_KEY;
                valOut->setType(JValue::JVT_OBJECT);
                return parseObject(valOut);
            case Token::TT_ARRAY_START:
                expectToken = Token::TT_OBJECT_START | Token::TT_ARRAY_END 
                    | Token::TT_NUM | Token::TT_STRING;
                valOut->setType(JValue::JVT_ARRAY);
                return parseArray(valOut);
            default:
                return JPARSER_ERROR;
        }
    }

    void debug()
    {
        cout << *valOut << endl;
    }

private:
    bool checkToken(Token::TokenType type)
    {
        if(type & expectToken) return true;
        return false;
    }

    JParserRet parseObject(JValue* valRet)
    {
        pos++;
        string key;
        JValue* val;
        JParserRet ret;
        while(1)
        {
            if(pos >= len) return JPARSER_ERROR;

            Token* token = tokens[pos++];
            Token::TokenType type = token->getType();
            bool b = checkToken(type);
            if(!b) return JPARSER_ERROR;
            switch(type)
            {
                case Token::TT_STRING:
                {
                    switch(status)
                    {
                        case ST_OBJ_KEY:
                            key = token->getVal().str;
                            expectToken = Token::TT_COLON;
                            break;
                        case ST_OBJ_VALUE:
                            val = new JValue(token->getVal().str);
                            valRet->addObject(key, val);
                            expectToken = Token::TT_COMMA | Token::TT_OBJECT_END;
                            break;
                        default:
                            return JPARSER_ERROR;
                    }
                    break;
                }
                case Token::TT_OBJECT_START:
                    val = new JValue(JValue::JVT_OBJECT);
                    expectToken = Token::TT_OBJECT_END | Token::TT_STRING;
					pos--;
					status = ST_OBJ_KEY;
                    ret = parseObject(val);
                    if(ret == JPARSER_ERROR) return ret;
                    valRet->addObject(key, val);
                    expectToken = Token::TT_COMMA | Token::TT_OBJECT_END;
                    break;
                case Token::TT_ARRAY_START:
                    val = new JValue(JValue::JVT_ARRAY);
                    expectToken = Token::TT_ARRAY_END | Token::TT_STRING | Token::TT_NUM | Token::TT_OBJECT_START;
                    pos--;
                    ret = parseArray(val);
                    if(ret == JPARSER_ERROR) return ret;
                    valRet->addObject(key, val);
                    expectToken = Token::TT_COMMA | Token::TT_OBJECT_END;
                    break;
                case Token::TT_NUM:
                    val = new JValue(token->getVal().num);
                    valRet->addObject(key, val);
                    expectToken = Token::TT_OBJECT_END | Token::TT_COMMA;
                    break;
                case Token::TT_OBJECT_END:
                    return JPARSER_OK;
                case Token::TT_COMMA:
                    status = ST_OBJ_KEY;
                    expectToken = Token::TT_STRING;
                    break;
                case Token::TT_COLON:
                    status = ST_OBJ_VALUE;
                    expectToken = Token::TT_NUM | Token::TT_STRING | Token::TT_BOOLEN 
                        | Token::TT_NULL | Token::TT_ARRAY_START | Token::TT_OBJECT_START;
                    break;
                case Token::TT_BOOLEN:
                    val = new JValue(token->getVal().b);
                    valRet->addObject(key, val);
                    expectToken = Token::TT_OBJECT_END | Token::TT_COMMA;
                    break;
                case Token::TT_NULL:
                    val = new JValue(JValue::JVT_NULL);
                    valRet->addObject(key, val);
                    expectToken = Token::TT_OBJECT_END | Token::TT_COMMA;
                    break;
                default:
                    return JPARSER_ERROR;
            }
        }
    }

    JParserRet parseArray(JValue* valRet)
    {
        pos++;
        JValue* val;
        JParserRet ret;
        while(1)
        {
            if(pos >= len) return JPARSER_ERROR;

            Token* token = tokens[pos++];
            Token::TokenType type = token->getType();
            bool b = checkToken(type);
            if(!b) return JPARSER_ERROR;
            switch(type)
            {
                case Token::TT_STRING:
                    val = new JValue(token->getVal().str);
                    valRet->addArray(val);
                    expectToken = Token::TT_COMMA | Token::TT_ARRAY_END;
                    break;
                case Token::TT_NUM:
                    val = new JValue(token->getVal().num);
                    valRet->addArray(val);
                    expectToken = Token::TT_COMMA | Token::TT_ARRAY_END;
                    break;
                case Token::TT_OBJECT_START:
                    val = new JValue(JValue::JVT_OBJECT);
                    expectToken = Token::TT_OBJECT_END | Token::TT_STRING;
                    status = ST_OBJ_KEY;
                    pos--;
                    ret = parseObject(val);
                    if(ret == JPARSER_ERROR) return ret;
                    valRet->addArray(val);
                    expectToken = Token::TT_COMMA | Token::TT_ARRAY_END;
                    break;
                case Token::TT_ARRAY_START:
                    val = new JValue(JValue::JVT_ARRAY);
                    expectToken = Token::TT_ARRAY_END | Token::TT_STRING | Token::TT_NUM | Token::TT_BOOLEN | Token::TT_NULL;
                    pos--;
                    ret = parseArray(val);
                    if(ret == JPARSER_ERROR) return ret;
                    valRet->addArray(val);
                    expectToken = Token::TT_COMMA | Token::TT_ARRAY_END | Token::TT_OBJECT_END;
                    break;
                case Token::TT_ARRAY_END:
                    return JPARSER_OK;
                case Token::TT_BOOLEN:
                    val = new JValue(token->getVal().b);
                    valRet->addArray(val);
                    expectToken = Token::TT_OBJECT_END | Token::TT_COMMA;
                    break;
                case Token::TT_COMMA:
                    expectToken = Token::TT_NUM | Token::TT_STRING | Token::TT_OBJECT_START;
                    break;
                case Token::TT_NULL:
                    val = new JValue(JValue::JVT_NULL);
                    valRet->addArray(val);
                    expectToken = Token::TT_ARRAY_END | Token::TT_COMMA;
                    break;
                default:
                    return JPARSER_ERROR;
            }
        }
        return JPARSER_ERROR;
    }

private:
    vector<Token*> tokens;

    JValue* valOut;

    size_t pos;
    size_t len;

    ParserStatus status;
    uint32_t expectToken;
};

class JObject
{
public:

    JObject() {}

private:
    map<std::string, JObject*> obj;
};

class JArray
{
public:

    JArray() {}

private:
};

inline JParserRet JParseDecode(const string& jsonStr, JValue* val)
{
    Scanner scanner(jsonStr.c_str(), jsonStr.length());
    Tokenizer tok(&scanner);
    JParserRet ret = tok.tokenzie();
    //tok.debug();
    //cout << "**********************************" << endl;
    Parser parser(tok.getTokens(), val);
    ret = parser.parse();
    //parser.debug();
    return ret;
}

inline JParserRet JParseEncode(JValue* val, string& outStr)
{
    ostringstream ostr;
    ostr << *val;
    outStr = ostr.str();
    return JPARSER_OK;
}
}

#endif
