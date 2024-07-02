#include "pch.h"

#include <string>

#include "MSXStruc.h"

// we don't want C++ name-mangling
#ifdef __cplusplus
extern "C" {
#endif

    const int NMyFuncs = 3; // three functions
    // arg constants for MyMov
    const int NMyMovArgs = 3; // 3 arguments
    const int NMyMovCustStrings = 8; // 8 custom strings for third arg
    // arg constants for AddN
    const int NAddNArgs = 2; // 2 arguments
    // arg constants for SumArrays
    const int NSumArraysNArgs = 1; // 1 argument


    BOOL __stdcall MSXInfo(MSXDLLDef* a_psDLLDef)
    {
        // copy in your copyright information...
        strncpy_s(a_psDLLDef->szCopyright, "C Sample MSX DLL, Copyright (c) MetaStock, 2024",
            sizeof(a_psDLLDef->szCopyright) - 1);
        // set the number of functions we are exporting
        // (don't forget to add them to your .def file)
        a_psDLLDef->iNFuncs = NMyFuncs;
        a_psDLLDef->iVersion = MSX_VERSION;
        return MSX_SUCCESS;
    }


    BOOL __stdcall MSXNthFunction(int a_iNthFunc, MSXFuncDef* a_psFuncDef)
    {
        BOOL l_bRtrn = MSX_SUCCESS;

        switch (a_iNthFunc) {
        case 0:
            strcpy_s(a_psFuncDef->szFunctionName, "MyMov");
            strcpy_s(a_psFuncDef->szFunctionDescription, "My Moving Average");
            a_psFuncDef->iNArguments = NMyMovArgs;
            break;
        case 1:
            strcpy_s(a_psFuncDef->szFunctionName, "AddN");
            strcpy_s(a_psFuncDef->szFunctionDescription, "Add 'n' to Any DataArray");
            a_psFuncDef->iNArguments = NAddNArgs;
            break;
        case 2:
            strcpy_s(a_psFuncDef->szFunctionName, "SumArrays");
            strcpy_s(a_psFuncDef->szFunctionDescription, "Sum Initialed Price Arrays (OHLCVI)");
            a_psFuncDef->iNArguments = NSumArraysNArgs;
            break;
        default:
            l_bRtrn = MSX_ERROR;
            break;
        }
        return l_bRtrn;
    }


    BOOL __stdcall MSXNthArg(int a_iNthFunc, int a_iNthArg,
        MSXFuncArgDef* a_psFuncArgDef)
    {
        BOOL l_bRtrn = MSX_SUCCESS;

        a_psFuncArgDef->iNCustomStrings = 0;

        switch (a_iNthFunc) {
        case 0:
            switch (a_iNthArg) {
            case 0:
                a_psFuncArgDef->iArgType = MSXDataArray; // DataArray;
                strcpy_s(a_psFuncArgDef->szArgName, "DataArray");
                break;
            case 1:
                a_psFuncArgDef->iArgType = MSXNumeric; // Numeric
                strcpy_s(a_psFuncArgDef->szArgName, "Period");
                break;
            case 2:
                a_psFuncArgDef->iArgType = MSXCustom; // CustomType
                a_psFuncArgDef->iNCustomStrings = NMyMovCustStrings;
                strcpy_s(a_psFuncArgDef->szArgName, "Method");
                break;
            default:
                l_bRtrn = MSX_ERROR;
                break;
            }
            break;
        case 1:
            switch (a_iNthArg) {
            case 0:
                a_psFuncArgDef->iArgType = MSXDataArray;
                strcpy_s(a_psFuncArgDef->szArgName, "DataArray");
                break;
            case 1:
                a_psFuncArgDef->iArgType = MSXNumeric;
                strcpy_s(a_psFuncArgDef->szArgName, "Increment");
                break;
            default:
                l_bRtrn = MSX_ERROR;
                break;
            }
            break;
        case 2:
            if (a_iNthArg == 0) {
                a_psFuncArgDef->iArgType = MSXString;
                strcpy_s(a_psFuncArgDef->szArgName, "DA Initials");
            }
            break;
        default:
            l_bRtrn = MSX_ERROR;
            break;
        }
        return l_bRtrn;
    }


    BOOL __stdcall MSXNthCustomString(int a_iNthFunc, int a_iNthArg,
        int a_iNthString, MSXFuncCustomString* a_psCustomString)
    {
        BOOL l_bRtrn = MSX_SUCCESS;

        a_psCustomString->szString[0] = '\0';
        a_psCustomString->iID = -1;

        typedef struct {
            char* szString;
            int  iID;
        } LocalStringElement;

        char simpleName[] = "Simple";
        char simpleInitial[] = "S";
        char triangularName[] = "Triangular";
        char triangularInitial[] = "T";
        char weightedName[] = "Weighted";
        char weightedInitial[] = "W";
        char exponentialName[] = "Exponential";
        char exponentialInitial[] = "E";

        LocalStringElement l_sTheStrings[] = {
          {simpleName,      0}, {simpleInitial, 0},
          {triangularName,  1}, {triangularInitial, 1},
          {weightedName,    2}, {weightedInitial, 2},
          {exponentialName, 3}, {exponentialInitial, 3}
        };

        switch (a_iNthFunc) {
        case 0:
            switch (a_iNthArg) {
            case 2:
                if (a_iNthString >= 0 && a_iNthString < NMyMovCustStrings) {
                    strncpy_s(a_psCustomString->szString, l_sTheStrings[a_iNthString].szString,
                        sizeof(a_psCustomString->szString) - 1);
                    a_psCustomString->iID = l_sTheStrings[a_iNthString].iID;
                }
                break;
            default:
                l_bRtrn = MSX_ERROR;
                break;
            }
            break;
        default:
            l_bRtrn = MSX_ERROR;
            break;
        }

        return l_bRtrn;
    }

    // ************************************************************************
    // This local utility function is used to help ensure that no overflows
    //   or underflows will occur during calculations.  The MSXTest program
    //   Stress Test function will call your DLL with a wide range of values,
    //   including positive and negative values of FLT_MAX and FLT_MIN.
    // Perform all intermediate calculations using doubles and then force the
    //   results into the range of a float.
    // Locally defined macros are used to avoid compiler definition differences.
    // ************************************************************************
#define MSXMax(a,b) (((a) > (b)) ? (a) : (b))
#define MSXMin(a,b) (((a) < (b)) ? (a) : (b))
    double ForceFloatRange(double a_lfDbl)
    {
        if (a_lfDbl > 0.0)
        {
            a_lfDbl = MSXMin(a_lfDbl, double(FLT_MAX)); // make sure pos number <= FLT_MAX
            a_lfDbl = MSXMax(a_lfDbl, double(FLT_MIN)); // make sure pos number >= FLT_MIN
        }
        else
        {
            if (a_lfDbl < 0.0)
            {
                a_lfDbl = MSXMax(a_lfDbl, double(-FLT_MAX)); // make sure neg number >= -FLT_MAX
                a_lfDbl = MSXMin(a_lfDbl, double(-FLT_MIN)); // make sure neg number <= -FLT_MIN
            }
        }

        return a_lfDbl;
    }

    //------------------------------------------------------------------------
    // This is an example of a local function used for calculations.  This
    //   one calculates a moving average on the source data array, and puts
    //   the results in the result array.  It differentiates its processing
    //   based on whether the moving average is to be weighted or not.
    //------------------------------------------------------------------------
    void MovingAverage(const MSXDataInfoRec* a_psSrc, MSXDataInfoRec* a_psRslt,
        int a_iPeriod, BOOL a_bIsWeighted)
    {
        int    l_iIndex = a_psSrc->iFirstValid;
        int    l_iMaxIndex = a_psSrc->iLastValid;
        double l_lfSum = 0.0;
        double l_lfDbl;
        float  l_fDivisor;
        int    i;

        if (a_bIsWeighted)
            l_fDivisor = float(a_iPeriod) * (float(a_iPeriod) + 1.0f) / 2.0f; // sum of the digits formula
        else
            l_fDivisor = float(a_iPeriod);
        l_fDivisor = float(ForceFloatRange(l_fDivisor));
        if (l_fDivisor == 0.0)
            l_fDivisor = 1.0f;
        while ((l_iIndex + a_iPeriod - 1) <= l_iMaxIndex) {
            l_lfSum = 0.0;
            for (i = 0; i < a_iPeriod; i++) {
                if (a_bIsWeighted)
                    l_lfSum += a_psSrc->pfValue[l_iIndex + i] * (i + 1.0f);
                else
                    l_lfSum += a_psSrc->pfValue[l_iIndex + i];
            }
            l_lfSum = ForceFloatRange(l_lfSum);
            l_lfDbl = l_lfSum / l_fDivisor;
            l_lfDbl = ForceFloatRange(l_lfDbl);
            a_psRslt->pfValue[l_iIndex + a_iPeriod - 1] = float(l_lfDbl);
            l_iIndex++;
        }
        a_psRslt->iFirstValid = a_psSrc->iFirstValid + a_iPeriod - 1;
        a_psRslt->iLastValid = l_iMaxIndex;
    }

    //------------------------------------------------------------------------
    // This is another example of a local function used for calculations.  This
    //   one calculates an exponential moving average on the source data array,
    //   and puts the results in the result array.  See 'The Technical Analysis
    //   Course' by Thomas A. Meyers for algorithm details.
    //------------------------------------------------------------------------
    void ExponentialMovingAverage(const MSXDataInfoRec* a_psSrc,
        MSXDataInfoRec* a_psRslt,
        int a_iPeriod)
    {
        int   l_iIndex = a_psSrc->iFirstValid;
        int   l_iMaxIndex = a_psSrc->iLastValid;
        double l_lfSum = 0.0;
        double l_lfDivisor;
        double l_lfExponent;
        int   i;

        if (a_iPeriod > 0 && ((l_iIndex + a_iPeriod - 1) <= l_iMaxIndex)) {
            l_lfExponent = ForceFloatRange(2.0 / (a_iPeriod + 1));
            l_lfDivisor = double(a_iPeriod);
            // start with simple moving average;
            for (i = 0; i < a_iPeriod; i++)
                l_lfSum += a_psSrc->pfValue[l_iIndex + i];
            l_lfSum = ForceFloatRange(l_lfSum);
            a_psRslt->pfValue[l_iIndex + a_iPeriod - 1] = float(ForceFloatRange(l_lfSum / l_lfDivisor));
            l_iIndex += a_iPeriod;
            while (l_iIndex <= l_iMaxIndex) {
                a_psRslt->pfValue[l_iIndex] =
                    float(ForceFloatRange((a_psSrc->pfValue[l_iIndex] - a_psRslt->pfValue[l_iIndex - 1]) *
                        l_lfExponent + a_psRslt->pfValue[l_iIndex - 1]));
                l_iIndex++;
            }
            a_psRslt->iFirstValid = a_psSrc->iFirstValid + a_iPeriod - 1;
            a_psRslt->iLastValid = l_iMaxIndex;
        }
        else {
            a_psRslt->iFirstValid = 0;
            a_psRslt->iLastValid = -1;
        }
    }


    //-------------------------------------------------------------------------------------------
    // The following function demonstrates use of three argument types:
    //   MSXDataArray, MSXNumeric and MSXCustom.
    //   A MovingAverage is calculated on the input DataArray for input Periods.
    //   Three moving average methods are available, specified by the Custom ID.
    //-------------------------------------------------------------------------------------------
    BOOL __stdcall MyMov(const MSXDataRec* a_psDataRec,
        const MSXDataInfoRecArgsArray* a_psDataInfoArgs,
        const MSXNumericArgsArray* a_psNumericArgs,
        const MSXStringArgsArray* a_psStringArgs,
        const MSXCustomArgsArray* a_psCustomArgs,
        MSXResultRec* a_psResultRec)
    {
        BOOL l_bRtrn = MSX_SUCCESS;

        // We expect 3 arguments, 1 DataArray, 1 Numeric and 1 Custom, in that order
        // The arguments will be found at:
        //   DataArray: a_psDataInfoArgs[0]
        //   Numeric  : a_psNumericArgs->fNumerics[0];
        //   Custom   : a_psCustomArgs->iCustomIDs[0];

        const MSXDataInfoRec* l_psData;
        int                  l_iPeriod;
        int                  l_iMethod;
        int                  l_iIndex;
        int                  l_iMaxIndex;

        if (a_psDataInfoArgs->iNRecs == 1 &&
            a_psNumericArgs->iNRecs == 1 &&
            a_psCustomArgs->iNRecs == 1) {
            l_psData = a_psDataInfoArgs->psDataInfoRecs[0];
            l_iPeriod = int(a_psNumericArgs->fNumerics[0]); // truncate any fractional period
            l_iMethod = a_psCustomArgs->iCustomIDs[0];
            l_iIndex = l_psData->iFirstValid;
            l_iMaxIndex = l_psData->iLastValid;

            if (l_iPeriod > 0 && (l_iIndex + l_iPeriod - 1) <= l_iMaxIndex &&
                l_iIndex > 0 && l_iMaxIndex > 0 &&
                l_iIndex >= a_psDataRec->sClose.iFirstValid &&
                l_iMaxIndex <= a_psDataRec->sClose.iLastValid) {
                switch (l_iMethod) {
                case 0: // Simple
                    MovingAverage(l_psData, a_psResultRec->psResultArray, l_iPeriod, FALSE);
                    break;
                case 1: // Triangular
                {
                    MSXDataInfoRec l_sTmpRec;
                    l_sTmpRec.pfValue = new float[l_iMaxIndex + 1];
                    if (l_sTmpRec.pfValue) {
                        l_iPeriod = (int)(((l_iPeriod + 1.0) / 2) + 0.5);
                        MovingAverage(l_psData, &l_sTmpRec, l_iPeriod, FALSE);
                        MovingAverage(&l_sTmpRec, a_psResultRec->psResultArray, l_iPeriod, FALSE);
                        delete l_sTmpRec.pfValue;
                    }
                    else {
                        a_psResultRec->psResultArray->iFirstValid = 0;
                        a_psResultRec->psResultArray->iLastValid = -1;
                    }
                }
                break;
                case 2: // Weighted
                    MovingAverage(l_psData, a_psResultRec->psResultArray, l_iPeriod, TRUE);
                    break;
                case 3: // Exponential
                    ExponentialMovingAverage(l_psData, a_psResultRec->psResultArray, l_iPeriod);
                    break;
                default:
                {
                    // Somehow we got called with an invalid argument
                    strncpy_s(a_psResultRec->szExtendedError, "Undefined method argument",
                        sizeof(a_psResultRec->szExtendedError) - 1);
                    l_bRtrn = MSX_ERROR; // report this as an error
                }
                break;
                }
            }
            else {
                a_psResultRec->psResultArray->iFirstValid = 0;
                a_psResultRec->psResultArray->iLastValid = -1;
            }
        }
        else { // wrong number of arguments!
            strncpy_s(a_psResultRec->szExtendedError, "Wrong number of arguments",
                sizeof(a_psResultRec->szExtendedError) - 1);
            l_bRtrn = MSX_ERROR;
        }

        if (l_bRtrn != MSX_SUCCESS) { // only for serious errors...
            a_psResultRec->psResultArray->iFirstValid = 0;
            a_psResultRec->psResultArray->iLastValid = -1;
        }

        return l_bRtrn;
    }

    //-------------------------------------------------------------------------------------------
    // The following function will add a constant numeric value to any DataArray
    //-------------------------------------------------------------------------------------------
    BOOL __stdcall AddN(const MSXDataRec* a_psDataRec,
        const MSXDataInfoRecArgsArray* a_psDataInfoArgs,
        const MSXNumericArgsArray* a_psNumericArgs,
        const MSXStringArgsArray* a_psStringArgs,
        const MSXCustomArgsArray* a_psCustomArgs,
        MSXResultRec* a_psResultRec)
    {
        BOOL   l_bRtrn = MSX_SUCCESS;
        int    i;

        // We expect 2 arguments, 1 DataArray, 1 Numeric
        // The arguments will be found at:
        //   DataArray: theDataInfoArgs[0]
        //   Numeric  : theNumericArgs->fNumerics[0];

        if (a_psDataInfoArgs->iNRecs == 1 &&
            a_psNumericArgs->iNRecs == 1) {
            const MSXDataInfoRec* l_psData = a_psDataInfoArgs->psDataInfoRecs[0];
            if (l_psData->iFirstValid > 0 && l_psData->iLastValid > 0 &&
                l_psData->iFirstValid >= a_psDataRec->sClose.iFirstValid &&
                l_psData->iLastValid <= a_psDataRec->sClose.iLastValid) {
                float l_fAddOn = a_psNumericArgs->fNumerics[0];

                if (l_psData) {
                    for (i = l_psData->iFirstValid; i <= l_psData->iLastValid; i++)
                        a_psResultRec->psResultArray->pfValue[i] =
                        float(ForceFloatRange(l_psData->pfValue[i] + l_fAddOn));
                    a_psResultRec->psResultArray->iFirstValid = l_psData->iFirstValid;
                    a_psResultRec->psResultArray->iLastValid = l_psData->iLastValid;
                }
                else {
                    strncpy_s(a_psResultRec->szExtendedError, "Data array argument missing",
                        sizeof(a_psResultRec->szExtendedError) - 1);
                    l_bRtrn = MSX_ERROR;
                }
            }
            else { // invalid incoming ranges
                a_psResultRec->psResultArray->iFirstValid = 0;
                a_psResultRec->psResultArray->iLastValid = -1;
            }
        }
        else { // wrong number of arguments!
            strncpy_s(a_psResultRec->szExtendedError, "Wrong number of arguments",
                sizeof(a_psResultRec->szExtendedError) - 1);
            l_bRtrn = MSX_ERROR;
        }

        if (l_bRtrn != MSX_SUCCESS) {
            a_psResultRec->psResultArray->iFirstValid = 0;
            a_psResultRec->psResultArray->iLastValid = -1;
        }
        return l_bRtrn;
    }

    //-------------------------------------------------------------------------------------------
    // The following function demonstrates the use of an MSXString argument.
    //   It parses the argument string for the initials identifying Open, High, Low, Close,
    //     Vol or Indicator, and sums their values into the result array.
    //   The result array FirstValid and LastValid are restricted to the common subset of
    //     all selected arrays.
    //-------------------------------------------------------------------------------------------
    BOOL __stdcall SumArrays(const MSXDataRec* a_psDataRec,
        const MSXDataInfoRecArgsArray* a_psDataInfoArgs,
        const MSXNumericArgsArray* a_psNumericArgs,
        const MSXStringArgsArray* a_psStringArgs,
        const MSXCustomArgsArray* a_psCustomArgs,
        MSXResultRec* a_psResultRec)
    {
        BOOL   l_bRtrn = MSX_SUCCESS;
        int    i;
        int    l_iFirstValid = a_psDataRec->sClose.iFirstValid;
        int    l_iLastValid = a_psDataRec->sClose.iLastValid;
        const  MSXDataInfoRec* l_psData;

        // We expect 1 arguments, 1 String
        // The argument will be found at:
        //   theStringArgs[0]

        if (a_psStringArgs->iNRecs == 1) {
            if (l_iFirstValid > 0 && l_iLastValid > 0) {

                const char* l_pc = a_psStringArgs->pszStrings[0];

                if (l_pc) {
                    // init result:
                    for (i = l_iFirstValid; i <= l_iLastValid; i++)
                        a_psResultRec->psResultArray->pfValue[i] = 0.0f;

                    while (*l_pc) {
                        switch (*l_pc++) {
                        case 'O':
                        case 'o':
                            l_psData = &a_psDataRec->sOpen;
                            break;
                        case 'H':
                        case 'h':
                            l_psData = &a_psDataRec->sHigh;
                            break;
                        case 'L':
                        case 'l':
                            l_psData = &a_psDataRec->sLow;
                            break;
                        case 'C':
                        case 'c':
                            l_psData = &a_psDataRec->sClose;
                            break;
                        case 'V':
                        case 'v':
                            l_psData = &a_psDataRec->sVol;
                            break;
                        case 'I':
                        case 'i':
                            l_psData = &a_psDataRec->sInd;
                            break;
                        default:
                            l_psData = NULL;
                            break;
                        }
                        if (l_psData) {
                            l_iFirstValid = __max(l_iFirstValid, l_psData->iFirstValid);
                            l_iLastValid = __min(l_iLastValid, l_psData->iLastValid);
                            for (i = l_iFirstValid; i <= l_iLastValid; i++)
                                a_psResultRec->psResultArray->pfValue[i] =
                                float(ForceFloatRange(a_psResultRec->psResultArray->pfValue[i] +
                                    l_psData->pfValue[i]));
                        }
                    }
                    a_psResultRec->psResultArray->iFirstValid = l_iFirstValid;
                    a_psResultRec->psResultArray->iLastValid = l_iLastValid;
                }
                else {
                    // empty string!
                    a_psResultRec->psResultArray->iFirstValid = 0;
                    a_psResultRec->psResultArray->iLastValid = -1;
                }
            }
            else { // invalid first/last
                a_psResultRec->psResultArray->iFirstValid = 0;
                a_psResultRec->psResultArray->iLastValid = -1;
            }
        }
        else {// error - no argument passed!
            strncpy_s(a_psResultRec->szExtendedError, "Wrong number of arguments",
                sizeof(a_psResultRec->szExtendedError) - 1);
            l_bRtrn = MSX_ERROR;
        }

        if (l_bRtrn != MSX_SUCCESS) {
            a_psResultRec->psResultArray->iFirstValid = 0;
            a_psResultRec->psResultArray->iLastValid = -1;
        }

        return l_bRtrn;
    }

#ifdef __cplusplus
}
#endif
