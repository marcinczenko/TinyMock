#ifndef COMPLEXARGUMENT_H
#define COMPLEXARGUMENT_H

#include <ostream>

class ComplexArgument
{
public:
	ComplexArgument(int value);
	bool operator==(const ComplexArgument& rhs);	
	int GetMemberValue() const;	
private:
	int m_member ;
};

std::ostream& operator<<(std::ostream& os, const ComplexArgument& complexArgument);

#endif
