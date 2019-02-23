class IChemistry : public IUnknown
{
public:
	virtual HRESULT __stdcall EfficiencyInCarnotCycleByTempreature(float, float, float*) = 0;
	virtual HRESULT __stdcall EfficiencyInCarnotCycleByHeat(float, float, float*) = 0;
	virtual HRESULT __stdcall EfficiencyInCarnotCycleByWork(float, float, float *) = 0;
};
// {34C85D57-0165-4105-9299-74D881233B0E}
const CLSID CLSID_ChemistryOperation = { 0x34c85d57, 0x165, 0x4105, 0x92, 0x99, 0x74, 0xd8, 0x81, 0x23, 0x3b, 0xe };
// {4CF68D01-084D-4570-98AC-FA9174735C0A}
const IID IID_IChemistry = { 0x4cf68d01, 0x84d, 0x4570, 0x98, 0xac, 0xfa, 0x91, 0x74, 0x73, 0x5c, 0xa };
