#ifndef _X_STRING_HPP
#define _X_STRING_HPP

#include <string>
#include <list>
#include "XGlobal.h"
#include "XNamespace.h"
#include "XChar.hpp"
#include "XUnitTest.hpp"

class XString : public std::wstring{
private:
	struct ArgEscapeData
	{
		int minEscape;		// lowest escape sequence number
		int occurrences;	// number of occurrences of the lowest escape sequence number;
		int escapeLength;	// total length of escapesequence which will be replaced
	};
public:
	XString():std::wstring(){}
	XString(const std::wstring& wstr) :std::wstring(wstr){}
	XString(const std::string& str) :std::wstring(XString().fromStdString(str)){}
	XString(const char* str) :std::wstring(XString().fromStdString(str)){}
	XString(const char* ch, int size) :std::wstring(XString().fromStdString(std::string(ch, size))){}
	XString(const wchar_t* wstr) :std::wstring(wstr){}
	XString(const wchar_t* wstr, size_t size) :std::wstring(wstr, size){}
	XString(char c){ fill(c, 1); }
	XString(wchar_t c){ fill(c, 1); }
	XString(int size, Xd::Initialization) :std::wstring() { resize(size);}

	XString& append(const XString& str){
		*this = *this + str;
		return (*this);
	}
	XString& append(wchar_t ch)
	{
		std::wstring::append(1, ch);
		return (*this);
	}
	XString& prepend(const XString& str){
		*this = str + *this;
		return (*this);
	}

	XString left(size_t n)const{ return substr(0, n); }
	XString right(size_t n)const{
		size_t len = length();
		int startPos = (len > n) ? (int)len - (int)n : 0;
		return substr(startPos, len);
	}

	XString& toUpper(){
		int offset = L'a' - L'A';
		size_t len = length();
		for (size_t i = 0; i < len; i++){
			wchar_t* wch = &(*this)[i];
			if (L'a' <= *wch && *wch <= L'z'){
				*wch -= (wchar_t)offset;
			}
		}
		return (*this);
	}
	XString& toLower(){
		int offset = L'a' - L'A';
		size_t len = length();
		for (size_t i = 0; i < len; i++){
			wchar_t* wch = &(*this)[i];
			if (L'A' <= *wch && *wch <= L'Z'){
				*wch += (wchar_t)offset;
			}
		}
		return (*this);
	}
	XString upper()const{ return XString(*this).toUpper(); }
	XString lower()const{ return XString(*this).toLower(); }

	int64_t toLong(int base = 10, bool* ok = nullptr)const{ return str_to_int(upper().toStdString(), base, ok); }
	int toInt(int base = 10, bool* ok = nullptr)const{ return (int)str_to_int(upper().toStdString(), base, ok); }
	//double toDouble(bool* ok = nullptr)const
	//{
	//	int dotpos = indexOf(".");
	//	if (dotpos == -1)
	//		return toInt(10,ok);

	//	int decimalLength = (int)(length() - dotpos - 1);
	//	int integer = left(dotpos).toInt();
	//	double decimal = (double)right(decimalLength).toInt() / pow(10, decimalLength);
	//	return integer + decimal;
	//}
	double toDouble(bool* ok = nullptr)const
	{
		XString str = this->trimmed();
		if (str.startWith("-"))
			return str.mid(1).toDouble(ok)*(-1);

		int dotpos = indexOf(".");
		if (dotpos == -1)
			return toInt(10, ok);

		int decimalLength = (int)(length() - dotpos - 1);
		int integer = left(dotpos).toInt();
		double decimal = (double)right(decimalLength).toInt() / pow(10, decimalLength);
		return integer + decimal;
	}

	XString& fromInt(int64_t num, int base = 10){
		*this = int_to_str(num, base);
		return *this;
	}

	bool toBool()const{
		bool ok = false;
		if (*this == XString("true"))
			return true;
		if (*this == XString("false"))
			return false;

		int n = toInt(10, &ok);
		if (ok){
			return (n != 0);
		}
		return false;
	}
	static XString fromBool(bool b){ return  b ? L"true" : L"false"; }

