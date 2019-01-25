// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME ddaschannelDictionary

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "ddaschannel.h"
#include "DDASEvent.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_ddaschannel(void *p = 0);
   static void *newArray_ddaschannel(Long_t size, void *p);
   static void delete_ddaschannel(void *p);
   static void deleteArray_ddaschannel(void *p);
   static void destruct_ddaschannel(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ddaschannel*)
   {
      ::ddaschannel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ddaschannel >(0);
      static ::ROOT::TGenericClassInfo 
         instance("ddaschannel", ::ddaschannel::Class_Version(), "ddaschannel.h", 41,
                  typeid(::ddaschannel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ddaschannel::Dictionary, isa_proxy, 4,
                  sizeof(::ddaschannel) );
      instance.SetNew(&new_ddaschannel);
      instance.SetNewArray(&newArray_ddaschannel);
      instance.SetDelete(&delete_ddaschannel);
      instance.SetDeleteArray(&deleteArray_ddaschannel);
      instance.SetDestructor(&destruct_ddaschannel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ddaschannel*)
   {
      return GenerateInitInstanceLocal((::ddaschannel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ddaschannel*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_DDASEvent(void *p = 0);
   static void *newArray_DDASEvent(Long_t size, void *p);
   static void delete_DDASEvent(void *p);
   static void deleteArray_DDASEvent(void *p);
   static void destruct_DDASEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DDASEvent*)
   {
      ::DDASEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::DDASEvent >(0);
      static ::ROOT::TGenericClassInfo 
         instance("DDASEvent", ::DDASEvent::Class_Version(), "DDASEvent.h", 26,
                  typeid(::DDASEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::DDASEvent::Dictionary, isa_proxy, 4,
                  sizeof(::DDASEvent) );
      instance.SetNew(&new_DDASEvent);
      instance.SetNewArray(&newArray_DDASEvent);
      instance.SetDelete(&delete_DDASEvent);
      instance.SetDeleteArray(&deleteArray_DDASEvent);
      instance.SetDestructor(&destruct_DDASEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DDASEvent*)
   {
      return GenerateInitInstanceLocal((::DDASEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::DDASEvent*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr ddaschannel::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *ddaschannel::Class_Name()
{
   return "ddaschannel";
}

//______________________________________________________________________________
const char *ddaschannel::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ddaschannel*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int ddaschannel::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ddaschannel*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ddaschannel::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ddaschannel*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ddaschannel::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ddaschannel*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr DDASEvent::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *DDASEvent::Class_Name()
{
   return "DDASEvent";
}

//______________________________________________________________________________
const char *DDASEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DDASEvent*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int DDASEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DDASEvent*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *DDASEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DDASEvent*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *DDASEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DDASEvent*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void ddaschannel::Streamer(TBuffer &R__b)
{
   // Stream an object of class ddaschannel.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(ddaschannel::Class(),this);
   } else {
      R__b.WriteClassBuffer(ddaschannel::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_ddaschannel(void *p) {
      return  p ? new(p) ::ddaschannel : new ::ddaschannel;
   }
   static void *newArray_ddaschannel(Long_t nElements, void *p) {
      return p ? new(p) ::ddaschannel[nElements] : new ::ddaschannel[nElements];
   }
   // Wrapper around operator delete
   static void delete_ddaschannel(void *p) {
      delete ((::ddaschannel*)p);
   }
   static void deleteArray_ddaschannel(void *p) {
      delete [] ((::ddaschannel*)p);
   }
   static void destruct_ddaschannel(void *p) {
      typedef ::ddaschannel current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ddaschannel

//______________________________________________________________________________
void DDASEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class DDASEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(DDASEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(DDASEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_DDASEvent(void *p) {
      return  p ? new(p) ::DDASEvent : new ::DDASEvent;
   }
   static void *newArray_DDASEvent(Long_t nElements, void *p) {
      return p ? new(p) ::DDASEvent[nElements] : new ::DDASEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_DDASEvent(void *p) {
      delete ((::DDASEvent*)p);
   }
   static void deleteArray_DDASEvent(void *p) {
      delete [] ((::DDASEvent*)p);
   }
   static void destruct_DDASEvent(void *p) {
      typedef ::DDASEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::DDASEvent

namespace ROOT {
   static TClass *vectorlEunsignedsPshortgR_Dictionary();
   static void vectorlEunsignedsPshortgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPshortgR(void *p = 0);
   static void *newArray_vectorlEunsignedsPshortgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPshortgR(void *p);
   static void deleteArray_vectorlEunsignedsPshortgR(void *p);
   static void destruct_vectorlEunsignedsPshortgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned short>*)
   {
      vector<unsigned short> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned short>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned short>", -2, "vector", 210,
                  typeid(vector<unsigned short>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPshortgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned short>) );
      instance.SetNew(&new_vectorlEunsignedsPshortgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPshortgR);
      instance.SetDelete(&delete_vectorlEunsignedsPshortgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPshortgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPshortgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned short> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<unsigned short>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPshortgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned short>*)0x0)->GetClass();
      vectorlEunsignedsPshortgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPshortgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPshortgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned short> : new vector<unsigned short>;
   }
   static void *newArray_vectorlEunsignedsPshortgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned short>[nElements] : new vector<unsigned short>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPshortgR(void *p) {
      delete ((vector<unsigned short>*)p);
   }
   static void deleteArray_vectorlEunsignedsPshortgR(void *p) {
      delete [] ((vector<unsigned short>*)p);
   }
   static void destruct_vectorlEunsignedsPshortgR(void *p) {
      typedef vector<unsigned short> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned short>

namespace ROOT {
   static TClass *vectorlEunsignedsPintgR_Dictionary();
   static void vectorlEunsignedsPintgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPintgR(void *p = 0);
   static void *newArray_vectorlEunsignedsPintgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPintgR(void *p);
   static void deleteArray_vectorlEunsignedsPintgR(void *p);
   static void destruct_vectorlEunsignedsPintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned int>*)
   {
      vector<unsigned int> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned int>", -2, "vector", 210,
                  typeid(vector<unsigned int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned int>) );
      instance.SetNew(&new_vectorlEunsignedsPintgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPintgR);
      instance.SetDelete(&delete_vectorlEunsignedsPintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPintgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned int> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<unsigned int>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned int>*)0x0)->GetClass();
      vectorlEunsignedsPintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPintgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned int> : new vector<unsigned int>;
   }
   static void *newArray_vectorlEunsignedsPintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned int>[nElements] : new vector<unsigned int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPintgR(void *p) {
      delete ((vector<unsigned int>*)p);
   }
   static void deleteArray_vectorlEunsignedsPintgR(void *p) {
      delete [] ((vector<unsigned int>*)p);
   }
   static void destruct_vectorlEunsignedsPintgR(void *p) {
      typedef vector<unsigned int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned int>

namespace ROOT {
   static TClass *vectorlEddaschannelmUgR_Dictionary();
   static void vectorlEddaschannelmUgR_TClassManip(TClass*);
   static void *new_vectorlEddaschannelmUgR(void *p = 0);
   static void *newArray_vectorlEddaschannelmUgR(Long_t size, void *p);
   static void delete_vectorlEddaschannelmUgR(void *p);
   static void deleteArray_vectorlEddaschannelmUgR(void *p);
   static void destruct_vectorlEddaschannelmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<ddaschannel*>*)
   {
      vector<ddaschannel*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<ddaschannel*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<ddaschannel*>", -2, "vector", 210,
                  typeid(vector<ddaschannel*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEddaschannelmUgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<ddaschannel*>) );
      instance.SetNew(&new_vectorlEddaschannelmUgR);
      instance.SetNewArray(&newArray_vectorlEddaschannelmUgR);
      instance.SetDelete(&delete_vectorlEddaschannelmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEddaschannelmUgR);
      instance.SetDestructor(&destruct_vectorlEddaschannelmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<ddaschannel*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<ddaschannel*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEddaschannelmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<ddaschannel*>*)0x0)->GetClass();
      vectorlEddaschannelmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEddaschannelmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEddaschannelmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<ddaschannel*> : new vector<ddaschannel*>;
   }
   static void *newArray_vectorlEddaschannelmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<ddaschannel*>[nElements] : new vector<ddaschannel*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEddaschannelmUgR(void *p) {
      delete ((vector<ddaschannel*>*)p);
   }
   static void deleteArray_vectorlEddaschannelmUgR(void *p) {
      delete [] ((vector<ddaschannel*>*)p);
   }
   static void destruct_vectorlEddaschannelmUgR(void *p) {
      typedef vector<ddaschannel*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<ddaschannel*>

namespace {
  void TriggerDictionaryInitialization_ddaschannelDictionary_Impl() {
    static const char* headers[] = {
"ddaschannel.h",
"DDASEvent.h",
0
    };
    static const char* includePaths[] = {
"/opt/nsclddas/nsclddas-2.0-002/include",
"/opt/root-build/include",
"/opt/build/nscldaq-ddas/main/ddasdumper/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "ddaschannelDictionary dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$ddaschannel.h")))  ddaschannel;
namespace std{template <typename _Tp> class __attribute__((annotate("$clingAutoload$bits/allocator.h")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}
class __attribute__((annotate("$clingAutoload$DDASEvent.h")))  DDASEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "ddaschannelDictionary dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "ddaschannel.h"
#include "DDASEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"DDASEvent", payloadCode, "@",
"ddaschannel", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("ddaschannelDictionary",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_ddaschannelDictionary_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_ddaschannelDictionary_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_ddaschannelDictionary() {
  TriggerDictionaryInitialization_ddaschannelDictionary_Impl();
}
