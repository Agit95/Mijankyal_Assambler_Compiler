#pragma once
class C_Compile_Error
{
private:
	const char* m_oError_message;
	int  m_oError_line;

public:
	C_Compile_Error();
	C_Compile_Error(const char *);
	C_Compile_Error(const char *, int);
	C_Compile_Error(const C_Compile_Error&);
	virtual ~C_Compile_Error();

	void setError_message(const char *);
	void setError_message(const char * ,  int);
	void show_error_message() const;
};