	XString& fromStdString(const std::string& str){
		const std::locale loc = std::locale("");

		const char* dataFromBegin = str.c_str();
		const char* dataFromEnd = dataFromBegin + str.size();
		const char* dataFromNext = 0;

		wchar_t* dataToBegin = new wchar_t[str.size() + 1];
		wchar_t* dataToEnd = dataToBegin + str.size() + 1;
		wchar_t* dataToNext = 0;

		wmemset(dataToBegin, 0, str.size() + 1);

		typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
		mbstate_t inState;
		memset(&inState, 0, sizeof(inState));

		auto result = std::use_facet<convert_facet>(loc).in(
			inState, dataFromBegin, dataFromEnd, dataFromNext,
			dataToBegin, dataToEnd, dataToNext);
		if (result == convert_facet::ok)
		{
			*this = dataToBegin;
		}
		delete[] dataToBegin;
		return *this;
	}
	std::wstring toStdWString()const{
		std::wstring retval = *this;
		return retval;
	}
    std::string toStdString()const{
        const std::locale loc = std::locale("");

        const wchar_t* dataFromBegin = c_str();
        const wchar_t* dataFromEnd = c_str() + size();
        const wchar_t* dataFromNext = 0;

        char* dataToBegin = new char[(size() + 1) * sizeof(wchar_t)];
        char* dataToEnd = dataToBegin + (size() + 1) * sizeof(wchar_t);
        char* dataToNext = 0;

        memset(dataToBegin, 0, (size() + 1) * sizeof(wchar_t));

        typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
        mbstate_t out_state;
		memset(&out_state, 0, sizeof(out_state));

        auto result = std::use_facet<convert_facet>(loc).out(
            out_state, dataFromBegin, dataFromEnd, dataFromNext,
            dataToBegin, dataToEnd, dataToNext);
        if (result == convert_facet::ok)
        {
            std::string dst = dataToBegin;
            delete[] dataToBegin;
            return dst;
        }
        else
        {
            delete[] dataToBegin;
            return std::string("");
        }
    }

	bool isEmpty()const{ return std::wstring::empty(); }
	XString mid(int pos, int len=-1)const{ return std::wstring::substr(pos, len); }

    int indexOf(const XString& str, int from = 0)const{
        size_t len = str.length();
        int endPos = (int)length() - (int)len;
        for (int i = from; i <= endPos; i++){
            if (substr(i, len) == str)
                return i;
        }
        return -1;
    }

    bool find(const XString& str)const{ return indexOf(str) >= 0; }

    std::list<int> indexOfAll(const XString& str)const{
        std::list<int> retval;
        size_t len = str.length();
        int endPos = (int)length() - (int)len;
        for (int i = 0; i <= endPos; i++){
            if (substr(i, len) == str){
                retval.push_back(i);
            }
        }
        return retval;
    }
    std::list<XString> split(const XString& str)const{
        std::list<XString> retval;
		if (this->isEmpty())
			return retval;

        std::list<int> poss = indexOfAll(str);
        size_t count = poss.size();
        if (count > 0){
            retval.push_back(substr(0, poss.front()));
        }
        else{
            retval.push_back(*this);
            return retval;
        }
        size_t splitStrLen = str.length();
		int prepos = -1;
		for (int pos : poss)
		{
			if (prepos != -1)
			{
				int len = pos - prepos - splitStrLen;
				retval.push_back(this->substr(prepos + splitStrLen + len));
			}
			prepos = pos;
		}
        retval.push_back(substr(poss.back() + splitStrLen));
        
        return retval;
    }
    XString& replace(const XString& before, const XString& after){

        std::list<XString> strs = split(before);
        *this = joinStringList(strs, after);
        return (*this);
    }
    XString& fill(const XChar& ch,const int size = -1){
        if (size > 0) 
            this->resize(size);
        for (auto& var : *this){
            var = ch.data();
        }
        return *this;
    }
    XString& leftJustified(int width, const XChar& fill = XChar(L' ')){
        int padlen = (int)(width - length());
        if (padlen > 0){
            XString padstr;
            padstr.fill(fill, padlen);
            this->prepend(padstr);
        }
        return *this;
    }
    XString section(const XString& sep, int start, int end = -1) const{
        assert(start >= 0);

        std::list<XString> strs = split(sep);
        size_t count = strs.size();
        if (end < 0)
            end = (int)count - 1;

        std::list<XString> dst;
		int i = 0;
		for (const XString& str : strs)
		{
			if (start <= i && i <= end)
			{
				dst.push_back(str);
			}
			++i;
		}
        return joinStringList(dst, sep);
    }

