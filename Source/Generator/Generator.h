
#ifndef generator_h
#define generator_h

#include <Core/Giga.h>
#include <MetaClass.h>

class Generator {
public:
	Generator() = default;
	~Generator() = default;

	virtual std::string WriteHeader() { }
	virtual std::string WriteFooter() { }

	virtual void WriteConstructor(MetaClass* mc, MetaFunction* mf) { }
	virtual void WriteStaticFunction(MetaClass* mc, MetaFunction* mf) { }
	virtual void WriteObjectFunction(MetaClass* mc, MetaFunction* mf) { }

	virtual void WriteSetterFunction(MetaClass* mc, MetaVariable* mf) { }
	virtual void WriteGetterFunction(MetaClass* mc, MetaVariable* mf) { }

	std::string GetContents() { return this->WriteHeader() + m_output + this->WriteFooter(); }

protected:
	// Total output
	std::string m_output;
};

#endif