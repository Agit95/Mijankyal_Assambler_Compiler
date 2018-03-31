#include "IO_Device_Class.h"
#include "C_Compile_Error.h"
#include "Parser.h"
#include <iostream>
#include "Lexer.h"

int  main()
{
	IO_Device_Class IO_object;
	char* input_name = new char[10];
	std::cout << "input file path and  file name  :";
	std::cin >> input_name;
	std::string string_object(input_name);
	std::cout << "output file path and  file name  :";
	std::cin >> input_name;
	std::string string_object2(input_name);
	
	input_name = NULL;
	delete input_name;

	
	try
	{
		//initalize  input  and  output  files
		IO_object.initializer_input_output_file(string_object, string_object2);
		//read  input  file
		IO_object.Read_from_file();

		// get  parsed  code  
		string_object = IO_object.get_parsed_code();
		C_Lexer Lexer_object;
		//initialize  lexer  object
		Lexer_object.initialize_lexer(string_object);
		Lexer_object.lexing_code_line();
		//code  line  number
		int line_number = 1;
		C_Parser Parser_object;
		//initialize  parse  code  and data  section
		Parser_object.initialize_Data_and_Code_sections();

		while (!Lexer_object.m_oParsedCode.empty())
		{
			//generatin token's
			Parser_object.tok_lexing_string(Lexer_object.m_oParsedCode.front(), line_number);
			++line_number;
			Lexer_object.m_oParsedCode.pop();
			//pasing  and  generating  code  
			Parser_object.parse_code_line(1);
		}
		//generate  output  code 
		Parser_object.generat_code_output();
		//genrate  output file
		IO_object.Write_to_out_file();
	}
	catch (C_Compile_Error & error)
	{
		error.show_error_message();
	}

	system("pause");
	return 0;
}