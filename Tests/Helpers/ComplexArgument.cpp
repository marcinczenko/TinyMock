#include "ComplexArgument.h"

std::ostream& operator<<(std::ostream& os, const ComplexArgument& complexArgument)
{
	return os << "{ m_member(" << complexArgument.GetMemberValue() << ") }";
}

ComplexArgument::ComplexArgument(int value) : m_member(value)
{
}

bool ComplexArgument::operator==(const ComplexArgument& rhs)
{
	return m_member == rhs.m_member;
}

int ComplexArgument::GetMemberValue() const
{
	return m_member ;
}
