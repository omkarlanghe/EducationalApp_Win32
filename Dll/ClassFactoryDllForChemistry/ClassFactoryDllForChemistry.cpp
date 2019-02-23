#define UNICODE
#include<windows.h>
#include<stdio.h>
#include "ClassFactoryDllForChemistry.h"

class CChemistryOperation :public IChemistry
{
private:
	long m_cRef;
public:
	CChemistryOperation(void);
	~CChemistryOperation(void);

	HRESULT __stdcall QueryInterface(REFIID, void **);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);

	HRESULT __stdcall EfficiencyInCarnotCycleByTempreature(float, float, float*);
	HRESULT __stdcall EfficiencyInCarnotCycleByHeat(float, float, float*);
	HRESULT __stdcall EfficiencyInCarnotCycleByWork(float, float, float *);
};

class CChemistryOperationClassFactory : public IClassFactory
{
private:
	long m_cRef;
public:
	CChemistryOperationClassFactory(void);
	~CChemistryOperationClassFactory(void);

	HRESULT __stdcall QueryInterface(REFIID, void **);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);

	HRESULT __stdcall CreateInstance(IUnknown *, REFIID, void **);
	HRESULT __stdcall LockServer(BOOL);
};

long glNumberOfActiveComponents = 0;
long glNumberOfServerLocks = 0;

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID Reserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return(TRUE);
}

CChemistryOperation::CChemistryOperation(void)
{
	m_cRef = 1;
	InterlockedIncrement(&glNumberOfActiveComponents);
}

CChemistryOperation::~CChemistryOperation(void)
{
	InterlockedDecrement(&glNumberOfActiveComponents);
}

HRESULT CChemistryOperation::QueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IUnknown)
		*ppv = static_cast<IChemistry *>(this);
	else if (riid == IID_IChemistry)
		*ppv = static_cast<IChemistry *>(this);
	else
	{
		*ppv = NULL;
		return(E_NOINTERFACE);
	}
	reinterpret_cast<IUnknown *>(*ppv)->AddRef();
	return(S_OK);
}

ULONG CChemistryOperation::AddRef(void)
{
	InterlockedIncrement(&m_cRef);
	return(m_cRef);
}

ULONG CChemistryOperation::Release(void)
{
	InterlockedDecrement(&m_cRef);

	if (m_cRef == 0)
	{
		delete(this);
		return(0);
	}
	return(m_cRef);
}

HRESULT CChemistryOperation::EfficiencyInCarnotCycleByTempreature(float t1, float t2, float *nEfficiency)
{
	TCHAR str[255];

	*nEfficiency = (t2 - t1) / t2;
	/*swprintf_s(str, TEXT("Inside Dll function Efficiency in carnot cycle is using tempreature is %f"), *nEfficiency);
	MessageBox(NULL, str, TEXT("Result of Chemistry"), MB_OK);*/
	return(S_OK);
}

HRESULT CChemistryOperation::EfficiencyInCarnotCycleByHeat(float q1, float q2, float *nEfficiency)
{
	TCHAR str[255];

	*nEfficiency = (q2 - q1) / q2;
	/*swprintf_s(str, TEXT("Inside Dll function Efficiency in carnot cycle is using tempreature is %f"), *nEfficiency);
	MessageBox(NULL, str, TEXT("Result of Chemistry"), MB_OK);*/
	return(S_OK);
}

HRESULT CChemistryOperation::EfficiencyInCarnotCycleByWork(float w, float q2, float *nEfficiency)
{
	TCHAR str[255];

	*nEfficiency = w / q2;
	/*swprintf_s(str, TEXT("Inside Dll function Efficiency in carnot cycle is using tempreature is %f"), *nEfficiency);
	MessageBox(NULL, str, TEXT("Result of Chemistry"), MB_OK);*/
	return(S_OK);
}

CChemistryOperationClassFactory::CChemistryOperationClassFactory(void)
{
	m_cRef = 1;

}
CChemistryOperationClassFactory::~CChemistryOperationClassFactory(void)
{

}

HRESULT CChemistryOperationClassFactory::QueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IUnknown)
		*ppv = static_cast<IClassFactory *>(this);
	else if (riid == IID_IClassFactory)
		*ppv = static_cast<IClassFactory *>(this);
	else
	{
		*ppv = NULL;
		return(E_NOINTERFACE);
	}

	reinterpret_cast<IUnknown *>(*ppv)->AddRef();
	return(S_OK);
}

ULONG CChemistryOperationClassFactory::AddRef(void)
{
	InterlockedIncrement(&m_cRef);
	return(m_cRef);
}

ULONG CChemistryOperationClassFactory::Release(void)
{
	InterlockedDecrement(&m_cRef);

	if (m_cRef == 0)
	{
		delete(this);
		return(0);
	}
	return(m_cRef);
}

HRESULT CChemistryOperationClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
	CChemistryOperation *pCChemistryOperation = NULL;
	HRESULT hr;

	if (pUnkOuter != NULL)
		return(CLASS_E_NOAGGREGATION);

	pCChemistryOperation = new CChemistryOperation;

	if (pCChemistryOperation == NULL)
		return(E_OUTOFMEMORY);

	hr = pCChemistryOperation->QueryInterface(riid, ppv);

	pCChemistryOperation->Release();
	return(hr);
}

HRESULT CChemistryOperationClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		InterlockedIncrement(&glNumberOfServerLocks);
	else
		InterlockedDecrement(&glNumberOfServerLocks);
	
	return(S_OK);
}

HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
	CChemistryOperationClassFactory *pcCChemistryOperationClassFactory = NULL;
	HRESULT hr;

	if (rclsid != CLSID_ChemistryOperation)
		return(CLASS_E_CLASSNOTAVAILABLE);

	pcCChemistryOperationClassFactory = new CChemistryOperationClassFactory;
	
	if (pcCChemistryOperationClassFactory == NULL)
		return(E_OUTOFMEMORY);

	hr = pcCChemistryOperationClassFactory->QueryInterface(riid, ppv);
	pcCChemistryOperationClassFactory->Release();

	return(hr);
}

HRESULT __stdcall DllCanUnloadNow(void)
{
	if ((glNumberOfActiveComponents == 0) && (glNumberOfServerLocks == 0))
		return(S_OK);
	else
		return(S_FALSE);
}