    bool startWith(const XString& str)const{ return left(str.length()) == str; }
    bool endWith(const XString& str)const{ return right(str.length()) == str; }

    XString trimmed()const{
        size_t count = length();
        const wchar_t* s = c_str();
        const wchar_t* begin = s;
        const wchar_t* end = s + count - 1;

        while (XChar::isSpace(*begin) && (count)>0){
            ++begin; --count;
        }
        while (XChar::isSpace(*end) && (count) > 0){
            --end; --count;
        }

        if (begin <= end){
            return std::wstring(begin, end - begin + 1);
        }
        else{
            return XString();
        }
    }

	XString arg(int n){ return arg(XString::number(n)); }
	XString arg(int64_t n){ return arg(XString::number(n)); }
	XString arg(double d){ return arg(XString::number(d)); }
	XString arg(char c){ return arg(XString(c)); }
	XString arg(uint n){ return arg(XString::number((int64_t)n)); }
	XString arg(const XString& a)
	{
		ArgEscapeData d = findArgEscapes(*this);
		return replaceArgEscapes(*this, d, a); 
	}

    static XString number(double d){
        char buf[64];
        sprintf_s(buf, "%lg", d);
        return XString(buf);
    }
	
	static XString number(int n, int base = 10){ return XString().fromInt(n, base); }
	static XString number(uint n, int base = 10){ return XString().fromInt(n, base); }
    static XString number(int64_t n,int base=10){ return XString().fromInt(n,base); }

    static XString joinStringList(std::list<XString>& strs, const XString& sep)
	{
		if (strs.size() == 0)
			return XString();

		if (strs.size() == 1)
			return strs.front();

		int len = (int)(sep.length()*(strs.size() - 1));
		for (const auto& var : strs)
		{
			len += (int)var.length();
		}

		XString retval(len,Xd::Uninitialized);
		wchar_t* c = &retval.at(0);

		for (const XString& str : strs)
		{
			memcpy(c, str.data(), str.length() * sizeof(wchar_t));
			c += str.length();

			memcpy(c, sep.data(), sep.length() * sizeof(wchar_t));
			c += sep.length();
		}

		const XString& str = strs.back();
		memcpy(c, str.data(), str.length()*sizeof(wchar_t));
		c += str.length();

		return retval; 
    }
	

private:
	
	static int64_t str_to_int(const std::string& str, int base = 10, bool* ok = nullptr){
        if (base < 2 || base>16){
            assert(false);
            base = 10;
        }
		const char* s = str.c_str();
        char c = *s++;
        bool neg = false;
        int64_t num = 0;

        if (c == '-'){
            neg = true;
            c = *s++;
        }
        else if (c == '+'){
            c = *s++;
        }

        if (ok)
            *ok = true;

        while (c != '\0'){
            int k = 0;
            if ('0' <= c&&c <= '9')
                k = c - '0';
            else if ('A' <= c&&c <= 'F' && base>10)
                k = 10 + c - 'A';
            else{
                if (ok)
                    *ok = false;
                break;
            }

            num = num*base + k;
            c = *s++;
        }

        if (neg)
            num = -num;
        return num;
    }
    static XString uint_to_str(uint64_t num, int base = 10){
        if (base < 2 || base>16){
            assert(false);
            base = 10;
        }

        wchar_t buf[65];	// length of 2^64 in base 2 = 64
        int idx = 64;
        buf[idx--] = 0;

        while (num)
        {
            buf[idx--] = num_alpha(num%base);
            num /= base;
        }

        if (idx == 63)
            return L"0";

        return XString(&buf[idx + 1]);
    }
    static XString int_to_str(int64_t num, int base = 10){
        if (num >= 0)
            return uint_to_str(num, base);
        else
			return uint_to_str(-num, base).prepend("-");
    }

