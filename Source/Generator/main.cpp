//
//  main.cpp
//  meta-generator
//
//  Created by Terry Smith on 2019-08-09.
//  Copyright © 2019 Grok It Games. All rights reserved.
//

#include <MetaClass.h>
#include <Core/Directory.h>
#include <Core/Variant.h>
#include <IO/Resource.h>
#include <IO/SQLiteDataLoader.h>
#include <Core/MetaSystem.h>

#include <clang-c/Index.h>
#include <openssl/md5.h>

#include <iostream>
#include <vector>
#include <map>
#include <sstream>

#ifdef WIN32
#include <direct.h>
#define mymkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#define mymkdir(path) mkdir(path, ACCESSPERMS)
#endif

using namespace std;

Array<const char*> args;
CXIndex cindex = 0;
CXTranslationUnit unit;
std::map<std::string, std::string> inheritance;
std::string currentGigaClassName;
std::string currentClassName;
std::map<std::string, MetaClass*> classes;
std::map<std::string, bool> exportGigaClasses;
std::map<std::string, bool> singletonClasses;
bool exportGigaClass = false;
MetaClass* currentMetaClass = 0;
std::map<int, int> typeMappings;
std::map <int, std::string> functionMappings;
std::map <int, std::string> csharpMappings;
std::string precompiled_header;
Array<std::string> addlIncludes;
std::map<std::string, std::string> fileHashes;
std::map<std::string, std::string> functionData;
std::map<std::string, std::string> registrationData;
std::map<std::string, Array<std::string>> inheritances;
SQLiteDataLoader* dataLoader = 0;
MetaSystem* metaSystem = 0;
bool addedData = false;
bool pullOptions = false;
bool markSingleton = false;
bool markGet = false;
bool markSet = false;
bool markSerialize = false;
bool markNonEditable = false;
bool markOptional = false;

bool grabNextFunction = false;
bool grabNextVar = false;
bool grabNextClass = false;

// Dummy getter/setter functions
void DummySetter(GigaObject* obj, Variant* value) { }
Variant* DummyGetter(GigaObject* obj) { return(new Variant(0)); }

