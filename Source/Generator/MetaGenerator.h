
#ifndef metagenerator_h
#define metagenerator_h

#include <Generator.h>

class MetaGenerator : public Generator {
public:
	MetaGenerator() = default;
	~MetaGenerator() = default;

	std::string WriteHeader();
	std::string WriteFooter();

	void WriteConstructor(MetaClass* mc, MetaFunction* mf);
	void WriteStaticFunction(MetaClass* mc, MetaFunction* mf);
	void WriteObjectFunction(MetaClass* mc, MetaFunction* mf);

	void WriteSetterFunction(MetaClass* mc, MetaVariable* mf);
	void WriteGetterFunction(MetaClass* mc, MetaVariable* mf);
};

#endif