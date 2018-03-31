#pragma once
#include "IO_Device_Class.h"
#include "Address_Register.h"
#include "Lexer.h"
#include "EType.h"
#include "GPR.h"
#include <map>



class C_Parser
{
public:
	C_Parser();
	C_Parser(const C_Parser&);
	virtual ~C_Parser();

	void  tok_lexing_string(std::string, int);
	void  parse_code_line(int);
	void  generat_code_output();
	void  initialize_Data_and_Code_sections();


private:
	
	void  generate_code_section_code();
	void  change_GPR_state(E_GPR);
	void  change_AR_state(E_AR) _NOEXCEPT;
	void  get_data_type(int,std::stringstream&);
	void  update_GPA_and_AR() _NOEXCEPT;
	void  generate_function_call_part_code(std::string&, std::stringstream&) _NOEXCEPT;
	void  generate_code(E_GPR, E_GPR, std::string&,int,std::stringstream&,std::stringstream&,std::stack<E_GPR>&, std::stack<std::string>&);
	E_GPR find_empty_GPR_in_input_type(EType) const _NOEXCEPT;
	E_AR  find_empty_AR() const _NOEXCEPT;
	bool  is_number(std::string&) const _NOEXCEPT;
	bool  is_variable(const std::string&) const;
	bool  is_operator(std::string&) const _NOEXCEPT;
	bool  is_function(std::string&) const _NOEXCEPT;
	bool  is_register_name(const std::string&) const _NOEXCEPT;
	EType get_type(int&) const  _NOEXCEPT;
	EType get_reg_type(int) const _NOEXCEPT;
	const std::string  get_typename(EType) const _NOEXCEPT;

protected:
	std::map<std::string,EType>  m_oVariable_name_and_type;
	std::stack<std::string>		 m_oVariable_Operators_and_Operands;
	std::stringstream			 m_oTemp_variable_stream;
	std::stringstream			 m_oTemp_function_stream;
	std::stringstream			 m_oData_section;
	std::stringstream			 m_oCode_section;
	int							 m_oParse_code_line_number;

	std::map<E_GPR, bool>		 m_oGPR_name_and_state
	{
		std::make_pair(E_GPR::R0,  true),
		std::make_pair(E_GPR::R1,  false),
		std::make_pair(E_GPR::R2,  false),
		std::make_pair(E_GPR::R3,  false),
		std::make_pair(E_GPR::R4,  false),
		std::make_pair(E_GPR::R5,  false),
		std::make_pair(E_GPR::R6,  false),
		std::make_pair(E_GPR::R7,  false),
		std::make_pair(E_GPR::R8,  false),
		std::make_pair(E_GPR::R9,  false),
		std::make_pair(E_GPR::R10, false),
		std::make_pair(E_GPR::R11, false),
		std::make_pair(E_GPR::R12, false),
		std::make_pair(E_GPR::R13, false),
		std::make_pair(E_GPR::R14, false),
		std::make_pair(E_GPR::R15, false),
		std::make_pair(E_GPR::R16, false),
		std::make_pair(E_GPR::R17, false),
		std::make_pair(E_GPR::R18, false),
		std::make_pair(E_GPR::R19, false),
		std::make_pair(E_GPR::R20, false),
		std::make_pair(E_GPR::R21, false),
		std::make_pair(E_GPR::R22, false),
		std::make_pair(E_GPR::R23, false),
		std::make_pair(E_GPR::R24, false),
		std::make_pair(E_GPR::R25, false),
		std::make_pair(E_GPR::R26, false),
		std::make_pair(E_GPR::R27, false),
		std::make_pair(E_GPR::R28, false),
		std::make_pair(E_GPR::R29, false),
		std::make_pair(E_GPR::R30, false),
		std::make_pair(E_GPR::R31, false),
		std::make_pair(E_GPR::R32, false),
		std::make_pair(E_GPR::R33, false),
		std::make_pair(E_GPR::R34, false),
		std::make_pair(E_GPR::R35, false),
		std::make_pair(E_GPR::R36, false),
		std::make_pair(E_GPR::R37, false),
		std::make_pair(E_GPR::R38, false),
		std::make_pair(E_GPR::R39, false),
		std::make_pair(E_GPR::R40, false),
		std::make_pair(E_GPR::R41, false),
		std::make_pair(E_GPR::R42, false),
		std::make_pair(E_GPR::R43, false),
		std::make_pair(E_GPR::R44, false),
		std::make_pair(E_GPR::R45, false),
		std::make_pair(E_GPR::R46, false),
		std::make_pair(E_GPR::R47, false),
		std::make_pair(E_GPR::R48, false),
		std::make_pair(E_GPR::R49, false),
		std::make_pair(E_GPR::R50, false),
		std::make_pair(E_GPR::R51, false),
		std::make_pair(E_GPR::R52, false),
		std::make_pair(E_GPR::R53, false),
		std::make_pair(E_GPR::R54, false),
		std::make_pair(E_GPR::R55, false),
		std::make_pair(E_GPR::R56, false),
		std::make_pair(E_GPR::R57, false),
		std::make_pair(E_GPR::R58, false),
		std::make_pair(E_GPR::R59, false),
		std::make_pair(E_GPR::R60, false),
		std::make_pair(E_GPR::R61, false),
		std::make_pair(E_GPR::R62, false),
		std::make_pair(E_GPR::R63, false),
		std::make_pair(E_GPR::undefinde,false),
	};
	std::map<E_AR, bool> m_oAddress_register_and_state
	{
		std::make_pair(E_AR::A2,false),
		std::make_pair(E_AR::A3,false),
		std::make_pair(E_AR::A4,false),
		std::make_pair(E_AR::A5,false),
		std::make_pair(E_AR::A6,false),
		std::make_pair(E_AR::A7,false),
		std::make_pair(E_AR::SP,true),
		std::make_pair(E_AR::SF,true)
	};
};

