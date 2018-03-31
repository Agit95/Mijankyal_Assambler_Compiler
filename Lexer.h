#pragma once
#include "C_Compile_Error.h"
#include "Operators_and_bracket.h"
#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <list>



class C_Lexer
{
public:
	C_Lexer();
	virtual ~C_Lexer();

	void lexing_code_line();

private:
	std::string getPostfixNotation();
	int   getPrioritet(const char &) const  _NOEXCEPT;
	bool  bracket_validator(const std::string&) const _NOEXCEPT;
	bool  operators_validator(const std::string&) const _NOEXCEPT;
	bool  is_bracket(const char&) const _NOEXCEPT;
	bool  is_operand(const char&) const _NOEXCEPT;
	bool  is_operator(const char&)const _NOEXCEPT;
	void  setExpression(const std::string &);
	void  geting_code_line() _NOEXCEPT;
	void  infixToPostfix();
	inline void  set_parsed_code();

protected:
	std::queue<std::string>  m_oLexed_code;
	std::list<std::string>   m_oPostfixNotation;
	std::string				 m_oCode_line;
	std::string				 m_oExpression;
	std::string				 m_oVariable;

public:

	std::queue<std::string>  m_oParsedCode;
	int						 m_oLine_number;

	void initialize_lexer(std::string&);

};