ostream& operator<<(ostream& stream, const CXString& str)
{
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

int map_internal_type(CXType kind, std::string rettype) {
    int internalType = 0;
    if (typeMappings.find(kind.kind) != typeMappings.end()) {
        internalType = typeMappings[kind.kind];
    }

    if (internalType == 0 && kind.kind == CXTypeKind::CXType_Typedef) {
        if (rettype == "vector2") {
            internalType = Variant::VAR_VECTOR2;
        }

        if (rettype == "vector3") {
            internalType = Variant::VAR_VECTOR3;
        }

        if (rettype == "vector4") {
            internalType = Variant::VAR_VECTOR4;
        }

        if (rettype == "quaternion") {
            internalType = Variant::VAR_QUATERNION;
        }
    }

    // Special case for templated objects
    if (internalType == 0 && rettype.find("<") != -1) {
        if(rettype.find("Array<") != -1) {
            internalType = Variant::VAR_ARRAY;
        }
    }

    if (internalType == 0 && rettype == "std::string") {
        internalType = Variant::VAR_STRING;
    }

    if (internalType == 0 && rettype == "uint32_t") {
        internalType = Variant::VAR_UINT32;
    }

    if (internalType == 0 && rettype == "int32_t") {
        internalType = Variant::VAR_INT32;
    }

    if (internalType == 0 && rettype == "uint64_t") {
        internalType = Variant::VAR_UINT64;
    }

    if (internalType == 0 && rettype == "int64_t") {
        internalType = Variant::VAR_INT64;
    }
    
    if (rettype == "Variant *") {
        internalType = Variant::VAR_VARIANT;
    }

    return(internalType);
}

CXChildVisitResult visitor(CXCursor c, CXCursor parent, CXClientData client_data) {
    CXCursorKind cursor = clang_getCursorKind(c);
    CXString str = clang_getCursorSpelling(c);
    CXType type = clang_getCursorType(c);
    std::string name = clang_getCString(str);
    CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(c);

    if (name.empty() == false) {
        size_t pos = name.find("class ");
        if (pos != name.npos) {
            name = name.substr(pos + 6);
        }
    }

    if (pullOptions && cursor == CXCursor_EnumConstantDecl) {
        if (strcmp("Singleton", name.c_str()) == 0) {
            markSingleton = true;
        }

        if (strcmp("Get", name.c_str()) == 0) {
            markGet = true;
        }

        if (strcmp("Set", name.c_str()) == 0) {
            markSet = true;
        }

        if (strcmp("Serialize", name.c_str()) == 0) {
            markSerialize = true;
        }

        if (strcmp("NonEditable", name.c_str()) == 0) {
            markNonEditable = true;
        }

        if (strcmp("Optional", name.c_str()) == 0) {
            markOptional = true;
        }

        //pullOptions = false;
        return CXChildVisit_Continue;
    }

    if (pullOptions && (cursor == CXCursor_CompoundStmt || cursor == CXCursor_DeclStmt || cursor == CXCursor_EnumDecl)) {
        return CXChildVisit_Recurse;
    }

    if (cursor == CXCursor_ClassDecl || cursor == CXCursor_ClassTemplate) {
        currentClassName = name;
        grabNextClass = true;
        //exportGigaClass = true;
        pullOptions = true;
        //markSingleton = false;
    }

    if (cursor == CXCursor_CXXBaseSpecifier) {
        inheritance[currentClassName] = name;
    }

    if (((grabNextFunction && cursor == CXCursor_CXXMethod) || (cursor == CXCursor_Constructor)) && currentMetaClass) {
        if (cursor == CXCursor_Constructor) {
            int num_args = clang_Cursor_getNumArguments(c);

            if (access == CX_CXXPublic) {
                currentMetaClass->hasPublicConstructor = true;
            }

            if (num_args == 0) {
                grabNextFunction = false;
                return CXChildVisit_Continue;
            }
        }

        if (name.find("operator") == 0) {
            grabNextFunction = false;
            return CXChildVisit_Continue;
        }

        cout << "Found GIGA function named '" << name.c_str() << "'" << endl;

        CXType returnType = clang_getResultType(type);
        bool isStatic = clang_CXXMethod_isStatic(c);
        std::string rettype = clang_getCString(clang_getTypeSpelling(returnType));

        int internalType = map_internal_type(returnType, rettype);

        if (internalType != 0) {
            if (currentMetaClass->templated == true && cursor == CXCursor_Constructor) {
                grabNextFunction = false;
                return CXChildVisit_Continue;
            }
            
            if(internalType == Variant::VAR_ARRAY) {
                rettype = rettype.substr(rettype.find("<") + 1);
                rettype = rettype.substr(0, rettype.find(">"));
            }

            MetaFunction* func = new MetaFunction();
            func->name = name;
            func->returnType = internalType;
            func->strReturnType = rettype;
            func->isStatic = isStatic;
            func->isConstructor = (cursor == CXCursor_Constructor);
            func->found = true;

            if (func->isConstructor && access == CX_CXXPublic) {
                currentMetaClass->hasPublicConstructor = true;
            }

            bool success = true;

            int num_args = clang_Cursor_getNumArguments(c);
            for (int i = 0; i < num_args; i++) {
                CXCursor arg = clang_Cursor_getArgument(c, i);
                CXString n = clang_getCursorSpelling(arg);
                std::string arg_name = clang_getCString(n);

                CXType arg_type = clang_getArgType(type, i);
                std::string argtype = clang_getCString(clang_getTypeSpelling(arg_type));

                int intArgType = map_internal_type(arg_type, argtype);
                if (intArgType != 0) {
                    if(intArgType == Variant::VAR_ARRAY) {
                        argtype = argtype.substr(argtype.find("<") + 1);
                        argtype = argtype.substr(0, argtype.find(">"));
                    }
                    
                    MetaFunction::Parameter* p = new MetaFunction::Parameter();
                    p->name = arg_name;
                    p->type = intArgType;
                    p->typeStr = argtype;
                    p->found = true;
                    func->params.push_back(p);
                }
                else {
                    success = false;
                    cout << "Unmapped argument type." << endl;
                }

                //cout << "Arg " << i << " type '" << argtype.c_str() << "'" << endl;
            }

            assert(func != 0);
            if (success == true) {
                if (currentMetaClass->AddFunction(func)) {
                    cout << "Added function " << func->name << " to class " << currentMetaClass->name << endl;
                }
                else {
                    cout << "Not adding duplicate function " << func->name << " to class " << currentMetaClass->name << endl;
                }
            }
        }
        else {
            int error = 1;
            cout << "Unmapped type error." << endl;
        }

        grabNextFunction = false;
        return CXChildVisit_Continue;
    }

    if (cursor == CXCursor_CXXMethod && name.compare("GFUNCTION") == 0) {
        grabNextFunction = true;
        return CXChildVisit_Continue;
    }

    if (cursor == CXCursor_FunctionDecl && currentGigaClassName.empty() == false) {
        int error = 1;
    }

    if (cursor == CXCursor_FunctionDecl && name.compare("GCLASS") == 0) {
        grabNextClass = true;
        exportGigaClass = true;
        pullOptions = true;
        markSingleton = false;
        return CXChildVisit_Recurse;
    }

    if (grabNextClass == true && (cursor == CXCursor_ClassDecl || cursor == CXCursor_ClassTemplate)) {
        currentGigaClassName = name;
        grabNextClass = false;

        if (exportGigaClass == true) {
            exportGigaClasses[name] = true;
        }

        std::map<std::string, MetaClass*>::iterator ci = classes.find(name);
        //if (ci == classes.end()) {
        cout << "Found GIGA class named '" << name.c_str() << "'" << endl;
        addedData = true;

        if (ci != classes.end()) {
            currentMetaClass = ci->second;
            currentMetaClass->name = name;
            currentMetaClass->singleton = markSingleton;
            currentMetaClass->templated = cursor == CXCursor_ClassTemplate;
        }
        else {
            MetaClass* m = new MetaClass;
            m->name = name;
            m->singleton = markSingleton;
            m->templated = cursor == CXCursor_ClassTemplate;

            classes[name] = m;
            currentMetaClass = m;
        }

        currentMetaClass->found = true;
        exportGigaClass = false;

        // Clear
        //cout << "Clearing meta class " << currentMetaClass->name << endl;
        //currentMetaClass->Clear();
    //}

        markSingleton = false;
        exportGigaClass = false;
        return CXChildVisit_Recurse;
    }

    if (grabNextVar && cursor == CXCursor_FieldDecl && currentMetaClass) {
        cout << "Found GIGA variable named '" << name.c_str() << "'" << endl;

        std::string typestr = clang_getCString(clang_getTypeSpelling(type));
        int internalType = map_internal_type(type, typestr);

        if (internalType != 0) {
            MetaVariable* var = new MetaVariable();
            var->name = name;
            var->type = internalType;
            var->typeStr = typestr;
            var->found = true;

            if (markGet) {
                var->getter = DummyGetter;
                markGet = false;
            }

            if (markSet) {
                var->setter = DummySetter;
                markSet = false;
            }

            currentMetaClass->AddVariable(var);
        }
        else {
            int error = 1;
        }

        grabNextVar = false;
        //cout << "Type '" << typestr.c_str() << "'" << endl;

        return CXChildVisit_Continue;
    }

    if (cursor == CXCursor_CXXMethod && name.compare("GVARIABLE") == 0) {
        grabNextVar = true;
        pullOptions = true;
        return CXChildVisit_Recurse;
    }

    clang_disposeString(str);

    if ((cursor == CXCursor_ClassDecl || cursor == CXCursor_ClassTemplate) && exportGigaClass == false) {
        addedData = false;
    }

    if (cursor == CXCursor_ClassDecl || cursor == CXCursor_ClassTemplate) {
        return CXChildVisit_Recurse;
    }
    
    if (cursor == CXCursor_CXXBaseSpecifier) {
        auto it = inheritances.find(currentClassName);
        if(it != inheritances.end()) {
            auto ii = inheritances[currentClassName].find(name);
            if(ii == inheritances[currentClassName].end()) inheritances[currentClassName].push_back(name);
        }
        else {
            inheritances[currentClassName] = Array<std::string>();
            inheritances[currentClassName].push_back(name);
        }
    }

    return CXChildVisit_Continue;
}

Array<std::string> ProcessInheritance(std::string className) {
    Array<std::string> addlClasses;
    
    if(inheritances.find(className) == inheritances.end()) {
        return(addlClasses);
    }
    
    auto it = inheritances[className].begin();
    for(; it != inheritances[className].end(); it++) {
        addlClasses.push_back(*it);
        Array<std::string> deeper = ProcessInheritance(*it);
        
        auto itt = deeper.begin();
        for(; itt != deeper.end(); itt++) {
            auto ii = addlClasses.find(*itt);
            if(ii == addlClasses.end()) {
                addlClasses.push_back(*itt);
            }
        }
    }
    
    return(addlClasses);
}

std::string GetInheritance(std::string className) {
    std::string inheritedClass = "";
    auto ici = inheritances.find(className);
    if(ici != inheritances.end()) {
        auto inh = ici->second.begin();
        for(; inh != ici->second.end(); inh++) {
            if(exportGigaClasses[*inh] == true) {
                inheritedClass = *inh;
                break;
            }
            
            inheritedClass = GetInheritance(*inh);
        }
    }
    
    return(inheritedClass);
}

void ProcessDirectory(Directory* dir) {
    auto fi = dir->files.begin();
    for (; fi != dir->files.end(); fi++) {
        addedData = false;

        std::string filename = dir->path + "/" + (*fi);
        size_t pos = filename.find_last_of(".");
        std::string extension = filename.substr(pos, filename.length() - pos);

        if (extension.compare(".h") != 0) {
            continue;
        }

        cout << filename << endl;

        // MD5 check
        Resource* resource = new Resource();
        resource->Initialize(filename, Resource::FILEMODE_READ);
        unsigned char* data = resource->GetData();
        int count = resource->filesize;

        unsigned char digest[MD5_DIGEST_LENGTH];
        char hashval[34];
        memset(hashval, 0, 34);

        MD5(data, resource->filesize, (unsigned char*)&digest);
        delete resource;

        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            sprintf(hashval + (i * 2), "%02x", digest[i]);
        }

        auto ait = addlIncludes.find(filename);
        if (ait == addlIncludes.end()) {
            addlIncludes.push_back(filename);
        }

        // Check agaist an existing file hash
        auto it = fileHashes.find(filename);
        if (it != fileHashes.end()) {
            if (strcmp(it->second.c_str(), hashval) == 0) {
                cout << "hash match" << endl;
                continue;
            }
            else {
                fileHashes[filename] = hashval;
            }
        }
        else {
            fileHashes[filename] = hashval;
        }

        int flags = (precompiled_header.empty() ? CXTranslationUnit_ForSerialization : 0);

        bool savePrecompiledHeader = false;
        clang_parseTranslationUnit2(
            cindex,
            filename.c_str(), args.data(), (int)args.size(),
            nullptr, 0,
            flags, &unit);
        if (unit == nullptr) {
            // First, try again without the precompiled header
            cerr << "Failed with precompiled header, trying without " << args.size() << endl;
            if (remove(precompiled_header.c_str())) {
                cerr << "Also failed to remove file " << precompiled_header.c_str() << ", exiting" << endl;
                exit(0);
            }

            bool success = false;
            auto it = args.begin();
            for (; it != args.end(); it++) {
                if (strcmp(*it, "-include-pch") == 0)
                    break;
            }

            if (it != args.end()) {
                args.erase(it, it + 1);

                cerr << "Removed arguments " << args.size() << endl;
                flags = CXTranslationUnit_ForSerialization;

                clang_parseTranslationUnit2(
                    cindex,
                    filename.c_str(), args.data(), (int)args.size(),
                    nullptr, 0,
                    flags, &unit);

                if (unit != nullptr) {
                    success = true;
                    savePrecompiledHeader = true;
                }
            }

            if (success == false) {
                cerr << "Unable to parse translation unit. Quitting." << endl;
                exit(-1);
            }
        }

        CXCursor cursor = clang_getTranslationUnitCursor(unit);
        clang_visitChildren(cursor, visitor, nullptr);

        if (precompiled_header.empty() || savePrecompiledHeader) {
            clang_saveTranslationUnit(unit, "generator.pch", clang_defaultSaveOptions(unit));
            precompiled_header = "generator.pch";

            args.push_back("-include-pch");
            args.push_back(precompiled_header.c_str());
        }

        clang_disposeTranslationUnit(unit);
        currentGigaClassName.clear();
        currentMetaClass = 0;
        pullOptions = false;
    }

    auto di = dir->subdirectories.begin();
    for (; di != dir->subdirectories.end(); di++) {
        ProcessDirectory((*di));
    }
}