	static ArgEscapeData findArgEscapes(const XString& s)
	{
		const wchar_t* cBegin = &s.at(0);
		const wchar_t* cEnd = cBegin + s.length();

		ArgEscapeData d;
		d.minEscape = INT_MAX;
		d.occurrences = 0;
		d.escapeLength = 0;

		const wchar_t* c = cBegin;
		while (c != cEnd)
		{
			while (c != cEnd && *c != L'%')
				++c;

			if (c == cEnd)
				break;

			const wchar_t* escapeStart = c;
			if (++c == cEnd)
				break;

			int escape = XChar::digitValue(*c);
			if (escape == -1)
				continue;

			++c;

			if (c != cEnd)
			{
				int nextEscape = XChar::digitValue(*c);
				if (nextEscape != -1)
				{
					escape = (10 * escape) + nextEscape;
					++c;
				}
			}

			if (escape > d.minEscape)
				continue;

			if (escape < d.minEscape)
			{
				d.minEscape = escape;
				d.occurrences = 0;
				d.escapeLength = 0;
			}

			++d.occurrences;
			d.escapeLength += (int)(c - escapeStart);
		}

		return d;
	}

	static XString replaceArgEscapes(const XString& s, const ArgEscapeData& d,const XString& arg)
	{
		const wchar_t* cBegin = &s.at(0);
		const wchar_t* cEnd = cBegin + s.length();

		int resultLength = (int)(s.length() - d.escapeLength + d.occurrences*arg.length());
		XString result(resultLength, Xd::Uninitialized);

		wchar_t* rc = &result.at(0);
		const wchar_t* c = cBegin;
		int replCnt = 0;
		while (c != cEnd)
		{
			const wchar_t* textStart = c;
			while (*c != L'%')
				++c;
			const wchar_t* escapeStart = c++;


			int escape = XChar::digitValue(*c);
			if (escape != -1)
			{
				if (c + 1 != cEnd)
				{
					int nextEscape = XChar::digitValue(*(c + 1));
					if (nextEscape != -1)
					{
						escape = (10 * escape) + nextEscape;
						++c;
					}
				}
			}

			if (escape != d.minEscape)
			{
				memcpy(rc, textStart, (c - textStart)*sizeof(wchar_t));
				rc += (c - textStart);
			}
			else
			{
				++c;

				memcpy(rc, textStart, (escapeStart - textStart)*sizeof(wchar_t));
				rc += (escapeStart - textStart);
				memcpy(rc, arg.data(), arg.length()*sizeof(wchar_t));
				rc += arg.length();

				if (++replCnt == d.occurrences)
				{
					memcpy(rc, c, (cEnd - c)*sizeof(wchar_t));
					rc += (cEnd - c);
					c = cEnd;
				}
			}

			
		}

		return result;
	}

private:
    static wchar_t num_alpha(int idx){
        assert(0 <= idx&& idx < 16);
        static const wchar_t _num_alpha[16] = { 
			L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', 
			L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
        return _num_alpha[idx];
    }
};

class XStringList :public std::list<XString>{
public:
    XStringList(const std::list<XString>& other = std::list<XString>()) :std::list<XString>(other){}
    XString join(const XString& sep)const{
        std::list<XString> strs = *this;
        return XString::joinStringList(strs, sep);
    }
};

#define X_STR_TEST(op,rs)   \
    op; \
    if(!XUnitTest::test(rs,L#op))  \
        std::wcout<<"\t"<<str<<" != "<<rs<<"\n";

//class XStringTest :public XUnitTest{
//protected:
//    void test(){
//        XString str("Hello");
//        X_STR_TEST(str.toUpper(),L"HELLO");
//        X_STR_TEST(str.toUpper(), L"HELLO");
//        X_STR_TEST(str.toLower(), L"hello");
//
//        str = L"#a#b#c#";
//        X_STR_TEST(str.replace(L"#",L"@"), L"@a@b@c@");
//        str = L"#";
//        X_STR_TEST(str.replace(L"#", L"123"), L"123");
//    }
//};


namespace std
{
	template<>
	struct hash<XString>:public hash<std::wstring>
	{  };
}

#endif //_X_STRING_HPP
