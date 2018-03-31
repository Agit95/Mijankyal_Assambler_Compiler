#define _CRT_SECURE_NO_WARNINGS
#include "Lexer.h"


//ctor by  default
C_Lexer::C_Lexer()
{
}


//~ctor  viirtual
C_Lexer::~C_Lexer()
{
}


//returned   symbole  is  operator  or not
bool  C_Lexer::is_operator(const char& symbole) const  _NOEXCEPT
{
	switch (symbole)
	{
	case E_Operators::multiplication:
	case E_Operators::division:
	case E_Operators::minus:
	case E_Operators::plus:
	case E_Operators::equal: return true;
	default:  return false;
	}
}


//returned   symbole  is  bracket  or not
bool  C_Lexer::is_bracket(const char& symbole) const  _NOEXCEPT
{
	if (symbole == E_Bracket::left_bracket || symbole == E_Bracket::right_bracket)
		return true;
	return false;
}



//validation  brcket  from  parsed string
bool  C_Lexer::bracket_validator(const std::string& code_line)  const _NOEXCEPT
{
	std::stack<char> bracket_validator_object;

	for (int i = 0; i < code_line.size(); ++i)
	{
		if (code_line[i] == ')' && bracket_validator_object.empty())
			return false;
		else if (code_line[i] == ')' && !bracket_validator_object.empty())
			bracket_validator_object.pop();
		else if (code_line[i] == '(')
			bracket_validator_object.push(code_line[i]);
		else if ((i != 0 && (code_line[i - 1] == '(' && is_operator(code_line[i]))) ||
			(i != 0 && (code_line[i] == ')' && is_operator(code_line[i - 1]))))
			return false;
	}

	if (bracket_validator_object.empty())
		return true;
	return false;
}

// get  code  line  from  code  file
void  C_Lexer::geting_code_line() _NOEXCEPT
{
	char* temp_Code = const_cast<char*>(m_oCode_line.c_str());
	char* temp_str = std::strtok(temp_Code, ";");

	while (temp_str != NULL)
	{
		std::string  obj;
		obj += temp_str;
		obj += ";";
		m_oLexed_code.push(obj);
		temp_str = strtok(NULL, ";");
	}
}



// set  to  queue  lexed code
inline void  C_Lexer::set_parsed_code()
{
	m_oParsedCode.push(getPostfixNotation());
}


void C_Lexer::setExpression(const std::string &expression)
{
	m_oPostfixNotation.clear();
	m_oVariable.clear();
	m_oExpression = expression;
	++m_oLine_number;

	if (!bracket_validator(m_oExpression))
		throw C_Compile_Error("INVALID BRACKET IN LINE  ", m_oLine_number);

	if (!operators_validator(m_oExpression))
		throw C_Compile_Error("INVALID OPERATOR  IN LINE  ", m_oLine_number);

	infixToPostfix();
}

std::string C_Lexer::getPostfixNotation()
{
	std::string postfix = m_oVariable + " ";

	for (auto token : m_oPostfixNotation)
	{
		postfix += token;
		postfix += " ";
	}
	postfix += E_Operators::equal;
	return postfix;
}


bool C_Lexer::is_operand(const char &ch)  const _NOEXCEPT
{
	return ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'));
}


int C_Lexer::getPrioritet(const char &ch) const _NOEXCEPT
{
	switch (ch)
	{
	case E_Operators::plus:
	case E_Operators::minus:
		return 2;
	case E_Operators::multiplication:
	case E_Operators::division:
		return 3;
	case E_Operators::equal:
		return 4;
	}

	return -1;
}


void C_Lexer::infixToPostfix()
{
	std::stack<char> operation_stack;
	int counter = 0;
	while (m_oExpression[counter] != E_Operators::equal)
	{
		m_oVariable += m_oExpression[counter];
		++counter;
	}
	++counter;

	for (size_t i = counter; i < m_oExpression.length() - 1; i++)
	{
		if (is_operand(m_oExpression[i]))
		{
			/* check if number has multiple digits or number is real then add to str */
			std::string operand;
			size_t j = i;
			operand += m_oExpression[j];
			j++;

			while (j < m_oExpression.length() && (is_operand(m_oExpression[j]) || m_oExpression[j] == '.'))
			{
				operand += m_oExpression[j];
				j++;
				i++;
			}
			/* **************************************************** */

			m_oPostfixNotation.push_back(operand);
		}
		else if (m_oExpression[i] == '(' &&  m_oExpression[i + 1] == ')' && !m_oPostfixNotation.empty())
		{
			std::string temp_opeand = m_oPostfixNotation.back();
			m_oPostfixNotation.pop_back();
			temp_opeand += "()";
			m_oPostfixNotation.push_back(temp_opeand);
			++i;
		}
		else if (is_operator(m_oExpression[i]))
		{
			while ((!operation_stack.empty()) && (operation_stack.top() != '(') &&
					(getPrioritet(m_oExpression[i]) < getPrioritet(operation_stack.top())))
			{
				// convert char to string and then push_back to the link list
				m_oPostfixNotation.push_back(std::string(1, operation_stack.top()));
				operation_stack.pop();
			}

			operation_stack.push(m_oExpression[i]);
		}
		else if (m_oExpression[i] == ')')
		{
			while ((!operation_stack.empty()) && (operation_stack.top() != '('))
			{
				m_oPostfixNotation.push_back(std::string(1, operation_stack.top()));
				operation_stack.pop();
			}

			operation_stack.pop();
		}
		else if (m_oExpression[i] == '(')
		{
			operation_stack.push(m_oExpression[i]);
		}
	}

	while (!operation_stack.empty())
	{
		if (operation_stack.top() != '(')
		{
			m_oPostfixNotation.push_back(std::string(1, operation_stack.top()));
			operation_stack.pop();
		}
	}
}


// initialize  lexer
void C_Lexer::initialize_lexer(std::string& input_code)
{
	m_oCode_line = input_code;
}



void C_Lexer::lexing_code_line()
{
	geting_code_line();
	while (!m_oLexed_code.empty())
	{
		setExpression(m_oLexed_code.front());
		m_oLexed_code.pop();
		set_parsed_code();
	}
}


// validate  operatos  in  expression
bool  C_Lexer::operators_validator(const std::string& code_line) const _NOEXCEPT
{
	for (int i = 0; i < code_line.length() - 1; ++i)
	{
		if (is_operator(code_line[i]) && is_operator(code_line[i + 1]))
			return false;
	}
	return true;
}