void AddInheritedClasses(MetaClass* cl, std::string name) {
    // Check if this class inherits from another
    std::map<std::string, std::string>::iterator it = inheritance.find(name);
    if (it != inheritance.end()) {
        // Get that class
        MetaClass* cl2 = classes[it->second];

        // If it does, add any meta functions from that class to this class too
        auto flist = cl2->GetFunctions();
        auto fi = flist.begin();
        for (; fi != flist.end(); fi++) {
            if ((*fi)->isConstructor == true) continue;

            assert(*fi != 0);
            cl->AddFunction((*fi));
        }

        // Also check if that class inherits from another
        AddInheritedClasses(cl, it->second);
    }

    Array<int> test;
}

int main(int argc, char** argv) {
    cindex = clang_createIndex(0, 0);

#ifndef WIN32
    args.push_back("-I/usr/local/include");
    args.push_back("-I/usr/include");
    args.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1");
#endif
    args.push_back("-x");
    args.push_back("c++");
    args.push_back("-std=c++11");

    // Create type mappings
    typeMappings[CXTypeKind::CXType_Void] = -1;
    typeMappings[CXTypeKind::CXType_Bool] = Variant::VAR_BOOL;
    typeMappings[CXTypeKind::CXType_Int] = Variant::VAR_INT32;
    typeMappings[CXTypeKind::CXType_UInt] = Variant::VAR_UINT32;
    typeMappings[CXTypeKind::CXType_Float] = Variant::VAR_FLOAT;
    typeMappings[CXTypeKind::CXType_Pointer] = Variant::VAR_OBJECT;
    typeMappings[CXTypeKind::CXType_SChar] = Variant::VAR_STRING;
    //typeMappings[CXTypeKind::CXType_LValueReference] = Variant::VAR_OBJECT;
    //typeMappings[CXTypeKind::CXType_Record] = Variant::VAR_OBJECT;

    // Mappings to functions
    functionMappings[Variant::VAR_INT32] = "Int";
    functionMappings[Variant::VAR_UINT32] = "UInt";
    functionMappings[Variant::VAR_INT64] = "Int64";
    functionMappings[Variant::VAR_UINT64] = "UInt64";
    functionMappings[Variant::VAR_BOOL] = "Bool";
    functionMappings[Variant::VAR_FLOAT] = "Float";
    functionMappings[Variant::VAR_VECTOR2] = "Vector2";
    functionMappings[Variant::VAR_VECTOR3] = "Vector3";
    functionMappings[Variant::VAR_VECTOR4] = "Vector4";
    functionMappings[Variant::VAR_QUATERNION] = "Quaternion";
    functionMappings[Variant::VAR_STRING] = "String";
    functionMappings[Variant::VAR_OBJECT] = "Object";
    functionMappings[Variant::VAR_ARRAY] = "Array";
    functionMappings[Variant::VAR_VARIANT] = "Variant";
    
    csharpMappings[Variant::VAR_INT32] = "int";
    csharpMappings[Variant::VAR_UINT32] = "uint";
    csharpMappings[Variant::VAR_BOOL] = "bool";
    csharpMappings[Variant::VAR_FLOAT] = "float";
    csharpMappings[Variant::VAR_VECTOR2] = "Vector2";
    csharpMappings[Variant::VAR_VECTOR3] = "Vector3";
    csharpMappings[Variant::VAR_VECTOR4] = "Vector4";
    csharpMappings[Variant::VAR_QUATERNION] = "Quaternion";
    csharpMappings[Variant::VAR_STRING] = "string";
    csharpMappings[Variant::VAR_VARIANT] = "object";

    std::string current = Directory::GetCurrent();

    metaSystem = new MetaSystem();

    dataLoader = new SQLiteDataLoader();
    dataLoader->Open("generator.db");

    Array<DataRecord*> clrecords = dataLoader->GetRecords("classes");
    Array<DataRecord*> fnrecords = dataLoader->GetRecords("functions");
    Array<DataRecord*> pmrecords = dataLoader->GetRecords("params");
    Array<DataRecord*> inhrecords = dataLoader->GetRecords("inheritance");
    Array<DataRecord*> varrecords = dataLoader->GetRecords("variables");
    Array<DataRecord*> filerecords = dataLoader->GetRecords("files");

    auto clit = clrecords.begin();
    for (; clit != clrecords.end(); clit++) {
        MetaClass* mc = new MetaClass();
        mc->name = (*clit)->Get("className")->AsString();
        mc->singleton = (*clit)->Get("singleton")->AsBool();
        mc->hasPublicConstructor = (*clit)->Get("hasPublicConstructor")->AsBool();

        classes[mc->name] = mc;
        exportGigaClasses[mc->name] = (*clit)->Get("exported")->AsBool();
        if (mc->singleton) {
            singletonClasses[mc->name] = true;
        }
    }
    
    auto ii = inhrecords.begin();
    for(; ii != inhrecords.end(); ii++) {
        std::string className = (*ii)->Get("className")->AsString();
        std::string inheritsFrom = (*ii)->Get("inheritsFrom")->AsString();
        
        auto it = inheritances.find(className);
        if(it != inheritances.end()) {
            auto ii = inheritances[className].find(inheritsFrom);
            if(ii == inheritances[className].end()) inheritances[className].push_back(inheritsFrom);
        }
        else {
            inheritances[className] = Array<std::string>();
            inheritances[className].push_back(inheritsFrom);
        }
    }

    auto fnit = fnrecords.begin();
    for (; fnit != fnrecords.end(); fnit++) {
        std::string className = (*fnit)->Get("className")->AsString();
        MetaClass* mcl = classes[className];

        MetaFunction* fn = new MetaFunction();
        fn->name = (*fnit)->Get("funcName")->AsString();
        fn->returnType = (*fnit)->Get("returnType")->AsInt();
        fn->strReturnType = (*fnit)->Get("strReturnType")->AsString();
        fn->isStatic = (*fnit)->Get("isStatic")->AsBool();
        fn->isConstructor = (*fnit)->Get("isConstructor")->AsBool();

        mcl->AddFunction(fn, false);
    }

    auto pmit = pmrecords.begin();
    for (; pmit != pmrecords.end(); pmit++) {
        std::string className = (*pmit)->Get("className")->AsString();
        MetaClass* mcl = classes[className];

        std::string funcName = (*pmit)->Get("funcName")->AsString();
        MetaFunction* mfc = mcl->FindFunction(funcName, (*pmit)->Get("offset")->AsUInt());

        MetaFunction::Parameter* p = new MetaFunction::Parameter();
        p->name = (*pmit)->Get("paramName")->AsString();
        p->type = (*pmit)->Get("type")->AsUInt();
        p->typeStr = (*pmit)->Get("typeStr")->AsString();

        mfc->params.push_back(p);
    }

    auto vit = varrecords.begin();
    for (; vit != varrecords.end(); vit++) {
        std::string className = (*vit)->Get("className")->AsString();
        MetaClass* mcl = classes[className];

        MetaVariable* fn = new MetaVariable();
        fn->name = (*vit)->Get("varName")->AsString();
        fn->type = (*vit)->Get("type")->AsUInt();
        fn->typeStr = (*vit)->Get("typeStr")->AsString();
        fn->getter = (*vit)->Get("getter")->AsBool() ? DummyGetter : 0;
        fn->setter = (*vit)->Get("setter")->AsBool() ? DummySetter : 0;

        mcl->AddVariable(fn, false);
    }


    auto fiit = filerecords.begin();
    for (; fiit != filerecords.end(); fiit++) {
        fileHashes[(*fiit)->Get("filename")->AsString()] = (*fiit)->Get("hash")->AsString();
    }

    if (argc > 1) {
        current = argv[1];
    }

    std::string trailing = current.substr(current.length() - 1, 1);
    if (trailing == std::string("\\")) {
        current = current.substr(0, current.length() - 1);
    }

    // Trim 3 directories
    std::string path = current.substr(0, current.find_last_of("/\\"));
    path = path.substr(0, path.find_last_of("/\\"));

    std::string definePath = path + "/Source/Generator/Defines.h";
    std::string enginePath = "-I" + path + "/Source/Engine";

    args.push_back("-include");
    args.push_back(definePath.c_str());
    args.push_back(enginePath.c_str());

    cout << path << std::endl;

    // Bin directory
    std::string binDirectory = path + "/Bin";
    mymkdir(binDirectory.c_str());

    // Check for an existing precompiled header
    std::string autogenerated = binDirectory + "/generator.pch";
    FILE* fp = fopen(autogenerated.c_str(), "r");

    if (fp != 0) {
        cout << "Using precompiled header in" << binDirectory.c_str() << endl;
        precompiled_header = "generator.pch";

        args.push_back("-include-pch");
        args.push_back(precompiled_header.c_str());
        fclose(fp);
    }

    Directory* dir = new Directory();
    dir->Open(path + "/Source/Engine");

    ProcessDirectory(dir);

    Array<std::string> classNames;
    std::string output = "#include <Core/MetaSystem.h>\n#include <Core/Application.h>\n";
    std::string sourceDir = path + "/Source/Engine/";
    for (size_t i = 0; i < addlIncludes.size(); i++) {
        std::string include = addlIncludes[i].substr(sourceDir.length());
        output += "#include <" + include + ">\n";
    }
    output += "\n";

    unsigned int startOffset = 0;

    // Write new functions for each meta class
    std::map<std::string, MetaClass*>::iterator it = classes.begin();
    for (; it != classes.end(); it++) {
        MetaClass* cl = it->second;

        cout << "Checking class " << cl->name << endl;

        std::map<std::string, bool>::iterator ei = exportGigaClasses.find(cl->name);
        if (ei == exportGigaClasses.end()) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }
        
        if(ei->second == false) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }

        std::map<std::string, bool>::iterator singleton = singletonClasses.find(cl->name);
        if (singleton != singletonClasses.end()) {
            cl->singleton = true;
        }

        // Process inheritance
        AddInheritedClasses(cl, cl->name);
        cl->inheritsFrom = ProcessInheritance(cl->name);

        startOffset = output.length();

        // Variables
        auto vlist = cl->GetVariables();
        std::cout << "Has " << (int)vlist.size() << " variables." << endl;
        auto vi = vlist.begin();
        for (; vi != vlist.end(); vi++) {
            std::cout << "Check variable " << (*vi)->name << endl;
            MetaVariable* mp = (*vi);
            if (mp->getter) {
                output += "Variant* meta_" + cl->name + "_" + mp->name + "_get(GigaObject* obj) {\n";
                output += "\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(obj);\n";
                output += "\tGIGA_ASSERT(cobj != 0, \"Object is not of the correct type.\");\n\n";

                output += "\treturn(new Variant(cobj->" + mp->name + "));\n";
                output += "}\n\n";
            }

            if (mp->setter) {
                output += "void meta_" + cl->name + "_" + mp->name + "_set(GigaObject* obj, Variant* value) {\n";
                output += "\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(obj);\n";
                output += "\tGIGA_ASSERT(cobj != 0, \"Object is not of the correct type.\");\n\n";

                output += "\tcobj->" + mp->name + " = value->As" + functionMappings[mp->type] + "();\n";
                output += "}\n\n";
            }
        }

        // Constructors
        bool hasConstructor = false;
        auto flist = cl->GetFunctions();
        auto f1 = flist.begin();
        for (; f1 != flist.end(); f1++) {
            if ((*f1)->isConstructor == false) continue;

            cout << "Has constructor" << endl;
            hasConstructor = true;
            break;
        }

        if (hasConstructor) {
            auto f2 = flist.begin();
            output += "Variant* metaClass_" + cl->name + "_New(GigaObject* obj, int argc, Variant** argv) {\n";

            for (; f2 != flist.end(); f2++) {
                MetaFunction* mf2 = (*f2);
                if (mf2->isConstructor == false) continue;

                output += "\tif(argc == " + std::to_string(mf2->params.size()) + ") {\n";
                auto ai = mf2->params.begin();
                int argc = 0;

                output += "\t\tif(true";
                for (; ai != mf2->params.end(); ai++) {
                    output += " && argv[" + std::to_string(argc) + "]->Is" + functionMappings[(*ai)->type] + "()";
                    argc++;
                }
                output += ") {\n\t\t\treturn(new Variant(new " + cl->name + "(";

                ai = mf2->params.begin();
                argc = 0;
                for (; ai != mf2->params.end(); ai++) {
                    output += "argv[" + std::to_string(argc) + "]->As" + functionMappings[(*ai)->type];
                    if ((*ai)->type == Variant::VAR_OBJECT) {
                        output += "<" + (*ai)->typeStr + ">";
                    }
                    output += "(),";
                    argc++;
                }

                // Trim final comma
                if (argc > 0) {
                    output = output.substr(0, output.length() - 1);
                }

                output += ")));\n\t\t}\n\t}\n";
            }

            output += "\n\tGIGA_ASSERT(false, \"No matching constructor with those arguments.\");\n\treturn(0);";
            output += "\n}\n\n";
        }

        // Functions
        cout << "Has " << (int)flist.size() << " functions." << endl;
        std::map<std::string, bool> funcNames;
        flist = cl->GetFunctions();
        auto fi = flist.begin();
        for (; fi != flist.end(); fi++) {
            MetaFunction* mf = (*fi);

            cout << "Checking function " << mf->name << endl;
            if (funcNames.find(mf->name) != funcNames.end()) {
                cout << "Skipping function " << mf->name << endl;
                continue;
            }

            funcNames[mf->name] = true;

            // Look for any other functions with the same name
            Array<MetaFunction*> funcs;
            funcs.push_back(mf);
            auto fi2 = flist.begin();
            for (; fi2 != flist.end(); fi2++) {
                MetaFunction* mf2 = (*fi2);
                if (mf2->name == mf->name && mf != mf2) {
                    funcs.push_back(mf2);
                }
            }

            // Skip constructors
            if (mf->isConstructor == true) {
                cout << "Skipping constructor function " << mf->name << endl;
                continue;
            }

            output += "Variant* metaClass_" + cl->name + "_" + mf->name + "(GigaObject* obj, int argc, Variant** argv) {\n";

            auto fli = funcs.begin();
            for (; fli != funcs.end(); fli++) {
                output += "\tif(argc == " + std::to_string((*fli)->params.size()) + ") {\n";
                auto ai = (*fli)->params.begin();
                if ((*fli)->params.size() > 0) {
                    output += "\t\tif(true";

                    int counter = 0;
                    for (; ai != (*fli)->params.end(); ai++) {
                        output += " && argv[" + std::to_string(counter) + "]->Is" + functionMappings[(*ai)->type] + "()";
                        counter++;
                    }

                    output += ") { \n";
                }

                if ((*fli)->isStatic == false) {
                    // Cast object
                    if (cl->singleton == false) {
                        output += "\t\t\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(obj);\n";
                        output += "\t\t\tGIGA_ASSERT(cobj != 0, \"Object is not of the correct type.\");\n";
                    }
                    else {
                        output += "\t\t\tMetaSystem* metaSystem = GetSystem<MetaSystem>();\n";
                        output += "\t\t\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(metaSystem->GetSingleton(\"" + cl->name + "\"));\n";
                        output += "\t\t\tGIGA_ASSERT(cobj != 0, \"Singleton class type not found.\");\n";
                    }

                    output += "\n";

                    if ((*fli)->returnType != -1) {
                        if((*fli)->returnType == Variant::VAR_VARIANT) {
                            output += "\t\t\treturn((";
                        }
                        else {
                            output += "\t\t\treturn(new Variant(";
                        }
                    }
                    else {
                        output += "\t\t\t";
                    }
                }
                else {
                    if ((*fli)->returnType != -1) {
                        if((*fli)->returnType == Variant::VAR_VARIANT) {
                            output += "\t\t\treturn((";
                        }
                        else {
                            output += "\t\t\treturn(new Variant(";
                        }
                    }
                    else {
                        output += "\t\t\t";
                    }
                }

                // Function call
                if ((*fli)->isStatic == false) {
                    output += "cobj->" + (*fli)->name + "(";
                }
                else {
                    output += "\t\t\t" + cl->name + "::" + (*fli)->name + "(";
                }
                ai = (*fli)->params.begin();
                argc = 0;
                for (; ai != (*fli)->params.end(); ai++) {
                    output += "argv[" + std::to_string(argc) + "]->As" + functionMappings[(*ai)->type];
                    if ((*ai)->type == Variant::VAR_OBJECT) {
                        output += "<" + (*ai)->typeStr + ">";
                    }
                    output += "(),";
                    argc++;
                }
                // Trim final comma
                if (argc > 0) {
                    output = output.substr(0, output.length() - 1);
                }
                else {
                    //output += ")";
                }

                output += ")";
                if ((*fli)->returnType == -1) {
                    output += ";\n\t\t\treturn(new Variant(0));";
                }
                else {
                    if ((*fli)->isStatic == false) {
                        output += "));";
                    }
                    else {
                        if ((*fli)->returnType == -1) {
                            output += ";";
                        }
                        else {
                            output += "));";
                        }
                    }
                }

                if ((*fli)->params.size() > 0) {
                    output += "\n\t\t}";
                }

                output += "\n\t}\n";
            }

            output += "\n\treturn(new Variant(0));\n\n}\n";
        }

        functionData[cl->name] = output.substr(startOffset);
    }

    /*auto fii = functionData.begin();
    for(; fii != functionData.end(); fii++) {
        output += fii->second;
    }*/

    // Registration
    output += "void MetaData::RegisterMetaFunctions() {\n\tMetaSystem* metaSystem = GetSystem<MetaSystem>();\n\n";
    it = classes.begin();
    int typeID = 10;
    for (; it != classes.end(); it++) {
        startOffset = output.length();
        MetaClass* cl = it->second;

        std::map<std::string, bool>::iterator ei = exportGigaClasses.find(cl->name);
        if (ei == exportGigaClasses.end()) {
            continue;
        }
        
        if(ei->second == false) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }

        output += "\tMeta::Class* metaClass" + cl->name + " = new Meta::Class();\n";
        if (cl->hasPublicConstructor == true) {
            output += "\tmetaClass" + cl->name + "->RegisterConstructor<" + cl->name + ">();\n";
        }
        output += "\tmetaClass" + cl->name + "->name = \"" + cl->name + "\";\n";
        output += "\tmetaClass" + cl->name + "->typeID = " + std::to_string(typeID) + ";\n";
        output += "\tmetaClass" + cl->name + "->singleton = " + (cl->singleton == true ? "true" : "false") + ";\n";
        output += "\tmetaClass" + cl->name + "->native = true;\n";
        
        auto pci = cl->inheritsFrom.begin();
        for(; pci != cl->inheritsFrom.end(); pci++) {
            output += "\tmetaClass" + cl->name + "->inheritsFrom.push_back(\"" + *pci + "\");\n";
        }

        output += "\n";

        auto flist = cl->GetFunctions();
        auto fi = flist.begin();
        std::map<std::string, bool> funcNames;
        for (; fi != flist.end(); fi++) {
            MetaFunction* mf = (*fi);

            if (funcNames.find(mf->name) != funcNames.end()) {
                continue;
            }

            std::string funcName = mf->isConstructor ? "New" : mf->name;
            output += "\tMeta::Function* metaFunc_" + cl->name + "_" + funcName + " = new Meta::Function();\n";
            output += "\tmetaFunc_" + cl->name + "_" + funcName + "->name = \"" + funcName + "\";\n";
            output += "\tmetaFunc_" + cl->name + "_" + funcName + "->func = metaClass_" + cl->name + "_" + funcName + ";\n";
            output += "\tmetaFunc_" + cl->name + "_" + funcName + "->isStatic = " + (mf->isStatic ? "true" : "false") + ";\n";
            output += "\tmetaFunc_" + cl->name + "_" + funcName + "->isConstructor = " + (mf->isConstructor ? "true" : "false") + ";\n";

            output += "\tmetaClass" + cl->name + "->AddFunction(metaFunc_" + cl->name + "_" + funcName + ");\n\n";
            funcNames[mf->name] = true;
        }

        auto vlist = cl->GetVariables();
        auto vi = vlist.begin();
        for (; vi != vlist.end(); vi++) {
            MetaVariable* mv = (*vi);

            output += "\tMeta::Variable* metaVar_" + cl->name + "_" + mv->name + " = new Meta::Variable();\n";
            output += "\tmetaVar_" + cl->name + "_" + mv->name + "->name = \"" + mv->name + "\";\n";
    
            if(mv->getter)
                output += "\tmetaVar_" + cl->name + "_" + mv->name + "->getter = meta_" + cl->name + "_" + mv->name + "_get;\n";
    
            if(mv->setter)
                output += "\tmetaVar_" + cl->name + "_" + mv->name + "->setter = meta_" + cl->name + "_" + mv->name + "_set;\n";

            output += "\tmetaClass" + cl->name + "->AddVariable(metaVar_" + cl->name + "_" + mv->name + ");\n\n";
        }

        output += "\tmetaSystem->AddClass(metaClass" + cl->name + ");\n\n";

        typeID += 10;

        /** TODO: Serialized variables
        std::map<std::string, MetaClass::MetaVariable*> serialized;

        std::map<std::string, MetaClass::MetaVariable*>::iterator vi = cl->variables.begin();
        for (; vi != cl->variables.end(); vi++) {
            if(vi->second->serialize == true) {
                serialized[vi->first] = vi->second;
            }

            if(vi->second->get == false && vi->second->set == false) {
                continue;
            }

            output += "\tmetaSystem->RegisterVariable(\"" + cl->name + "\", \"" + vi->second->name + "\", ";
            output += vi->second->get ? "meta_" + cl->name + "_" + vi->second->name + "_get" : "0";
            output += ", ";
            output += vi->second->set ? "meta_" + cl->name + "_" + vi->second->name + "_set" : "0";
            output += ");\n";
        }*/

        /*if(serialized.size()) {
         std::string objectName = "drt" + cl->name;
         output += "\n\tDataRecordType* " + objectName + " = new DataRecordType(\"" + cl->name + "\");\n";
         output += "\t" + objectName + "->SetPrimaryKey(\"" + cl->name + "_id\");\n";
         vi = serialized.begin();
         for (; vi != serialized.end(); vi++) {
         output += "\t" + objectName + "->AddKey(\"" + vi->second->name + "\", " + std::to_string(vi->second->type) + ", " +
         std::string((vi->second->noneditable == true) ? "false" : "true") + ");\n";
         }

         output += "\n\tDataRecordType::Register<" + cl->name + ">(\"" + cl->name + "\", " + objectName + ");\n";
         }*/

        if (flist.size()) {
            output += "\n";
        }

        registrationData[cl->name] = output.substr(startOffset);
    }

    /*auto rii = registrationData.begin();
    for(; rii != registrationData.end(); rii++) {
        output += rii->second;
    }*/
    output += "}\n\n";

    std::string autogeneratedFolder = path + "/Source/Autogenerated";

    cout << "Writing output to " << autogeneratedFolder << endl;
    mymkdir(autogeneratedFolder.c_str());

    // Write to metadata file
    std::string outFile = autogeneratedFolder + "/MetaData.cpp";
    fp = fopen(outFile.c_str(), "w");
    if (fp != 0) {
        fwrite(output.data(), output.size(), 1, fp);
        fclose(fp);
    }
    else {
        cout << "Error writing to output file." << std::endl;
    }

    // Create Mono bindings
    output = "#include <Core/MetaSystem.h>\n#include <Core/Application.h>\n";
    sourceDir = path + "/Source/Engine/";
    for (size_t i = 0; i < addlIncludes.size(); i++) {
        std::string include = addlIncludes[i].substr(sourceDir.length());
        output += "#include <" + include + ">\n";
    }
    output += "\n";

    // Write new functions for each meta class
    it = classes.begin();
    for (; it != classes.end(); it++) {
        MetaClass* cl = it->second;

        cout << "Checking class " << cl->name << endl;

        std::map<std::string, bool>::iterator ei = exportGigaClasses.find(cl->name);
        if (ei == exportGigaClasses.end()) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }
        
        if(ei->second == false) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }

        std::map<std::string, bool>::iterator singleton = singletonClasses.find(cl->name);
        if (singleton != singletonClasses.end()) {
            cl->singleton = true;
        }

        // Process inheritance
        AddInheritedClasses(cl, cl->name);

        startOffset = output.length();

        // Variables
        auto vlist = cl->GetVariables();
        std::cout << "Has " << (int)vlist.size() << " variables." << endl;
        auto vi = vlist.begin();
        for (; vi != vlist.end(); vi++) {
            std::cout << "Check variable " << (*vi)->name << endl;
            MetaVariable* mp = (*vi);
            if (mp->getter) {
                std::string returnType = mp->typeStr;
                switch(mp->type) {
                    case Variant::VAR_STRING:
                        returnType = "MonoString*";
                        break;
                    case Variant::VAR_OBJECT:
                    case Variant::VAR_VECTOR2:
                    case Variant::VAR_VECTOR3:
                    case Variant::VAR_VECTOR4:
                    case Variant::VAR_QUATERNION:
                        returnType = "MonoObject*";
                        break;
                    case Variant::VAR_ARRAY:
                        returnType = "MonoArray*";
                    default:
                        returnType = mp->typeStr;
                };
    
                output += returnType + " mono_" + cl->name + "_" + mp->name + "_get(MonoObject* mobj) {\n";
                output += "\tScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();\n\n";
                output += "\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(scriptingSystem->GetLocalObject(mobj));\n";
                output += "\tGIGA_ASSERT(cobj != 0, \"Object is not of the correct type.\");\n";

                switch(mp->type) {
                    case Variant::VAR_STRING:
                        output += "\treturn(mono_string_new(mono_domain_get(), cobj->" + mp->name + ".c_str()));\n";
                        break;
                    case Variant::VAR_OBJECT:
                        output += "\treturn(scriptingSystem->GetRemoteObject(dynamic_cast<GigaObject*>(cobj->" + mp->name + ")));\n";
                        break;
                    case Variant::VAR_VECTOR2:
                    case Variant::VAR_VECTOR3:
                    case Variant::VAR_VECTOR4:
                    case Variant::VAR_QUATERNION:
                    case Variant::VAR_ARRAY:
                        output += "return(scriptingSystem->VariantToMonoObject(new Variant(cobj->" + mp->name + ")));\n";
                        break;
                    default:
                        output += "\treturn(cobj->" + mp->name + ");\n";
                };
                
                output += "}\n\n";
            }

            if (mp->setter) {
                std::string paramType = mp->typeStr;
                switch(mp->type) {
                    case Variant::VAR_STRING:
                        paramType = "MonoString*";
                        break;
                    case Variant::VAR_OBJECT:
                    case Variant::VAR_VECTOR2:
                    case Variant::VAR_VECTOR3:
                    case Variant::VAR_VECTOR4:
                    case Variant::VAR_QUATERNION:
                    case Variant::VAR_VARIANT:
                        paramType = "MonoObject*";
                        break;
                    case Variant::VAR_ARRAY:
                        paramType = "MonoArray*";
                    default:
                        paramType = mp->typeStr;
                };
    
                output += "void mono_" + cl->name + "_" + mp->name + "_set(MonoObject* mobj, " + paramType + " value) {\n";
                output += "\tScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();\n\n";
                output += "\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(scriptingSystem->GetLocalObject(mobj));\n";
                output += "\tGIGA_ASSERT(cobj != 0, \"Object is not of the correct type.\");\n";

                switch(mp->type) {
                    case Variant::VAR_STRING:
                        output += "\tcobj->" + mp->name + " = mono_string_to_utf8(value);\n";
                        break;
                    case Variant::VAR_OBJECT:
                        output += "\tcobj->" + mp->name + " = scriptingSystem->GetLocalObject(value);\n";
                        break;
                    case Variant::VAR_VECTOR2:
                    case Variant::VAR_VECTOR3:
                    case Variant::VAR_VECTOR4:
                    case Variant::VAR_QUATERNION:
                    case Variant::VAR_VARIANT:
                    case Variant::VAR_ARRAY:
                        output += "\tcobj->" + mp->name + " = scriptingSystem->MonoObjectToVariant(value)->As" + functionMappings[mp->type] + "();\n";
                        break;
                    default:
                        output += "\tcobj->" + mp->name + " = value;\n";
                };
    
                output += "}\n\n";
            }
        }

        // Constructors
        auto flist = cl->GetFunctions();

        // Functions
        cout << "Has " << (int)flist.size() << " functions." << endl;
        flist = cl->GetFunctions();
        auto fi = flist.begin();
        std::map<std::string, bool> funcNames;
        for (; fi != flist.end(); fi++) {
            MetaFunction* mf = (*fi);

            // Skip constructors
            if (mf->isConstructor == true) {
                cout << "Skipping constructor function " << mf->name << endl;
                continue;
            }
    
            std::string params;
            std::string paramsShort;
            if((*fi)->isStatic == false && cl->singleton == false) {
                params += "MonoObject* mobj";
            }
            
            auto pi = (*fi)->params.begin();
            for(; pi != (*fi)->params.end(); pi++) {
                if(params.length() > 0) params += ",";
                if(paramsShort.length() > 0) paramsShort += ",";
                
                std::string paramType;
                std::string shortType;
                switch((*pi)->type) {
                    case Variant::VAR_STRING:
                        paramType = "MonoString*";
                        shortType = "mono_string_to_utf8(" + (*pi)->name + ")";
                        break;
                    case Variant::VAR_OBJECT:
                        paramType = "MonoObject*";
                        shortType = "dynamic_cast<" + (*pi)->typeStr + ">(scriptingSystem->GetLocalObject(" + (*pi)->name + "))";
                        break;
                    case Variant::VAR_VECTOR2:
                    case Variant::VAR_VECTOR3:
                    case Variant::VAR_VECTOR4:
                    case Variant::VAR_QUATERNION:
                    case Variant::VAR_VARIANT:
                        paramType = "MonoObject*";
                        shortType = "scriptingSystem->MonoObjectToVariant(" + (*pi)->name + ")->As" + functionMappings[(*pi)->type] + "()";
                        break;
                    case Variant::VAR_ARRAY:
                        paramType = "MonoArray*";
                        shortType = "scriptingSystem->MonoObjectToVariant(" + (*pi)->name + ")->As" + functionMappings[(*pi)->type] + "()";
                        break;
                    default:
                        paramType = (*pi)->typeStr;
                        shortType = (*pi)->name;
                };
            
                params += paramType + " " + (*pi)->name;
                paramsShort += shortType;
            }

            std::string returnType;
            switch((*fi)->returnType) {
                case -1:
                    returnType = "void";
                    break;
                case Variant::VAR_STRING:
                    returnType = "MonoString*";
                    break;
                case Variant::VAR_OBJECT:
                case Variant::VAR_VECTOR2:
                case Variant::VAR_VECTOR3:
                case Variant::VAR_VECTOR4:
                case Variant::VAR_QUATERNION:
                case Variant::VAR_VARIANT:
                    returnType = "MonoObject*";
                    break;
                case Variant::VAR_ARRAY:
                    returnType = "MonoObject*";
                    break;
                default:
                    returnType = (*fi)->strReturnType;
            };
    
            // Create a unique function name
            std::string origFuncName = "monoClass_" + cl->name + "_" + mf->name;
            std::string funcName = "monoClass_" + cl->name + "_" + mf->name;
            auto fni = funcNames.find(funcName);
            int counter = 2;
            while(fni != funcNames.end()) {
                funcName = origFuncName + std::to_string(counter);
                fni = funcNames.find(funcName);
                counter++;
            }
            funcNames[funcName] = true;
            
            output += returnType + " " + funcName + "(" + params + ") {\n";
            output += "\tScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();\n\n";

            if ((*fi)->isStatic == false) {
                // Cast object
                if (cl->singleton == false) {
                    output += "\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(scriptingSystem->GetLocalObject(mobj));\n";
                    output += "\tGIGA_ASSERT(cobj != 0, \"Object is not of the correct type.\");\n";
                }
                else {
                    output += "\tMetaSystem* metaSystem = GetSystem<MetaSystem>();\n";
                    output += "\t" + cl->name + "* cobj = dynamic_cast<" + cl->name + "*>(metaSystem->GetSingleton(\"" + cl->name + "\"));\n";
                    output += "\tGIGA_ASSERT(cobj != 0, \"Singleton class type not found.\");\n";
                }
            }
            
            output += "\n\t";
            
            // Return value
            returnType = (*fi)->strReturnType;
            if((*fi)->returnType == Variant::VAR_ARRAY || (*fi)->returnType == Variant::VAR_OBJECT) {
                returnType = returnType.substr(0, returnType.length() - 2);
            }
            
            if((*fi)->returnType != -1) {
                std::string retType = (*fi)->strReturnType;
                if((*fi)->returnType == Variant::VAR_ARRAY) {
                    retType = "Array<" + retType + ">";
                }
                output += retType + " retval = ";
            }

            // Function call
            if ((*fi)->isStatic == false) {
                output += "cobj->" + (*fi)->name + "(";
            }
            else {
                output += cl->name + "::" + (*fi)->name + "(";
            }
            
            output += paramsShort;

            output += ");\n\n";
            
            // Return
            switch((*fi)->returnType) {
                case -1:
                    break;
                case Variant::VAR_STRING:
                    output += "\treturn(mono_string_new(mono_domain_get(), retval.c_str()));";
                    break;
                case Variant::VAR_OBJECT:
                    output += "\treturn(scriptingSystem->GetRemoteObject(retval));";
                    break;
                case Variant::VAR_VECTOR2:
                case Variant::VAR_VECTOR3:
                case Variant::VAR_VECTOR4:
                case Variant::VAR_QUATERNION:
                    output += "\treturn(scriptingSystem->VariantToMonoObject(new Variant(retval)));";
                    break;
                case Variant::VAR_ARRAY:
                    output += "\treturn(scriptingSystem->VariantToMonoObject(new Variant(retval), \"" + returnType + "\"));";
                    break;
                case Variant::VAR_VARIANT:
                    output += "\treturn(scriptingSystem->VariantToMonoObject(retval));";
                    break;
                default:
                    output += "\treturn(retval);";
            };
            
            output += "\n}\n\n";
        }
    }
    
    // Registration
    output += "void RegisterMonoFunctions() {\n\n";
    it = classes.begin();
    for (; it != classes.end(); it++) {
        MetaClass* cl = it->second;
    
        std::map<std::string, bool>::iterator ei = exportGigaClasses.find(cl->name);
        if (ei == exportGigaClasses.end()) {
            continue;
        }
        
        if(ei->second == false) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }

        auto flist = cl->GetFunctions();
        auto fi = flist.begin();
        std::map<std::string, bool> funcNames;
        for (; fi != flist.end(); fi++) {
            MetaFunction* mf = (*fi);
            if(mf->isConstructor) continue;
        
            // Create a unique function name
            std::string origFuncName = "monoClass_" + cl->name + "_" + mf->name;
            std::string funcName = "monoClass_" + cl->name + "_" + mf->name;
            auto fni = funcNames.find(funcName);
            int counter = 2;
            while(fni != funcNames.end()) {
                funcName = origFuncName + std::to_string(counter);
                fni = funcNames.find(funcName);
                counter++;
            }
            funcNames[funcName] = true;
    
            // Get signature
            std::string params;
            auto pi = (*fi)->params.begin();
            for(; pi != (*fi)->params.end(); pi++) {
                if(params.length() > 0) params += ",";
            
                std::string paramType = (*pi)->typeStr;
                auto mi = csharpMappings.find((*pi)->type);
                if(mi != csharpMappings.end()) paramType = mi->second;
                if((*pi)->type == Variant::VAR_OBJECT) paramType = paramType.substr(0, paramType.length() - 2);
                if((*pi)->type == Variant::VAR_VECTOR2) paramType = "GIGA." + paramType;
                if((*pi)->type == Variant::VAR_VECTOR3) paramType = "GIGA." + paramType;
                if((*pi)->type == Variant::VAR_VECTOR4) paramType = "GIGA." + paramType;
                if((*pi)->type == Variant::VAR_QUATERNION) paramType = "GIGA." + paramType;
                if((*pi)->type == Variant::VAR_OBJECT) paramType = "GIGA." + paramType;
            
                params += paramType;
            }
    
            output += "\tmono_add_internal_call(\"GIGA." + cl->name + "::" + (*fi)->name + "(" + params + ")\", (void*)" + funcName + ");\n";
        }
    
        // Variables
        auto vlist = cl->GetVariables();
        std::cout << "Has " << (int)vlist.size() << " variables." << endl;
        auto vi = vlist.begin();
        for (; vi != vlist.end(); vi++) {
            if((*vi)->getter)
                output += "\tmono_add_internal_call(\"GIGA." + cl->name + "::get_" + (*vi)->name + "\", (void*)mono_" + cl->name + "_" + (*vi)->name + "_get);\n";
    
            if((*vi)->setter)
                output += "\tmono_add_internal_call(\"GIGA." + cl->name + "::set_" + (*vi)->name + "\", (void*)mono_" + cl->name + "_" + (*vi)->name + "_set);\n";
        }
    }
    output += "}\n";
    
    // Write to metadata file
    outFile = autogeneratedFolder + "/MonoBindings.cpp";
    fp = fopen(outFile.c_str(), "w");
    if (fp != 0) {
        fwrite(output.data(), output.size(), 1, fp);
        fclose(fp);
    }
    else {
        cout << "Error writing to output file." << std::endl;
    }
    
    // Create C# bindings
    autogeneratedFolder = path + "/Source/Autogenerated/CSharp";

    cout << "Writing output to " << autogeneratedFolder << endl;
    mymkdir(autogeneratedFolder.c_str());

    // Write CSharp files
    it = classes.begin();
    for (; it != classes.end(); it++) {
        MetaClass* cl = it->second;
        
        // Find the highest level exported inheritance class
        std::string inheritedClass = GetInheritance(cl->name);
        
        if(inheritedClass.length()) {
            inheritedClass = ": " + inheritedClass;
        }
        
        if(inheritedClass.length() == 0 && cl->name != "GigaObject") {
            inheritedClass = ": GigaObject";
        }
    
        output = "using System;\nusing System.Runtime.CompilerServices;\n\n";
        output += "namespace GIGA {\n";
        output += "\tpublic partial class " + cl->name + inheritedClass + " {\n";

        cout << "Checking class " << cl->name << endl;

        std::map<std::string, bool>::iterator ei = exportGigaClasses.find(cl->name);
        if (ei == exportGigaClasses.end()) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }
        
        if(ei->second == false) {
            cout << "Non-exportable, skipping" << endl;
            continue;
        }

        std::map<std::string, bool>::iterator singleton = singletonClasses.find(cl->name);
        if (singleton != singletonClasses.end()) {
            cl->singleton = true;
        }

        // Process inheritance
        AddInheritedClasses(cl, cl->name);

        // Variables
        auto vlist = cl->GetVariables();
        std::cout << "Has " << (int)vlist.size() << " variables." << endl;
        auto vi = vlist.begin();
        for (; vi != vlist.end(); vi++) {
            std::cout << "Check variable " << (*vi)->name << endl;
            MetaVariable* mp = (*vi);
    
            if(mp->getter == 0 && mp->setter == 0) continue;

            std::string paramType = mp->typeStr;
            auto mi = csharpMappings.find(mp->type);
            if(mi != csharpMappings.end()) paramType = mi->second;
            if(mp->type == Variant::VAR_OBJECT) {
                paramType = paramType.substr(0, paramType.length() - 2);
            }
    
            output += "\t\tpublic " + paramType + " " + mp->name + " {\n";
    
            if (mp->getter) {
                output += "\t\t\t[MethodImpl(MethodImplOptions.InternalCall)]\n\t\t\tget;\n";
            }

            if (mp->setter) {
                output += "\t\t\t[MethodImpl(MethodImplOptions.InternalCall)]\n\t\t\tset;\n";
            }
    
            output += "\t\t}\n\n";
        }

        // Constructors
        auto flist = cl->GetFunctions();

        // Functions
        cout << "Has " << (int)flist.size() << " functions." << endl;
        flist = cl->GetFunctions();
        auto fi = flist.begin();
        for (; fi != flist.end(); fi++) {
            MetaFunction* mf = (*fi);

            // Skip constructors
            if (mf->isConstructor == true) {
                cout << "Skipping constructor function " << mf->name << endl;
                continue;
            }
    
            output += "\t\t[MethodImplAttribute(MethodImplOptions.InternalCall)]\n";
            output += "\t\tpublic extern ";
            if(cl->singleton || (*fi)->isStatic) output += "static ";
            else output += "virtual ";
    
            std::string returnType = "void";
            auto mi = csharpMappings.find((*fi)->returnType);
            if(mi != csharpMappings.end()) returnType = mi->second;
            
            if((*fi)->returnType == Variant::VAR_OBJECT || (*fi)->returnType == Variant::VAR_ARRAY) returnType = (*fi)->strReturnType;
            if((*fi)->returnType == Variant::VAR_OBJECT) returnType = returnType.substr(0, returnType.length() - 2);
            if((*fi)->returnType == Variant::VAR_ARRAY) {
                if(returnType.find("*") != std::string::npos)
                    returnType = returnType.substr(0, returnType.length() - 2);
                if(returnType == "std::string")
                    returnType = "string";
                returnType += "[]";
            }
            output += returnType + " " + (*fi)->name;
            
            std::string params;
                    
            auto pi = (*fi)->params.begin();
            for(; pi != (*fi)->params.end(); pi++) {
                if(params.length() > 0) params += ",";
                    
                std::string paramType = (*pi)->typeStr;
                mi = csharpMappings.find((*pi)->type);
                if(mi != csharpMappings.end()) paramType = mi->second;
                if((*pi)->type == Variant::VAR_OBJECT) paramType = paramType.substr(0, paramType.length() - 2);
                if((*pi)->type == Variant::VAR_ARRAY) {
                    if(paramType.find("*") != std::string::npos)
                        paramType = paramType.substr(0, paramType.length() - 2);
                    if(paramType == "std::string")
                        paramType = "string";
                    paramType += "[]";
                }
    
                params += paramType + " " + (*pi)->name;
            }

            output += "(" + params + ");\n\n";
        }
    
        output += "\t}\n}";

        // Write to biindings file
        outFile = autogeneratedFolder + "/" + cl->name + ".cs";
        fp = fopen(outFile.c_str(), "w");
        if (fp != 0) {
            fwrite(output.data(), output.size(), 1, fp);
            fclose(fp);
        }
        else {
            cout << "Error writing to output file." << std::endl;
        }
    }

    // Clear
    dataLoader->Delete("classes");
    dataLoader->Delete("functions");
    dataLoader->Delete("params");
    dataLoader->Delete("inheritance");
    dataLoader->Delete("files");
    dataLoader->Delete("variables");

    // Save data
    auto cli = classes.begin();
    Array<DataRecord*> cldrs;
    Array<DataRecord*> fndrs;
    Array<DataRecord*> pmdrs;
    Array<DataRecord*> inhdrs;
    Array<DataRecord*> vardrs;
    for (; cli != classes.end(); cli++) {
        std::map<std::string, bool>::iterator ei = exportGigaClasses.find(cli->second->name);
        if (ei == exportGigaClasses.end()) {
            continue;
        }

        DataRecord* dr = new DataRecord();

        dr->Set("className", new Variant(cli->first));
        dr->Set("singleton", new Variant(cli->second->singleton));
        dr->Set("hasPublicConstructor", new Variant(cli->second->hasPublicConstructor));
        dr->Set("exported", new Variant((bool)exportGigaClasses[cli->first]));
        cldrs.push_back(dr);
        
        auto fns = cli->second->GetFunctions();
        auto fni = fns.begin();
        std::map<std::string, int> offsets;
        for (; fni != fns.end(); fni++) {
            DataRecord* fdr = new DataRecord();
            fdr->Set("className", new Variant(cli->first));
            fdr->Set("funcName", new Variant((*fni)->name));
            fdr->Set("returnType", new Variant((*fni)->returnType));
            fdr->Set("strReturnType", new Variant((*fni)->strReturnType));
            fdr->Set("isConstructor", new Variant((*fni)->isConstructor));
            fdr->Set("isStatic", new Variant((*fni)->isStatic));

            int offset = 0;
            if (offsets.find((*fni)->name) != offsets.end()) {
                offset = offsets[(*fni)->name];
            }

            fdr->Set("offset", new Variant((uint32_t)offset));

            fndrs.push_back(fdr);

            auto pms = (*fni)->params;
            auto pmi = pms.begin();
            for (; pmi != pms.end(); pmi++) {
                DataRecord* pdr = new DataRecord();
                pdr->Set("className", new Variant(cli->first));
                pdr->Set("funcName", new Variant((*fni)->name));
                pdr->Set("offset", new Variant((uint32_t)offset));
                pdr->Set("paramName", new Variant((*pmi)->name));
                pdr->Set("type", new Variant((*pmi)->type));
                pdr->Set("typeStr", new Variant((*pmi)->typeStr));

                pmdrs.push_back(pdr);
            }

            offset++;
            offsets[(*fni)->name] = offset;
        }

        auto vars = cli->second->GetVariables();
        auto vi = vars.begin();
        for (; vi != vars.end(); vi++) {
            DataRecord* fdr = new DataRecord();
            fdr->Set("className", new Variant(cli->first));
            fdr->Set("varName", new Variant((*vi)->name));
            fdr->Set("type", new Variant((*vi)->type));
            fdr->Set("typeStr", new Variant((*vi)->typeStr));
            fdr->Set("getter", new Variant((*vi)->getter != 0));
            fdr->Set("setter", new Variant((*vi)->setter != 0));

            vardrs.push_back(fdr);
        }
    }
    
    // Inheritance (global)
    auto inhi = inheritances.begin();
    for(; inhi != inheritances.end(); inhi++) {
        auto ii2 = inhi->second.begin();
        for(; ii2 != inhi->second.end(); ii2++) {
            DataRecord* dr2 = new DataRecord();
            dr2->Set("className", new Variant(inhi->first));
            dr2->Set("inheritsFrom", new Variant(*ii2));
            
            inhdrs.push_back(dr2);
        }
    }


    dataLoader->SaveRecords("classes", cldrs);
    dataLoader->SaveRecords("functions", fndrs);
    dataLoader->SaveRecords("params", pmdrs);
    dataLoader->SaveRecords("inheritance", inhdrs);
    dataLoader->SaveRecords("variables", vardrs);

    auto fili = fileHashes.begin();
    Array<DataRecord*> fdrs;
    for (; fili != fileHashes.end(); fili++) {
        DataRecord* dr = new DataRecord();
        dr->Set("filename", new Variant(fili->first));
        dr->Set("hash", new Variant(fili->second));
        fdrs.push_back(dr);
    }

    dataLoader->SaveRecords("files", fdrs);
    dataLoader->Close();

    clang_disposeIndex(cindex);

    return(0);
}
