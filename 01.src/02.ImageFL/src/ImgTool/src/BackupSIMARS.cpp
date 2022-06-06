/*
 * BackupSIMARS.cpp
 *
 *  Created on: 2021/04/05
 *      Author: 10097663
 */

//SIMARSのソースコードが大きすぎて構文解析できないため、一部をこちらにもってきておく


#if 0



//---------------------------------------------------------------------------
TSIMARS::TSIMARS()
{
    SubPixel = 0;
    MainPixel = 0;
    SetBit(16);
    DtExist = false;
	m_fMainPixelSize = fPIX_SIZE;
    m_fSubPixelSize = fPIX_SIZE;
	iEDR = iEDR_OFF;
    iSk = 200;
	iL = 400;

	InitializeLPEdgeInf(m_SintiEdgeU);
	InitializeLPEdgeInf(m_GlassEdgeU);
	InitializeLPEdgeInf(m_CalibSintiEdgeU);
	InitializeLPEdgeInf(m_CalibGlassEdgeU);
	InitializeLPEdgeInf(m_EdgeShiftU);

	InitializeLPEdgeInf(m_SintiEdgeD);
	InitializeLPEdgeInf(m_GlassEdgeD);
	InitializeLPEdgeInf(m_CalibSintiEdgeD);
	InitializeLPEdgeInf(m_CalibGlassEdgeD);
	InitializeLPEdgeInf(m_EdgeShiftD);

}
//---------------------------------------------------------------------------
TSIMARS::TSIMARS(const int iS, const int iM, const int iBit,
			const int iEDRVal,
			const int iSkVal,
            const int iLVal)
{
	TTMA::NewDt(iS, iM, iBit, fPIX_SIZE, iEDRVal, iSkVal, iLVal);

	InitializeLPEdgeInf(m_SintiEdgeU);
	InitializeLPEdgeInf(m_GlassEdgeU);
	InitializeLPEdgeInf(m_CalibSintiEdgeU);
	InitializeLPEdgeInf(m_CalibGlassEdgeU);
	InitializeLPEdgeInf(m_EdgeShiftU);

	InitializeLPEdgeInf(m_SintiEdgeD);
	InitializeLPEdgeInf(m_GlassEdgeD);
	InitializeLPEdgeInf(m_CalibSintiEdgeD);
	InitializeLPEdgeInf(m_CalibGlassEdgeD);
	InitializeLPEdgeInf(m_EdgeShiftD);
}
//---------------------------------------------------------------------------
//test

TSIMARS::TSIMARS(const CTDAT &copy)
{
	TTMA::NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
	for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
		for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
			Dt[iSCnt][iMCnt] = copy.Dt[iSCnt][iMCnt];
		}
	}

	InitializeLPEdgeInf(m_SintiEdgeU);
	InitializeLPEdgeInf(m_GlassEdgeU);
	InitializeLPEdgeInf(m_CalibSintiEdgeU);
	InitializeLPEdgeInf(m_CalibGlassEdgeU);
	InitializeLPEdgeInf(m_EdgeShiftU);

	InitializeLPEdgeInf(m_SintiEdgeD);
	InitializeLPEdgeInf(m_GlassEdgeD);
	InitializeLPEdgeInf(m_CalibSintiEdgeD);
	InitializeLPEdgeInf(m_CalibGlassEdgeD);
	InitializeLPEdgeInf(m_EdgeShiftD);
}
//---------------------------------------------------------------------------

TSIMARS::~TSIMARS()
{
	if (NULL != m_SintiEdgeU.y10) {
		delete[] m_SintiEdgeU.y10;
		m_SintiEdgeU.y10 = NULL;
	}
	if (NULL != m_GlassEdgeU.y10) {
		delete[] m_GlassEdgeU.y10;
		m_GlassEdgeU.y10 = NULL;
	}
	if (NULL != m_CalibSintiEdgeU.y10) {
		delete[] m_CalibSintiEdgeU.y10;
		m_CalibSintiEdgeU.y10 = NULL;
	}
	if (NULL != m_CalibGlassEdgeU.y10) {
		delete[] m_CalibGlassEdgeU.y10;
		m_CalibGlassEdgeU.y10 = NULL;
	}
	if (NULL != m_EdgeShiftU.y10) {
		delete[] m_EdgeShiftU.y10;
		m_EdgeShiftU.y10 = NULL;
	}
	if (NULL != m_SintiEdgeD.y10) {
		delete[] m_SintiEdgeD.y10;
		m_SintiEdgeD.y10 = NULL;
	}
	if (NULL != m_GlassEdgeD.y10) {
		delete[] m_GlassEdgeD.y10;
		m_GlassEdgeD.y10 = NULL;
	}
	if (NULL != m_CalibSintiEdgeD.y10) {
		delete[] m_CalibSintiEdgeD.y10;
		m_CalibSintiEdgeD.y10 = NULL;
	}
	if (NULL != m_CalibGlassEdgeD.y10) {
		delete[] m_CalibGlassEdgeD.y10;
		m_CalibGlassEdgeD.y10 = NULL;
	}
	if (NULL != m_EdgeShiftD.y10) {
		delete[] m_EdgeShiftD.y10;
		m_EdgeShiftD.y10 = NULL;
	}
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorMainBadLineBothSide(int iM, int iS0, int iS1)
{
    int iM0 = iM;
    int iM1 = iM;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iM0 != iM || iM1 != iM || iM == 0 || iM == MainPixel - 1){
        return;
    }
    if(iS0 == 0){
        Dt[iS0][iM] = (unsigned short int)
                ((4 * Dt[iS0][iM - 1]     + 4 * Dt[iS0][iM + 1]
                + 3 * Dt[iS0 + 1][iM - 1] + 3 * Dt[iS0 + 1][iM + 1]) / 14);
        ++iS0;
    }
    if(iS1 == SubPixel - 1){
        Dt[iS1][iM] = (unsigned short int)
                ((3 * Dt[iS1 - 1][iM - 1] + 3 * Dt[iS1 - 1][iM + 1]
                + 4 * Dt[iS1][iM - 1]     + 4 * Dt[iS1][iM + 1]) / 14);
        --iS1;
    }
    for(int iSCnt = iS0; iSCnt <= iS1; ++iSCnt){
        Dt[iSCnt][iM] = (unsigned short int)
                ((3 * Dt[iSCnt - 1][iM - 1] + 3 * Dt[iSCnt - 1][iM + 1]
                + 4 * Dt[iSCnt][iM - 1]     + 4 * Dt[iSCnt][iM + 1]
                + 3 * Dt[iSCnt + 1][iM - 1] + 3 * Dt[iSCnt + 1][iM + 1]) / 20);
    }
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorMainBadLineDecSide(int iM, int iS0, int iS1)
{
    int iM0 = iM;
    int iM1 = iM;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iM0 != iM || iM1 != iM || iM == 0){
        return;
    }
    if(iS0 == 0){
        Dt[iS0][iM] = (unsigned short int)
                ((4 * Dt[iS0][iM - 1]
                + 3 * Dt[iS0 + 1][iM - 1]) / 7);
        ++iS0;
    }
    if(iS1 == SubPixel - 1){
        Dt[iS1][iM] = (unsigned short int)
                ((3 * Dt[iS1 - 1][iM - 1]
                + 4 * Dt[iS1][iM - 1]    ) / 7);
        --iS1;
    }
    for(int iSCnt = iS0; iSCnt <= iS1; ++iSCnt){
        Dt[iSCnt][iM] = (unsigned short int)
                ((3 * Dt[iSCnt - 1][iM - 1]
                + 4 * Dt[iSCnt][iM - 1]
                + 3 * Dt[iSCnt + 1][iM - 1]) / 10);
    }
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorMainBadLineIncSide(int iM, int iS0, int iS1)
{
    int iM0 = iM;
    int iM1 = iM;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iM0 != iM || iM1 != iM || iM == MainPixel - 1){
        return;
    }
    if(iS0 == 0){
        Dt[iS0][iM] = (unsigned short int)
                ((4 * Dt[iS0][iM + 1]
                + 3 * Dt[iS0 + 1][iM + 1]) / 7);
        ++iS0;
    }
    if(iS1 == SubPixel - 1){
        Dt[iS1][iM] = (unsigned short int)
                ((3 * Dt[iS1 - 1][iM + 1]
                + 4 * Dt[iS1][iM + 1]    ) / 7);
        --iS1;
    }
    for(int iSCnt = iS0; iSCnt <= iS1; ++iSCnt){
        Dt[iSCnt][iM] = (unsigned short int)
                ((3 * Dt[iSCnt - 1][iM + 1]
                + 4 * Dt[iSCnt][iM + 1]
                + 3 * Dt[iSCnt + 1][iM + 1]) / 10);
    }
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorSubBadLineBothSide(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS || iS == 0 || iS == SubPixel - 1){
        return;
    }
    if(iM0 == 0){
        Dt[iS][iM0] = (unsigned short int)
                ((4 * Dt[iS - 1][iM0] + 3 * Dt[iS - 1][iM0 + 1]
                + 4 * Dt[iS + 1][iM0] + 3 * Dt[iS + 1][iM0 + 1]) / 14);
        ++iM0;
    }
    if(iM1 == MainPixel - 1){
        Dt[iS][iM1] = (unsigned short int)
                ((3 * Dt[iS - 1][iM1 - 1] + 4 * Dt[iS - 1][iM1]
                + 3 * Dt[iS + 1][iM1 - 1] + 4 * Dt[iS + 1][iM1]) / 14);
        --iM1;
    }
	for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        Dt[iS][iMCnt] = (unsigned short int)
                ((3 * Dt[iS - 1][iMCnt - 1] + 4 * Dt[iS - 1][iMCnt] + 3 * Dt[iS - 1][iMCnt + 1]
                + 3 * Dt[iS + 1][iMCnt - 1] + 4 * Dt[iS + 1][iMCnt] + 3 * Dt[iS + 1][iMCnt + 1]) / 20);
    }
}
//---------------------------------------------------------------------------
/*

*/
/*
void TSIMARS::CorSubBadLineDecSide(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS || iS == 0){
        return;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        Dt[iS][iMCnt] = Dt[iS - 1][iMCnt];
    }
}
*/
/*
void TSIMARS::CorSubBadLineDecSide(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS || iS == 0){
        return;
    }
    if(iM0 == 0){
        Dt[iS][iM0] = (unsigned short int)
                ((4 * Dt[iS - 1][iM0] + 3 * Dt[iS - 1][iM0 + 1]) / 7);
        ++iM0;
    }
    if(iM1 == MainPixel - 1){
        Dt[iS][iM1] = (unsigned short int)
                ((3 * Dt[iS - 1][iM1 - 1] + 4 * Dt[iS - 1][iM1]) / 7);
        --iM1;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        Dt[iS][iMCnt] = (unsigned short int)
                ((3 * Dt[iS - 1][iMCnt - 1] + 4 * Dt[iS - 1][iMCnt] + 3 * Dt[iS - 1][iMCnt + 1]) / 10);
    }
}
*/
/*
void TSIMARS::CorSubBadLineDecSide(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS || iS == 0){
        return;
    }
    if(iM0 == 0){
        Dt[iS][iM0] = (unsigned short int)
                ((2 * Dt[iS - 1][iM0] + Dt[iS - 1][iM0 + 1]) / 3);
        ++iM0;
    }
    if(iM1 == MainPixel - 1){
        Dt[iS][iM1] = (unsigned short int)
                ((Dt[iS - 1][iM1 - 1] + 2 * Dt[iS - 1][iM1]) / 3);
        --iM1;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        Dt[iS][iMCnt] = (unsigned short int)
                ((Dt[iS - 1][iMCnt - 1] + 2 * Dt[iS - 1][iMCnt] + Dt[iS - 1][iMCnt + 1]) / 4);
    }
}
*/
void TSIMARS::CorSubBadLineDecSide(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS || iS == 0){
        return;
    }
//    randomize();
    const int RMAX = 1 << 15;
    if(iM0 == 0){
        int iDif = Dt[iS - 1][iM0] - Dt[iS - 1][iM0 + 1];
        Dt[iS][iM0] = (unsigned short int)(Dt[iS - 1][iM0] - ((random(RMAX) * iDif) >> 15));
        ++iM0;
    }
    if(iM1 == MainPixel - 1){
        int iDif = Dt[iS - 1][iM1] - Dt[iS - 1][iM1 - 1];
        Dt[iS][iM1] = (unsigned short int)(Dt[iS - 1][iM1] - ((random(RMAX) * iDif) >> 15));
        --iM1;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        int iDif = Dt[iS - 1][iMCnt] - (Dt[iS - 1][iMCnt - 1] + Dt[iS - 1][iMCnt + 1]) / 2;
        Dt[iS][iMCnt] = (unsigned short int)(Dt[iS - 1][iMCnt] - ((random(RMAX) * iDif) >> 15));
    }
}

//---------------------------------------------------------------------------
void TSIMARS::CorSubBadLineIncSide(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS || iS == SubPixel - 1){
        return;
    }
    randomize();
    const int RMAX = 1 << 15;
    if(iM0 == 0){
        int iDif = Dt[iS + 1][iM0] - Dt[iS + 1][iM0 + 1];
        Dt[iS][iM0] = (unsigned short int)(Dt[iS + 1][iM0] - ((random(RMAX) * iDif) >> 15));
        ++iM0;
    }
    if(iM1 == MainPixel - 1){
        int iDif = Dt[iS + 1][iM1] - Dt[iS + 1][iM1 - 1];
        Dt[iS][iM1] = (unsigned short int)(Dt[iS + 1][iM1] - ((random(RMAX) * iDif) >> 15));
        --iM1;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        int iDif = Dt[iS + 1][iMCnt] - (Dt[iS + 1][iMCnt - 1] + Dt[iS + 1][iMCnt + 1]) / 2;
        Dt[iS][iMCnt] = (unsigned short int)(Dt[iS + 1][iMCnt] - ((random(RMAX) * iDif) >> 15));
    }
}

//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorStep()
{
	const int iASIC = 128;//FPD1用
    const int iOFFSET = - 96; // = (128 * 24 - 2880) / 2
    TSINGLE_DATA step[4];
    for(int i = 0; i < 4; ++i){
        step[i].NewDt(SubPixel);
    }
    TSINGLE_DATA Step(SubPixel);

    int iNum = MainPixel / iASIC - 1;
    if((MainPixel + iOFFSET) % iASIC > 2){      //隣接2画素取れない時は補正しない
        ++iNum;
    }

    for(int iCnt = 1; iCnt < iNum; ++iCnt){
        int iM0 = (iCnt + 1) * iASIC + iOFFSET;
        int iM1 = iM0 + iASIC - 1;
        int iS0 = 0;
        int iS1 = SubPixel - 1;
        TrimArea(iS0, iM0, iS1, iM1);

        for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            step[0].dDt[iSCnt] = Dt[iSCnt][iM0 - 3];
            step[1].dDt[iSCnt] = Dt[iSCnt][iM0 - 2];
            step[2].dDt[iSCnt] = Dt[iSCnt][iM0 + 1];
            step[3].dDt[iSCnt] = Dt[iSCnt][iM0 + 2];
        }

        const int iMEDIAN = 101;
        for(int i = 0; i < 4; ++i){
            step[i].Median(iMEDIAN);
        }

        for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
//            Step.dDt[iSCnt] = (step[2].dDt[iSCnt] - step[1].dDt[iSCnt]) -
//                ((step[3].dDt[iSCnt] - step[2].dDt[iSCnt]) + (step[1].dDt[iSCnt] - step[0].dDt[iSCnt])) / 2.0;
            Step.dDt[iSCnt] = (step[2].dDt[iSCnt] - step[1].dDt[iSCnt]);
        }
        Step.Median(iMEDIAN);
        const int iMOVEAVE = 3;
        Step.MoveAve(iMOVEAVE);

        TSIMARS low(SubPixel, iM1 - iM0 + 1, Bit);
        for(int iMCnt = iM0, iLowMCnt = 0; iMCnt <= iM1; ++iMCnt, ++iLowMCnt){
            for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
                low.Dt[iSCnt][iLowMCnt] = Dt[iSCnt][iMCnt];
            }
        }
        const int iAVE = 11;
        low.MainMoveAveAveExpand(iAVE);
        low.SubMoveAveAveExpand(iAVE);

        const int iUPPER = 100;
        vector<int> PrevCor(low.MainPixel);
        float fCoe;
        int iSCnt = 2;
        if(step[2].dDt[iSCnt] != 0){
            fCoe = (step[2].dDt[iSCnt] - Step.dDt[iSCnt]) / step[2].dDt[iSCnt];
        }else{
            fCoe = 1.0f;
        }
        for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
            int iCor;
            if(low.Dt[iSCnt][iCorCnt] >= step[2].dDt[iSCnt]){
                iCor = - round(Step.dDt[iSCnt]);
            }else{
                iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
            }
            if(iCor > iUPPER){
                iCor = 0;
            }
            SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
            PrevCor[iCorCnt] = iCor;
        }

        for(++iSCnt; iSCnt < SubPixel; ++iSCnt){
            if(step[2].dDt[iSCnt] != 0){
                fCoe = (step[2].dDt[iSCnt] - Step.dDt[iSCnt]) / step[2].dDt[iSCnt];
            }else{
                fCoe = 1.0f;
            }
            for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
                int iCor;
                if(low.Dt[iSCnt][iCorCnt] >= step[2].dDt[iSCnt]){
                    iCor = - round(Step.dDt[iSCnt]);
                }else{
                    iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
                }
                int iDif = abs(iCor - PrevCor[iCorCnt]);
                if(iDif > 1){
                    if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt]) < iDif){
                        if(iCor > PrevCor[iCorCnt]){
                            iCor = PrevCor[iCorCnt];
//                            iCor = PrevCor[iCorCnt] + 1;
                        }else{
                            iCor = PrevCor[iCorCnt];
//                            iCor = PrevCor[iCorCnt] - 1;
                        }
                    }
                }
                if(iCor > iUPPER){
                    iCor = PrevCor[iCorCnt];
                }
                SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
                PrevCor[iCorCnt] = iCor;
            }
        }
    }
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorStep(const int iMid)
{
    TSINGLE_DATA step[4];
    for(int i = 0; i < 4; ++i){
        step[i].NewDt(MainPixel);
    }

    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        step[0].dDt[iMCnt] = Dt[iMid - 3][iMCnt];
        step[1].dDt[iMCnt] = Dt[iMid - 2][iMCnt];
        step[2].dDt[iMCnt] = Dt[iMid + 1][iMCnt];
        step[3].dDt[iMCnt] = Dt[iMid + 2][iMCnt];
    }

    const int iMEDIAN = 101;
    for(int i = 0; i < 4; ++i){
        step[i].Median(iMEDIAN);
    }

    TSINGLE_DATA Step(MainPixel);
    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        Step.dDt[iMCnt] = (step[2].dDt[iMCnt] - step[1].dDt[iMCnt]) -
                ((step[3].dDt[iMCnt] - step[2].dDt[iMCnt]) + (step[1].dDt[iMCnt] - step[0].dDt[iMCnt])) / 2.0;
    }
    Step.Median(iMEDIAN);
    const int iMOVEAVE = 3;
    Step.MoveAve(iMOVEAVE);

    TSIMARS low(SubPixel - iMid, MainPixel, Bit);
    for(int iSCnt = iMid, iLowSCnt = 0; iSCnt < SubPixel; ++iSCnt, ++iLowSCnt){
        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            low.Dt[iLowSCnt][iMCnt] = Dt[iSCnt][iMCnt];
        }
    }
    const int iAVE = 11;
    low.MainMoveAveAveExpand(iAVE);
    low.SubMoveAveAveExpand(iAVE);

    const int iUPPER = 500;
    vector<int> PrevCor(low.SubPixel);
    float fCoe;
    if(step[2].dDt[0] != 0){
		fCoe = (step[2].dDt[0] - Step.dDt[0]) / step[2].dDt[0];
    }else{
        fCoe = 1.0f;
    }
    for(int iSCnt = iMid, iCorCnt = 0; iSCnt < SubPixel; ++iSCnt, ++iCorCnt){
        int iCor;
        if(low.Dt[iCorCnt][0] >= step[2].dDt[0]){
            iCor = - round(Step.dDt[0]);
        }else{
            iCor = - round(Dt[iSCnt][0] * (1.0f - fCoe));
        }
        if(abs(iCor) > iUPPER){
            iCor = 0;
        }
        SetVal(iSCnt, 0, Dt[iSCnt][0] + iCor);
        PrevCor[iCorCnt] = iCor;
    }

	for(int iMCnt = 1; iMCnt < MainPixel; ++iMCnt){
		if(step[2].dDt[iMCnt] != 0){
			fCoe = (step[2].dDt[iMCnt] - Step.dDt[iMCnt]) / step[2].dDt[iMCnt];
		}else{
			fCoe = 1.0f;
		}
		for(int iSCnt = iMid, iCorCnt = 0; iSCnt < SubPixel; ++iSCnt, ++iCorCnt){
			int iCor;
            if(low.Dt[iCorCnt][iMCnt] >= step[2].dDt[iMCnt]){
                iCor = - round(Step.dDt[iMCnt]);
            }else{
                iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
            }
            int iDif = abs(iCor - PrevCor[iCorCnt]);
            if(iDif > 1){
                if(abs(low.Dt[iCorCnt][iMCnt] - low.Dt[iCorCnt][iMCnt - 1]) < iDif){
                    if(iCor > PrevCor[iCorCnt]){
                        iCor = PrevCor[iCorCnt] + 1;
                    }else{
                        iCor = PrevCor[iCorCnt] - 1;
                    }
                }
            }
            if(abs(iCor) > iUPPER){
                iCor = PrevCor[iCorCnt];
            }
            SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
            PrevCor[iCorCnt] = iCor;
        }
    }
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorStep(const int iMm, const int iMp)
{
	const int iASIC = 128;//FPD1用
    const int iMEDIAN = 201;
    const int iOFFSET = 0;

    if(MainPixel < iASIC + iMp){
        return;
    }

//////////////

    vector<float> R0_1(iASIC), R1_0(iASIC), flat(iASIC);
    int i;
    for(i = 0; i < iASIC; ++i){
        flat[i] = 1.0f;
    }

    int iNum = iASIC + iMm;
    for(i = 0; i < iNum; ++i){
        R0_1[i] = float(i) / iNum;
    }
    for( ; i < iASIC; ++i){
        R0_1[i] = 1.0f;
    }

    iNum = iASIC - 1 - iMp;
    for(i = 0; i < iNum; ++i){
        R1_0[iASIC - 1 - i] = float(i) / iNum;
    }
    for( ; i < iASIC; ++i){
        R1_0[iASIC - 1 - i] = 1.0f;
    }

////////////////////

    TFIMG cor(MainPixel, SubPixel);
    cor.Fill(0.0f);

    TSINGLE_DATA Step(SubPixel);

////////////////////

    int iM0 = iASIC + iOFFSET;
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        Step.dDt[iSCnt] = Dt[iSCnt][iM0 + iMp] - Dt[iSCnt][iM0 + iMm];
    }

    Step.Median(iMEDIAN);
    Step /= 2;

    CorStepASIC(0, iM0 - 1, flat, Step, iM0 + iMm, cor);

    int iM1 = iM0 + iASIC - 1;
	int iS0(0), iS1(0);
//	TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iS1, iM1);
	Step *= -1;
    CorStepASIC(iM0, iM1, R1_0, Step, iM0 + iMp, cor);

////////////////////

    for(iM0 = 2 * iASIC + iOFFSET; (iM0 + iASIC + iMp) < MainPixel; iM0 += iASIC){
        for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            Step.dDt[iSCnt] = Dt[iSCnt][iM0 + iMp] - Dt[iSCnt][iM0 + iMm];
        }

        Step.Median(iMEDIAN);
        Step /= 2;

        CorStepASIC(iM0 - iASIC, iM0 - 1, R0_1, Step, iM0 + iMm, cor);

        int iM1 = iM0 + iASIC - 1;
		iS0 = iS1 = 0;
//	    TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
		TrimArea(iS0, iM0, iS1, iM1);
		Step *= -1;
        CorStepASIC(iM0, iM1, R1_0, Step, iM0 + iMp, cor);
	}

////////////////////////

    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		Step.dDt[iSCnt] = Dt[iSCnt][iM0 + iMp] - Dt[iSCnt][iM0 + iMm];
    }

    Step.Median(iMEDIAN);
    Step /= 2;

    CorStepASIC(iM0 - iASIC, iM0 - 1, R0_1, Step, iM0 + iMm, cor);

    iM1 = iM0 + iASIC - 1;
	iS0 = iS1 = 0;
//	TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iS1, iM1);
    Step *= -1;
    CorStepASIC(iM0, iM1,flat, Step, iM0 + iMp, cor);

////////////////////////

    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt]);
        }
    }
}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorStep(const int iMm2, const int iMm1, const int iMp1, const int iMp2, const bool bDoDiagonal)
{
	const int iASIC = 128;//FPD1用
    const int iOFFSET = - 96; // = (128 * 24 - 2880) / 2
    bool bDif = true;
    const int iNum = 4;
    if(abs(iMm2) > iASIC || abs(iMp2) > iASIC){
        bDif = false;
    }

    TSINGLE_DATA step[iNum];
    for(int i = 0; i < iNum; ++i){
        step[i].NewDt(SubPixel);
    }
    TSINGLE_DATA Step(SubPixel);

    for(int iM0 = iASIC + iOFFSET; (iM0 + iMp2) < MainPixel; iM0 += iASIC){
        const int iMEDIAN = 101;
        if(bDif){
            for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
                step[0].dDt[iSCnt] = Dt[iSCnt][iM0 + iMm2];
                step[1].dDt[iSCnt] = Dt[iSCnt][iM0 + iMm1];
                step[2].dDt[iSCnt] = Dt[iSCnt][iM0 + iMp1];
                step[3].dDt[iSCnt] = Dt[iSCnt][iM0 + iMp2];
            }
            for(int i = 0; i < iNum; ++i){
                step[i].Median(iMEDIAN);
            }
            for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
                Step.dDt[iSCnt] = (step[2].dDt[iSCnt] - step[1].dDt[iSCnt]) -
                        ((step[3].dDt[iSCnt] - step[2].dDt[iSCnt]) + (step[1].dDt[iSCnt] - step[0].dDt[iSCnt])) / 2.0;
            }
        }else{
            for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
                step[1].dDt[iSCnt] = Dt[iSCnt][iM0 + iMm1];
                step[2].dDt[iSCnt] = Dt[iSCnt][iM0 + iMp1];
            }
            step[1].Median(iMEDIAN);
            step[2].Median(iMEDIAN);
            for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
                Step.dDt[iSCnt] = step[2].dDt[iSCnt] - step[1].dDt[iSCnt];
            }
        }

        Step.Median(iMEDIAN);
        const int iMOVEAVE = 3;
        Step.MoveAve(iMOVEAVE);

        int iM1 = iM0 + iASIC - 1;
		int iS0(0), iS1(0);
//	    TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
		TrimArea(iS0, iM0, iS1, iM1);
        TTMA low(SubPixel, iM1 - iM0 + 1, Bit);
        for(int iMCnt = iM0, iLowMCnt = 0; iMCnt <= iM1; ++iMCnt, ++iLowMCnt){
            for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
                low.Dt[iSCnt][iLowMCnt] = Dt[iSCnt][iMCnt];
            }
        }
        const int iAVE = 11;
        low.MainMoveAveAveExpand(iAVE);
        low.SubMoveAveAveExpand(iAVE);

        const int iUPPER = 200;
        vector<int> PrevCor(low.MainPixel);
        float fCoe;
        if(step[2].dDt[0] != 0){
            fCoe = (step[2].dDt[0] - Step.dDt[0]) / step[2].dDt[0];
        }else{
            fCoe = 1.0f;
        }
        for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
            int iCor;
            if(low.Dt[0][iCorCnt] >= step[2].dDt[0]){
                iCor = - round(Step.dDt[0]);
            }else{
                iCor = - round(Dt[0][iMCnt] * (1.0f - fCoe));
            }
            if(abs(iCor) > iUPPER){
                iCor = 0;
            }

            SetVal(0, iMCnt, Dt[0][iMCnt] + iCor);
            PrevCor[iCorCnt] = iCor;
        }
        if(bDoDiagonal){
            for(int iSCnt = 1; iSCnt < SubPixel; ++iSCnt){
                if(step[2].dDt[iSCnt] != 0){
                    fCoe = (step[2].dDt[iSCnt] - Step.dDt[iSCnt]) / step[2].dDt[iSCnt];
                }else{
                    fCoe = 1.0f;
                }

                int iMCnt = iM0, iCorCnt = 0;
                int iCor;
                if(low.Dt[iSCnt][iCorCnt] >= step[2].dDt[iSCnt]){
                    iCor = - round(Step.dDt[iSCnt]);
                }else{
                    iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
                }
                int iDif = abs(iCor - PrevCor[iCorCnt]);
                if(iDif > 1){
                    if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt]) < iDif){
                        if(iCor > PrevCor[iCorCnt]){
                            iCor = PrevCor[iCorCnt] + 1;
                        }else{
                            iCor = PrevCor[iCorCnt] - 1;
                        }
                    }
                }
                if(abs(iCor) > iUPPER){
                    iCor = PrevCor[iCorCnt];
                }
                SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
                PrevCor[iCorCnt] = iCor;

                for(int iMCnt = iM0 + 1, iCorCnt = 1; iMCnt < iM1; ++iMCnt, ++iCorCnt){
                    int iCor;
                    if(low.Dt[iSCnt][iCorCnt] >= step[2].dDt[iSCnt]){
                        iCor = - round(Step.dDt[iSCnt]);
                    }else{
                        iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
                    }
                    iDif = abs(iCor - PrevCor[iCorCnt]);
                    if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt - 1]) < iDif){
                        iCor = PrevCor[iCorCnt - 1];
                    }
                    if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt]) < iDif && abs(iCor) > abs(PrevCor[iCorCnt])){
                        iCor = PrevCor[iCorCnt];
                    }
                    if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt + 1]) < iDif && abs(iCor) > abs(PrevCor[iCorCnt + 1])){
                        iCor = PrevCor[iCorCnt + 1];
                    }
                    if(abs(iCor) > iUPPER){
                        iCor = PrevCor[iCorCnt];
                    }
                    SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
                    PrevCor[iCorCnt] = iCor;
                }

                iMCnt = iM1;
                iCorCnt = low.MainPixel - 1;
                if(low.Dt[iSCnt][iCorCnt] >= step[2].dDt[iSCnt]){
                    iCor = - round(Step.dDt[iSCnt]);
                }else{
                    iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
                }
                iDif = abs(iCor - PrevCor[iCorCnt]);
                if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt - 1]) < iDif){
                    iCor = PrevCor[iCorCnt - 1];
                }
                if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt]) < iDif && abs(iCor) > abs(PrevCor[iCorCnt])){
                    iCor = PrevCor[iCorCnt];
                }
                if(abs(iCor) > iUPPER){
                    iCor = PrevCor[iCorCnt];
                }
                SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
                PrevCor[iCorCnt] = iCor;
            }
        }else{
            for(int iSCnt = 1; iSCnt < SubPixel; ++iSCnt){
                if(step[2].dDt[iSCnt] != 0){
                    fCoe = (step[2].dDt[iSCnt] - Step.dDt[iSCnt]) / step[2].dDt[iSCnt];
                }else{
                    fCoe = 1.0f;
                }
                for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
                    int iCor;
                    if(low.Dt[iSCnt][iCorCnt] >= step[2].dDt[iSCnt]){
                        iCor = - round(Step.dDt[iSCnt]);
                    }else{
                        iCor = - round(Dt[iSCnt][iMCnt] * (1.0f - fCoe));
                    }
                    int iDif = abs(iCor - PrevCor[iCorCnt]);
                    if(iDif > 1){
                        if(abs(low.Dt[iSCnt][iCorCnt] - low.Dt[iSCnt - 1][iCorCnt]) < iDif){
                            if(iCor > PrevCor[iCorCnt]){
                                iCor = PrevCor[iCorCnt] + 1;
                            }else{
                                iCor = PrevCor[iCorCnt] - 1;
                            }
                        }
                    }
                    if(abs(iCor) > iUPPER){
                        iCor = PrevCor[iCorCnt];
                    }
                    SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iCor);
                    PrevCor[iCorCnt] = iCor;
                }
            }
        }
    }

}
//---------------------------------------------------------------------------
/*

*/
void TSIMARS::CorStepASIC(const int iM0, const int iM1, vector<float>& r, TSINGLE_DATA& step, const int iMPos, TFIMG& cor)
{
    const float fUPPER = 500;

    vector<float> PrevCor(iM1 - iM0 + 1);
    float fCoe;
    if(Dt[0][iMPos] != 0){
        fCoe = (Dt[0][iMPos] + step.dDt[0]) / Dt[0][iMPos];
    }else{
        fCoe = 1.0f;
    }
    for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
        float fCor;
        if(Dt[0][iMCnt] >= Dt[0][iMPos]){
            fCor = step.dDt[0];
        }else{
            fCor = Dt[0][iMCnt] * (1.0f - fCoe);
        }
        if(abs(fCor) > fUPPER){
            fCor = 0.0f;
        }

        fCor *= r[iCorCnt];
        cor.Dt[iMCnt][0] += fCor;
        PrevCor[iCorCnt] = fCor;
    }

    for(int iSCnt = 1; iSCnt < SubPixel; ++iSCnt){
        if(Dt[iSCnt][iMPos] != 0){
            fCoe = (Dt[iSCnt][iMPos] - step.dDt[iSCnt]) / Dt[iSCnt][iMPos];
        }else{
            fCoe = 1.0f;
        }
        for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
            float fCor;
            if(Dt[iSCnt][iMCnt] >= Dt[iSCnt][iMPos]){
                fCor = step.dDt[iSCnt];
            }else{
                fCor = Dt[iSCnt][iMCnt] * (1.0f - fCoe);
            }
            if(abs(fCor) > fUPPER){
                fCor = PrevCor[iCorCnt];
            }

            fCor *= r[iCorCnt];
            cor.Dt[iMCnt][iSCnt] += fCor;
            PrevCor[iCorCnt] = fCor;
        }
    }
}
//---------------------------------------------------------------------------
/*

*/
/*
void TSIMARS::CorSubBad2Line(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS + 1;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS + 1 || iS == 0 || iS == SubPixel - 2){
        return;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        Dt[iS][iMCnt] = (unsigned short int)((2 * Dt[iS - 1][iMCnt] + Dt[iS + 2][iMCnt]) / 3);
        Dt[iS + 1][iMCnt] = (unsigned short int)((Dt[iS - 1][iMCnt] + 2 * Dt[iS + 2][iMCnt]) / 3);
    }
}
*/
/*
void TSIMARS::CorSubBad2Line(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS + 1;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS + 1 || iS == 0 || iS == SubPixel - 2){
        return;
    }
    randomize();
    const int RMAX = 1 << 15;
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        int iDif = Dt[iS - 1][iMCnt] - Dt[iS + 2][iMCnt];
        Dt[iS][iMCnt] = (unsigned short int)(Dt[iS - 1][iMCnt] - ((random(RMAX) * iDif) >> 15));
        Dt[iS + 1][iMCnt] = (unsigned short int)(Dt[iS - 1][iMCnt] - ((random(RMAX) * iDif) >> 15));
    }
}
*/
//---------------------------------------------------------------------------
void TSIMARS::CorSubBad2Line(int iS, int iM0, int iM1)
{
    int iS0 = iS;
    int iS1 = iS + 1;
    TrimArea(iS0, iM0, iS1, iM1);
    if(iS0 != iS || iS1 != iS + 1 || iS == 0 || iS == SubPixel - 2){
        return;
    }
    randomize();
    const int RMAX = 1 << 15;
    if(iM0 == 0){
        int iMin = Dt[iS - 1][iM0];
        int iMax = Dt[iS - 1][iM0];
        if(iMin > Dt[iS - 1][iM0 + 1]){
            iMin = Dt[iS - 1][iM0 + 1];
        }else if(iMax < Dt[iS - 1][iM0 + 1]){
            iMax = Dt[iS - 1][iM0 + 1];
        }
        for(int iMCnt = iM0; iMCnt <= iM0 + 1; ++iMCnt){
            if(iMin > Dt[iS + 2][iMCnt]){
                iMin = Dt[iS + 2][iMCnt];
            }else if(iMax < Dt[iS + 2][iMCnt]){
                iMax = Dt[iS + 2][iMCnt];
            }
        }
        int iDif = iMax - iMin;
        Dt[iS][iM0] = (unsigned short int)(iMin + ((random(RMAX) * iDif) >> 15));
        Dt[iS + 1][iM0] = (unsigned short int)(iMin + ((random(RMAX) * iDif) >> 15));
        ++iM0;
    }
    if(iM1 == MainPixel - 1){
        int iMin = Dt[iS - 1][iM1];
        int iMax = Dt[iS - 1][iM1];
        if(iMin > Dt[iS - 1][iM1 - 1]){
            iMin = Dt[iS - 1][iM1 - 1];
        }else if(iMax < Dt[iS - 1][iM1 - 1]){
            iMax = Dt[iS - 1][iM1 - 1];
        }
        for(int iMCnt = iM1 - 1; iMCnt <= iM1; ++iMCnt){
            if(iMin > Dt[iS + 2][iMCnt]){
                iMin = Dt[iS + 2][iMCnt];
            }else if(iMax < Dt[iS + 2][iMCnt]){
                iMax = Dt[iS + 2][iMCnt];
            }
        }
        int iDif = iMax - iMin;
        Dt[iS][iM0] = (unsigned short int)(iMin + ((random(RMAX) * iDif) >> 15));
        Dt[iS + 1][iM0] = (unsigned short int)(iMin + ((random(RMAX) * iDif) >> 15));
        --iM1;
    }
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        int iMin = Dt[iS - 1][iMCnt - 1];
        int iMax = Dt[iS - 1][iMCnt - 1];
        for(int iMC = iMCnt; iMC <= iMCnt + 1; ++iMC){
            if(iMin > Dt[iS - 1][iMC]){
                iMin = Dt[iS - 1][iMC];
            }else if(iMax < Dt[iS - 1][iMC]){
                iMax = Dt[iS - 1][iMC];
            }
        }
        for(int iMC = iMCnt - 1; iMC <= iMCnt + 1; ++iMC){
            if(iMin > Dt[iS + 2][iMC]){
                iMin = Dt[iS + 2][iMC];
            }else if(iMax < Dt[iS + 2][iMC]){
                iMax = Dt[iS + 2][iMC];
            }
        }
        int iDif = iMax - iMin;
        Dt[iS][iMCnt] = (unsigned short int)(iMin + ((random(RMAX) * iDif) >> 15));
        Dt[iS + 1][iMCnt] = (unsigned short int)(iMin + ((random(RMAX) * iDif) >> 15));
    }
}
//---------------------------------------------------------------------------
/*
    主縮小:16
    副GPR OFF
    差分裾check clip
*/
#if 0
bool TSIMARS::HorUmbMainRedAmp4(const string SubHighCutFilter,
            const string MainLowPassFilter,
            const string SubLowPassFilter,
			const float fClip,
			const int iMAIN_DIV,
			const bool bDebugYokokasa,
			const string strImage)
{
/*  //関数引数に移動
    const int iMAIN_DIV = 16;//default
//    const int iMAIN_DIV = 32;
//    const int iMAIN_DIV = 64;
*/
    const int iYU = 16;

	TFIMG   fimg;
	TFIMG   clipImg;
	TFIMG orgFimg;
    int iMainCnt, iSubCnt;

//noise↓ 主方向縮小

    orgFimg.NewDt(MainPixel, SubPixel);
    for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
        for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
			orgFimg.Dt[iMainCnt][iSubCnt] = Dt[iSubCnt][iMainCnt];
        }
    }
	orgFimg.DivWithRest(iMAIN_DIV, 1);

//副差分clip画像作成
    float *pfDif, *pfClipDif;
    int iSrt, iEnd;
    int iClipCnt;
	TFIMG fClipDif;
	fClipDif.NewDt(1, orgFimg.SubPixel - 1);

    pfDif = new float[orgFimg.SubPixel - 1];
    pfClipDif = new float[orgFimg.SubPixel - 1];
    fimg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
    clipImg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
	for (iMainCnt = 0; iMainCnt < orgFimg.MainPixel; iMainCnt++){
			//差分
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			pfDif[iSubCnt] = orgFimg.Dt[iMainCnt][iSubCnt + 1] - orgFimg.Dt[iMainCnt][iSubCnt];
			pfClipDif[iSubCnt] = pfDif[iSubCnt];
		}
            //差分clip
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
            iEnd = 0;
			if (pfDif[iSubCnt] > fClip){
                iSrt = iSubCnt;
                int iYuCnt = 0;
                while(iSrt > iEnd && pfDif[iSrt - 1] < pfDif[iSrt] && iYuCnt < iYU){
                    --iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] > fClip){
                    ++iSubCnt;
                }
                iEnd = iSubCnt;
				while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] < pfDif[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }else if(pfDif[iSubCnt] < - fClip){
                iSrt = iSubCnt;
				int iYuCnt = 0;
                while(iSrt > iEnd && pfDif[iSrt - 1] > pfDif[iSrt] && iYuCnt < iYU){
					--iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] < - fClip){
                    ++iSubCnt;
                }
                iEnd = iSubCnt;
                while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] > pfDif[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }
        }

		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			fClipDif.Dt[0][iSubCnt] = pfClipDif[iSubCnt];
        }

            //積分
		fimg.Dt[iMainCnt][0] = clipImg.Dt[iMainCnt][0] = orgFimg.Dt[iMainCnt][0];
		for (iSubCnt = 1; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt - 1] + fClipDif.Dt[0][iSubCnt - 1];
			clipImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
		}
    }
	delete[] pfDif;
	delete[] pfClipDif;

	TTMA sabun, sabunclip, sekibun, divImage;//Dt[S][M]の順
	int iMidColor = 1 << (Bit - 1);
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);

	if (bDebugYokokasa){
		divImage = orgFimg;//主方向縮小画像
		divImage.SaveTmaTdt(FilePath + "1横傘_Div" + FileName);

		sabun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sabunclip.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sekibun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);

		for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
			sabun.Dt[0][iMainCnt] = iMidColor;
			sabunclip.Dt[0][iMainCnt] = iMidColor;
			for (iSubCnt = 1 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							orgFimg.Dt[iMainCnt][iSubCnt] - orgFimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);
			}

			for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + iMidColor);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2横傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3横傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4横傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
		divImage.DeleteDt();
	}


	if(fimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "5横傘_副LowPassfilter" + FileName);
	}


//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < fimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt];
		}
	}
	clipImg.DeleteDt();

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "6横傘_副HighPassfilter" + FileName);
	}


//主移動平均処理

	if(fimg.MainExpandMultiAve(MainLowPassFilter) == false){
    	CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません").c_str(), "Error", MB_OK);
        return false;
	}
	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "7横傘_主LowPassFilter" + FileName);
	}

#if 1
//副方向移動平均処理（HighCutフィルタ）
	if(fimg.SubExpandMultiAve(SubHighCutFilter) == false){
		CApplication::GetInstance()->MessageBox((SubHighCutFilter + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}
#endif

/*
	for(iSubCnt = 0; iSubCnt < fimg.SubPixel; ++iSubCnt){
		float fSum = 0.0f;
		int iCnt = 0;
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			if(orgFimg.Dt[iMainCnt][iSubCnt] > 0.0f && orgFimg.Dt[iMainCnt][iSubCnt] < 1000.0f){
				fSum += fimg.Dt[iMainCnt][iSubCnt];
				++iCnt;
			}
		}
		if(iCnt == 0){
			fSum = 0.0f;
		}else{
			fSum /= iCnt;
		}
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			fimg.Dt[iMainCnt][iSubCnt] = fSum;
		}
	}
*/

	fimg.LinearExpand(iMAIN_DIV, 1);

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "8横傘_雨" + FileName);
		sabun.DeleteDt();
	}


    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            SetVal(iSubCnt, iMainCnt,
                    (float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
/*
            if(Dt[iSubCnt][iMainCnt] < 1000){
                SetVal(iSubCnt, iMainCnt,
					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
			}
*/
        }
    }

	return true;
}
#endif
//---------------------------------------------------------------------------
//アルゴ改良版

//bool TSIMARS::bYokoKasa(const String SubHighCutFilter, const String MainLowPassFilter, const String SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, const String strImage)
//bool TSIMARS::bYokoKasa(const String SubHighCutFilter, const String MainLowPassFilter, const String SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, const String strImage, bool bConserveBoundaryDensity, const int iSubMedianKeepBoundary)
bool TSIMARS::bYokoKasa(string SubHighCutFilter, string MainLowPassFilter, string SubLowPassFilter, const float fClip, const int iMAIN_DIV,
		const bool bDebugYokokasa, string strImage, bool bConserveBoundaryDensity, const int iSubMedianKeepBoundary)
{
	TSIMARS OrgImg;
	OrgImg = *this;

	const int iYU = 16;//clip画像の連続clip限度画素数

	TFIMG   fimg;
	TFIMG   clipImg;
	TFIMG orgFimg;
    int iMainCnt, iSubCnt;

//noise↓ 主方向縮小

	orgFimg.NewDt(MainPixel, SubPixel);
    for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
        for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
			orgFimg.Dt[iMainCnt][iSubCnt] = Dt[iSubCnt][iMainCnt];
        }
    }
	orgFimg.DivWithRest(iMAIN_DIV, 1);

//副差分clip画像作成
    float *pfDif, *pfClipDif;
    int iSrt, iEnd;
    int iClipCnt;
	TFIMG fClipDif;
	fClipDif.NewDt(1, orgFimg.SubPixel - 1);

    pfDif = new float[orgFimg.SubPixel - 1];
    pfClipDif = new float[orgFimg.SubPixel - 1];
    fimg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
    clipImg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
	for (iMainCnt = 0; iMainCnt < orgFimg.MainPixel; iMainCnt++){
			//差分
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			pfDif[iSubCnt] = orgFimg.Dt[iMainCnt][iSubCnt + 1] - orgFimg.Dt[iMainCnt][iSubCnt];
			pfClipDif[iSubCnt] = pfDif[iSubCnt];
		}
            //差分clip
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
            iEnd = 0;
			if (pfDif[iSubCnt] > fClip){
                iSrt = iSubCnt;
                int iYuCnt = 0;
                while(iSrt > iEnd && pfDif[iSrt - 1] < pfDif[iSrt] && iYuCnt < iYU){
                    --iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] > fClip){
                    ++iSubCnt;
                }
                iEnd = iSubCnt;
                while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] < pfDif[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }else if(pfDif[iSubCnt] < - fClip){
                iSrt = iSubCnt;
				int iYuCnt = 0;
                while(iSrt > iEnd && pfDif[iSrt - 1] > pfDif[iSrt] && iYuCnt < iYU){
					--iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] < - fClip){
                    ++iSubCnt;
                }
                iEnd = iSubCnt;
                while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] > pfDif[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }
        }

		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			fClipDif.Dt[0][iSubCnt] = pfClipDif[iSubCnt];
        }

            //積分
		fimg.Dt[iMainCnt][0] = clipImg.Dt[iMainCnt][0] = orgFimg.Dt[iMainCnt][0];
		for (iSubCnt = 1; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt - 1] + fClipDif.Dt[0][iSubCnt - 1];
			clipImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
		}
    }
	delete[] pfDif;
	delete[] pfClipDif;

	TTMA sabun, sabunclip, sekibun, divImage;//Dt[S][M]の順
	int iMidColor = 1 << (Bit - 1);
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);

	if (bDebugYokokasa){
		divImage = orgFimg;//主方向縮小画像
		divImage.SaveTmaTdt(FilePath + "1横傘_Div" + FileName);

		sabun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sabunclip.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sekibun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);

		for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
			sabun.Dt[0][iMainCnt] = iMidColor;
			sabunclip.Dt[0][iMainCnt] = iMidColor;
			for (iSubCnt = 1 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							orgFimg.Dt[iMainCnt][iSubCnt] - orgFimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);
			}

			for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
//				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + iMidColor);
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt]);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2横傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3横傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4横傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
		divImage.DeleteDt();
	}


	if(fimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox(string(SubLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
        return false;
	}

	if (bDebugYokokasa){
//		sabun = (fimg + iMidColor);
		sabun = fimg;
		sabun.SaveTmaTdt(FilePath + "5横傘_副LowPassfilter" + FileName);
	}


//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < fimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt];
		}
	}
	clipImg.DeleteDt();

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "6横傘_副HighPassfilter" + FileName);
	}


//主移動平均処理

	if(fimg.MainExpandMultiAve(MainLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
        return false;
	}
	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "7横傘_主LowPassFilter" + FileName);
	}

#if 1
//副方向移動平均処理（HighCutフィルタ）
	if(fimg.SubExpandMultiAve(SubHighCutFilter) == false){
		CApplication::GetInstance()->MessageBox((SubHighCutFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
#endif

/*
	for(iSubCnt = 0; iSubCnt < fimg.SubPixel; ++iSubCnt){
		float fSum = 0.0f;
		int iCnt = 0;
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			if(orgFimg.Dt[iMainCnt][iSubCnt] > 0.0f && orgFimg.Dt[iMainCnt][iSubCnt] < 1000.0f){
				fSum += fimg.Dt[iMainCnt][iSubCnt];
				++iCnt;
			}
		}
		if(iCnt == 0){
			fSum = 0.0f;
		}else{
			fSum /= iCnt;
		}
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			fimg.Dt[iMainCnt][iSubCnt] = fSum;
		}
	}
*/

	fimg.LinearExpand(iMAIN_DIV, 1);

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "8横傘_雨" + FileName);
		sabun.DeleteDt();
	}


    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            SetVal(iSubCnt, iMainCnt,
					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
/*
            if(Dt[iSubCnt][iMainCnt] < 1000){
                SetVal(iSubCnt, iMainCnt,
                    (float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
            }
*/
        }
    }


	//短冊画像の上下端の不連続性を補正
	if (bConserveBoundaryDensity) {
		TFIMG fImgRate;
		fImgRate.NewDt(OrgImg.MainPixel, OrgImg.SubPixel);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				fImgRate.Dt[iM][iS] = (float)OrgImg.Dt[iS][iM] - Dt[iS][iM];//画像data（Dt[iM][iS]の順）
			}
		}
		fImgRate.SubMedian(iSubMedianKeepBoundary);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				SetVal(iS, iM, fImgRate.Dt[iM][iS] + Dt[iS][iM]);//画像data（Dt[iM][iS]の順）
			}
		}
		WriteTma( AddPrefix( strImage, "y(boundary)" ));
	}

	return true;
}
//---------------------------------------------------------------------------
//LongPanelの短冊横傘用に微分画像をInt型へ変換(TFIMG→TIIMGへ)

bool TSIMARS::bYokoKasaIntType(string SubHighCutFilter, string MainLowPassFilter, string SubLowPassFilter, int iClip, int iMAIN_DIV, bool bDebugYokokasa, string strImage, bool bConserveBoundaryDensity, int iSubMedianKeepBoundary)
{
	TSIMARS OrgImg;
	OrgImg = *this;

	const int iYU = 16;//clip画像の連続clip限度画素数

	TIIMG   iimg;
	TIIMG   clipImg;
	TTMA    orgUIimg;
	int iMainCnt, iSubCnt;

//noise↓ 主方向縮小
	orgUIimg.NewDt(SubPixel, MainPixel, Bit);
	orgUIimg = *this;
	orgUIimg.DivWithRest(1, iMAIN_DIV);


//副差分clip画像作成
	int *piDif, *piClipDif;
	int iSrt, iEnd;
	int iClipCnt;
	TIIMG iClipDif;
	iClipDif.NewDt(1, orgUIimg.SubPixel - 1);

	piDif = new int[orgUIimg.SubPixel - 1];
	piClipDif = new int[orgUIimg.SubPixel - 1];
	iimg.NewDt(orgUIimg.MainPixel, orgUIimg.SubPixel);
	clipImg.NewDt(orgUIimg.MainPixel, orgUIimg.SubPixel);
	for (iMainCnt = 0; iMainCnt < orgUIimg.MainPixel; iMainCnt++){
			//差分
		for (iSubCnt = 0; iSubCnt < orgUIimg.SubPixel - 1; iSubCnt++){
			piDif[iSubCnt] = orgUIimg.Dt[iSubCnt + 1][iMainCnt] - orgUIimg.Dt[iSubCnt][iMainCnt];
			piClipDif[iSubCnt] = piDif[iSubCnt];
		}
			//差分clip
		for (iSubCnt = 0; iSubCnt < orgUIimg.SubPixel - 1; iSubCnt++){
			iEnd = 0;
			if (piDif[iSubCnt] > iClip){
				iSrt = iSubCnt;
				int iYuCnt = 0;
				while(iSrt > iEnd && piDif[iSrt - 1] < piDif[iSrt] && iYuCnt < iYU){
					--iSrt;
					++iYuCnt;
				}
				while(iSubCnt < orgUIimg.SubPixel - 2 && piDif[iSubCnt + 1] > iClip){
					++iSubCnt;
				}
				iEnd = iSubCnt;
				while(iEnd < orgUIimg.SubPixel - 2 && piDif[iEnd + 1] < piDif[iEnd]){
					++iEnd;
				}
				for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
					piClipDif[iClipCnt] = 0;
				}
			}else if(piDif[iSubCnt] < - iClip){
				iSrt = iSubCnt;
				int iYuCnt = 0;
				while(iSrt > iEnd && piDif[iSrt - 1] > piDif[iSrt] && iYuCnt < iYU){
					--iSrt;
					++iYuCnt;
				}
				while(iSubCnt < orgUIimg.SubPixel - 2 && piDif[iSubCnt + 1] < - iClip){
					++iSubCnt;
				}
				iEnd = iSubCnt;
				while(iEnd < orgUIimg.SubPixel - 2 && piDif[iEnd + 1] > piDif[iEnd]){
					++iEnd;
				}
				for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
					piClipDif[iClipCnt] = 0;
				}
			}
		}

		for (iSubCnt = 0; iSubCnt < orgUIimg.SubPixel - 1; iSubCnt++){
			iClipDif.Dt[0][iSubCnt] = piClipDif[iSubCnt];
		}

			//積分
		iimg.Dt[iMainCnt][0] = clipImg.Dt[iMainCnt][0] = orgUIimg.Dt[0][iMainCnt];
		for (iSubCnt = 1; iSubCnt < iimg.SubPixel; iSubCnt++){
			iimg.Dt[iMainCnt][iSubCnt] = iimg.Dt[iMainCnt][iSubCnt - 1] + iClipDif.Dt[0][iSubCnt - 1];
			clipImg.Dt[iMainCnt][iSubCnt] = iimg.Dt[iMainCnt][iSubCnt];
		}
	}
	delete[] piDif;
	delete[] piClipDif;



	TTMA sabun, sabunclip, sekibun, divImage;//Dt[S][M]の順
	int iMidColor = 1 << (Bit - 1);
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);

	if (bDebugYokokasa){
		divImage = orgUIimg;//主方向縮小画像
#ifdef _LP_DEBUG_OUTPUT
		divImage.SaveTmaTdt(FilePath + "1横傘_Div" + FileName);
#endif
		sabun.NewDt(iimg.SubPixel, iimg.MainPixel, Bit);
		sabunclip.NewDt(iimg.SubPixel, iimg.MainPixel, Bit);
		sekibun.NewDt(iimg.SubPixel, iimg.MainPixel, Bit);

		for (iMainCnt = 0 ; iMainCnt < iimg.MainPixel ; iMainCnt++){
			sabun.Dt[0][iMainCnt] = iMidColor;
			sabunclip.Dt[0][iMainCnt] = iMidColor;
			for (iSubCnt = 1 ; iSubCnt < iimg.SubPixel ; iSubCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							orgUIimg.Dt[iSubCnt][iMainCnt] - orgUIimg.Dt[iSubCnt - 1][iMainCnt] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							iimg.Dt[iMainCnt][iSubCnt] - iimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);
			}

			for(iSubCnt = 0 ; iSubCnt < iimg.SubPixel ; iSubCnt++){
//				sekibun.SetVal(iSubCnt, iMainCnt, iimg.Dt[iMainCnt][iSubCnt] + iMidColor);
				sekibun.SetVal(iSubCnt, iMainCnt, iimg.Dt[iMainCnt][iSubCnt]);
			}
		}
#ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "2横傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3横傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4横傘_積分" + FileName);
#endif
		sabunclip.DeleteDt();
		sekibun.DeleteDt();
		divImage.DeleteDt();
	}

	TFIMG fimg;
#if 0
	fimg = iimg;
	if(fimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
	iimg = fimg;
#else
	if(iimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
#endif
//	if (bDebugYokokasa){
////		sabun = (fimg + iMidColor);
//		sabun = (CTDAT)iimg;
//		sabun.SetBit(Bit);
//#ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "5横傘_副LowPassfilter" + FileName);
//#endif
//	}



//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < iimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < iimg.SubPixel; iSubCnt++){
			iimg.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - iimg.Dt[iMainCnt][iSubCnt];
		}
	}
	clipImg.DeleteDt();

//	if (bDebugYokokasa){
////		sabun = (fimg + iMidColor);
//		sabun = (CTDAT)(iimg + iMidColor);//iimgがiMicColor加算されて更新される
//		sabun.SetBit(Bit);
//#ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "6横傘_副HighPassfilter" + FileName);
//#endif
//	}

//主移動平均処理
#if 0
	fimg = iimg;
	if(fimg.MainExpandMultiAve(MainLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
#else
	iimg.MainSub();
	if(iimg.SubExpandMultiAve(MainLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
	iimg.MainSub();
#endif
//	if (bDebugYokokasa){
//		sabun.DeleteDt();
//		sabun = (CTDAT)iimg;
//		sabun.SetBit(Bit);
//#ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "7横傘_主LowPassFilter" + FileName);
//#endif
//	}

#if 1
//副方向移動平均処理（HighCutフィルタ）
//	if(fimg.SubExpandMultiAve(SubHighCutFilter) == false){
	if(iimg.SubExpandMultiAve(SubHighCutFilter) == false){
		CApplication::GetInstance()->MessageBox((SubHighCutFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
//	iimg = fimg;
#endif


#if 0
	fimg.LinearExpand(iMAIN_DIV, 1);
#else
//	sabun = (CTDAT)iimg;
//	sabun.SetBit(Bit);
//	sabun.LinearExpand(1, iMAIN_DIV);
#endif

	if (bDebugYokokasa){
#ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "8横傘_雨" + FileName);
#endif
	}


#if 0 //確認実装（LongPanel短冊横傘時の副方向低周波を除去した雨をここで計算する場合）
	int iSubMedTmp = 541;
	TTMA SubMed;
	SubMed = sabun;
	SubMed.SubMedian(iSubMedTmp);//雨に副Median（端処理は可変長）をかけて副方向の低周波成分を抽出
	if (bDebugYokokasa){
 #ifdef _LP_DEBUG_OUTPUT
		SubMed.SaveTmaTdt(FilePath + "8.1SubMedian(横傘_雨)" + FileName);
 #endif
	}
//	sabun = sabun - SubMed + iMidColor;
	for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			sabun.SetVal(iSubCnt, iMainCnt, sabun.Dt[iSubCnt][iMainCnt] - SubMed.Dt[iSubCnt][iMainCnt] + iMidColor);
		}
	}
	if (bDebugYokokasa){
 #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "8.2横傘_雨(低周波除去)" + FileName);
 #endif
	}
#endif

	for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			SetVal(iSubCnt, iMainCnt,
//					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
					Dt[iSubCnt][iMainCnt] - sabun.Dt[iSubCnt][iMainCnt] + iMidColor);
/*
			if(Dt[iSubCnt][iMainCnt] < 1000){
				SetVal(iSubCnt, iMainCnt,
					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
			}
*/
		}
	}
	if (bDebugYokokasa){
#ifdef _LP_DEBUG_OUTPUT
		SaveTmaTdt(FilePath + "9横傘後" + FileName);
#endif
	}


	//短冊画像の上下端の不連続性を補正
	if (bConserveBoundaryDensity) {
		TFIMG fImgRate;
		fImgRate.NewDt(OrgImg.MainPixel, OrgImg.SubPixel);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				fImgRate.Dt[iM][iS] = (float)OrgImg.Dt[iS][iM] - Dt[iS][iM];//画像data（Dt[iM][iS]の順）
			}
		}
		fImgRate.SubMedian(iSubMedianKeepBoundary);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				SetVal(iS, iM, fImgRate.Dt[iM][iS] + Dt[iS][iM]);//画像data（Dt[iM][iS]の順）
			}
		}
#ifdef _LP_DEBUG_OUTPUT
		WriteTma( AddPrefix( strImage, "y(boundary)" ));
#endif
	}
	return true;
}
//---------------------------------------------------------------------------
//LongPanelの短冊横傘用に微分画像をInt型へ変換
//主・副2つのフィルターをMIX

bool TSIMARS::bYokoKasaIntTypeMix2Filter(string SubHighCutFilter, string MainLowPassFilter, string SubLowPassFilter, string SubHighCutFilter2, string MainLowPassFilter2, string SubLowPassFilter2, const int iClip, const int iMAIN_DIV, const bool bDebugYokokasa, const string strImage, bool bConserveBoundaryDensity, const int iSubMedianKeepBoundary, TTMA& ioUmbImg)
{
	TSIMARS OrgImg;
	OrgImg = *this;

	const int iYU = 16;//clip画像の連続clip限度画素数

	TIIMG   iimg, iimg2;
	TIIMG   clipImg;
	TTMA    orgUIimg;
	int iMainCnt, iSubCnt;

//noise↓ 主方向縮小
	orgUIimg.NewDt(SubPixel, MainPixel, Bit);
	orgUIimg = *this;
	orgUIimg.DivWithRest(1, iMAIN_DIV);


//副差分clip画像作成
	int *piDif, *piClipDif;
	int iSrt, iEnd;
	int iClipCnt;
	TIIMG iClipDif;
	iClipDif.NewDt(1, orgUIimg.SubPixel - 1);

	piDif = new int[orgUIimg.SubPixel - 1];
	piClipDif = new int[orgUIimg.SubPixel - 1];
	iimg.NewDt(orgUIimg.MainPixel, orgUIimg.SubPixel);
	clipImg.NewDt(orgUIimg.MainPixel, orgUIimg.SubPixel);
	for (iMainCnt = 0; iMainCnt < orgUIimg.MainPixel; iMainCnt++){
			//差分
		for (iSubCnt = 0; iSubCnt < orgUIimg.SubPixel - 1; iSubCnt++){
			piDif[iSubCnt] = orgUIimg.Dt[iSubCnt + 1][iMainCnt] - orgUIimg.Dt[iSubCnt][iMainCnt];
			piClipDif[iSubCnt] = piDif[iSubCnt];
		}
			//差分clip
		for (iSubCnt = 0; iSubCnt < orgUIimg.SubPixel - 1; iSubCnt++){
			iEnd = 0;
			if (piDif[iSubCnt] > iClip){
				iSrt = iSubCnt;
				int iYuCnt = 0;
				while(iSrt > iEnd && piDif[iSrt - 1] < piDif[iSrt] && iYuCnt < iYU){
					--iSrt;
					++iYuCnt;
				}
				while(iSubCnt < orgUIimg.SubPixel - 2 && piDif[iSubCnt + 1] > iClip){
					++iSubCnt;
				}
				iEnd = iSubCnt;
				while(iEnd < orgUIimg.SubPixel - 2 && piDif[iEnd + 1] < piDif[iEnd]){
					++iEnd;
				}
				for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
					piClipDif[iClipCnt] = 0;
				}
			}else if(piDif[iSubCnt] < - iClip){
				iSrt = iSubCnt;
				int iYuCnt = 0;
				while(iSrt > iEnd && piDif[iSrt - 1] > piDif[iSrt] && iYuCnt < iYU){
					--iSrt;
					++iYuCnt;
				}
				while(iSubCnt < orgUIimg.SubPixel - 2 && piDif[iSubCnt + 1] < - iClip){
					++iSubCnt;
				}
				iEnd = iSubCnt;
				while(iEnd < orgUIimg.SubPixel - 2 && piDif[iEnd + 1] > piDif[iEnd]){
					++iEnd;
				}
				for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
					piClipDif[iClipCnt] = 0;
				}
			}
		}

		for (iSubCnt = 0; iSubCnt < orgUIimg.SubPixel - 1; iSubCnt++){
			iClipDif.Dt[0][iSubCnt] = piClipDif[iSubCnt];
		}

			//積分
		iimg.Dt[iMainCnt][0] = clipImg.Dt[iMainCnt][0] = orgUIimg.Dt[0][iMainCnt];
		for (iSubCnt = 1; iSubCnt < iimg.SubPixel; iSubCnt++){
			iimg.Dt[iMainCnt][iSubCnt] = iimg.Dt[iMainCnt][iSubCnt - 1] + iClipDif.Dt[0][iSubCnt - 1];
			clipImg.Dt[iMainCnt][iSubCnt] = iimg.Dt[iMainCnt][iSubCnt];
		}
	}
	delete[] piDif;
	delete[] piClipDif;



	TTMA sabun, sabunclip, sekibun, divImage;//Dt[S][M]の順
	int iMidColor = 1 << (Bit - 1);
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);

	if (bDebugYokokasa){
		divImage = orgUIimg;//主方向縮小画像
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		divImage.SaveTmaTdt(FilePath + "1横傘_Div" + FileName);
  #endif
 #endif
		sabun.NewDt(iimg.SubPixel, iimg.MainPixel, Bit);
		sabunclip.NewDt(iimg.SubPixel, iimg.MainPixel, Bit);
		sekibun.NewDt(iimg.SubPixel, iimg.MainPixel, Bit);

		for (iMainCnt = 0 ; iMainCnt < iimg.MainPixel ; iMainCnt++){
			sabun.Dt[0][iMainCnt] = iMidColor;
			sabunclip.Dt[0][iMainCnt] = iMidColor;
			for (iSubCnt = 1 ; iSubCnt < iimg.SubPixel ; iSubCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							orgUIimg.Dt[iSubCnt][iMainCnt] - orgUIimg.Dt[iSubCnt - 1][iMainCnt] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							iimg.Dt[iMainCnt][iSubCnt] - iimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);
			}

			for(iSubCnt = 0 ; iSubCnt < iimg.SubPixel ; iSubCnt++){
//				sekibun.SetVal(iSubCnt, iMainCnt, iimg.Dt[iMainCnt][iSubCnt] + iMidColor);
				sekibun.SetVal(iSubCnt, iMainCnt, iimg.Dt[iMainCnt][iSubCnt]);
			}
		}
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "2横傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3横傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4横傘_積分" + FileName);
  #endif
 #endif
		sabunclip.DeleteDt();
		sekibun.DeleteDt();
		divImage.DeleteDt();
	}

	iimg2 = iimg;

//副フィルター処理1
 #if 0
	SubLowPassFilter = "C:\\yFPD1_0.18_150um_16.txt"; //副cutoff0.18c/mm
 #endif
	if(iimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
//	if (bDebugYokokasa){
//		sabun = (CTDAT)iimg;
//		sabun.SetBit(Bit);
// #ifdef _LOG_TANZAKUYOKOKASA
//  #ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "5横傘_副LowPassfilter" + FileName);
//  #endif
// #endif
//	}


//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < iimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < iimg.SubPixel; iSubCnt++){
			iimg.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - iimg.Dt[iMainCnt][iSubCnt];
		}
	}
//	clipImg.DeleteDt();

//	if (bDebugYokokasa){
//		sabun = (CTDAT)(iimg + iMidColor);//iimgがiMicColor加算されて更新される
//		sabun.SetBit(Bit);
// #ifdef _LOG_TANZAKUYOKOKASA
//  #ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "6横傘_副HighPassfilter" + FileName);
//  #endif
// #endif
//	}

//主フィルター処理１
 #if 0
	MainLowPassFilter = "C:\\yFPD1_0.02_150um_16.txt"; //主cutoff0.02c/mm
 #endif
	iimg.MainSub();

 #if 1
	if(iimg.SubExpandMultiAve(MainLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
 #else //Median処理に変更してみる（日赤VGで椎体の視認性問題を受けて）（160506 ttsuji）　→シンチ段差の補正残差と椎体視認性の両立はできずNG。
	const int iSubMedNum = 31;
//	const int iSubMedNum = 51;
//	const int iSubMedNum = 101;
	iimg.SubMedian(iSubMedNum);
 #endif
	iimg.MainSub();
//	if (bDebugYokokasa){
//		sabun.DeleteDt();
//		sabun = (CTDAT)iimg;
//		sabun.SetBit(Bit);
// #ifdef _LOG_TANZAKUYOKOKASA
//  #ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "7横傘_主LowPassFilter" + FileName);
//  #endif
// #endif
//	}



//副フィルター処理2
 #if 1 //２冪型フィルター
  #if 0
	String SubLowPassFilter2 = "C:\\yFPD1_0.02_150um_16.txt"; //副cutoff0.02c/mm (default)
////String SubLowPassFilter2 = "C:\\yFPD1_0.01_150um_16.txt"; //副cutoff0.01c/mm（サイドローブあり、フィルターNG）
//	String SubLowPassFilter2 = "C:\\yFPD1_0.01_150um(int691).txt"; //副cutoff0.01c/mm（画質OKとなるが、段数200くらいあり、処理時間がかかりNG）
//	String SubLowPassFilter2 = "C:\\yFPD1_0.01_150um(int597).txt"; //副cutoff0.01c/mm（画質OK、かつ段数13で、処理時間もOK）
  #endif
	if(iimg2.SubExpandMultiAve(SubLowPassFilter2) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter2 + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
 #else //浮動小数点型フィルター（ｆ特の精度はこちらが上）
	String SubLowPassFilter2 = "C:\\yFPD1_0.01_150um(float701).txt"; //副cutoff0.01c/mm →タップ数を701も使って面積１の厳密なフィルターであり、最も精度が良い。
//	String SubLowPassFilter2 = "C:\\yFPD1_0.01_150um(float501).txt"; //副cutoff0.01c/mm
	float filter[1024];//タップ数の上限をふやした
	fstream file;
	int ct;
	file.open(SubLowPassFilter2.c_str(), ios::in);
	if(file.fail()){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter2 + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}
	ct = -1;
	while(!file.eof()){
		++ct;
		file >> filter[ct];
	}
	file.close();
	iimg2.SubExpandFilter(filter, ct);
 #endif
//	if (bDebugYokokasa){
//		sabun = (CTDAT)iimg2;
//		sabun.SetBit(Bit);
// #ifdef _LOG_TANZAKUYOKOKASA
//  #ifdef _LP_DEBUG_OUTPUT
//		sabun.SaveTmaTdt(FilePath + "8横傘_副LowPassfilter2" + FileName);
//  #endif
// #endif
//	}


//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < iimg2.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < iimg2.SubPixel; iSubCnt++){
			iimg2.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - iimg2.Dt[iMainCnt][iSubCnt];
		}
	}
	clipImg.DeleteDt();

	if (bDebugYokokasa){
 #if 0	//実行エラーで落ちるため、べた書きする。
		sabun = (iimg2 + iMidColor);//iimgがiMicColor加算されて更新される
 #else
		int i, j;
		for (i = 0 ; i < iimg2.SubPixel ; i++){
			for (j = 0 ; j < iimg2.MainPixel ; j++){
				iimg2.SetVal(j, i, iimg2.Dt[j][i] + iMidColor);
				sabun.SetVal(i, j, iimg2.Dt[j][i]);
			}
		}

 #endif
		sabun.SetBit(Bit);
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "9横傘_副HighPassfilter2" + FileName);
  #endif
 #endif
	}

//主フィルター処理2
 #if 0
//	String MainLowPassFilter2 = "C:\\yFPD1_0.05_150um_16.txt"; //主cutoff0.05c/mm
	String MainLowPassFilter2 = "C:\\yFPD1_0.09_150um_16.txt"; //主cutoff0.09c/mm (default)
//	String MainLowPassFilter2 = "C:\\yFPD1_0.12_150um_16.txt"; //主cutoff0.12c/mm
//	String MainLowPassFilter2 = "C:\\yFPD1_0.18_150um_16.txt"; //主cutoff0.18c/mm
 #endif
	iimg2.MainSub();
	if(iimg2.SubExpandMultiAve(MainLowPassFilter2) == false){
		CApplication::GetInstance()->MessageBox((MainLowPassFilter2 + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
	iimg2.MainSub();
	if (bDebugYokokasa){
		sabun.DeleteDt();
		sabun = (CTDAT)iimg2;
		sabun.SetBit(Bit);
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "10横傘_主LowPassFilter2" + FileName);
  #endif
 #endif
	}

//副HighCutフィルター処理
//	SubHighCutFilter = "C:\\yFPD1_0.18_150um_16.txt"; //副cutoff0.18c/mm (default)
	if(iimg2.SubExpandMultiAve(SubHighCutFilter2) == false){
		CApplication::GetInstance()->MessageBox((SubHighCutFilter2 + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
	if (bDebugYokokasa){
		sabun.DeleteDt();
		sabun = (CTDAT)iimg2;
		sabun.SetBit(Bit);
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "11横傘_副HighCutFilter2" + FileName);
  #endif
 #endif
	}


 #if 1
//	sabun = iimg + iimg2;
	for (iMainCnt = 0; iMainCnt < iimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < iimg.SubPixel; iSubCnt++){
			sabun.SetVal(iSubCnt, iMainCnt, iimg.Dt[iMainCnt][iSubCnt] + iimg2.Dt[iMainCnt][iSubCnt] - iMidColor);
		}
	}
 #elif 0 //test 160420(副低周波側のみの雨として傘処理する)
	sabun = iimg2;
 #elif 0 //test 160421(シンチ段差近傍とそれ以外とで雨を分ける) →NG（非重複領域がｙ方向に幅を持っており、この幅内に椎体が配置される可能性が高くなるため）
	for (iMainCnt = 0; iMainCnt < iimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < iimg.SubPixel; iSubCnt++){
//			if (161 <= iSubCnt && iSubCnt <= 168 || 129 <= iSubCnt && iSubCnt <= 135) {//シンチ段差近傍　→副方向の高周波も含んだ雨とする（シンチ段差を補正するため）
			if (157 <= iSubCnt && iSubCnt <= 163 || 145 <= iSubCnt && iSubCnt <= 155 || 128 <= iSubCnt && iSubCnt <= 135) {//シンチ段差近傍　→副方向の高周波も含んだ雨とする（シンチ段差を補正するため）
				sabun.SetVal(iSubCnt, iMainCnt, iimg.Dt[iMainCnt][iSubCnt] + iimg2.Dt[iMainCnt][iSubCnt] - iMidColor);
			}
			else{//シンチ段差近傍以外　→副方向の高周波は含まない雨とする（椎体の横エッジを残すため）
				sabun.SetVal(iSubCnt, iMainCnt, iimg2.Dt[iMainCnt][iSubCnt]);
			}
		}
	}

 #endif


	sabun.SetBit(Bit);
	sabun.LinearExpand(1, iMAIN_DIV);
	ioUmbImg = sabun;

	if (bDebugYokokasa){
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "12横傘_雨" + FileName);
  #endif
 #endif
	}


#if 0 //確認実装（LongPanel短冊横傘時の副方向低周波を除去した雨をここで計算する場合）
	int iSubMedTmp = 541;
	TTMA SubMed;
	SubMed = sabun;
	SubMed.SubMedian(iSubMedTmp);//雨に副Median（端処理は可変長）をかけて副方向の低周波成分を抽出
	if (bDebugYokokasa){
 #ifdef _LP_DEBUG_OUTPUT
		SubMed.SaveTmaTdt(FilePath + "8.1SubMedian(横傘_雨)" + FileName);
 #endif
	}
//	sabun = sabun - SubMed + iMidColor;
	for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			sabun.SetVal(iSubCnt, iMainCnt, sabun.Dt[iSubCnt][iMainCnt] - SubMed.Dt[iSubCnt][iMainCnt] + iMidColor);
		}
	}
	if (bDebugYokokasa){
 #ifdef _LP_DEBUG_OUTPUT
		sabun.SaveTmaTdt(FilePath + "8.2横傘_雨(低周波除去)" + FileName);
 #endif
	}
#endif

	for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			SetVal(iSubCnt, iMainCnt,
//					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
					Dt[iSubCnt][iMainCnt] - sabun.Dt[iSubCnt][iMainCnt] + iMidColor);
/*
			if(Dt[iSubCnt][iMainCnt] < 1000){
				SetVal(iSubCnt, iMainCnt,
					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
			}
*/
		}
	}
	if (bDebugYokokasa){
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		SaveTmaTdt(FilePath + "13横傘後" + FileName);
  #endif
 #endif
	}


	//短冊画像の上下端の不連続性を補正
	if (bConserveBoundaryDensity) {
		TFIMG fImgRate;
		fImgRate.NewDt(OrgImg.MainPixel, OrgImg.SubPixel);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				fImgRate.Dt[iM][iS] = (float)OrgImg.Dt[iS][iM] - Dt[iS][iM];//画像data（Dt[iM][iS]の順）
			}
		}
		fImgRate.SubMedian(iSubMedianKeepBoundary);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				SetVal(iS, iM, fImgRate.Dt[iM][iS] + Dt[iS][iM]);//画像data（Dt[iM][iS]の順）
			}
		}
 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		WriteTma( AddPrefix( strImage, "y(boundary)" ));
  #endif
 #endif
	}
	return true;
}
//---------------------------------------------------------------------------
//2013.6.25 FPGA実装時のコンペア用に, const bool IntYokokasaを追加 （田島）
//2014.07.18 D-EVO2FPGAのコンペア実施結果は、
//①蓄積までは膝画像（FPGAはクリップ処理で1/2するところをやらずに一致することを確認
//②高周波から乱数を入力し、高周波と低周波と拡大は乱数で1QL以内であることを確認
//ゆえに一致した判断した。

//bool TSIMARS::bYokoKasa(String test, const String SubHighCutFilter, const String MainLowPassFilter, const String SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, const String strImage)
bool TSIMARS::bYokoKasa(const String SubHighCutFilter, const String MainLowPassFilter, const String SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, const String strImage, const bool bIntYokokasa)
{
	const int iYU = 16;//clip画像の連続clip限度画素数

	TFIMG   fimg;
	TFIMG   clipImg;
	TFIMG orgFimg;
	int iMainCnt, iSubCnt;
	//IMG test;20170718tajimaコンペアで追加

//noise↓ 主方向縮小

	orgFimg.NewDt(MainPixel, SubPixel);
	for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
		for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
			orgFimg.Dt[iMainCnt][iSubCnt] = Dt[iSubCnt][iMainCnt];
		}
	}
#if 1
	orgFimg.DivWithRest(iMAIN_DIV, 1);
#else
	TTMA ImgTmp = *this;
	ImgTmp.DivTrancation(1, iMAIN_DIV);
	orgFimg = ImgTmp;
#endif
	//test = orgFimg;

//-----------------------
//差分画像をintで計算させるためfloatをintにキャストする
	if (bIntYokokasa) {
		for (iMainCnt = 0; iMainCnt < orgFimg.MainPixel; iMainCnt++){
			for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel ; iSubCnt++){
				orgFimg.Dt[iMainCnt][iSubCnt] = int( orgFimg.Dt[iMainCnt][iSubCnt] );
			}
		}
	}
//----------------------

//副差分clip画像作成
    float *pfDif, *pfClipDif;
    int iSrt, iEnd;
    int iClipCnt;
	TFIMG fClipDif;
	fClipDif.NewDt(1, orgFimg.SubPixel - 1);

    pfDif = new float[orgFimg.SubPixel - 1];
    pfClipDif = new float[orgFimg.SubPixel - 1];
	fimg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
    clipImg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
	for (iMainCnt = 0; iMainCnt < orgFimg.MainPixel; iMainCnt++){
			//差分
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			pfDif[iSubCnt] = orgFimg.Dt[iMainCnt][iSubCnt + 1] - orgFimg.Dt[iMainCnt][iSubCnt];
			pfClipDif[iSubCnt] = pfDif[iSubCnt];
		}
            //差分clip
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			iEnd = 0;
			if (pfDif[iSubCnt] > fClip){
				iSrt = iSubCnt;
				int iYuCnt = 0;
				while(iSrt > iEnd && pfDif[iSrt - 1] < pfDif[iSrt] && iYuCnt < iYU){//①探索エリア内で右側が単調増加のときは、開始点を左にずらす
                    --iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] > fClip){//②副方向を越えない範囲でクリップを超えているところまで探す
                    ++iSubCnt;
				}
                iEnd = iSubCnt;//終了点とする
		iYuCnt = 0;//追記田島2013/6/20
		while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] < pfDif[iEnd] && iYuCnt < iYU){//③副方向を越えない範囲で右側が単調減少している場合は終了点を右にずらす
                    ++iEnd;
					++iYuCnt;//追記田島2013/6/20 探索エリア内を追記
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){//開始点から終了点まで0にして、ムラとして除去されないようにする
                    pfClipDif[iClipCnt] = 0;
                }

				//iSubCnt -= extendPix;			//追記田島2013/6/20→上記理由で不要

            }else if(pfDif[iSubCnt] < - fClip){
                iSrt = iSubCnt;
		int iYuCnt = 0;
		while(iSrt > iEnd && pfDif[iSrt - 1] > pfDif[iSrt] && iYuCnt < iYU){//①左側が単調増加のときは開始点を左にする
			--iSrt;
			++iYuCnt;
		}
		while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] < - fClip){//②副方向の範囲内で、差分値がずっとクリップを超えているところまで終了点をずらす
			++iSubCnt;
		}
		iEnd = iSubCnt;

		iYuCnt = 0;//追記田島2013/6/20

                while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] > pfDif[iEnd] && iYuCnt < iYU){//③副方向を越えない範囲で右側が単調増加しているときは
			++iEnd;
			++iYuCnt;//追記田島2013/6/20 探索エリア内を追記
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }
        }

		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			fClipDif.Dt[0][iSubCnt] = pfClipDif[iSubCnt];
        }

            //積分
		fimg.Dt[iMainCnt][0] = clipImg.Dt[iMainCnt][0] = orgFimg.Dt[iMainCnt][0];

		for (iSubCnt = 1; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt - 1] + fClipDif.Dt[0][iSubCnt - 1];
			//fimg.Dt[iMainCnt][iSubCnt] = (fimg.Dt[iMainCnt][iSubCnt - 1] + fClipDif.Dt[0][iSubCnt - 1])/2;//20140717tajimaコンペア用に一時的に変更①下にもある

			clipImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
		}
	}
	delete[] pfDif;
	delete[] pfClipDif;


	TTMA sabun, sabunclip, sekibun, divImage;//Dt[S][M]の順
	int iMidColor2 = 1 << (Bit - 1);//2013/6/21 田島（ノマケンとデジタルコンペアするため）
	int iMidColor = 1 << (Bit - 1);//メモ：コンペアのとき0でやっていた
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);

	if (bDebugYokokasa){
		divImage = orgFimg;//主方向縮小画像
		divImage.SaveTmaTdt(FilePath + "1横傘_Div" + FileName);

		sabun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sabunclip.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sekibun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);

		for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
			sabun.Dt[0][iMainCnt] = iMidColor;
			sabunclip.Dt[0][iMainCnt] = iMidColor;
			for (iSubCnt = 1 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							orgFimg.Dt[iMainCnt][iSubCnt] - orgFimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor2);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);
			}

			for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + iMidColor2);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2横傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3横傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4横傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
		divImage.DeleteDt();
	}


	//fimg = test;20170718tajimaコンペアで追加
	//clipImg = test;20170718tajimaコンペアで追加

	if(fimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "5横傘_副LowPassfilter" + FileName);
	}


//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < fimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt];

		}
	}
	clipImg.DeleteDt();

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor2);
		sabun.SaveTmaTdt(FilePath + "6横傘_副HighPassfilter" + FileName);
	}

//-----------------------
//田島floatをintにキャストした。
	if (bIntYokokasa) {
		for (iMainCnt = 0; iMainCnt < fimg.MainPixel; iMainCnt++){
			for (iSubCnt = 0; iSubCnt < fimg.SubPixel ; iSubCnt++){
				fimg.Dt[iMainCnt][iSubCnt] = int( fimg.Dt[iMainCnt][iSubCnt]+0.5+ iMidColor2);//20140718tajima コンペアのために0.5削除+0.5
				fimg.Dt[iMainCnt][iSubCnt] -= iMidColor2;
			}
		}

		if (bDebugYokokasa){
			sabun = (fimg);
			sabun.SaveTmaTdt(FilePath + "キャスト後6横傘_副HighPassfilter" + FileName);
		}
	}
//-----------------------
//主移動平均処理

	if(fimg.MainExpandMultiAve(MainLowPassFilter) == false){
    	CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor2);
		sabun.SaveTmaTdt(FilePath + "7横傘_主LowPassFilter" + FileName);
	}

#if 1
//副方向移動平均処理（HighCutフィルタ）
	if(fimg.SubExpandMultiAve(SubHighCutFilter) == false){
		CApplication::GetInstance()->MessageBox((SubHighCutFilter + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}
#endif

/*
	for(iSubCnt = 0; iSubCnt < fimg.SubPixel; ++iSubCnt){
		float fSum = 0.0f;
		int iCnt = 0;
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			if(orgFimg.Dt[iMainCnt][iSubCnt] > 0.0f && orgFimg.Dt[iMainCnt][iSubCnt] < 1000.0f){
				fSum += fimg.Dt[iMainCnt][iSubCnt];
				++iCnt;
			}
		}
		if(iCnt == 0){
			fSum = 0.0f;
		}else{
			fSum /= iCnt;
		}
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			fimg.Dt[iMainCnt][iSubCnt] = fSum;
		}
	}
*/

//-----------------------
//田島floatをintにキャストした。
	if (bIntYokokasa) {
		for (iMainCnt = 0; iMainCnt < fimg.MainPixel; iMainCnt++){
			for (iSubCnt = 0; iSubCnt < fimg.SubPixel ; iSubCnt++){
				fimg.Dt[iMainCnt][iSubCnt] = int( fimg.Dt[iMainCnt][iSubCnt]+0.5+ iMidColor2);
				fimg.Dt[iMainCnt][iSubCnt] -= iMidColor2;
			}
		}
		if (bDebugYokokasa){
			sabun = (fimg);
			sabun.SaveTmaTdt(FilePath + "キャスト後7横傘_主LowPassFilter" + FileName);
		}
	}
//-----------------------

	fimg.LinearExpand(iMAIN_DIV, 1);

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor2);
		sabun.SaveTmaTdt(FilePath + "8横傘_雨" + FileName);
		sabun.DeleteDt();
	}


	for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			SetVal(iSubCnt, iMainCnt,
					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
			//SetVal(iSubCnt, iMainCnt,
			//		(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]*2);//20140717tajimaコンペア用に一時的に変更②上にもあり→結局FPGAを2で割らないようにしてコンペア
/*
			if(Dt[iSubCnt][iMainCnt] < 1000){
				SetVal(iSubCnt, iMainCnt,
                    (float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
            }
*/
        }
    }

	return true;
}
//---------------------------------------------------------------------------
//福与さん作成(DEVO3 FPGA G面補正における横傘コンペアのため)

bool TSIMARS::bYokoKasaD3(const String SubHighCutFilter, const String MainLowPassFilter, const String SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, const String strImage)
{
	CTDAT OrgImg;
	OrgImg = *this;

	const int iYU = 16;//clip画像の連続clip限度画素数

	TFIMG   fimg;
	TFIMG   clipImg;
	TFIMG orgFimg;
    int iMainCnt, iSubCnt;

//noise↓ 主方向縮小

	orgFimg.NewDt(MainPixel, SubPixel);
	for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
		for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
			orgFimg.Dt[iMainCnt][iSubCnt] = Dt[iSubCnt][iMainCnt];
		}
	}
	orgFimg.DivWithRest(iMAIN_DIV, 1);
	for(iSubCnt = 0; iSubCnt < orgFimg.SubPixel; ++iSubCnt){
		for(iMainCnt = 0; iMainCnt < orgFimg.MainPixel; ++iMainCnt){
			orgFimg.Dt[iMainCnt][iSubCnt] = int(orgFimg.Dt[iMainCnt][iSubCnt] + 0.5f);
		}
	}

//副差分clip画像作成
    float *pfDif, *pfClipDif;
    int iSrt, iEnd;
    int iClipCnt;
	TFIMG fClipDif;
	fClipDif.NewDt(1, orgFimg.SubPixel - 1);

    pfDif = new float[orgFimg.SubPixel - 1];
    pfClipDif = new float[orgFimg.SubPixel - 1];
    fimg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
    clipImg.NewDt(orgFimg.MainPixel, orgFimg.SubPixel);
	for (iMainCnt = 0; iMainCnt < orgFimg.MainPixel; iMainCnt++){
			//差分
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			pfDif[iSubCnt] = orgFimg.Dt[iMainCnt][iSubCnt + 1] - orgFimg.Dt[iMainCnt][iSubCnt];
			pfClipDif[iSubCnt] = pfDif[iSubCnt];
		}
            //差分clip
		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
            iEnd = 0;
			if (pfDif[iSubCnt] > fClip){
                iSrt = iSubCnt;
                int iYuCnt = 0;
                while(iSrt > iEnd && pfDif[iSrt - 1] < pfDif[iSrt] && iYuCnt < iYU){
                    --iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] > fClip){
                    ++iSubCnt;
                }
                iEnd = iSubCnt;
                while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] < pfDif[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }else if(pfDif[iSubCnt] < - fClip){
                iSrt = iSubCnt;
				int iYuCnt = 0;
                while(iSrt > iEnd && pfDif[iSrt - 1] > pfDif[iSrt] && iYuCnt < iYU){
					--iSrt;
                    ++iYuCnt;
                }
                while(iSubCnt < orgFimg.SubPixel - 2 && pfDif[iSubCnt + 1] < - fClip){
                    ++iSubCnt;
                }
                iEnd = iSubCnt;
                while(iEnd < orgFimg.SubPixel - 2 && pfDif[iEnd + 1] > pfDif[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; ++iClipCnt){
                    pfClipDif[iClipCnt] = 0;
                }
            }
        }

		for (iSubCnt = 0; iSubCnt < orgFimg.SubPixel - 1; iSubCnt++){
			fClipDif.Dt[0][iSubCnt] = pfClipDif[iSubCnt];
        }

            //積分
		fimg.Dt[iMainCnt][0] = clipImg.Dt[iMainCnt][0] = orgFimg.Dt[iMainCnt][0];
		for (iSubCnt = 1; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt - 1] + fClipDif.Dt[0][iSubCnt - 1];
			clipImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
		}
    }
	delete[] pfDif;
	delete[] pfClipDif;

	TTMA sabun, sabunclip, sekibun, divImage;//Dt[S][M]の順
	int iMidColor = 1 << (Bit - 1);
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);

	if (bDebugYokokasa){
		divImage = orgFimg;//主方向縮小画像
		divImage.SaveTmaTdt(FilePath + "1横傘_Div" + FileName);

		sabun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sabunclip.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);
		sekibun.NewDt(fimg.SubPixel, fimg.MainPixel, Bit);

		for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
			sabun.Dt[0][iMainCnt] = iMidColor;
			sabunclip.Dt[0][iMainCnt] = iMidColor;
			for (iSubCnt = 1 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							orgFimg.Dt[iMainCnt][iSubCnt] - orgFimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt - 1] + iMidColor);
			}

			for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
//				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + iMidColor);
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt]);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2横傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3横傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4横傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
		divImage.DeleteDt();
	}


	if(fimg.SubExpandMultiAveD3(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((SubLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
        return false;
	}

	if (bDebugYokokasa){
//		sabun = (fimg + iMidColor);
		sabun = fimg;
		sabun.SaveTmaTdt(FilePath + "5横傘_副LowPassfilter" + FileName);
	}


//元画像－低周波画像
	for (iMainCnt = 0; iMainCnt < fimg.MainPixel; iMainCnt++){
		for (iSubCnt = 0; iSubCnt < fimg.SubPixel; iSubCnt++){
			fimg.Dt[iMainCnt][iSubCnt] = clipImg.Dt[iMainCnt][iSubCnt] - int(fimg.Dt[iMainCnt][iSubCnt]);
		}
	}
	clipImg.DeleteDt();

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "6横傘_副HighPassfilter" + FileName);
	}


//主移動平均処理

	if(fimg.MainExpandMultiAveD3(MainLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((MainLowPassFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
        return false;
	}
	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "7横傘_主LowPassFilter" + FileName);
	}
/*
#if 1
//副方向移動平均処理（HighCutフィルタ）
	if(fimg.SubExpandMultiAve(SubHighCutFilter) == false){
		CApplication::GetInstance()->MessageBox((SubHighCutFilter + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return false;
	}
#endif
*/
/*
	for(iSubCnt = 0; iSubCnt < fimg.SubPixel; ++iSubCnt){
		float fSum = 0.0f;
		int iCnt = 0;
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			if(orgFimg.Dt[iMainCnt][iSubCnt] > 0.0f && orgFimg.Dt[iMainCnt][iSubCnt] < 1000.0f){
				fSum += fimg.Dt[iMainCnt][iSubCnt];
				++iCnt;
			}
		}
		if(iCnt == 0){
			fSum = 0.0f;
		}else{
			fSum /= iCnt;
		}
		for(iMainCnt = 0; iMainCnt < fimg.MainPixel; ++iMainCnt){
			fimg.Dt[iMainCnt][iSubCnt] = fSum;
		}
	}
*/
	for(int iMCnt = 0; iMCnt < fimg.MainPixel; ++iMCnt){
	  for(int iSCnt = 0; iSCnt < fimg.SubPixel; ++iSCnt){
		fimg.Dt[iMCnt][iSCnt] = round(fimg.Dt[iMCnt][iSCnt] + 32768.0f);
	  }
	}
	fimg.LinearExpand(iMAIN_DIV, 1);
	for(int iMCnt = 0; iMCnt < fimg.MainPixel; ++iMCnt){
	  for(int iSCnt = 0; iSCnt < fimg.SubPixel; ++iSCnt){
		fimg.Dt[iMCnt][iSCnt] = round(fimg.Dt[iMCnt][iSCnt]) - 32768.0f;
	  }
	}

	if (bDebugYokokasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "8横傘_雨" + FileName);
		sabun.DeleteDt();
	}


	for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            SetVal(iSubCnt, iMainCnt,
					(float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
/*
            if(Dt[iSubCnt][iMainCnt] < 1000){
                SetVal(iSubCnt, iMainCnt,
                    (float)Dt[iSubCnt][iMainCnt] - fimg.Dt[iMainCnt][iSubCnt]);
            }
*/
        }
    }


	//短冊画像の上下端の不連続性を補正
/*
	if (bConserveBoundaryDensity) {
		TFIMG fImgRate;
		fImgRate.NewDt(OrgImg.MainPixel, OrgImg.SubPixel);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				fImgRate.Dt[iM][iS] = (float)OrgImg.Dt[iS][iM] - Dt[iS][iM];//画像data（Dt[iM][iS]の順）
			}
		}
		fImgRate.SubMedian(iSubMedianKeepBoundary);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				SetVal(iS, iM, fImgRate.Dt[iM][iS] + Dt[iS][iM]);//画像data（Dt[iM][iS]の順）
			}
		}
		WriteTma( AddPrefix( strImage, "y(boundary)" ));
	}
*/
	return true;
}
//---------------------------------------------------------------------------
/*
   ぼけて良くない
*/
/*
void TSIMARS::To100um()
{
    CTDAT org;
    Copy(org);
    int iMCell = MainPixel / 2;
    int iSCell = SubPixel / 2;
    NewDt(iSCell * 3, iMCell * 3, Bit);
    for(int iSCellCnt = 0; iSCellCnt < iSCell; ++iSCellCnt){
        for(int iMCellCnt = 0; iMCellCnt < iMCell; ++iMCellCnt){
            Dt[iSCellCnt * 3][iMCellCnt * 3] = org.Dt[iSCellCnt * 2][iMCellCnt * 2];
            Dt[iSCellCnt * 3][iMCellCnt * 3 + 1] = (unsigned short int)
                    ((org.Dt[iSCellCnt * 2][iMCellCnt * 2] + org.Dt[iSCellCnt * 2][iMCellCnt * 2 + 1]) / 2);
            Dt[iSCellCnt * 3][iMCellCnt * 3 + 2] = org.Dt[iSCellCnt * 2][iMCellCnt * 2 + 1];

            Dt[iSCellCnt * 3 + 1][iMCellCnt * 3] = (unsigned short int)
                    ((org.Dt[iSCellCnt * 2][iMCellCnt * 2] + org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2]) / 2);
            Dt[iSCellCnt * 3 + 1][iMCellCnt * 3 + 1] = (unsigned short int)
                    ((org.Dt[iSCellCnt * 2][iMCellCnt * 2] + org.Dt[iSCellCnt * 2][iMCellCnt * 2 + 1]
                    + org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2] + org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2 + 1]) / 4);
            Dt[iSCellCnt * 3 + 1][iMCellCnt * 3 + 2] = (unsigned short int)
                    ((org.Dt[iSCellCnt * 2][iMCellCnt * 2 + 1] + org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2 + 1]) / 2);

            Dt[iSCellCnt * 3 + 2][iMCellCnt * 3] = org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2];
            Dt[iSCellCnt * 3 + 2][iMCellCnt * 3 + 1] = (unsigned short int)
                    ((org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2] + org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2 + 1]) / 2);
            Dt[iSCellCnt * 3 + 2][iMCellCnt * 3 + 2] = org.Dt[iSCellCnt * 2 + 1][iMCellCnt * 2 + 1];
        }
    }
}
*/
//---------------------------------------------------------------------------
//ASIC
void TSIMARS::CorStepASIC2(const int iMm, const int iMp,const int iMEDIAN,const int iTh,vector<unsigned short> a01,vector<unsigned short> a10,vector<unsigned short> a11)
{
    //TODO: この下にコードを追加してください
//    const int iMEDIAN = 201;//51;//101;//201;
	const int iASIC = 128;//FPD1用
    const int iOFFSET = -96;// = (128 * 24 - 2880) / 2

    if(MainPixel < iASIC + iMp){
		return;
    }

//////////////
/*    //補正強度テーブル作成（1→1、0→1、1→0）
    //16ビット型テーブルのテスト
    vector<unsigned short> flat(iASIC), R0_1(iASIC), R1_0(iASIC);
    float rate = (float)65535/(iASIC - 1);
    int i;
    for(i = 0; i < iASIC; i++){
        flat[i] = (unsigned short)65535;
    }

    int iNum = iASIC + iMm;
    for(i = 0; i < iNum; i++){
        R0_1[i] = (unsigned short)(rate*i + 0.5);
    }
    for( ; i < iASIC; i++){
        R0_1[i] = (unsigned short)65535;
    }

    iNum = iASIC - 1 - iMp;
    for(i = 0; i < iNum; i++){
        R1_0[iASIC - 1 - i] = (unsigned short)(rate*i + 0.5);
    }
    for( ; i < iASIC; i++){
        R1_0[iASIC - 1 - i] = (unsigned short)65535;
    }


    FILE* fp_A01  = fopen(A01, "rt");
    FILE* fp_A10  = fopen(A10, "rt");
    FILE* fp_A11  = fopen(A11, "rt");

    vector<unsigned short> flat(iASIC),R0_1(iASIC), R1_0(iASIC);

    int i;
    for(i = 0; i < iASIC; i++){
        fscanf(fp_A11, "%d\n", &flat[i]);
    }

    int iNum = iASIC + iMm;
    for(i = 0; i < iNum; i++){
        fscanf(fp_A01, "%d\n", &R0_1[i]);
    }

    iNum = iASIC - 1 - iMp;
    for(i = 0; i < iNum; i++){
        fscanf(fp_A10, "%d\n", &R1_0[i]);
    }

    fclose(fp_A01);
    fclose(fp_A10);
    fclose(fp_A11);
 */
////////////////////
    //変数定義
    TFIMG cor(MainPixel, SubPixel);//補正量
    cor.Fill(0.0f);

    TSINGLE_DATA Step(SubPixel);//縦方向の段差配列

////////////////////
    //最も左側のASIC濃度段差を補正
    int iM0 = iASIC + iOFFSET;// 128 - 96 = 32
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        Step.dDt[iSCnt] = Dt[iSCnt][iM0 + iMp] - Dt[iSCnt][iM0 + iMm];
    }

    Step.Median(iMEDIAN);
    Step /= 2;//段差を半分にして段差の左側に加算

    CorStepASIC(0, iM0 - 1, a11, Step, iM0 + iMm, cor,(float)iTh/2.0);//x=0～31までの画素を段差配列と重み付けテーブルとに基づいて補正

    int iM1 = iM0 + iASIC - 1;
	int iS0(0), iS1(0);
//	TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iS1, iM1);
	Step *= -1;//半分にした段差の符号を反対にして段差の右側に加算
    CorStepASIC(iM0, iM1, a10, Step, iM0 + iMp, cor,(float)iTh/2.0);//x=32～32+127までの画素を段差配列と重み付けテーブルとに基づいて補正

////////////////////

    for(iM0 = 2 * iASIC + iOFFSET; (iM0 + iASIC + iMp) < MainPixel; iM0 += iASIC){
        for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            Step.dDt[iSCnt] = Dt[iSCnt][iM0 + iMp] - Dt[iSCnt][iM0 + iMm];
        }

        Step.Median(iMEDIAN);
        Step /= 2;

        CorStepASIC(iM0 - iASIC, iM0 - 1, a01, Step, iM0 + iMm, cor,(float)iTh/2.0);

        int iM1 = iM0 + iASIC - 1;
		iS0 = iS1 = 0;
//	    TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
		TrimArea(iS0, iM0, iS1, iM1);
		Step *= -1;
        CorStepASIC(iM0, iM1, a10, Step, iM0 + iMp, cor,(float)iTh/2.0);
    }

////////////////////////

    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        Step.dDt[iSCnt] = Dt[iSCnt][iM0 + iMp] - Dt[iSCnt][iM0 + iMm];
    }

    Step.Median(iMEDIAN);
    Step /= 2;

    CorStepASIC(iM0 - iASIC, iM0 - 1, a01, Step, iM0 + iMm, cor,(float)iTh/2.0);

    iM1 = iM0 + iASIC - 1;
	iS0 = iS1 = 0;
//	TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iS1, iM1);
	Step *= -1;
    CorStepASIC(iM0, iM1,a11, Step, iM0 + iMp, cor,(float)iTh/2.0);

////////////////////////

    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt]);
		}
    }

}
//---------------------------------------------------------------------------
//完成版ASIC濃度段差補正関数（ttsuji追加）
void TSIMARS::CorStepASIC(const int iM0, const int iM1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iMPos, TFIMG& cor,const float fUPPER)
{
/*
#if 0
    const float fUPPER = 500;//500
#else
    //14bit画像と16bit画像とで場合分け
    float fUPPER = 500;
    if (16 == Bit){
        fUPPER = 2000;
    }
#endif
*/
	vector<float> PrevCor(iM1 - iM0 + 1);
	float fCoe;
	if(Dt[0][iMPos] != 0){
//        fCoe = (Dt[0][iMPos] + step.dDt[0]) / Dt[0][iMPos];//original（誤り！）
		fCoe = (Dt[0][iMPos] - step.dDt[0]) / Dt[0][iMPos];
	}else{
		fCoe = 1.0f;
	}
	for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
		float fCor;
		if(Dt[0][iMCnt] >= Dt[0][iMPos]){
			fCor = step.dDt[0];
		}else{
			fCor = Dt[0][iMCnt] * (1.0f - fCoe);
		}
/*
#if 0
		if(abs(fCor) > fUPPER){
#elif 1
        if(fabs(step.dDt[0]) > fUPPER){ //補正量のリミットとするのではなく、段差量がある閾値以上だとその列はすべて補正しないようにする。
#endif
            fCor = 0.0f;
		}
*/
        if(fabs(step.dDt[0])>fUPPER){
            fCor = 0.0f;
		}

        PrevCor[iCorCnt] = fCor;//補正強度を乗じる前の補正量をPrevCorとする
        fCor *= (float)r[iCorCnt]/65535;
		cor.Dt[iMCnt][0] += fCor;
//        PrevCor[iCorCnt] = fCor;//bug
    }

    for(int iSCnt = 1; iSCnt < SubPixel; ++iSCnt){
        if(Dt[iSCnt][iMPos] != 0){
            fCoe = (Dt[iSCnt][iMPos] - step.dDt[iSCnt]) / Dt[iSCnt][iMPos];
        }else{
            fCoe = 1.0f;
		}
        for(int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; ++iMCnt, ++iCorCnt){
			float fCor;
			if(Dt[iSCnt][iMCnt] >= Dt[iSCnt][iMPos]){
				fCor = step.dDt[iSCnt];
			}else{
                fCor = Dt[iSCnt][iMCnt] * (1.0f - fCoe);
            }
/*
#if 0
            if(abs(fCor) > fUPPER){
#elif 1
            if(fabs(step.dDt[iSCnt]) > fUPPER){ //補正量のリミットとするのではなく、段差量がある閾値以上だとその列はすべて補正しないようにする。
#endif
				fCor = PrevCor[iCorCnt];
			}
*/
			if(fabs(step.dDt[0]) > fUPPER){
				fCor = PrevCor[iCorCnt];
			}

			PrevCor[iCorCnt] = fCor;//補正強度を乗じる前の補正量をPrevCorとする
			fCor *= (float)r[iCorCnt]/65535;
			cor.Dt[iMCnt][iSCnt] += fCor;
//            PrevCor[iCorCnt] = fCor;//bug
		}
	}
}
//---------------------------------------------------------------------------
//SELENE向けASICブロック段差補正（ttsuji追加）
bool TSIMARS::CorStepASIC2_DEBUG(const int iOFFSET, const int iASIC, const int iMm, const int iMp, const int iMEDIAN, const int iMOVEAVE, const float fTh, const int flag_Table, const int flag_DefStep, const int flag_LowPassFilter)
{
	if (MainPixel < iASIC){
		return false;
	}

//////////////
    //補正強度テーブル作成（1→1、0→1、1→0）
	//16ビット型テーブル
	vector<unsigned short> flat(iASIC), R0_1(iASIC), R1_0(iASIC);
	float rate = (float)65535/(iASIC - 1);
	int i;
	for (i = 0; i < iASIC; i++){
		flat[i] = (unsigned short)65535;
	}
	//簡単化(ttsuji)
	for (i = 0; i < iASIC; i++){
		R0_1[i] = (unsigned short)(rate*i + 0.5);
	}
	for (i = 0; i < iASIC; i++){
		R1_0[iASIC - 1 - i] = (unsigned short)(rate*i + 0.5);
	}
#if 0
	FILE *fp1, *fp2;
	fp1 = fopen("R01.txt", "wt");
	fp2 = fopen("R10.txt", "wt");
	for (i = 0; i < iASIC; i++) {
		fprintf(fp1, "%d\n", R0_1[i]);
		fprintf(fp2, "%d\n", R1_0[i]);
	}
	fclose(fp1);
	fclose(fp2);
#endif

	////////////////////
    //変数定義
    TFIMG cor(MainPixel, SubPixel);//補正量
    cor.Fill(0.0f);
	TSINGLE_DATA Step(SubPixel);//縦方向の段差配列
	TSINGLE_DATA dMm(SubPixel), dMp(SubPixel);//縦方向の段差計算画素値の配列
////////////////////
	//先頭のASICブロック段差を補正
	int iM0 = iOFFSET;

	//段差プロファイルを計算
	CalcStepDt_ASIC(flag_DefStep, iM0, iMm, iMp, dMm, dMp, Step);
	LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);//段差プロファイルをスムージング
	Step /= 2;//段差を半分にして段差の左側に加算
//	Step += 0.25;//段差量から補正量を計算する際の四捨五入による誤差を最小化するため

	//x=0～iOFFSET-1までの画素を段差配列と重み付けテーブルとに基づいて補正
//	CorStepASIC_DEBUG(0, iM0 - 1, flat, Step, iM0 + iMm, cor, (float)fTh/2.0);
	CorStepASIC_DEBUG(0, iM0 - 1, flat, Step, dMm, cor, (float)fTh/2.0);

	int iM1 = iM0 + iASIC - 1;
	int iS0(0), iS1(0);
//	TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iS1, iM1);
	Step *= -1;//半分にした段差の符号を反対にして段差の右側に加算
//	Step += 0.5;//段差量から補正量を計算する際の四捨五入による誤差を最小化するため

	//x=iOFFSET～iOFFSET+iASIC-1までの画素を段差配列と重み付けテーブルとに基づいて補正
	switch (flag_Table) {
	case _TABLE_FLAT:
//		CorStepASIC_DEBUG(iM0, iM1, flat, Step, iM0 + iMp, cor, (float)fTh/2.0);
		CorStepASIC_DEBUG(iM0, iM1, flat, Step, dMp, cor, (float)fTh/2.0);
		break;
	case _TABLE_R01:
//		CorStepASIC_DEBUG(iM0, iM1, R1_0, Step, iM0 + iMp, cor, (float)fTh/2.0);
		CorStepASIC_DEBUG(iM0, iM1, R1_0, Step, dMp, cor, (float)fTh/2.0);
		break;
	default:
		return false;
	}

////////////////////
	//画像中央のASICブロック段差を補正
	for (iM0 = iASIC + iOFFSET; (iM0 + iASIC) < MainPixel; iM0 += iASIC){
		CalcStepDt_ASIC(flag_DefStep, iM0, iMm, iMp, dMm, dMp, Step);
		LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);
		Step /= 2;
//		Step += 0.25;//段差量から補正量を計算する際の四捨五入による誤差を最小化するため

		switch (flag_Table) {
		case _TABLE_FLAT:
//			CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, flat, Step, iM0 + iMm, cor, (float)fTh/2.0);
			CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, flat, Step, dMm, cor, (float)fTh/2.0);
			break;
		case _TABLE_R01:
//			CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, R0_1, Step, iM0 + iMm, cor, (float)fTh/2.0);
			CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, R0_1, Step, dMm, cor, (float)fTh/2.0);
			break;
		default:
			return false;
		}

		int iM1 = iM0 + iASIC - 1;
		iS0 = iS1 = 0;
//	    TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
		TrimArea(iS0, iM0, iS1, iM1);
		Step *= -1;
//		Step += 0.5;//段差量から補正量を計算する際の四捨五入による誤差を最小化するため

		switch (flag_Table) {
		case _TABLE_FLAT:
//			CorStepASIC_DEBUG(iM0, iM1, flat, Step, iM0 + iMp, cor, (float)fTh/2.0);
			CorStepASIC_DEBUG(iM0, iM1, flat, Step, dMp, cor, (float)fTh/2.0);
			break;
		case _TABLE_R01:
//			CorStepASIC_DEBUG(iM0, iM1, R1_0, Step, iM0 + iMp, cor, (float)fTh/2.0);
			CorStepASIC_DEBUG(iM0, iM1, R1_0, Step, dMp, cor, (float)fTh/2.0);
			break;
		default:
			return false;
		}
	}

////////////////////////
	//最後のASICブロック段差を補正
	CalcStepDt_ASIC(flag_DefStep, iM0, iMm, iMp, dMm, dMp, Step);
	LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);
	Step /= 2;
//	Step += 0.25;//段差量から補正量を計算する際の四捨五入による誤差を最小化するため

	switch (flag_Table) {
	case _TABLE_FLAT:
//		CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, flat, Step, iM0 + iMm, cor, (float)fTh/2.0);
		CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, flat, Step, dMm, cor, (float)fTh/2.0);
		break;
	case _TABLE_R01:
//		CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, R0_1, Step, iM0 + iMm, cor, (float)fTh/2.0);
		CorStepASIC_DEBUG(iM0 - iASIC, iM0 - 1, R0_1, Step, dMm, cor, (float)fTh/2.0);
		break;
	default:
		return false;
	}

	iM1 = iM0 + iASIC - 1;
	iS0 = iS1 = 0;
//	TrimArea(0, iM0, 0, iM1);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iS1, iM1);
	Step *= -1;
//	Step += 0.5;//段差量から補正量を計算する際の四捨五入による誤差を最小化するため
//	CorStepASIC_DEBUG(iM0, iM1, flat, Step, iM0 + iMp, cor, (float)fTh/2.0);
	CorStepASIC_DEBUG(iM0, iM1, flat, Step, dMp, cor, (float)fTh/2.0);

////////////////////////
	//元画像に補正量を加算
	for (int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
		for (int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
#if 0
			SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt]);//TFIMGクラスは変な設計でcor[M][S]の順！
#elif 0
			SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt] / 65535);//TFIMGクラスは変な設計でcor[M][S]の順！
#else
        	//高速化のため除算をビットシフトに変更
			SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + ((int)cor.Dt[iMCnt][iSCnt] >> 16));
#endif
		}
    }

	return true;
}
//---------------------------------------------------------------------------
//ttsuji
void TSIMARS::CalcStepDt_ASIC(int flag_DefStep, int iM, int iMm, int iMp, TSINGLE_DATA& Step)
{
	int i, iS;
	unsigned short int Neighbor_p[7], Neighbor_m[7];//ASIC段差の近辺画素
	switch (flag_DefStep)
	{
	case _1PIX:
		for (iS = 0; iS < SubPixel; iS++) {
			Step.dDt[iS] = Dt[iS][iM + iMp] - Dt[iS][iM + iMm];//default
		}
		break;
	case _2PIX_AVE:
		for (iS = 0; iS < SubPixel; iS++) {
			Step.dDt[iS] = (Dt[iS][iM] + Dt[iS][iM + 1]) / 2. - (Dt[iS][iM - 1] + Dt[iS][iM - 2]) / 2.;
    	}
		break;
	case _3PIX_AVE:
		for (iS = 0; iS < SubPixel; iS++) {
			Step.dDt[iS] = (Dt[iS][iM] + Dt[iS][iM + 1] + Dt[iS][iM + 2]) / 3. - (Dt[iS][iM - 1] + Dt[iS][iM - 2] + Dt[iS][iM - 3]) / 3.;
		}
		break;
	case _MEDIAN3:
		for (iS = 0; iS < SubPixel; iS++) {
			for (i = 0; i < 3; i++){
				Neighbor_p[i] = Dt[iS][iM + i];
				Neighbor_m[i] = Dt[iS][iM - i - 1];
			}
			qsort(Neighbor_p, 3, sizeof(unsigned short int), Cmp);
			qsort(Neighbor_m, 3, sizeof(unsigned short int), Cmp);
			Step.dDt[iS] = Neighbor_p[1] - Neighbor_m[1];
		}
		break;
	case _MEDIAN5:
		for (iS = 0; iS < SubPixel; iS++) {
			for (i = 0; i < 5; i++){
				Neighbor_p[i] = Dt[iS][iM + i];
				Neighbor_m[i] = Dt[iS][iM - i - 1];
			}
			qsort(Neighbor_p, 5, sizeof(unsigned short int), Cmp);
			qsort(Neighbor_m, 5, sizeof(unsigned short int), Cmp);
			Step.dDt[iS] = Neighbor_p[2] - Neighbor_m[2];
		}
		break;
	case _MEDIAN7:
		for (iS = 0; iS < SubPixel; iS++) {
			for (i = 0; i < 7; i++){
				Neighbor_p[i] = Dt[iS][iM + i];
				Neighbor_m[i] = Dt[iS][iM - i - 1];
			}
			qsort(Neighbor_p, 7, sizeof(unsigned short int), Cmp);
			qsort(Neighbor_m, 7, sizeof(unsigned short int), Cmp);
			Step.dDt[iS] = Neighbor_p[3] - Neighbor_m[3];
		}
		break;
	default:
		for (iS = 0; iS < SubPixel; iS++) {
			Step.dDt[iS] = 0.0;//段差０で補正しない。
		}
	}
}
//---------------------------------------------------------------------------
//ttsuji
void TSIMARS::CalcStepDt_ASIC(int flag_DefStep, int iM, int iMm, int iMp, TSINGLE_DATA& dMm, TSINGLE_DATA& dMp, TSINGLE_DATA& Step)
{
	int i, iS;
	unsigned short int Neighbor_p[7], Neighbor_m[7];//ASIC段差の近辺画素
	switch (flag_DefStep)
	{
	case _1PIX:
		for (iS = 0; iS < SubPixel; iS++) {
			dMp.dDt[iS] = Dt[iS][iM + iMp];
			dMm.dDt[iS] = Dt[iS][iM + iMm];
			Step.dDt[iS] = dMp.dDt[iS] - dMm.dDt[iS];
		}
		break;
	case _2PIX_AVE:
		for (iS = 0; iS < SubPixel; iS++) {
			dMp.dDt[iS] = (Dt[iS][iM] + Dt[iS][iM + 1]) / 2.;
			dMm.dDt[iS] = (Dt[iS][iM - 1] + Dt[iS][iM - 2]) / 2.;
			Step.dDt[iS] = dMp.dDt[iS] - dMm.dDt[iS];
		}
		break;
	case _3PIX_AVE:
		for (iS = 0; iS < SubPixel; iS++) {
			dMp.dDt[iS] = (Dt[iS][iM] + Dt[iS][iM + 1] + Dt[iS][iM + 2]) / 3.;
			dMm.dDt[iS] = (Dt[iS][iM - 1] + Dt[iS][iM - 2] + Dt[iS][iM - 3]) / 3.;
			Step.dDt[iS] = dMp.dDt[iS]- dMm.dDt[iS];
		}
		break;
	case _MEDIAN3:
		for (iS = 0; iS < SubPixel; iS++) {
			for (i = 0; i < 3; i++){
				Neighbor_p[i] = Dt[iS][iM + i];
				Neighbor_m[i] = Dt[iS][iM - i - 1];
			}
			qsort(Neighbor_p, 3, sizeof(unsigned short int), Cmp);
			qsort(Neighbor_m, 3, sizeof(unsigned short int), Cmp);
			dMp.dDt[iS] = Neighbor_p[1];
			dMm.dDt[iS] = Neighbor_m[1];
			Step.dDt[iS] = dMp.dDt[iS] - dMm.dDt[iS];
		}
		break;
	case _MEDIAN5:
		for (iS = 0; iS < SubPixel; iS++) {
			for (i = 0; i < 5; i++){
				Neighbor_p[i] = Dt[iS][iM + i];
				Neighbor_m[i] = Dt[iS][iM - i - 1];
			}
			qsort(Neighbor_p, 5, sizeof(unsigned short int), Cmp);
			qsort(Neighbor_m, 5, sizeof(unsigned short int), Cmp);
			dMp.dDt[iS] = Neighbor_p[2];
			dMm.dDt[iS] = Neighbor_m[2];
			Step.dDt[iS] = dMp.dDt[iS] - dMm.dDt[iS];
		}
		break;
	case _MEDIAN7:
		for (iS = 0; iS < SubPixel; iS++) {
			for (i = 0; i < 7; i++){
				Neighbor_p[i] = Dt[iS][iM + i];
				Neighbor_m[i] = Dt[iS][iM - i - 1];
			}
			qsort(Neighbor_p, 7, sizeof(unsigned short int), Cmp);
			qsort(Neighbor_m, 7, sizeof(unsigned short int), Cmp);
			dMp.dDt[iS] = Neighbor_p[3];
			dMm.dDt[iS] = Neighbor_m[3];
			Step.dDt[iS] = dMp.dDt[iS] - dMm.dDt[iS];
		}
		break;
	default:
		for (iS = 0; iS < SubPixel; iS++) {
			dMp.dDt[iS] = Dt[iS][iM + iMp];
			dMm.dDt[iS] = Dt[iS][iM + iMm];
			Step.dDt[iS] = 0.0;//段差０で補正しない。
		}
	}
}
//---------------------------------------------------------------------------
//ttsuji
void TSIMARS::LowPassFilterForStep(int flag_LowPassFilter, int iMEDIAN, int iMOVEAVE, TSINGLE_DATA& Step)
{
	switch (flag_LowPassFilter) {
	case _MEDIAN_ONLY:
		Step.Median(iMEDIAN);//横方向にメディアンかけることで縦筋を消す！
		break;
	case _MOVEAVE_ONLY:
		Step.MoveAve(iMOVEAVE);
		break;
	case _MED_AND_MOVE:
		Step.Median(iMEDIAN);//まずは欠陥ラインなどの顕著なノイズを小さなメディアンフィルターで除去
		Step.MoveAve(iMOVEAVE);//エッジを保存型のメディアンではなく、あえてエッジをなめる移動平均とする
		break;
	default:
		return;
	}
	return;
}
//---------------------------------------------------------------------------
//SELENE向けASICブロック段差補正（ttsuji追加）
//void TSIMARS::CorStepASIC_DEBUG(const int iM0, const int iM1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iMPos, TFIMG& cor, const float fUPPER)
void TSIMARS::CorStepASIC_DEBUG(const int iM0, const int iM1, vector<unsigned short>& r, TSINGLE_DATA& step, const TSINGLE_DATA& DtASIC, TFIMG& cor, const float fUPPER)
{
	float fCor1, fCor2;
	int iSSearch(0), iSOffset(0);
	bool flag_fUPPER = false;
	vector<float> PrevCor(iM1 - iM0 + 1);
	float fCor, fCoe;
	fCoe = Calc_fCoe((int)(DtASIC.dDt[0] + 0.5), step.dDt[0]);

	//0行目の補正量を計算
#if 0
	for (int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; iMCnt++, iCorCnt++){
		fCor = Calc_fCor(Dt[0][iMCnt], (int)(DtASIC.dDt[0] + 0.5), step.dDt[0], fCoe);

		//段差量がある閾値以上の場合
		if (fabs(step.dDt[0]) > fUPPER){
			fCor = 0.0f;//その列はすべて補正しないようにする
		}
		PrevCor[iCorCnt] = fCor;//主0アドレスの補正値をバッファ（縦筋状アーチファクト発生を防止）
		fCor *= (float)r[iCorCnt]/65535;
		cor.Dt[iMCnt][0] += fCor;
	}
#else
	//0行目の段差量がある閾値以上の場合
	if (fabs(step.dDt[0]) > fUPPER){
		//fUPPERを越える副方向範囲の終端点をサーチ
		iSSearch = 1;
		while ((fabs(step.dDt[iSSearch]) > fUPPER) && (iSSearch < SubPixel - 1))
		{
			iSSearch++;
		}

		//すべての行で段差量がfUPPERを越えている場合はすべての行で補正量ゼロ
		if (iSSearch == SubPixel - 1) {
			return;
		}

		//同終端点の補正量fCoeを計算
		fCoe = Calc_fCoe((int)(DtASIC.dDt[iSSearch] + 0.5), step.dDt[iSSearch]);

		for (int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; iMCnt++, iCorCnt++){
			fCor = Calc_fCor(Dt[iSSearch][iMCnt], (int)(DtASIC.dDt[iSSearch] + 0.5), step.dDt[iSSearch], fCoe);

			//閾値以上となる副方向レンジのすべての列に対して、補正量corを決定
			for (int iSCnt2 = 0; iSCnt2 < iSSearch; iSCnt2++)
			{
				cor.Dt[iMCnt][iSCnt2] += fCor * (float)r[iCorCnt];
			}
			PrevCor[iCorCnt] = fCor;//主0アドレスの補正値をバッファ（縦筋状アーチファクト発生を防止）
		}
		iSOffset = iSSearch;
	}
	else{
		for (int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; iMCnt++, iCorCnt++){
			fCor = Calc_fCor(Dt[0][iMCnt], (int)(DtASIC.dDt[0] + 0.5), step.dDt[0], fCoe);

			PrevCor[iCorCnt] = fCor;//主0アドレスの補正値をバッファ（縦筋状アーチファクト発生を防止）
			fCor *= (float)r[iCorCnt];
			cor.Dt[iMCnt][0] += fCor;
		}

	}
#endif


	//1行目以降の補正量を計算
	for (int iSCnt = 1 + iSOffset; iSCnt < SubPixel; iSCnt++){
		//段差量がある閾値以上だとその列はすべて補正しないようにする
		if (fabs(step.dDt[iSCnt]) > fUPPER && iSCnt < SubPixel - 1){//終端点の場合は含まない
			flag_fUPPER = true;

			//fUPPERを越える副方向範囲の終端点をサーチ
			iSSearch = 1;
			while ((fabs(step.dDt[iSCnt + iSSearch]) > fUPPER) && (iSCnt + iSSearch < SubPixel - 1)){
				iSSearch++;
			}
			//同終端点の補正量fCoeを計算
			fCoe = Calc_fCoe((int)(DtASIC.dDt[iSCnt + iSSearch] + 0.5), step.dDt[iSCnt + iSSearch]);
		}
		else{
			flag_fUPPER = false;
			fCoe = Calc_fCoe((int)(DtASIC.dDt[iSCnt] + 0.5), step.dDt[iSCnt]);
		}

		for (int iMCnt = iM0, iCorCnt = 0; iMCnt <= iM1; iMCnt++, iCorCnt++){
			//段差量が閾値以上の場合
			if (flag_fUPPER){
				//fUPPERを越える副方向範囲の始点における補正量
				fCor1 = PrevCor[iCorCnt];

				//画像の下端までfUPPERを越えつづけた場合
				if (iSCnt + iSSearch == SubPixel - 1) {
					fCor = fCor1;
					//閾値以上となる副方向レンジのすべての列に対して、始点と終点の補正量から当該列の補正量fCorを決定
					for (int iSCnt2 = 0; iSCnt2 <= iSSearch; iSCnt2++){
						cor.Dt[iMCnt][iSCnt + iSCnt2] += fCor * (float)r[iCorCnt];
					}
				}
				else{
					//fUPPERを越える副方向範囲の終点における補正量
					fCor2 = Calc_fCor(Dt[iSCnt + iSSearch][iMCnt], (int)(DtASIC.dDt[iSCnt + iSSearch] + 0.5), step.dDt[iSCnt + iSSearch], fCoe);
					//閾値以上となる副方向レンジのすべての列に対して、始点と終点の補正量から当該列の補正量fCorを決定
					for (int iSCnt2 = 0; iSCnt2 < iSSearch; iSCnt2++)
					{
						fCor = (fCor1 * (iSSearch - iSCnt2) + fCor2 * iSCnt2) / iSSearch;//fCorをfCor1,fCor2から線形に割り付ける
						cor.Dt[iMCnt][iSCnt + iSCnt2] += fCor * (float)r[iCorCnt];
					}
				}

			}//endif flag_fUPPER
			else{
				fCor = Calc_fCor(Dt[iSCnt][iMCnt], (int)(DtASIC.dDt[iSCnt] + 0.5), step.dDt[iSCnt], fCoe);
				PrevCor[iCorCnt] = fCor;
				fCor *= (float)r[iCorCnt];
				cor.Dt[iMCnt][iSCnt] += fCor;
			}//endif flag_fUPPER
		}//end for iMCnt

		if (flag_fUPPER) {
			flag_fUPPER = false;
//			iSCnt += iSSearch - 1;//iSCntのループをiSSearch分だけスキップ
			iSCnt += iSSearch;//iSCntのループをiSSearch分だけスキップ
		}

	}//end for iSCnt
}
//---------------------------------------------------------------------------
//完成版TFT分割段差補正関数（ttsuji追加）（iMidが画像の上下中央でない場合にも対応）
bool TSIMARS::CorStepTFT2(const int iMid, const int iSm, const int iSp, const int iMEDIAN, const float iTh, vector<unsigned short> r01, vector<unsigned short> r10)
{
		//TODO: この下にコードを追加してください
//    const int iMEDIAN = 201;

	if(iMid >= SubPixel - 1 || iMid <= 1 || iSm >= 0 || iSp < 0){
		return false;
	}
/*
//////////////
	//補正強度テーブル作成（0→1、1→0）
	int iNumPixelU = iMid;
	int iNumPixelD = SubPixel - iMid;
	vector<unsigned short> R0_1(iNumPixelU), R1_0(iNumPixelD);
	int iLen = iNumPixelU + iSm;
	if (iLen == 0){
		return false;
	}
	float rateU = (float)65535/iLen;
	int i;
	for(i = 0; i <= iLen; i++){
		R0_1[i] = (unsigned short)(rateU*i + 0.5);
	}
	for( ; i < iNumPixelU; i++){
		R0_1[i] = (unsigned short)65535;
	}

	iLen = iNumPixelD - 1 - iSp;
	if (iLen == 0){
		return false;
	}
	float rateD = (float)65535/iLen;
	for(i = 0; i <= iLen; i++){
		R1_0[iNumPixelD - 1 - i] = (unsigned short)(rateD*i + 0.5);
	}
	for( ; i < iNumPixelD; i++){
		R1_0[iNumPixelD - 1 - i] = (unsigned short)65535;
	}


	//補正強度テーブルファイル出力
	FILE* fp_R01  = fopen("table16_R01.txt", "wt");
	FILE* fp_R10  = fopen("table16_R10.txt", "wt");
	for (i = 0; i < iNumPixelU; i++)
	{
		fprintf(fp_R01, "%d\n", R0_1[i]);
	}
	for (i = 0; i < iNumPixelD; i++)
    {
        fprintf(fp_R10, "%d\n", R1_0[i]);
	}
    fclose(fp_R01);
    fclose(fp_R10);
*/

/*
    FILE* fp_R01  = fopen(str1, "rt");
	FILE* fp_R10  = fopen(str2, "rt");

    int iNumPixelU = iMid;
    int iNumPixelD = SubPixel - iMid;
    vector<unsigned short> R0_1(iNumPixelU), R1_0(iNumPixelD);

    for (i = 0; i < iNumPixelU; i++)
    {
        fscanf(fp_R01, "%d\n", &R0_1[i]);
    }
	for (i = 0; i < iNumPixelD; i++)
	{
        fscanf(fp_R10, "%d\n", &R1_0[i]);
    }
    fclose(fp_R01);
    fclose(fp_R10);

*/

////////////////////
	//変数定義
    TFIMG cor(MainPixel, SubPixel);//補正量(float型)
    cor.Fill(0.0f);

    TSINGLE_DATA Step(MainPixel);//横方向の段差配列(double型)

////////////////////
    //中央分割段差を計算
    for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		Step.dDt[iMCnt] = Dt[iMid + iSp][iMCnt] - Dt[iMid + iSm][iMCnt];
	}

    Step.Median(iMEDIAN);//横方向にメディアンかけることで縦筋を消す！
    Step /= 2;//段差を半分にして段差の左側に加算
	CorStepTFT(0, iMid - 1, r01, Step, iMid + iSm, cor, (float)iTh/2.0);
	int iM0(0), iM1(0), iS0(iMid), iSub1(SubPixel - 1);
//	TrimArea(iMid, 0, SubPixel - 1, 0);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iSub1, iM1);
	Step *= -1;//半分にした段差の符号を反対にして段差の右側に加算
//    CorStepTFT(iMid, SubPixel - 1, R1_0, Step, iMid + iSp, cor, (float)4000/2.0);//x=Mid～2*Mid-1までの画素を段差配列と重み付けテーブルとに基づいて補正
	CorStepTFT(iMid, SubPixel - 1, r10, Step, iMid + iSp, cor,(float)iTh/2.0);

////////////////////////
    //元画像に補正量を加算
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt]);//TFIMGクラスは変な設計でcor[M][S]の順！
        }
	}

	return true;
}


//---------------------------------------------------------------------------
//透視向けTFT段差補正検討用(151007 ttsuji）

bool TSIMARS::CorStepTFTforFluoro(const int iMid,const int iSm,const int iSp,const int iMEDIAN,const float fTh,vector<unsigned short> r01, vector<unsigned short> r10, bool bflagMDivStepProf, bool bflagStepThTraceBack , bool bflagStepThRatio, string in_str)
{
	if (iMid >= SubPixel - 1 || iMid <= 1 || iSm >= 0 || iSp < 0){
		return false;
	}

////////////////////
	//変数定義
	TFIMG cor(MainPixel, SubPixel);//補正量(float型)
	cor.Fill(0.0f);

	TSINGLE_DATA Step(MainPixel);//横方向の段差配列(double型)
	TSINGLE_DATA StepRatio(MainPixel);//横方向の段差配列(double型)

////////////////////
	//中央分割段差を計算
	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		Step.dDt[iMCnt] = Dt[iMid + iSp][iMCnt] - Dt[iMid + iSm][iMCnt];

 		if (bflagStepThRatio) {//段差のQL比
			if (Dt[iMid + iSm][iMCnt]) {
				StepRatio.dDt[iMCnt] = (double)Dt[iMid + iSp][iMCnt] / Dt[iMid + iSm][iMCnt];
			}
			else{
				StepRatio.dDt[iMCnt] = 100.;//とりあえず
			}
		}

	}

	switch (bflagMDivStepProf) {
	case true:
		Step.SaveWithIndex(ExtractFilePath(in_str) + "StepProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
		Step.Bin(2);//2分の1に平均縮小
		Step.SaveWithIndex(ExtractFilePath(in_str) + "BStepProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
		Step.Median(iMEDIAN);//横方向にメディアンかけることで縦筋を消す！
		Step.SaveWithIndex(ExtractFilePath(in_str) + "M" + IntToStr(iMEDIAN) + "BStepProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
		Step.LinExpand(2);//2倍に線形拡張
		Step.SaveWithIndex(ExtractFilePath(in_str) + "EM" + IntToStr(iMEDIAN) + "BStepProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));

 		if (bflagStepThRatio) {
			//段差のQL比
			StepRatio.SaveWithIndex(ExtractFilePath(in_str) + "StepRatioProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
			StepRatio.Bin(2);//2分の1に平均縮小
			StepRatio.SaveWithIndex(ExtractFilePath(in_str) + "BStepRatioProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
			StepRatio.Median(iMEDIAN);//横方向にメディアンかけることで縦筋を消す！
			StepRatio.SaveWithIndex(ExtractFilePath(in_str) + "M" + IntToStr(iMEDIAN) + "BStepRatioProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
			StepRatio.LinExpand(2);//2倍に線形拡張
			StepRatio.SaveWithIndex(ExtractFilePath(in_str) + "EM" + IntToStr(iMEDIAN) + "BStepRatioProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
		}
		break;
	case false:
		Step.SaveWithIndex(ExtractFilePath(in_str) + "StepProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
		Step.Median(iMEDIAN);//横方向にメディアンかけることで縦筋を消す！
		Step.SaveWithIndex(ExtractFilePath(in_str) + "M" + IntToStr(iMEDIAN) + "StepProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));

		if (bflagStepThRatio) {
			//段差のQL比
			StepRatio.SaveWithIndex(ExtractFilePath(in_str) + "StepRatioProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
			StepRatio.Median(iMEDIAN);//横方向にメディアンかけることで縦筋を消す！
			StepRatio.SaveWithIndex(ExtractFilePath(in_str) + "M" + IntToStr(iMEDIAN) + "StepRatioProf_" + ExtractFileName(ChangeFileExt(in_str, ".txt")));
    	}
		break;
	}
	Step /= 2;//段差を半分にして段差の左側に加算

	if (bflagStepThTraceBack) {
		if (bflagStepThRatio) {
			CorStepTFT_ModifyThAlgo3(0, iMid - 1, r01, Step, StepRatio, iMid + iSm, cor, fTh);//段差QL比の閾値判定
		}
		else{
			CorStepTFT_ModifyThAlgo2(0, iMid - 1, r01, Step, iMid + iSm, cor, (float)fTh/2.0);//段差量の閾値判定
		}
	}
	else{
		CorStepTFT(0, iMid - 1, r01, Step, iMid + iSm, cor, (float)fTh/2.0);
	}

	int iM0(0), iM1(0), iS0(iMid), iSub1(SubPixel - 1);
//	TrimArea(iMid, 0, SubPixel - 1, 0);//参照渡しなので０はまずい（BCB2007コンパイラ）
	TrimArea(iS0, iM0, iSub1, iM1);
	Step *= -1;//半分にした段差の符号を反対にして段差の右側に加算
	if (bflagStepThTraceBack) {
		if (bflagStepThRatio) {
			CorStepTFT_ModifyThAlgo3(iMid, SubPixel - 1, r10, Step, StepRatio, iMid + iSp, cor, fTh);//段差QL比の閾値判定
		}
		else{
			CorStepTFT_ModifyThAlgo2(iMid, SubPixel - 1, r10, Step, iMid + iSp, cor,(float)fTh/2.0);//段差量の閾値判定
        }
	}
	else{
		CorStepTFT(iMid, SubPixel - 1, r10, Step, iMid + iSp, cor,(float)fTh/2.0);
	}


////////////////////////
	//元画像に補正量を加算
	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt]);//TFIMGクラスは変な設計でcor[M][S]の順！
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//完成版TFT分割段差補正関数（ttsuji追加）
void TSIMARS::CorStepTFT(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iSPos, TFIMG& cor,const float fUPPER)
{
	vector<float> PrevCor(iS1 - iS0 + 1);
	float fCoe;
	if(Dt[iSPos][0] != 0){
		fCoe = (Dt[iSPos][0] - step.dDt[0]) / Dt[iSPos][0];
	}else{
		fCoe = 1.0f;
	}
	for(int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
		float fCor;
		if(Dt[iSCnt][0] >= Dt[iSPos][0]){
			fCor = step.dDt[0];
		}else{
			fCor = Dt[iSCnt][0] * (1.0f - fCoe);//original
//            fCor = Dt[iSCnt][0] * (step.dDt[0] / Dt[iSPos][0]);//別表記（こっちの方が表記的にはわかりやすい）
        }

		//段差量がある閾値以上だとその列はすべて補正しないようにする
		if(fabs(step.dDt[0]) > fUPPER){
            fCor = 0.0f;
        }
        PrevCor[iCorCnt] = fCor;//主0アドレスの補正値をバッファ（縦筋状アーチファクト発生を防止）
		fCor *= (float)r[iCorCnt]/65535;
        cor.Dt[0][iSCnt] += fCor;
	}

	for(int iMCnt = 1; iMCnt < MainPixel; iMCnt++){
		if(Dt[iSPos][iMCnt] != 0){
			fCoe = (Dt[iSPos][iMCnt] - step.dDt[iMCnt]) / Dt[iSPos][iMCnt];
		}else{
			fCoe = 1.0f;
		}
		for(int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
			float fCor;
            if(Dt[iSCnt][iMCnt] >= Dt[iSPos][iMCnt]){
                fCor = step.dDt[iMCnt];
            }else{
                fCor = Dt[iSCnt][iMCnt] * (1.0f - fCoe);//original
//                fCor = Dt[iSCnt][iMCnt] * (step.dDt[iMCnt] / Dt[iSPos][iMCnt]);//別表記（こっちの方が表記的にはわかりやすい）
            }

			//段差量がある閾値以上だとその列はすべて補正しないようにする
			if(fabs(step.dDt[iMCnt]) > fUPPER){
				fCor = PrevCor[iCorCnt];
            }

            PrevCor[iCorCnt] = fCor;
            fCor *= (float)r[iCorCnt]/65535;
			cor.Dt[iMCnt][iSCnt] += fCor;
		}
    }
}
//---------------------------------------------------------------------------
//ttsuji
double TSIMARS::CalcStepDt_TFT(int flag_DefStep, int iM, int iS, int iSm, int iSp)
{
	int i;
	double step;
	unsigned short int Neighbor_p[7], Neighbor_m[7];//ASIC段差の近辺画素
	switch (flag_DefStep)
	{
	case _1PIX:
		step = Dt[iS + iSp][iM] - Dt[iS + iSm][iM];//default
		break;
	case _2PIX_AVE:
		step = (Dt[iS][iM] + Dt[iS + 1][iM]) / 2. - (Dt[iS - 1][iM] + Dt[iS - 2][iM]) / 2.;
		break;
	case _3PIX_AVE:
		step = (Dt[iS][iM] + Dt[iS + 1][iM] + Dt[iS + 2][iM]) / 3. - (Dt[iS - 1][iM] + Dt[iS - 2][iM] + Dt[iS - 3][iM]) / 3.;
		break;
	case _MEDIAN3:
		for (i = 0; i < 3; i++){
			Neighbor_p[i] = Dt[iS + i][iM];
			Neighbor_m[i] = Dt[iS - i - 1][iM];
		}
		qsort(Neighbor_p, 3, sizeof(unsigned short int), Cmp);
		qsort(Neighbor_m, 3, sizeof(unsigned short int), Cmp);
		step = Neighbor_p[1] - Neighbor_m[1];
		break;
	case _MEDIAN5:
		for (i = 0; i < 5; i++){
			Neighbor_p[i] = Dt[iS + i][iM];
			Neighbor_m[i] = Dt[iS - i - 1][iM];
		}
		qsort(Neighbor_p, 5, sizeof(unsigned short int), Cmp);
		qsort(Neighbor_m, 5, sizeof(unsigned short int), Cmp);
		step = Neighbor_p[2] - Neighbor_m[2];
		break;
	case _MEDIAN7:
		for (i = 0; i < 7; i++){
			Neighbor_p[i] = Dt[iS + i][iM];
			Neighbor_m[i] = Dt[iS - i - 1][iM];
		}
		qsort(Neighbor_p, 7, sizeof(unsigned short int), Cmp);
		qsort(Neighbor_m, 7, sizeof(unsigned short int), Cmp);
		step = Neighbor_p[3] - Neighbor_m[3];
		break;
	default:
		step = 0.0;//段差０で補正しない。
	}
	return step;
}
//---------------------------------------------------------------------------
//TFT分割段差補正関数（ttsuji追加）
//段差閾値fUPPERを越えた場合の処理を修正（閾値を越える主方向レンジの両端の段差量から補正量を計算するように修正）
void TSIMARS::CorStepTFT_ModifyThAlgo(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iSPos, TFIMG& cor,const float fUPPER)
{
	vector<float> PrevCor(iS1 - iS0 + 1);
	float fCor, fCoe;
	fCoe = Calc_fCoe(Dt[iSPos][0], step.dDt[0]);

	for (int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
		fCor = Calc_fCor(Dt[iSCnt][0], Dt[iSPos][0], step.dDt[0], fCoe);

		//段差量がある閾値以上だとその列はすべて補正しないようにする
		if (fabs(step.dDt[0]) > fUPPER){
			fCor = 0.0f;
		}
		PrevCor[iCorCnt] = fCor;//主0アドレスの補正値をバッファ（縦筋状アーチファクト発生を防止）
		fCor *= (float)r[iCorCnt]/65535;
		cor.Dt[0][iSCnt] += fCor;
	}

	float fCor1, fCor2;
	int iMSearch;
	bool flag_fUPPER = false;

	for (int iMCnt = 1; iMCnt < MainPixel; iMCnt++)
	{
		//段差量がある閾値以上だとその列はすべて補正しないようにする
		if (fabs(step.dDt[iMCnt]) > fUPPER){
			flag_fUPPER = true;

			//fUPPERを越える主方向範囲の終端点をサーチ
			iMSearch = 1;
			while (fabs(step.dDt[iMCnt + iMSearch]) > fUPPER)
			{
				iMSearch++;
			}
			//同終端点の補正量fCoeを計算
			fCoe = Calc_fCoe(Dt[iSPos][iMCnt + iMSearch], step.dDt[iMCnt + iMSearch]);
		}
		else{
			flag_fUPPER = false;
			fCoe = Calc_fCoe(Dt[iSPos][iMCnt], step.dDt[iMCnt]);
		}

		for (int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++)
		{
			//段差量が閾値以上の場合
			if (flag_fUPPER)
			{
				//fUPPERを越える主方向範囲の始点における補正量
				fCor1 = PrevCor[iCorCnt];

				//fUPPERを越える主方向範囲の終点における補正量
				fCor2 = Calc_fCor(Dt[iSCnt][iMCnt + iMSearch], Dt[iSPos][iMCnt + iMSearch], step.dDt[iMCnt + iMSearch], fCoe);

				//閾値以上となる主方向レンジのすべての列に対して、始点と終点の補正量から当該列の補正量fCorを決定
				for (int iMCnt2 = 0; iMCnt2 < iMSearch; iMCnt2++)
				{
					fCor = (fCor1 * (iMSearch - iMCnt2) + fCor2 * iMCnt2) / iMSearch;//fCorをfCor1,fCor2から線形に割り付ける
					cor.Dt[iMCnt + iMCnt2][iSCnt] += fCor * (float)r[iCorCnt]/65535;
				}
			}//endif flag_fUPPER
			else
			{
				fCor = Calc_fCor(Dt[iSCnt][iMCnt], Dt[iSPos][iMCnt], step.dDt[iMCnt], fCoe);
				PrevCor[iCorCnt] = fCor;
				fCor *= (float)r[iCorCnt]/65535;
				cor.Dt[iMCnt][iSCnt] += fCor;
			}//endif flag_fUPPER
		}//end for iSCnt

		if (flag_fUPPER) {
			flag_fUPPER = false;
			iMCnt += iMSearch - 1;//iMCntのループをiMSearch分だけスキップ
		}

	}//end for iMCnt
}
//---------------------------------------------------------------------------
//透視用の段差補正（段差閾値を越えた場合のアルゴ改良）(160224 ttsuji)
//段差閾値fUPPERを越えた場合の処理を修正（閾値を越えたら閾値を越えない範囲を遡り、収束するところを探索してそこの段差量から補正量を計算するように修正）
void TSIMARS::CorStepTFT_ModifyThAlgo2(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iSPos, TFIMG& cor,const float fUPPER)
{
//	vector<float> PrevCor(iS1 - iS0 + 1);
	float fCor, fCoe;
	fCoe = Calc_fCoe(Dt[iSPos][0], step.dDt[0]);

	for (int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
		fCor = Calc_fCor(Dt[iSCnt][0], Dt[iSPos][0], step.dDt[0], fCoe);

		//段差量がある閾値以上だとその列はすべて補正しないようにする
		if (fabs(step.dDt[0]) > fUPPER){

			fCor = 0.0f;
		}
//		PrevCor[iCorCnt] = fCor;//主0アドレスの補正値をバッファ（縦筋状アーチファクト発生を防止）
		fCor *= (float)r[iCorCnt]/65535;
		cor.Dt[0][iSCnt] += fCor;
	}

	float fCor1, fCor2;
	float fCoe1, fCoe2;
	int iMSearch, iMStart, iMEnd;
	bool flag_fUPPER = false;
	const int iExtendLimit = 10;//とりあえず
	int iExtendCnt;//遡行画素数

	for (int iMCnt = 1; iMCnt < MainPixel; iMCnt++){
		if (iMCnt == 1491) {
			int c = 0;
		}

		iMEnd = 0;
		//段差量が正で閾値より大きい場合
		if (step.dDt[iMCnt] > fUPPER) {
			flag_fUPPER = true;
			iMStart = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMStart > iMEnd && step.dDt[iMStart - 1] < step.dDt[iMStart] && iExtendCnt < iExtendLimit){
				iMStart--;
				iExtendCnt++;
			}
			while (iMCnt < MainPixel - 2 && step.dDt[iMCnt + 1] > fUPPER){
				iMCnt++;
			}
			iMEnd = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMEnd < MainPixel - 2 && step.dDt[iMEnd + 1] < step.dDt[iMEnd] && iExtendCnt < iExtendLimit){
				iMCnt++;
				iMEnd++;
				iExtendCnt++;
			}

			//端点の補正量fCoe1,2を計算
			fCoe1 = Calc_fCoe(Dt[iSPos][iMStart], step.dDt[iMStart]);
			fCoe2 = Calc_fCoe(Dt[iSPos][iMEnd], step.dDt[iMEnd]);
		}
		//段差量が負で-閾値より小さい場合
		else if (step.dDt[iMCnt] < - fUPPER) {
			flag_fUPPER = true;
			iMStart = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMStart > iMEnd && step.dDt[iMStart - 1] > step.dDt[iMStart] && iExtendCnt < iExtendLimit){
				iMStart--;
				iExtendCnt++;
			}
			while (iMCnt < MainPixel - 2 && step.dDt[iMCnt + 1] < - fUPPER){
				iMCnt++;
			}
			iMEnd = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMEnd < MainPixel - 2 && step.dDt[iMEnd + 1] > step.dDt[iMEnd] && iExtendCnt < iExtendLimit){
				iMCnt++;
				iMEnd++;
				iExtendCnt++;
			}

			//端点の補正量fCoe1,2を計算
			fCoe1 = Calc_fCoe(Dt[iSPos][iMStart], step.dDt[iMStart]);
			fCoe2 = Calc_fCoe(Dt[iSPos][iMEnd], step.dDt[iMEnd]);
		}
		else{
			flag_fUPPER = false;
			fCoe = Calc_fCoe(Dt[iSPos][iMCnt], step.dDt[iMCnt]);
        }


		//該当列の補正量を行ごとに計算
		for (int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
			//段差量が閾値以上の場合
			if (flag_fUPPER){
				//fUPPERを越える主方向範囲の始点における補正量
				fCor1 = Calc_fCor(Dt[iSCnt][iMStart], Dt[iSPos][iMStart], step.dDt[iMStart], fCoe1);
//				fCor1 = PrevCor[iCorCnt];

				//fUPPERを越える主方向範囲の終点における補正量
				fCor2 = Calc_fCor(Dt[iSCnt][iMEnd], Dt[iSPos][iMEnd], step.dDt[iMEnd], fCoe2);

				//閾値以上となる主方向レンジのすべての列に対して、始点と終点の補正量から当該列の補正量fCorを決定
				for (int iMCnt2 = iMStart; iMCnt2 <= iMEnd; iMCnt2++){
					fCor = (fCor1 * (iMEnd - iMCnt2) + fCor2 * (iMCnt2 - iMStart)) / (iMEnd - iMStart);//fCorをfCor1,fCor2から線形に割り付ける
//					cor.Dt[iMCnt2][iSCnt] += fCor * (float)r[iCorCnt]/65535;
					cor.Dt[iMCnt2][iSCnt] = fCor * (float)r[iCorCnt]/65535;
				}
			}//endif flag_fUPPER
			else{
				fCor = Calc_fCor(Dt[iSCnt][iMCnt], Dt[iSPos][iMCnt], step.dDt[iMCnt], fCoe);
//				PrevCor[iCorCnt] = fCor;
				fCor *= (float)r[iCorCnt]/65535;
//				cor.Dt[iMCnt][iSCnt] += fCor;
				cor.Dt[iMCnt][iSCnt] = fCor;
			}//endif flag_fUPPER
		}//end for iSCnt

		int a = 0;

/*
		if (flag_fUPPER) {
			flag_fUPPER = false;
			iMCnt += iMSearch - 1;//iMCntのループをiMSearch分だけスキップ
		}
*/
	}//end for iMCnt
}
//---------------------------------------------------------------------------
//透視用の段差補正（段差閾値を越えた場合のアルゴ改良（段差閾値は段差のQL比で規定））(160225 ttsuji)
//段差閾値fUPPERを越えた場合の処理を修正（閾値を越えたら閾値を越えない範囲を遡り、収束するところを探索してそこの段差量から補正量を計算するように修正）
void TSIMARS::CorStepTFT_ModifyThAlgo3(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, TSINGLE_DATA& StepRatio, const int iSPos, TFIMG& cor,const float fThRatio)
{
	if (fThRatio < 1.) {
		return;
	}

	float fCor, fCoe;
	fCoe = Calc_fCoe(Dt[iSPos][0], step.dDt[0]);

	for (int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
		fCor = Calc_fCor(Dt[iSCnt][0], Dt[iSPos][0], step.dDt[0], fCoe);

		//段差量がある閾値以上だとその列はすべて補正しないようにする
		if (StepRatio.dDt[0] > fThRatio || StepRatio.dDt[0] < 1. / fThRatio){
			fCor = 0.0f;
		}
		fCor *= (float)r[iCorCnt]/65535;
		cor.Dt[0][iSCnt] += fCor;
	}

	float fCor1, fCor2;
	float fCoe1, fCoe2;
	int iMSearch, iMStart, iMEnd;
	bool flag_fUPPER = false;
//	const int iExtendLimit = 10;//とりあえず
	const int iExtendLimit = 100;//QCファントムメッシュパターンはこの程度必要か
	int iExtendCnt;//遡行画素数

	for (int iMCnt = 1; iMCnt < MainPixel; iMCnt++){
		if (iMCnt == 1491) {
			int c = 0;
		}

		iMEnd = 0;
		//段差量が正で閾値より大きい場合
//		if (step.dDt[iMCnt] > fThRatio) {
		if (StepRatio.dDt[iMCnt] > fThRatio){

			flag_fUPPER = true;
			iMStart = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMStart > iMEnd && StepRatio.dDt[iMStart - 1] < StepRatio.dDt[iMStart] && iExtendCnt < iExtendLimit){
				iMStart--;
				iExtendCnt++;
			}
			while (iMCnt < MainPixel - 2 && StepRatio.dDt[iMCnt + 1] > fThRatio){
				iMCnt++;
			}
			iMEnd = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMEnd < MainPixel - 2 && StepRatio.dDt[iMEnd + 1] < StepRatio.dDt[iMEnd] && iExtendCnt < iExtendLimit){
				iMCnt++;
				iMEnd++;
				iExtendCnt++;
			}

			//端点の補正量fCoe1,2を計算
			fCoe1 = Calc_fCoe(Dt[iSPos][iMStart], step.dDt[iMStart]);
			fCoe2 = Calc_fCoe(Dt[iSPos][iMEnd], step.dDt[iMEnd]);
		}
		//段差量が負で-閾値より小さい場合
//		else if (step.dDt[iMCnt] < - fThRatio) {
		else if (StepRatio.dDt[iMCnt] < 1.0 / fThRatio){

			flag_fUPPER = true;
			iMStart = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMStart > iMEnd && StepRatio.dDt[iMStart - 1] > StepRatio.dDt[iMStart] && iExtendCnt < iExtendLimit){
				iMStart--;
				iExtendCnt++;
			}
			while (iMCnt < MainPixel - 2 && StepRatio.dDt[iMCnt + 1] < 1.0 / fThRatio){
				iMCnt++;
			}
			iMEnd = iMCnt;
			iExtendCnt = 0;//遡行画素数
			while (iMEnd < MainPixel - 2 && StepRatio.dDt[iMEnd + 1] > StepRatio.dDt[iMEnd] && iExtendCnt < iExtendLimit){
				iMCnt++;
				iMEnd++;
				iExtendCnt++;
			}

			//端点の補正量fCoe1,2を計算
			fCoe1 = Calc_fCoe(Dt[iSPos][iMStart], step.dDt[iMStart]);
			fCoe2 = Calc_fCoe(Dt[iSPos][iMEnd], step.dDt[iMEnd]);
		}
		else{
			flag_fUPPER = false;
			fCoe = Calc_fCoe(Dt[iSPos][iMCnt], step.dDt[iMCnt]);
		}


		//該当列の補正量を行ごとに計算
		for (int iSCnt = iS0, iCorCnt = 0; iSCnt <= iS1; iSCnt++, iCorCnt++){
			//段差量が閾値以上の場合
			if (flag_fUPPER){
				//fUPPERを越える主方向範囲の始点における補正量
				fCor1 = Calc_fCor(Dt[iSCnt][iMStart], Dt[iSPos][iMStart], step.dDt[iMStart], fCoe1);

				//fUPPERを越える主方向範囲の終点における補正量
				fCor2 = Calc_fCor(Dt[iSCnt][iMEnd], Dt[iSPos][iMEnd], step.dDt[iMEnd], fCoe2);

				//閾値以上となる主方向レンジのすべての列に対して、始点と終点の補正量から当該列の補正量fCorを決定
				for (int iMCnt2 = iMStart; iMCnt2 <= iMEnd; iMCnt2++){
					fCor = (fCor1 * (iMEnd - iMCnt2) + fCor2 * (iMCnt2 - iMStart)) / (iMEnd - iMStart);//fCorをfCor1,fCor2から線形に割り付ける
					cor.Dt[iMCnt2][iSCnt] = fCor * (float)r[iCorCnt]/65535;
				}
			}//endif flag_fUPPER
			else{
				fCor = Calc_fCor(Dt[iSCnt][iMCnt], Dt[iSPos][iMCnt], step.dDt[iMCnt], fCoe);
				fCor *= (float)r[iCorCnt]/65535;
				cor.Dt[iMCnt][iSCnt] = fCor;
			}//endif flag_fUPPER
		}//end for iSCnt

		int a = 0;

	}//end for iMCnt
}
//---------------------------------------------------------------------------
//デバッグ用TFT分割段差補正関数（ttsuji追加）
bool TSIMARS::CorStepTFT2_DEBUG(const int iMid, const int iSm, const int iSp, const int iMEDIAN, const int iMOVEAVE, const float fTh, const int flag_Table, const int flag_DefStep, const int flag_LowPassFilter)
{
	//TODO: この下にコードを追加してください
	if(SubPixel < iMid + iSp){
		return false;
	}

//////////////
    //補正強度テーブル作成（1→1、0→1、1→0）
	//16ビット型テーブル
	vector<unsigned short> flat(iMid), R0_1(iMid), R1_0(iMid);
    float rate = (float)65535/(iMid - 1);
	int i;
	for(i = 0; i < iMid; i++){
        flat[i] = (unsigned short)65535;
    }

    int iNum = iMid + iSm;
    for(i = 0; i < iNum; i++){
		R0_1[i] = (unsigned short)(rate*i + 0.5);
	}
    for( ; i < iMid; i++){
        R0_1[i] = (unsigned short)65535;
    }

	iNum = iMid - 1 - iSp;
    for(i = 0; i < iNum; i++){
        R1_0[iMid - 1 - i] = (unsigned short)(rate*i + 0.5);
    }
    for( ; i < iMid; i++){
        R1_0[iMid - 1 - i] = (unsigned short)65535;
    }
/*
    //tmp
    FILE* fp_flat = fopen("table16_flat.txt", "wt");
	FILE* fp_R01  = fopen("table16_R01.txt", "wt");
    FILE* fp_R10  = fopen("table16_R10.txt", "wt");
    for (i = 0; i < iMid; i++)
    {
        fprintf(fp_flat, "%d\n", flat[i]);
        fprintf(fp_R01, "%d\n", R0_1[i]);
        fprintf(fp_R10, "%d\n", R1_0[i]);
    }
    fclose(fp_flat);
    fclose(fp_R01);
    fclose(fp_R10);
*/

////////////////////
    //変数定義
    TFIMG cor(MainPixel, SubPixel);//補正量(float型)
    cor.Fill(0.0f);

    TSINGLE_DATA Step(MainPixel);//横方向の段差配列(double型)

////////////////////
    //中央分割段差を計算
	unsigned short int Neighbor_p[5], Neighbor_m[5];//TFT段差の近辺画素
	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		Step.dDt[iMCnt] = CalcStepDt_TFT(flag_DefStep, iMCnt, iMid, iSm, iSp);
	}

	switch (flag_DefStep)
	{
	case _1PIX:
// #ifdef _DEBUG
 #if 0
		//メディアンフィルター前の段差をファイル出力
		if (iSm == -1 && iSp == 0){
			Step.SaveWithIndex("step-1_0.lin");
		}
		else if (iSm == -2 && iSp == 1){
			Step.SaveWithIndex("step-2_1.lin");
		}
		else if (iSm == -3 && iSp == 2){
			Step.SaveWithIndex("step-3_2.lin");
		}
		else if (iSm == -4 && iSp == 3){
			Step.SaveWithIndex("step-4_3.lin");
        }
 #endif
		LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);

// #ifdef _DEBUG
 #if 0
		//メディアンフィルター後の段差をファイル出力
		if (iSm == -1 && iSp == 0){
            Step.SaveWithIndex("median_step-1_0.lin");
        }
		else if (iSm == -2 && iSp == 1){
            Step.SaveWithIndex("median_step-2_1.lin");
        }
		else if (iSm == -3 && iSp == 2){
            Step.SaveWithIndex("median_step-3_2.lin");
        }
		else if (iSm == -4 && iSp == 3){
            Step.SaveWithIndex("median_step-4_3.lin");
        }
 #endif
        break;
	case _2PIX_AVE:
 #ifdef _DEBUG
        //メディアンフィルター前の段差をファイル出力
        Step.SaveWithIndex("step-2-1_01.lin");
 #endif
		LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);

 #ifdef _DEBUG
        //メディアンフィルター後の段差をファイル出力
        Step.SaveWithIndex("median_step-2-1_01.lin");
 #endif
		break;
	case _3PIX_AVE:
 #ifdef _DEBUG
        //メディアンフィルター前の段差をファイル出力
        Step.SaveWithIndex("step-3-2-1_012.lin");
 #endif
		LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);

 #ifdef _DEBUG
        //メディアンフィルター後の段差をファイル出力
        Step.SaveWithIndex("median_step-3-2-1_012.lin");
 #endif
		break;
	default:
		LowPassFilterForStep(flag_LowPassFilter, iMEDIAN, iMOVEAVE, Step);
		break;
	}

	Step /= 2;//段差を半分にして段差の左側に加算

	switch (flag_Table) {
	case _TABLE_FLAT:
		CorStepTFT_ModifyThAlgo(0, iMid - 1, flat, Step, iMid + iSm, cor, (float)fTh/2.0);//x=0～iMid-1までの画素を段差配列と補正強度テーブルとに基づいて補正
		break;
	case _TABLE_R01:
		CorStepTFT_ModifyThAlgo(0, iMid - 1, R0_1, Step, iMid + iSm, cor, (float)fTh/2.0);//x=0～iMid-1までの画素を段差配列と補正強度テーブルとに基づいて補正
		break;
	default:
		return false;
	}

	int S0(iMid), M0(0), S1(2*iMid - 1), M1(0);
	TrimArea(S0, M0, S1, M1);
	Step *= -1;//半分にした段差の符号を反対にして段差の右側に加算

	switch (flag_Table) {
	case _TABLE_FLAT:
		CorStepTFT_ModifyThAlgo(S0, S1, flat, Step, S0 + iSp, cor, (float)fTh/2.0);//x=Mid～2*Mid-1までの画素を段差配列と補正強度テーブルとに基づいて補正
		break;
	case _TABLE_R01:
		CorStepTFT_ModifyThAlgo(S0, S1, R1_0, Step, S0 + iSp, cor, (float)fTh/2.0);//x=Mid～2*Mid-1までの画素を段差配列と重み付けテーブルとに基づいて補正
		break;
	default:
		return false;
	}

////////////////////////
	//元画像に補正量を加算
	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + cor.Dt[iMCnt][iSCnt]);//TFIMGクラスは変な設計でcor[M][S]の順！
		}
	}
	return true;
}
//---------------------------------------------------------------------------
float TSIMARS::Calc_fCoe(unsigned short int usDt, double dDt)
{
	float fCoe;

	if (usDt){
		fCoe = (float)(usDt - dDt) / usDt;
	}
	else{
		fCoe = 1.0f;
	}
	return fCoe;
}
//---------------------------------------------------------------------------
float TSIMARS::Calc_fCoe2(double dDtBorder, double dDt)
{
	int iDtBorder = (dDtBorder >= 0.) ? (int)(dDtBorder + 0.5) : (int)(dDtBorder - 0.5);//int型へのキャストは絶対値の小さい方向の最も近い整数になる
	float fCoe = Calc_fCoe(iDtBorder, dDt);
	return fCoe;
}
//---------------------------------------------------------------------------
float TSIMARS::Calc_fCor(unsigned short int usDtCnt, unsigned short int usDtPos, double dStep, float fCoe)
{
	float fCor;
	if(usDtCnt >= usDtPos){
		fCor = dStep;
	}else{
		fCor = usDtCnt * (1.0f - fCoe);//original
//       fCor = usDtCnt * (dStep / usDtPos);//別表記（こっちの方が表記的にはわかりやすい）
	}
	return fCor;
}
//---------------------------------------------------------------------------
float TSIMARS::Calc_fCor2(unsigned short int usDtCnt, double dDtPos, double dStep, float fCoe)
{
	int iDtPos = (dDtPos >= 0.) ? (int)(dDtPos + 0.5) : (int)(dDtPos - 0.5);
	float fCor = Calc_fCor(usDtCnt, iDtPos, dStep, fCoe);
	return fCor;
}
//---------------------------------------------------------------------------
//#define _LOG_KASA //LOG


//FPD1向けムラ除去コンポ（16bitLOG→16bitLinear出力）フローに合わせた縦傘処理関数
bool TSIMARS::bSuperRedUMB2_FPD1(bool bKasa, const String KasaMainFilter, const String KasaSubFilter,
			const String KasaClipFile, const int LogMidVal, const int LogKetaVal, const bool bDebugKasa, const String strImage)
{
	const int upConvBit = 4;//LOG画像が12ビットから16ビットに変わることによるビットシフト数
    const int upConv = 1 << upConvBit;//変倍率


	const int iSUB_RED = 8;

	TFIMG   fimg;
    TFIMG   difImg;
    TFIMG   linOrgFimg;     //Linear Original Image
	TFIMG   SubRedFimg;     //Sub Direction Reduction Image
	int iMainCnt, iSubCnt;
	float   fresult;

//QL依存の振幅clip
    TFIMG   difAveImg;
//    float clip[4096];
	float* clip = new float[4096 * upConv];
    fstream file;
    int i, j;

    if(Form1->bReadTbl(KasaClipFile, clip, DtMax + 1) == false){
		CApplication::GetInstance()->MessageBox("clip table作成できませんでした", "Error", MB_OK);
        delete[] clip;
        return(false);
    }


//前段の縦傘用テーブル（anti log変換と線形変換の組み合わせ）
        //table作成
//    float   fTable[4096];
    float* fTable = new float[4096 * upConv];
//    float   fTable[65536];

    for(i = 0; i < 2149 * upConv ; i++){
        fTable[i] = 100.0 * pow(10.0, i / (1024.0 * upConv));
    }
    for(i = 2149 * upConv; i <= DtMax ; i++){
		fTable[i] = 28896.7060898455 * (2.0 + i / (1024.0 * upConv)) - 105887.307753009;
    }

#if 0
	if (bDebugKasa){
		FILE* fp_VerUmb1 = fopen("VerticalUmbrellaTable1.txt", "wt");
		for(i = 0; i <= DtMax; i+=10)
		{
			fprintf(fp_VerUmb1, "%d %f\n", i, fTable[i]);
		}
		fclose(fp_VerUmb1);
	}
#endif

    TTMA clipImg = *this;
    clipImg.DivWithRest(iSUB_RED, 1);

	linOrgFimg.NewDt(MainPixel, SubPixel);
	SubRedFimg.NewDt(MainPixel, SubPixel);
    for(i = 0 ; i < SubPixel ; i++){
    	for(j = 0 ; j < MainPixel ; j++){
			linOrgFimg.Dt[j][i] = fTable[Dt[i][j]];
			SubRedFimg.Dt[j][i] = linOrgFimg.Dt[j][i];
		}
    }
    delete[] fTable;//ttsuji追加


//縦傘処理OFFか否か
if (bKasa)
{

    SubRedFimg.Div(1, iSUB_RED);


//主差分画像作成
    TFIMG lowFimg;

    TSINGLE_DATA Dif, ClipDif;
    int iSrt, iEnd;
    int iClipCnt;

	for(int iCnt = 0; iCnt < 4096 * upConv; iCnt++){
        clip[iCnt] /= 3.0 ;  //clipは3画素加算を前提で作成されているため
    }

    Dif.NewDt(MainPixel - 1);
    ClipDif.NewDt(MainPixel - 1);
    fimg.NewDt(SubRedFimg.MainPixel, SubRedFimg.SubPixel);
    difImg.NewDt(SubRedFimg.MainPixel, SubRedFimg.SubPixel);
    for(iSubCnt = 0; iSubCnt < difImg.SubPixel; iSubCnt++){
            //差分
        for(iMainCnt = 0; iMainCnt < Dif.iDtNum; iMainCnt++){
            Dif.dDt[iMainCnt] = SubRedFimg.Dt[iMainCnt + 1][iSubCnt] - SubRedFimg.Dt[iMainCnt][iSubCnt];
            ClipDif.dDt[iMainCnt] = Dif.dDt[iMainCnt];
        }
        Dif.MoveAve(3);
            //差分clip
        for(iMainCnt = 0; iMainCnt < Dif.iDtNum; iMainCnt++){
            if(Dif.dDt[iMainCnt] > clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                iSrt = iMainCnt;
                while(iSrt > 0 && Dif.dDt[iSrt - 1] < Dif.dDt[iSrt]){
                    --iSrt;
                }
                while(iMainCnt < Dif.iDtNum - 1 && Dif.dDt[iMainCnt + 1] > clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                    ++iMainCnt;
                }
                iEnd = iMainCnt;
                while(iEnd < Dif.iDtNum - 1 && Dif.dDt[iEnd + 1] < Dif.dDt[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; iClipCnt++){
                    ClipDif.dDt[iClipCnt] = 0;
                }
            }else if(Dif.dDt[iMainCnt] < - clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                iSrt = iMainCnt;
				while(iSrt > 0 && Dif.dDt[iSrt - 1] > Dif.dDt[iSrt]){
                    --iSrt;
                }
                while(iMainCnt < Dif.iDtNum - 1 && Dif.dDt[iMainCnt + 1] < - clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                    ++iMainCnt;
                }
                iEnd = iMainCnt;
                while(iEnd < Dif.iDtNum - 1 && Dif.dDt[iEnd + 1] > Dif.dDt[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; iClipCnt++){
                    ClipDif.dDt[iClipCnt] = 0;
                }
			}
        }
        //積分
	fimg.Dt[0][iSubCnt] = difImg.Dt[0][iSubCnt] = SubRedFimg.Dt[0][iSubCnt];
        for(iMainCnt = 1; iMainCnt < fimg.MainPixel; iMainCnt++){
            fimg.Dt[iMainCnt][iSubCnt] =
                    fimg.Dt[iMainCnt - 1][iSubCnt] + ClipDif.dDt[iMainCnt - 1];
            difImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
        }
    }
    delete[] clip;

	TTMA sabun, sabunclip, sekibun;
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);
	int iMidColor = 1 << (Bit - 1);

	if (bDebugKasa){
		clipImg.SaveTmaTdt(FilePath	+ "1縦傘_Div" + FileName);

		sabun.NewDt(SubPixel, MainPixel, Bit);
		sabunclip.NewDt(SubPixel, MainPixel, Bit);
		sekibun.NewDt(SubPixel, MainPixel, Bit);

		for (iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
			sabun.Dt[iSubCnt][0] = 2047 * upConv;
			sabunclip.Dt[iSubCnt][0] = 2047 * upConv;
			for (iMainCnt = 1 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
//							SubRedFimg.Dt[iMainCnt][iSubCnt] - SubRedFimg.Dt[iMainCnt - 1][iSubCnt] + 2047.0 * upConv);
							SubRedFimg.Dt[iMainCnt][iSubCnt] - SubRedFimg.Dt[iMainCnt - 1][iSubCnt] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
//							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt - 1][iSubCnt] + 2047.0 * upConv);
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt - 1][iSubCnt] + iMidColor);
			}

			for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
//				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + 2047.0 * upConv);
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + iMidColor);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2縦傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3縦傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4縦傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
	}

    SubRedFimg.DeleteDt();

 //主filter処理（"AveZh.txt"）
    if(fimg.MainExpandMultiAve(KasaMainFilter) == false){
		CApplication::GetInstance()->MessageBox((ExtractFileName(KasaMainFilter) +
                " がopenできません").c_str(), "Error", MB_OK);
        return(false);
	}

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "5縦傘_主LowPassfilter" + FileName);
	}

//元画像－低周波画像
    for(iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
        for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
            fimg.Dt[iMainCnt][iSubCnt] =
                    difImg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt];
        }
    }
    difImg.DeleteDt();

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "6縦傘_主HighPassFilter" + FileName);
	}

//副filter処理（"Ave0,03h2L39_8.txt"）
    if(fimg.SubExpandMultiAve(KasaSubFilter) == false){
    	CApplication::GetInstance()->MessageBox((ExtractFileName(KasaSubFilter) +
                " がopenできません").c_str(), "Error", MB_OK);
		return(false);
    }

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "7縦傘_副LowPassfilter" + FileName);
	}

	fimg.LinearExpand(1, iSUB_RED);

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "8縦傘_LinearExpand" + FileName);
		sabun.DeleteDt();
	}

    for(iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
		for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
            fresult = linOrgFimg.Dt[iMainCnt][iSubCnt]
                    - fimg.Dt[iMainCnt][iSubCnt];
			if(fresult <= 0.0){
                linOrgFimg.Dt[iMainCnt][iSubCnt] = 0;
            }else if(fresult >= 65535){
                linOrgFimg.Dt[iMainCnt][iSubCnt] = 65535;
            }else{
				linOrgFimg.Dt[iMainCnt][iSubCnt] = fresult;
            }
        }
    }

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "9縦傘_雨" + FileName);
	}

}//endif


//後段の縦傘用テーブル（Log変換と線形変換の組み合わせ）
        //table作成
    unsigned short int nTable[65536];
    float   fResult;

    nTable[0] = 0;

    for(i = 1; i < 12550 ; i++){
//        fResult = 1024.0 * log10(i) - 2048.0;
        fResult = upConv * (1024.0 * log10((float)i) - 2048.0);
        if (fResult < 0.0){
            nTable[i] = 0;
        }else if (fResult > 4095.0 * upConv){
            nTable[i] = 4095 * upConv;
        }else{
            nTable[i] = (unsigned short int)fResult;
        }
    }
    for(i = 12550; i <= 65535 ; ++i){
		fResult = upConv * ((i + 105887.307753009) / 28896.7060898455 * 1024.0 - 2048.0);
        if (fResult < 0.0){
            nTable[i] = 0;
        }else if (fResult > 4095.0 * upConv){
            nTable[i] = 4095 * upConv;
		}else{
            nTable[i] = (unsigned short int)fResult;
        }
    }

#if 0
	if (bDebugKasa){
		FILE* fp_VerUmb2 = fopen("VerticalUmbrellaTable2.txt", "wt");
//    for(i = 0; i <= DtMax; i++)
		for(i = 0; i <= DtMax; i+=10)
		{
			fprintf(fp_VerUmb2, "%d %d\n", i, nTable[i]);
		}
		fclose(fp_VerUmb2);
	}
#endif

//後段の縦傘テーブルとAntiLogテーブルを１つのテーブルにマージする
	const int iMid = LogMidVal;//16bitLOG変換中心値
	const int Keta = LogKetaVal;//収録桁数

    //AntiLog変換(16bitLOG→16bitLinear)
    unsigned short int nTable2[65536];
    double dTemp, dTemp2, dTemp3;
	dTemp = (double)Keta / pow((float)2, (12 + upConvBit));// Keta/65536
	dTemp2 = -(pow((float)2, (12 + upConvBit)) - 1.) / 2.;// -65525/2

    for(i = 0; i < 4096 * upConv; i++){  //2^12 = 4096
        dTemp3 = iMid * pow(10, dTemp * (i + dTemp2));
//        if ((unsigned short int)dTemp3 > 4096 * upConv - 1)   //Bug!　65535より大きいdouble型を16bit整数型にキャストするのはNG
        if (dTemp3 > 4096 * upConv - 1)   //2^12 = 4096
        {
            nTable2[i] = 4096 * upConv - 1;//65535
		}
        else if (dTemp3 < 0)
        {
            nTable2[i] = 0;
        }
        else
        {
            nTable2[i] = (unsigned short int)dTemp3;
        }
    }
#if 0
	if (bDebugKasa){
		FILE* fp_AntiLog = fopen("AntiLogTable.txt", "wt");
//    for(i = 0; i <= DtMax; i++)
		for(i = 0; i <= DtMax; i+=10)
		{
			fprintf(fp_AntiLog, "%d %d\n", i, nTable2[i]);
		}
		fclose(fp_AntiLog);
	}
#endif

    //後段の縦傘用テーブルとAntiLog変換テーブルを合成
    for (i = 0; i < 4096 * upConv; i++)
    {
        nTable[i] = nTable2[nTable[i]];
    }

#if 0
	if (bDebugKasa){
		FILE* fp = fopen("MergedTable_2ndTableOfUmb_AntiLogTable.txt", "wt");
//    for(i = 0; i <= DtMax; i++)
		for(i = 0; i <= DtMax; i+=10)
		{
			fprintf(fp, "%d %d\n", i, nTable[i]);
		}
		fclose(fp);
	}
#endif

    //マージしたテーブルを通す
    for(i = 0 ; i < SubPixel ; i++)
    	for(j = 0 ; j < MainPixel ; j++)
            Dt[i][j] = nTable[(linOrgFimg.Dt[j][i] > (4096 * upConv - 1)) ? (4096 * upConv - 1) : ((linOrgFimg.Dt[j][i] < 0) ? 0 : (int)linOrgFimg.Dt[j][i])];

    return true;
}
//---------------------------------------------------------------------------
//77G向けムラ除去コンポ（16bitLOG→16bitLinear出力）フローに合わせた縦傘処理関数
bool TSIMARS::bSuperRedUMB2_77G(bool bKasa, const String KasaMainFilter, const String KasaSubFilter,
			const String KasaClipFile, const int LogMidVal, const int LogKetaVal, const bool bDebugKasa, const String strImage)
{
	const int upConvBit = 4;//LOG画像が12ビットから16ビットに変わることによるビットシフト数
	const int upConv = 1 << upConvBit;//変倍率


	const int iSUB_RED = 8;

	TFIMG   fimg;
	TFIMG   difImg;
	TFIMG   linOrgFimg;     //Linear Original Image
	TFIMG   SubRedFimg;     //Sub Direction Reduction Image
	int iMainCnt, iSubCnt;
	float   fresult;

//QL依存の振幅clip
	TFIMG   difAveImg;
//    float clip[4096];
	float* clip = new float[4096 * upConv];
	fstream file;
	int i, j;

	if(Form1->bReadTbl(KasaClipFile, clip, DtMax + 1) == false){
		CApplication::GetInstance()->MessageBox("clip table作成できませんでした", "Error", MB_OK);
		delete[] clip;
		return(false);
    }


//前段の縦傘用テーブル（anti log変換）
		//table作成
//    float   fTable[4096];
	float* fTable = new float[4096 * upConv];
//    float   fTable[65536];
/*
	for(i = 0; i < 2149 * upConv ; i++){
		fTable[i] = 100.0 * pow(10.0, i / (1024.0 * upConv));
	}
	for(i = 2149 * upConv; i <= DtMax ; i++){
		fTable[i] = 28896.7060898455 * (2.0 + i / (1024.0 * upConv)) - 105887.307753009;
	}
*/
	for(i = 0; i <= DtMax ; i++){
		fTable[i] = 948.0 * pow(10.0, (i/4. - 8192)/4096.);
		if (65535 < fTable[i]) {
            fTable[i] = 65535.;
		}
	}

#if 0
	FILE* fp_VerUmb1 = fopen("VerticalUmbrellaTable1.txt", "wt");
	for(i = 0; i <= DtMax; i+=10)
	{
		fprintf(fp_VerUmb1, "%d %f\n", i, fTable[i]);
	}
	fclose(fp_VerUmb1);
#endif

    TTMA clipImg = *this;
    clipImg.DivWithRest(iSUB_RED, 1);

	linOrgFimg.NewDt(MainPixel, SubPixel);
	SubRedFimg.NewDt(MainPixel, SubPixel);
	for(i = 0 ; i < SubPixel ; i++){
    	for(j = 0 ; j < MainPixel ; j++){
			linOrgFimg.Dt[j][i] = fTable[Dt[i][j]];
			SubRedFimg.Dt[j][i] = linOrgFimg.Dt[j][i];
		}
    }
    delete[] fTable;//ttsuji追加


//縦傘処理OFFか否か
if (bKasa)
{

    SubRedFimg.Div(1, iSUB_RED);


//主差分画像作成
    TFIMG lowFimg;

    TSINGLE_DATA Dif, ClipDif;
	int iSrt, iEnd;
    int iClipCnt;

#if 0 //110629 横傘とそろえるテスト(ttsuji)
	for(int iCnt = 0; iCnt < 4096 * upConv; iCnt++){
		clip[iCnt] /= 3.0 ;  //clipは3画素加算を前提で作成されているため
	}
#endif

	Dif.NewDt(MainPixel - 1);
	ClipDif.NewDt(MainPixel - 1);
    fimg.NewDt(SubRedFimg.MainPixel, SubRedFimg.SubPixel);
    difImg.NewDt(SubRedFimg.MainPixel, SubRedFimg.SubPixel);
    for(iSubCnt = 0; iSubCnt < difImg.SubPixel; iSubCnt++){
            //差分
        for(iMainCnt = 0; iMainCnt < Dif.iDtNum; iMainCnt++){
            Dif.dDt[iMainCnt] = SubRedFimg.Dt[iMainCnt + 1][iSubCnt] - SubRedFimg.Dt[iMainCnt][iSubCnt];
            ClipDif.dDt[iMainCnt] = Dif.dDt[iMainCnt];
        }

#if 0 //110629 横傘とそろえるテスト(ttsuji)
		Dif.MoveAve(3);
#endif

		//差分clip
        for(iMainCnt = 0; iMainCnt < Dif.iDtNum; iMainCnt++){
            if(Dif.dDt[iMainCnt] > clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                iSrt = iMainCnt;
                while(iSrt > 0 && Dif.dDt[iSrt - 1] < Dif.dDt[iSrt]){
                    --iSrt;
				}
                while(iMainCnt < Dif.iDtNum - 1 && Dif.dDt[iMainCnt + 1] > clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                    ++iMainCnt;
                }
                iEnd = iMainCnt;
                while(iEnd < Dif.iDtNum - 1 && Dif.dDt[iEnd + 1] < Dif.dDt[iEnd]){
                    ++iEnd;
                }
				for(iClipCnt = iSrt; iClipCnt <= iEnd; iClipCnt++){
					ClipDif.dDt[iClipCnt] = 0;
				}
            }else if(Dif.dDt[iMainCnt] < - clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                iSrt = iMainCnt;
				while(iSrt > 0 && Dif.dDt[iSrt - 1] > Dif.dDt[iSrt]){
                    --iSrt;
                }
                while(iMainCnt < Dif.iDtNum - 1 && Dif.dDt[iMainCnt + 1] < - clip[clipImg.Dt[iSubCnt][iMainCnt]]){
                    ++iMainCnt;
                }
                iEnd = iMainCnt;
				while(iEnd < Dif.iDtNum - 1 && Dif.dDt[iEnd + 1] > Dif.dDt[iEnd]){
                    ++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; iClipCnt++){
                    ClipDif.dDt[iClipCnt] = 0;
                }
			}
        }
        //積分
	fimg.Dt[0][iSubCnt] = difImg.Dt[0][iSubCnt] = SubRedFimg.Dt[0][iSubCnt];
		for(iMainCnt = 1; iMainCnt < fimg.MainPixel; iMainCnt++){
            fimg.Dt[iMainCnt][iSubCnt] =
					fimg.Dt[iMainCnt - 1][iSubCnt] + ClipDif.dDt[iMainCnt - 1];
            difImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
        }
    }
    delete[] clip;

	TTMA sabun, sabunclip, sekibun;
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);
	int iMidColor = 1 << (Bit - 1);

	if (bDebugKasa){
		clipImg.SaveTmaTdt(FilePath	+ "1縦傘_Div" + FileName);

		sabun.NewDt(SubPixel, MainPixel, Bit);
		sabunclip.NewDt(SubPixel, MainPixel, Bit);
		sekibun.NewDt(SubPixel, MainPixel, Bit);

		for (iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
			sabun.Dt[iSubCnt][0] = 2047 * upConv;
			sabunclip.Dt[iSubCnt][0] = 2047 * upConv;
			for (iMainCnt = 1 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
//							SubRedFimg.Dt[iMainCnt][iSubCnt] - SubRedFimg.Dt[iMainCnt - 1][iSubCnt] + 2047.0 * upConv);
							SubRedFimg.Dt[iMainCnt][iSubCnt] - SubRedFimg.Dt[iMainCnt - 1][iSubCnt] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
//							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt - 1][iSubCnt] + 2047.0 * upConv);
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt - 1][iSubCnt] + iMidColor);
			}

			for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
//				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] + 2047.0 * upConv);
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] /*+*/- iMidColor);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2縦傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3縦傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4縦傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
	}

    SubRedFimg.DeleteDt();

 //主filter処理（"AveZh.txt"）
    if(fimg.MainExpandMultiAve(KasaMainFilter) == false){
		CApplication::GetInstance()->MessageBox((ExtractFileName(KasaMainFilter) +
                " がopenできません").c_str(), "Error", MB_OK);
        return(false);
	}

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "5縦傘_主LowPassfilter" + FileName);
	}

//元画像－低周波画像
    for(iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
        for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
            fimg.Dt[iMainCnt][iSubCnt] =
                    difImg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt];
        }
    }
    difImg.DeleteDt();

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "6縦傘_主HighPassFilter" + FileName);
	}

//副filter処理（"Ave0,03h2L39_8.txt"）
    if(fimg.SubExpandMultiAve(KasaSubFilter) == false){
    	CApplication::GetInstance()->MessageBox((ExtractFileName(KasaSubFilter) +
                " がopenできません").c_str(), "Error", MB_OK);
		return(false);
    }

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "7縦傘_副LowPassfilter" + FileName);
	}

	fimg.LinearExpand(1, iSUB_RED);

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "8縦傘_LinearExpand" + FileName);
		sabun.DeleteDt();
	}

    for(iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
		for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
			fresult = linOrgFimg.Dt[iMainCnt][iSubCnt]
					- fimg.Dt[iMainCnt][iSubCnt];
			if(fresult <= 0.0){
                linOrgFimg.Dt[iMainCnt][iSubCnt] = 0;
			}else if(fresult >= 65535){
                linOrgFimg.Dt[iMainCnt][iSubCnt] = 65535;
            }else{
				linOrgFimg.Dt[iMainCnt][iSubCnt] = fresult;
            }
        }
	}

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "9縦傘_雨" + FileName);
		sabun = linOrgFimg;
		sabun.SaveTmaTdt(FilePath + "10結果" + FileName);
	}

}//endif

/*
//後段の縦傘用テーブル（Log変換）
        //table作成
	unsigned short int nTable[65536];
    float   fResult;

    nTable[0] = 0;
	for(i = 0; i <= DtMax; i+=10)
	{
		fResult = 4 * (4096. * log10(i/948.) + 8192.);
		if (fResult < 0.0){
			nTable[i] = 0;
		}else if (fResult > 4095.0 * upConv){
			nTable[i] = 4095 * upConv;
		}else{
			nTable[i] = (unsigned short int)fResult;
		}
	}



#if 0
	FILE* fp_VerUmb2 = fopen("VerticalUmbrellaTable2.txt", "wt");
	for(i = 0; i <= DtMax; i+=10)
	{
		fprintf(fp_VerUmb2, "%d %d\n", i, nTable[i]);
	}
	fclose(fp_VerUmb2);
#endif

//後段の縦傘テーブルとAntiLogテーブルを１つのテーブルにマージする

	const int iMid = LogMidVal;//16bitLOG変換中心値
	const int Keta = LogKetaVal;//収録桁数

	//AntiLog変換(16bitLOG→16bitLinear)
	unsigned short int nTable2[65536];
	double dTemp, dTemp2, dTemp3;
	dTemp = (double)Keta / pow(2, (12 + upConvBit));// Keta/65536
	dTemp2 = -(pow(2, (12 + upConvBit)) - 1.) / 2.;// -65525/2

	for(i = 0; i < 4096 * upConv; i++){  //2^12 = 4096
		dTemp3 = iMid * pow(10, dTemp * (i + dTemp2));
//        if ((unsigned short int)dTemp3 > 4096 * upConv - 1)   //Bug!　65535より大きいdouble型を16bit整数型にキャストするのはNG
		if (dTemp3 > 4096 * upConv - 1)   //2^12 = 4096
		{
			nTable2[i] = 4096 * upConv - 1;//65535
		}
		else if (dTemp3 < 0)
		{
			nTable2[i] = 0;
		}
		else
		{
			nTable2[i] = (unsigned short int)dTemp3;
		}
	}
#if 0
	FILE* fp_AntiLog = fopen("AntiLogTable.txt", "wt");
	for(i = 0; i <= DtMax; i+=10)
	{
		fprintf(fp_AntiLog, "%d %d\n", i, nTable2[i]);
	}
	fclose(fp_AntiLog);
#endif

	//後段の縦傘用テーブルとAntiLog変換テーブルを合成
	for (i = 0; i < 4096 * upConv; i++)
	{
		nTable[i] = nTable2[nTable[i]];
	}
*/
#if 1
	//tmp
	unsigned short int nTable[65536];
	for (i = 0; i < 4096 * upConv; i++)
	{
		nTable[i] = i;
	}
/*
	FILE* fp = fopen("C:\\MergedTable_2ndTableOfUmb_AntiLogTable.txt", "wt");
	for(i = 0; i <= DtMax; i+=10)
	{
		fprintf(fp, "%d %d\n", i, nTable[i]);
	}
	fclose(fp);


	fp = fopen("C:\\18.txt", "wt");
	for(i = 0; i < 4096 ; i++)
	{
		fprintf(fp, "18 ");
	}
	fclose(fp);
*/
#endif

	//マージしたテーブルを通す
	for(i = 0 ; i < SubPixel ; i++)
		for(j = 0 ; j < MainPixel ; j++)
			Dt[i][j] = nTable[(linOrgFimg.Dt[j][i] > (4096 * upConv - 1)) ? (4096 * upConv - 1) : ((linOrgFimg.Dt[j][i] < 0) ? 0 : (int)linOrgFimg.Dt[j][i])];


	if (bDebugKasa){
		string FilePath = ExtractFilePath(strImage);
		string FileName = ExtractFileName(strImage);
		SaveTmaTdt(FilePath + "11結果table変換" + FileName);
	}


	return true;
}
//---------------------------------------------------------------------------
//FPD1ムラ除去コンポ（16bitLOG→16bitLinear出力）フローに合わせた縦横傘処理関数

bool TSIMARS::bFPD1MuraJokyoCompo(bool bKasa, bool bYokokasa,
            const String KasaMainFilter,
			const String KasaSubFilter,
            const String KasaClipFile,
			const String YokokasaMainFilter,
            const String YokokasaSubFilter,
            const String YokokasaHighCutFilter,
			const double YokokasaClipVal,
			const int YokokasaMainDiv,
			const int LogMidVal,
			const int LogKetaVal,
			const bool bDebugKasa,
			const bool bDebugYokokasa,
			const string strImage)
{
	//縦傘（16bitLOG→16bitLinear出力）
//	if (!bSuperRedUMB2_FPD1(bKasa, KasaMainFilter, KasaSubFilter, KasaClipFile, LogMidVal, LogKetaVal, bDebugKasa, strImage)){
	if (!bSuperRedUMB2_77G(bKasa, KasaMainFilter, KasaSubFilter, KasaClipFile, LogMidVal, LogKetaVal, bDebugKasa, strImage)){
		CApplication::GetInstance()->MessageBox("縦傘処理エラー", "Error", MB_OK);
		return false;
	}

	//横傘１（16bitLinear→16bitLinear）
	if (bYokokasa){
		HorUmbMainRedAmp4(YokokasaHighCutFilter, YokokasaMainFilter, YokokasaSubFilter, (float)YokokasaClipVal, YokokasaMainDiv, bDebugYokokasa, strImage);//16bitLinearなのでとりあえずclip値は28（14bitLinearで7くらいが最適値だった）
	}

	return true;
}
//---------------------------------------------------------------------------
//77G対策を入れた16bitLinear入出力の縦傘（ログ変換を内部で行っていない）

bool TSIMARS::bTateKasa(const string KasaMainFilter, const string KasaSubFilter, const int iKasaClipVal, const int iSubRed, const bool bDebugKasa, const string strImage)
{

	const int upConvBit = 4;//LOG画像が12ビットから16ビットに変わることによるビットシフト数
	const int upConv = 1 << upConvBit;//変倍率


//	const int iSUB_RED = 8;

	TFIMG   fimg;
	TFIMG   difImg;
	TFIMG   linOrgFimg;     //Linear Original Image
	TFIMG   SubRedFimg;     //Sub Direction Reduction Image
	int iMainCnt, iSubCnt;
	float   fresult;

//QL依存の振幅clip
	TFIMG   difAveImg;
	int i, j;

	TTMA clipImg = *this;
	clipImg.DivWithRest(iSubRed, 1);

	linOrgFimg.NewDt(MainPixel, SubPixel);
	SubRedFimg.NewDt(MainPixel, SubPixel);
	for(i = 0 ; i < SubPixel ; i++){
		for(j = 0 ; j < MainPixel ; j++){
			SubRedFimg.Dt[j][i] = linOrgFimg.Dt[j][i] = Dt[i][j];
		}
	}

	SubRedFimg.Div(1, iSubRed);


//主差分画像作成
    TFIMG lowFimg;

	TSINGLE_DATA Dif, ClipDif;
	int iSrt, iEnd;
    int iClipCnt;

#if 0 //110629 横傘とそろえるテスト(ttsuji)
	for(int iCnt = 0; iCnt < 4096 * upConv; iCnt++){
		clip[iCnt] /= 3.0 ;  //clipは3画素加算を前提で作成されているため
	}
#endif

	Dif.NewDt(MainPixel - 1);
	ClipDif.NewDt(MainPixel - 1);
    fimg.NewDt(SubRedFimg.MainPixel, SubRedFimg.SubPixel);
    difImg.NewDt(SubRedFimg.MainPixel, SubRedFimg.SubPixel);
    for(iSubCnt = 0; iSubCnt < difImg.SubPixel; iSubCnt++){
            //差分
        for(iMainCnt = 0; iMainCnt < Dif.iDtNum; iMainCnt++){
            Dif.dDt[iMainCnt] = SubRedFimg.Dt[iMainCnt + 1][iSubCnt] - SubRedFimg.Dt[iMainCnt][iSubCnt];
            ClipDif.dDt[iMainCnt] = Dif.dDt[iMainCnt];
        }

#if 0 //110629 横傘とそろえるテスト(ttsuji)
		Dif.MoveAve(3);
#endif

		//差分clip
        for(iMainCnt = 0; iMainCnt < Dif.iDtNum; iMainCnt++){
			if(Dif.dDt[iMainCnt] > iKasaClipVal){
				iSrt = iMainCnt;
                while(iSrt > 0 && Dif.dDt[iSrt - 1] < Dif.dDt[iSrt]){
					--iSrt;
				}
				while(iMainCnt < Dif.iDtNum - 1 && Dif.dDt[iMainCnt + 1] > iKasaClipVal){
					++iMainCnt;
                }
                iEnd = iMainCnt;
                while(iEnd < Dif.iDtNum - 1 && Dif.dDt[iEnd + 1] < Dif.dDt[iEnd]){
                    ++iEnd;
                }
				for(iClipCnt = iSrt; iClipCnt <= iEnd; iClipCnt++){
					ClipDif.dDt[iClipCnt] = 0;
				}
			}else if(Dif.dDt[iMainCnt] < - iKasaClipVal){
				iSrt = iMainCnt;
				while(iSrt > 0 && Dif.dDt[iSrt - 1] > Dif.dDt[iSrt]){
					--iSrt;
                }
				while(iMainCnt < Dif.iDtNum - 1 && Dif.dDt[iMainCnt + 1] < - iKasaClipVal){
					++iMainCnt;
				}
                iEnd = iMainCnt;
				while(iEnd < Dif.iDtNum - 1 && Dif.dDt[iEnd + 1] > Dif.dDt[iEnd]){
					++iEnd;
                }
                for(iClipCnt = iSrt; iClipCnt <= iEnd; iClipCnt++){
                    ClipDif.dDt[iClipCnt] = 0;
                }
			}
        }
        //積分
	fimg.Dt[0][iSubCnt] = difImg.Dt[0][iSubCnt] = SubRedFimg.Dt[0][iSubCnt];
		for(iMainCnt = 1; iMainCnt < fimg.MainPixel; iMainCnt++){
            fimg.Dt[iMainCnt][iSubCnt] =
					fimg.Dt[iMainCnt - 1][iSubCnt] + ClipDif.dDt[iMainCnt - 1];
            difImg.Dt[iMainCnt][iSubCnt] = fimg.Dt[iMainCnt][iSubCnt];
        }
    }

	TTMA sabun, sabunclip, sekibun;
	string FilePath = ExtractFilePath(strImage);
	string FileName = ExtractFileName(strImage);
	int iMidColor = 1 << (Bit - 1);

	if (bDebugKasa){
		clipImg.SaveTmaTdt(FilePath	+ "1縦傘_Div" + FileName);

		sabun.NewDt(SubPixel, MainPixel, Bit);
		sabunclip.NewDt(SubPixel, MainPixel, Bit);
		sekibun.NewDt(SubPixel, MainPixel, Bit);

		for (iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
			sabun.Dt[iSubCnt][0] = 2047 * upConv;
			sabunclip.Dt[iSubCnt][0] = 2047 * upConv;
			for (iMainCnt = 1 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
				sabun.SetVal(iSubCnt, iMainCnt,
							SubRedFimg.Dt[iMainCnt][iSubCnt] - SubRedFimg.Dt[iMainCnt - 1][iSubCnt] + iMidColor);

				sabunclip.SetVal(iSubCnt, iMainCnt,
							fimg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt - 1][iSubCnt] + iMidColor);
			}

			for (iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
				sekibun.SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt] /*+*/- iMidColor);
			}
		}
		sabun.SaveTmaTdt(FilePath + "2縦傘_差分" + FileName);
		sabunclip.SaveTmaTdt(FilePath + "3縦傘_差分clip" + FileName);
		sekibun.SaveTmaTdt(FilePath + "4縦傘_積分" + FileName);

		sabunclip.DeleteDt();
		sekibun.DeleteDt();
	}

    SubRedFimg.DeleteDt();

 //主filter処理（"AveZh.txt"）
    if(fimg.MainExpandMultiAve(KasaMainFilter) == false){
		CApplication::GetInstance()->MessageBox((ExtractFileName(KasaMainFilter) +
                " がopenできません").c_str(), "Error", MB_OK);
        return(false);
	}

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "5縦傘_主LowPassfilter" + FileName);
	}

//元画像－低周波画像
    for(iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
        for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
            fimg.Dt[iMainCnt][iSubCnt] =
                    difImg.Dt[iMainCnt][iSubCnt] - fimg.Dt[iMainCnt][iSubCnt];
        }
    }
    difImg.DeleteDt();

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "6縦傘_主HighPassFilter" + FileName);
	}

//副filter処理（"Ave0,03h2L39_8.txt"）
    if(fimg.SubExpandMultiAve(KasaSubFilter) == false){
    	CApplication::GetInstance()->MessageBox((ExtractFileName(KasaSubFilter) +
                " がopenできません").c_str(), "Error", MB_OK);
		return(false);
    }

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "7縦傘_副LowPassfilter" + FileName);
	}

	fimg.LinearExpand(1, iSubRed);

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "8縦傘_LinearExpand" + FileName);
		sabun.DeleteDt();
	}

	for(iMainCnt = 0 ; iMainCnt < fimg.MainPixel ; iMainCnt++){
		for(iSubCnt = 0 ; iSubCnt < fimg.SubPixel ; iSubCnt++){
			fresult = linOrgFimg.Dt[iMainCnt][iSubCnt]
					- fimg.Dt[iMainCnt][iSubCnt];
			if(fresult <= 0.0){
				Dt[iSubCnt][iMainCnt] = 0;
			}else if(fresult >= 65535){
				Dt[iSubCnt][iMainCnt] = 65535;
			}else{
				Dt[iSubCnt][iMainCnt] = fresult;
			}
        }
	}

	if (bDebugKasa){
		sabun = (fimg + iMidColor);
		sabun.SaveTmaTdt(FilePath + "9縦傘_雨" + FileName);
		sabun = *this;
		sabun.SaveTmaTdt(FilePath + "10結果" + FileName);
	}

	return true;
}
//---------------------------------------------------------------------------
/*
	パルス縦スジ除去（MARS補正より抽出）
*/
bool TSIMARS::bRemVerLine(const int iMMedian, const String SubLowPassFilter, const int iClip, const int iBIN, const bool bOut)
{
	TFIMG   SubRedFimg(*this);
	SubRedFimg.DivWithRest(1, iBIN);
	TFIMG median(SubRedFimg);

//主filter処理
	median.MainMedian(iMMedian);

//元画像－高周波画像
	SubRedFimg -= median;

//Clip処理
	for (int iMCnt = 0; iMCnt < SubRedFimg.MainPixel; ++iMCnt){
		if (fabs(SubRedFimg.Dt[iMCnt][0]) > iClip){
			SubRedFimg.Dt[iMCnt][0] = 0.0f;
		}
		for (int iSCnt = 1; iSCnt < SubRedFimg.SubPixel; ++iSCnt){
			if (fabs(SubRedFimg.Dt[iMCnt][iSCnt]) > iClip){
				SubRedFimg.Dt[iMCnt][iSCnt] = SubRedFimg.Dt[iMCnt][iSCnt - 1];
				//SubRedFimg.Dt[iMCnt][0] = 0.0f;
			}
		}
	}
//副filter処理
	if (SubRedFimg.SubExpandMultiAve(SubLowPassFilter) == false){
		CApplication::GetInstance()->MessageBox((ExtractFileName(SubLowPassFilter) + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}

	SubRedFimg.LinearExpand(1, iBIN);
	if (bOut) {  /*MARS用テストデータ出力*/
		TTMA temp(SubPixel, MainPixel, Bit);
		for (int iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
			for (int iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
				temp.SetVal(iSubCnt, iMainCnt, SubRedFimg.Dt[iMainCnt][iSubCnt] + DtMax / 2);
			}
		}
		temp.WriteTma("LogRemLine.tma");
	}

	for (int iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
		for (int iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			SetVal(iSubCnt, iMainCnt, Dt[iSubCnt][iMainCnt] - SubRedFimg.Dt[iMainCnt][iSubCnt]);
		}
	}

	return true ;
}
//---------------------------------------------------------------------------
/*
	パルス横スジ除去
*/
bool TSIMARS::bRemHorLine(const int iSMedian, const String MainLowPassFilter, const int iClip, const int iBIN, const bool bOut, const string in_str, const bool bMainExpandMultiAve, const int iMainMedian, const bool bAntiClip)
{
	TFIMG   MainRedFimg(*this);
	MainRedFimg.DivWithRest(iBIN, 1);
	TFIMG median(MainRedFimg);

//副Medianfilter処理
	median.SubMedian(iSMedian);

//元画像－高周波画像
	MainRedFimg -= median;

//Clip処理
	switch (bAntiClip) {
	case 0:
		for (int iSCnt = 0; iSCnt < MainRedFimg.SubPixel; iSCnt++){
			if (fabs(MainRedFimg.Dt[0][iSCnt]) > iClip){
				MainRedFimg.Dt[0][iSCnt] = 0.0f;
			}
			for (int iMCnt = 1; iMCnt < MainRedFimg.MainPixel; iMCnt++){
				if (fabs(MainRedFimg.Dt[iMCnt][iSCnt]) > iClip){
					MainRedFimg.Dt[iMCnt][iSCnt] = MainRedFimg.Dt[iMCnt - 1][iSCnt];
					//MainRedFimg.Dt[0][iSCnt] = 0.0f;
				}
			}
		}
		break;
	default:
		//逆クリップ（クリップ値以上のスジのみ抽出する）
		for (int iSCnt = 0; iSCnt < MainRedFimg.SubPixel; iSCnt++){
			if (fabs(MainRedFimg.Dt[0][iSCnt]) < iClip){//逆クリップ
				MainRedFimg.Dt[0][iSCnt] = 0.0f;
			}
			for (int iMCnt = 1; iMCnt < MainRedFimg.MainPixel; iMCnt++){
				if (fabs(MainRedFimg.Dt[iMCnt][iSCnt]) < iClip){//逆クリップ
//					MainRedFimg.Dt[iMCnt][iSCnt] = MainRedFimg.Dt[iMCnt - 1][iSCnt];
					MainRedFimg.Dt[iMCnt][iSCnt] = 0.0f;
				}
			}
		}
		break;
	}

//主filter処理
	if (bMainExpandMultiAve) {
    	//主多段フィルター
		if (MainRedFimg.MainExpandMultiAve(MainLowPassFilter) == false){
			CApplication::GetInstance()->MessageBox((ExtractFileName(MainLowPassFilter) + " がopenできません").c_str(), "Error", MB_OK);
			return false;
		}
	}
	else{
		//主Medianfilter処理
		MainRedFimg.MainMedian(iMainMedian);
	}

	MainRedFimg.LinearExpand(iBIN, 1);
	if (bOut) {
		TTMA temp(SubPixel, MainPixel, Bit);
		for (int iMainCnt = 0; iMainCnt < MainPixel; iMainCnt++){
			for (int iSubCnt = 0; iSubCnt < SubPixel; iSubCnt++){
				temp.SetVal(iSubCnt, iMainCnt, MainRedFimg.Dt[iMainCnt][iSubCnt] + DtMax / 2);
			}
		}
//		temp.SaveTmaTdt(ExtractFilePath(in_str) + "LogRemLine_" + ExtractFileName(in_str));
		temp.SaveTmaTdt(ExtractFilePath(in_str) + "LogRemLine" + "(s" + IntToStr(iSMedian) + "m" + IntToStr(iMainMedian) + "div" + IntToStr(iBIN) + "clip" + IntToStr(iClip) + ")_" + ExtractFileName(in_str));
	}

	for (int iMainCnt = 0; iMainCnt < MainPixel; iMainCnt++){
		for (int iSubCnt = 0; iSubCnt < SubPixel; iSubCnt++){
			SetVal(iSubCnt, iMainCnt, Dt[iSubCnt][iMainCnt] - MainRedFimg.Dt[iMainCnt][iSubCnt]);
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool TSIMARS::bExtractHorLine(const string in_str, const int in_iSMedian, const int in_iKasaageQL, const int in_iMMoveAve, const int in_iQL_Threshold, const bool in_bImageOutput, int& out_Cnt)//全幅横スジ抽出
{
	TFIMG   MainFimg(*this);
	TFIMG   median(MainFimg);
	TFIMG	MainFMed;
	MainFMed.NewDt(1, MainFimg.SubPixel);

	median.SubMedian(in_iSMedian);//副Medianfilter処理

	//元画像－低周波画像
	MainFimg -= median;
	MainFimg += in_iKasaageQL;//かさ上げ
 //	CTDAT simars = MainFimg;//自然数に変換
	TSIMARS simars;
	const int   BIT = 16;       //変換後の画像bit
	int i,j;

	simars.NewDt(MainFimg.SubPixel, MainFimg.MainPixel, BIT);
/*	for(i = 0 ; i < MainFimg.SubPixel ; ++i){
		for(j = 0 ; j < MainFimg.MainPixel ; ++j){
			simars.SetVal(i, j, MainFimg.Dt[j][i]);
		}
	}
	simars.SaveTmaTdt(ExtractFilePath(in_str) + "MainMedTmp_" + ExtractFileName(in_str));
*/

	MainFimg.MainExpandMoveAve(in_iMMoveAve);//主移動平均処理

	if (in_bImageOutput) {
		for(i = 0 ; i < MainFimg.SubPixel ; ++i){
			for(j = 0 ; j < MainFimg.MainPixel ; ++j){
				simars.SetVal(i, j, MainFimg.Dt[j][i]);
			}
		}
		simars.SaveTmaTdt(ExtractFilePath(in_str) + "FullWidthHorLine_" + ExtractFileName(in_str));
	}


	float* ftmp = new float[MainFimg.MainPixel];

	int iSub, iMain;
	for (iSub = 0; iSub < MainFimg.SubPixel; iSub++) {
		for (iMain = 0; iMain < MainFimg.MainPixel; iMain++) {
			ftmp[iMain] = MainFimg.Dt[iMain][iSub];
		}

		qsort(ftmp, MainFimg.MainPixel, sizeof(float), Cmp);

		MainFMed.Dt[0][iSub] = ftmp[(int)(MainFimg.MainPixel / 2)];
	}
	delete[] ftmp;

	string FileStr = in_str;
	if (!MainFMed.bSaveAsText(ExtractFilePath(in_str) + "MainMed_" + ExtractFileName(ChangeFileExt(FileStr, ".txt")))) {
		return false;
	}

	//副プロファイルの平均値を計算
	float fAve = 0.0f;
	for (iSub = 0; iSub < MainFimg.SubPixel; iSub++) {
		fAve += MainFMed.Dt[0][iSub];
	}
	fAve /= MainFimg.SubPixel;

	//副プロファイルから全幅横スジの存在を判定
	string str_tmp = ExtractFilePath(in_str) + "FullWidthHorLine_" + ExtractFileName(ChangeFileExt(FileStr, ".txt"));
	FILE* fp = fopen(str_tmp.c_str(), "wt");
	fprintf(fp, "iSubMedian:	%d\n", in_iSMedian);
	fprintf(fp, "iMMoveAve:		%d\n", in_iMMoveAve);
	fprintf(fp, "Average: %f\n", fAve);
	fprintf(fp, "iQL_Threshold: %d\n\n", in_iQL_Threshold);
	fprintf(fp, "iSub	QL\n");

	out_Cnt = 0;
	for (iSub = 0; iSub < MainFimg.SubPixel; iSub++) {
		float ftemp = MainFMed.Dt[0][iSub] - fAve;
		if (in_iQL_Threshold < fabs(ftemp)) {
			fprintf(fp, "%d	%f\n", iSub, ftemp);
			out_Cnt++;
		}
	}
	fclose(fp);

	return true;
}
//---------------------------------------------------------------------------
/*
		17インチ評価機Phase1画像の読み込み(ttsuji追加)
*/
bool TSIMARS::ReadImage17(const string fileName)
{
	const int iMain = 9200;//固定
	int iSub = 0;//ここを計算
	int iByteNum = 2;//２バイトデータ（固定）
	long lsize = 0;//画像ファイルのバイト数

	FILE* fp = fopen(fileName.c_str(), "rb");
	if (!fp){
		CApplication::GetInstance()->MessageBox("ファイルオープンに失敗しました", "Error", MB_OK);
		return false;
	}
	fseek(fp, 0, SEEK_END);
	if (fgetpos(fp, &lsize)){
		CApplication::GetInstance()->MessageBox("ファイルサイズ取得に失敗しました", "Error", MB_OK);
		fclose(fp);
		return false;
	}
	fclose(fp);

	//tmp
	const long NGsize = 18860000;//FTAのファイルサイズ
	if (NGsize == lsize) {//FTAファイルをFXAファイルとして読もうとした場合
		CApplication::GetInstance()->MessageBox("ファイルサイズが不正です（18860000バイトはFTAです）", "Error", MB_OK);
		return false;
	}

	int mod = lsize % (iMain * iByteNum);
	if (mod){//割り切れなければ
		CApplication::GetInstance()->MessageBox("ファイルサイズが不正です（主サイズ9200のみ）", "Error", MB_OK);
		return false;
	}
	iSub = lsize / (iMain * iByteNum);

	//主副画素数
	MainPixel = iMain;
	SubPixel = iSub;

	//主副画素密度
	mainGasoMitsudo[0] = '6';
	mainGasoMitsudo[1] = '7';
	mainGasoMitsudo[2] = '\0';
	SetMainPixelSize(MitsudoToPixelSize(mainGasoMitsudo));
	subGasoMitsudo[0] = '6';
	subGasoMitsudo[1] = '7';
	subGasoMitsudo[2] = '\0';
	SetSubPixelSize(MitsudoToPixelSize(subGasoMitsudo));

	Bit = 16;
	iEDR = iEDR_OFF;
	iSk = '0';
    iL = '0';

	if (!ReadDt(fileName)){
		CApplication::GetInstance()->MessageBox("画像データ読出しに失敗しました", "Error", MB_OK);
		return false;
	}

    return true;
}
//---------------------------------------------------------------------------
/*
17インチ評価機Phase1画像並び替え
　　主方向の先頭からiM1個の画素と、iM2番目からiM1個の画素とを抽出して接続する
*/
bool TSIMARS::Image17Sort()//ttsuji追加
{
	const int iM1 = 1536;
	const int iM2 = 1840;//1840*5=9000
	const int iMain = 9200;//主方向画素数がこの値のときのみ計算する
	if (MainPixel != iMain){
   	CApplication::GetInstance()->MessageBox("ファイルサイズが不正です（主サイズ9200のみ）", "Error", MB_OK);
        return false;
	}

	CTDAT temp;
	int iMCnt, iSCnt;
	temp.NewDt(SubPixel, MainPixel, Bit);
    for(iSCnt = 0; iSCnt < SubPixel; iSCnt++){
		for(iMCnt = 0; iMCnt < MainPixel; iMCnt++){
			temp.Dt[iSCnt][iMCnt] = Dt[iSCnt][iMCnt];
        }
	}

    NewDt(SubPixel, iM1*2, Bit);
    for(iSCnt = 0; iSCnt < SubPixel; iSCnt++){
        for(iMCnt = 0; iMCnt < iMain; iMCnt++){//MainPixelはNewDt内部で上書きされるのでiMain
			if (iMCnt < iM1){
				Dt[iSCnt][iMCnt] = temp.Dt[iSCnt][iMCnt];
			}
			else if (iM2*3 <= iMCnt && iMCnt < iM2*3 + iM1){
				Dt[iSCnt][iMCnt - iM2*3 + iM1] = temp.Dt[iSCnt][iMCnt];
            }
        }
	}
	return true;
}
//---------------------------------------------------------------------------
//FTA用画像並び替え関数　ttsuji追加
bool TSIMARS::bChangeOrder(String fileName)
{
	const int iASIC = 256;
	const int iASIC_NUM = 14;
	const int iBIT =    16;
	const int iMAIN =   9200;
	const float fPIX_SIZE = 0.15f;
	int iSrt[] = {  6544,   0,      6800,   256,    7056,
					512,    5520,   768,    5776,   1024,
					6032,   1280,   6288,   1536};
	TTMA Img;
	int handle;		//file handler
	if ((handle = open(fileName.c_str(), O_RDONLY|O_BINARY)) == -1){
		return(false);
	}
	int fileLen = filelength(handle);		//file byte数
	close(handle);

	int iSub = fileLen / 2 / iMAIN;

	Img.NewDt(iSub, iMAIN, iBIT, fPIX_SIZE);
	NewDt(iSub, iASIC * iASIC_NUM, iBIT, fPIX_SIZE);

	if(Img.ReadDt(fileName) == false){
		return(false);
	}

	for(int iMxCnt = 0, iASICCnt = 0; iASICCnt < iASIC_NUM; ++iASICCnt){
		for(int iASICMCnt = 0, iMCnt = iSrt[iASICCnt]; iASICMCnt < iASIC; ++iASICMCnt, ++iMCnt, ++iMxCnt){
			for(int iSCnt = 0; iSCnt < Img.SubPixel; ++iSCnt){
				Dt[iSCnt][iMxCnt] = Img.Dt[iSCnt][iMCnt];
			}
		}
	}

	return true ;
}
//---------------------------------------------------------------------------
//FPDprint専用（HGHのイメージャ拡大DLLを使用  ttsuji追加）
//  2880x2880 -> 4320x4320(VRS) -> 3520x4280(切り出し)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <atlbase.h>
//#include "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\atlmfc\include\atlbase.h"
//#include <atlcomcli.h>
//#include "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\atlmfc\include\atlcomcli.h"

// TypeLib
//#import "FMImgVRS.dll"
//using namespace FMIMGVRSLib;
bool TSIMARS::ImagerExpansionForSIMARS(const string str)
{
#if 1
	//DLLのロード
	hDll = LoadLibrary("UNLHA32.DLL");

	if (!hDll){
		//DLLが見つからなかった場合
		CApplication::GetInstance()->MessageBox(
							"UNLHA32.DLL が見つかりませんでした。",
							"DLL エラー",
							 MB_ICONSTOP | MB_OK);
		return false;
	} else
		//DLLがあったら関数ポインタを取得する
	Unlha = (ProcUnlha)GetProcAddress(hDll, "Unlha");
#endif


	short PixBitNo = 10;
	short AllocBit = 16;
	short theInImageWidth = 2880;
	short theInImageHeight = 2880;
	unsigned long theInImageBytes = theInImageWidth * theInImageHeight * 2;

	//3/2倍
	short Deno = 2;
	short Nume = 3;
	short prm = 60;//シャープネスパラメータ60はソ画技デフォルト（HGHイメージャ拡大：シャープ130、ミディアム70、スムース30）
	short theTmpImageWidth = theInImageWidth * Nume / Deno;
	short theTmpImageHeight = theInImageHeight * Nume / Deno;
	unsigned long theTmpImageBytes = theTmpImageWidth * theTmpImageHeight * 2;

	//出力画像
	short theOutImageWidth = 3520;
	short theOutImageHeight = 4280;
	short theOutOriginX = (theTmpImageWidth - theOutImageWidth) / 2;
	short theOutOriginY = (theTmpImageHeight - theOutImageHeight) / 2;
	unsigned long theOutImageBytes = theOutImageWidth * theOutImageHeight * 2;

	// LOG
	printf("/**** Input Argument ****/\n");
/*	printf("Input File Name : %s \n", sImgName);
	printf("Output File Name: %s \n", sOutName);
	printf("PixEndian		: %d \n", PixEndian);
	printf("PixBitNo		: %d \n", PixBitNo);
	printf("AllocBit		: %d \n", AllocBit);
	printf("VRS %d/%d prm=%d\n", Nume, Deno, prm);
	printf("IN  (%dx%d)\n", theInImageWidth, theInImageHeight);
	printf("TMP (%dx%d)\n", theTmpImageWidth, theTmpImageHeight);
	printf("OUT (%dx%d) origin(%d,%d)\n", theOutImageWidth, theOutImageHeight, theOutOriginX, theOutOriginY);*/
	printf("\n");
#if 1
	// 入力
	unsigned char* pInImage = new unsigned char [theInImageBytes];
	FILE *pInFile = fopen(str.c_str(), "rb");
	if (!pInFile){
		printf("File open error: %d\n", str.c_str());
		exit(1);
	}
	fread(pInImage, 1, theInImageBytes, pInFile);
	fclose(pInFile);

	// VRS
	unsigned char* pTmpImage = new unsigned char [theTmpImageBytes];
	memset(pTmpImage,0,theTmpImageBytes);


   HMODULE hInstDLL;
   if ((hInstDLL = LoadLibrary("FMImgVRS.dll")) == NULL){
		abort();
   }
#endif



	////////////////////////////////////////////////////////////////////
	//
	// create data
	//
	////////////////////////////////////////////////////////////////////
/*	CComVariant VInImg;
	CComVariant VTmpImg;
	VInImg.vt     = (VT_BYREF | VT_UI1);
	VInImg.pbVal  = pInImage;
	VTmpImg.vt    = (VT_BYREF | VT_UI1);
	VTmpImg.pbVal = pTmpImage;

	////////////////////////////////////////////////////////////////////
	//
	// create component
	//
	////////////////////////////////////////////////////////////////////
    CoInitialize(NULL);
	IFMImgVRSCtrlPtr pVRS(__uuidof(FMImgVRSCtrl));

    ////////////////////////////////////////////////////////////////////
    //
    // set property
    //
    ////////////////////////////////////////////////////////////////////
    pVRS->PixBitNo = PixBitNo;
    pVRS->AllocBit = AllocBit;
	pVRS->PixEndian = PixEndian;

    pVRS->ImgXSize    = theInImageWidth;
    pVRS->ImgYSize    = theInImageHeight;
    pVRS->InpImgPtr   = VInImg;

	pVRS->OutImgXSize = theTmpImageWidth;
    pVRS->OutImgYSize = theTmpImageHeight;
    pVRS->OutImgPtr   = VTmpImg;

	pVRS->ColmDeno = Deno;
    pVRS->ColmNume = Nume;

    pVRS->RowDeno  = Deno;
    pVRS->RowNume  = Nume;

    pVRS->SharpPrm = prm;

    ////////////////////////////////////////////////////////////////////
	//
    // run VRS
    //
    ////////////////////////////////////////////////////////////////////
	{
      short RetCode;

      pVRS->VRSProc(&RetCode,0);

      if (RetCode < 0) {
        printf("error: RetCode = %d\n",RetCode);
        exit(1);
      }

      if (theTmpImageBytes != pVRS->OutImgSize) {
        printf("error: Tool Bug!\n");
		exit(1);
      }
    }


	FILE *pOutFile = NULL;
	unsigned char* pOutImage = new unsigned char [theOutImageBytes];
	memset(pOutImage,(1<<PixBitNo)-1,theOutImageBytes);
	short j;
	for (j=0; j<theOutImageHeight; j++) {
      memcpy(pOutImage+2*(theOutImageWidth*j), pTmpImage+2*(theTmpImageWidth*(j+theOutOriginY)+theOutOriginX), theOutImageWidth*2);
	}

	// 出力
	pOutFile = fopen(sOutName, "wb");
	fwrite(pOutImage, 1, theOutImageBytes, pOutFile);
	fclose(pOutFile);

	delete [] pInImage;
	delete [] pTmpImage;
	delete [] pOutImage;
*/
	return true;
}
//---------------------------------------------------------------------------
//ttsuji追加

TSIMARS& TSIMARS::operator =(const TIIMG& copy)
{
	if (!(DtExist && MainPixel == copy.MainPixel && SubPixel == copy.SubPixel)){
//		NewDt(copy.MainPixel, copy.SubPixel, 16);//デフォルトを16bitとする
		NewDt(copy.SubPixel, copy.MainPixel, 16);//デフォルトを16bitとする
	}
	int i,j;
	for(i = 0 ; i < SubPixel ; i++)
		for(j = 0 ; j < MainPixel ; j++)
			SetVal(i, j, copy.Dt[j][i]);

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji追加

TSIMARS& TSIMARS::operator =(const TFIMG &copy)
{
	if (!(DtExist && MainPixel == copy.MainPixel && SubPixel == copy.SubPixel)){
		NewDt(copy.SubPixel, copy.MainPixel, 16);//デフォルトを16bitとする
	}
	int i,j;
	for(i = 0 ; i < SubPixel ; i++)
		for(j = 0 ; j < MainPixel ; j++)
			SetVal(i, j, copy.Dt[j][i]);

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji追加
bool TSIMARS::ZanzouHoseiTEST()
{
	double x[5] = {0.090, 0.311, 0.442, 0.800, 1.073};
	double y[5] = {311, 1067, 1535, 2640, 3575};

	CLeastSquare test(5, x, y, _LINEAR);
	test.Calc();
	Vct v = test.GetSolve();


	double x2[10] = {5,6,7,8,9,10,11,12,13,14};
	double y2[10] = {1.375386024,1.275053913,1.02840414,0.898808496,0.719046797,0.564368125,0.418050463,0.326079361,0.392967435,0.213205736};

	CLeastSquare test2(10, x2, y2, _EXPONENTIAL);
//	CLeastSquare test2(10, x2, y2, _LINEAR);
	test2.Calc();
	Vct v2 = test2.GetSolve();

	return true;
}
//---------------------------------------------------------------------------
//ttsuji追加
bool TSIMARS::CalcTau(LAG& lag, int iSampleStart, int iSampleEnd)
{
	if (iSampleStart > lag.NumOfFrame || iSampleStart < 0) {
		return false;
	}
	if (iSampleEnd > lag.NumOfFrame || iSampleEnd < 0) {
		return false;
	}

	int NumOfSample = iSampleEnd - iSampleStart + 1;
	if (NumOfSample < 2) {
		return false;
	}
	double* SampleTime = new double[NumOfSample];
	double* SampleDose = new double[NumOfSample];

	int iROI, iFrame;
	int iNumOfROI = lag.NumOfROI_X * lag.NumOfROI_Y;
	for (iROI = 0; iROI < iNumOfROI; iROI++) {
		for (iFrame = 0; iFrame < NumOfSample; iFrame++) {
			SampleTime[iFrame] = lag.time[iSampleStart + iFrame];
//			SampleDose[iFrame] = lag.X_ROI[iROI].Dose[iFrame];//bug!!
			SampleDose[iFrame] = lag.X_ROI[iROI].Dose[iSampleStart + iFrame];
		}
#if 0
		if (iROI == 4) {
			FILE* fp = fopen("ls.txt", "wt");
			if (!fp) {
				return false;
			}
			fprintf(fp, "time	Dose\n");
			for (iFrame = 0; iFrame < NumOfSample; iFrame++) {
				fprintf(fp, "%lf	%lf\n", SampleTime[iFrame], SampleDose[iFrame]);
			}
			fclose(fp);
		}
#endif
//		CLeastSquare ls(NumOfSample, SampleTime, SampleDose, _EXPONENTIAL);
		CLeastSquare ls(NumOfSample, SampleTime, SampleDose, lag.Model);

		if (!ls.Calc()){
			delete[] SampleTime;
			delete[] SampleDose;
			return false;
		}
		Vct v = ls.GetSolve();
		lag.A[iROI] = v[1];
		lag.B[iROI] = v[0];
		lag.Coef2[iROI] = ls.GetCorCoef2();
		lag.ZanzouCoef[iROI] = exp(lag.A[iROI] * lag.DeltaTime);
	}
	delete[] SampleTime;
	delete[] SampleDose;

	return true;
}
//---------------------------------------------------------------------------
#define _A_ITPL		(0)
#define _TAU_ITPL	(1)
#define	_CALC_ORDER   _TAU_ITPL
//ttsuji追加
bool TSIMARS::CalcTauImage(LAG& lag)
{
	int i, j;
	long numOfLUTGridPoints[2] = {3, 3};
	float Grid[6];
	//ｘ軸
	Grid[0] = (float)(lag.X_ROI[0].x1 + lag.X_ROI[0].x2) / 2.;
	Grid[1] = (float)(lag.X_ROI[3].x1 + lag.X_ROI[3].x2) / 2.;
	Grid[2] = (float)(lag.X_ROI[6].x1 + lag.X_ROI[6].x2) / 2.;
	//y軸
	Grid[3] = (float)(lag.X_ROI[0].y1 + lag.X_ROI[0].y2) / 2.;
	Grid[4] = (float)(lag.X_ROI[1].y1 + lag.X_ROI[1].y2) / 2.;
	Grid[5] = (float)(lag.X_ROI[2].y1 + lag.X_ROI[2].y2) / 2.;

	int iNumOfROI = lag.NumOfROI_X * lag.NumOfROI_Y;
	float* lut = new float[iNumOfROI];//ｘのループ、ｙのループのうち、ｙのループがまず回る
	for (i = 0; i < iNumOfROI; i++) {
		lut[i] = lag.ZanzouCoef[i];
	}

	FloatLUT flut;
	flut._numOfInputChannels = 2;//x,yの２次元
	flut._numOfOutputChannels = 1;//Rfの1次元
	flut._numOfLUTGridPoints = numOfLUTGridPoints;//各軸の格子点数
	flut._lutGridPointValues = Grid;//各軸の格子点座標データ
	flut._lut = lut;//LUTデータ

	CItplCubeFloat itpl;
	if (!itpl.SetLUT(flut)){//ポインタの代入のみ
		delete[] lut;
		return false;
	}

	float* src = new float[MainPixel * SubPixel * flut._numOfInputChannels];//x,yの順
	float* dst = new float[MainPixel * SubPixel * flut._numOfOutputChannels];
	for (i = 0; i < SubPixel; i++) {
		for (j = 0; j < MainPixel; j++) {
			src[2 * (i*MainPixel + j)]     = (float)j;
			src[2 * (i*MainPixel + j) + 1] = (float)i;
		}
	}

	//規格化なしのFLOAT型画像を補間により計算
	if (!itpl.Interpolate(src, dst, MainPixel * SubPixel, true)){//ポインタの代入のみ
		delete[] lut;
		delete[] src;
		delete[] dst;
		return false;
	}

	//iNormalizeで規格化した画像を作成
//	unsigned short int iNormalize = 10000;//10000QLが1倍に相当
// 	unsigned short int iNormalize = 16384;//16384QLが1倍に相当
	unsigned short int iNormalize =  8192;// 8192QLが1倍に相当
	Copy(dst, iNormalize, MainPixel, SubPixel, 16);

	delete[] lut;
	delete[] src;
	delete[] dst;

	return true;
}
//---------------------------------------------------------------------------
//ttsuji追加
bool TSIMARS::ItplTEST()
{
	long numOfLUTGridPoints[2] = {3, 3};
	float Grid[6] = {0,1440,2879,500,1440,2879};//2880×2880のうち各軸3点
	float lut[9] = {0.30, 0.28, 0.35,
					0.39, 0.45, 0.32,
					0.35, 0.20, 0.30};

	FloatLUT flut;
	flut._numOfInputChannels = 2;//x,yの２次元
	flut._numOfOutputChannels = 1;//Rfの1次元
	flut._numOfLUTGridPoints = numOfLUTGridPoints;//各軸の格子点数
	flut._lutGridPointValues = Grid;//各軸の格子点座標データ
	flut._lut = lut;//LUTデータ

	CItplCubeFloat itpl;
	if (!itpl.SetLUT(flut)){//ポインタの代入のみ
		return false;
	}

	int MainPixel = 2880;
	int SubPixel = 2880;

	float* src = new float[MainPixel * SubPixel * flut._numOfInputChannels];
	float* dst = new float[MainPixel * SubPixel * flut._numOfOutputChannels];
	int i, j, k;
	for (i = 0; i < SubPixel; i++) {
		for (j = 0; j < MainPixel; j++) {
/*
			for (k = 0; k < flut._numOfInputChannels; k++) {
				src[(i*MainPixel + j) * flut._numOfInputChannels + k] = (float)(i*MainPixel + j);
			}*/
			src[2 * (i*MainPixel + j)]     = (float)j;
			src[2 * (i*MainPixel + j) + 1] = (float)i;
		}
	}

	float* ptr = src + (int)(2880 * 1440.5 * 2 + 0.5);
	if (!itpl.Interpolate(ptr, dst, 1, false)){//ポインタの代入のみ
		delete[] src;
		delete[] dst;
		return false;
	}
	if (!itpl.Interpolate(src, dst, MainPixel * SubPixel, false)){//ポインタの代入のみ
		delete[] src;
		delete[] dst;
		return false;
	}
	TSIMARS tmp;
	unsigned short int iNormalize = 10000;
	tmp.Copy(dst, iNormalize, MainPixel, SubPixel, 16);
	tmp.SaveTmaTdt("J:\\develop4\\test.tma");


	delete[] src;
	delete[] dst;


	return true;
}
//---------------------------------------------------------------------------
//2880×2880,10bit以外は未サポート(ttsuji追加)
bool TSIMARS::Raw2Fcr(string str)
{
	if (!ReadTma(str)) {
		CApplication::GetInstance()->MessageBox((str + " readできませんでした").c_str(), "Error", MB_OK);
		return false;
	}
	if (10 != Bit) {
		CApplication::GetInstance()->MessageBox("10ビット画像を入力してください", "Error", MB_OK);
		return false;
	}
	if (2880 != MainPixel || 2880 != SubPixel) {
		CApplication::GetInstance()->MessageBox("2880×2880の画像を入力してください", "Error", MB_OK);
		return false;
	}
	//エンディアン変更
	LittleBig();


	//INIファイルから読み込む
	string strAppPath = ExtractFilePath(CApplication::GetInstance()->ExeName);
	string strIniPath;
	strIniPath = strAppPath + "ImageTool.ini";
	TIniFile *pini;
	pini = new TIniFile(strIniPath);
	string dir = pini->ReadString("SIMARSCorrection", "SIMARS_ParamPath", "C:");
	delete pini;
	pini = NULL;

	//fcrヘッダファイル読出し
//	string fn_fcrheader = "J:\\develop4\\ImageProcessingTools\\ESTool\\2880_2880_10bit.fcrheader";
	string fn_fcrheader = dir + "\\2880_2880_10bit.fcrheader";
	ifstream inFile(fn_fcrheader.c_str(), ios::in | ios::binary);
	if(!inFile){
		CApplication::GetInstance()->MessageBox((fn_fcrheader + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}
	long head_size = 2048;//固定
	unsigned char* buffer = new unsigned char [head_size];
	inFile.read((unsigned char*)buffer, head_size);
	if (inFile.bad()){
		CApplication::GetInstance()->MessageBox((fn_fcrheader + " readに失敗しました").c_str(), "Error", MB_OK);
		delete[] buffer;
		inFile.close();
		return false;
	}
	inFile.close();

	//raw→fcr変換後の画像
	string fn_fcr = ChangeFileExt(str, ".fcr");;
#if 0
	ofstream outFile(fn_fcr.c_str(), ios::out | ios::binary);
	outFile.write((unsigned char*)buffer, head_size);
	delete[] buffer;

	if (outFile.bad()){
		CApplication::GetInstance()->MessageBox((fn_fcr + " writeできませんでした").c_str(), "", MB_OK);
		outFile.close();
		return false;
	}
	for(int i = 0 ; i < SubPixel ; i++){
		outFile.write(Dt[i], MainPixel * 2);
		if (outFile.bad()){
			CApplication::GetInstance()->MessageBox((fn_fcr + " writeできませんでした").c_str(), "", MB_OK);
			outFile.close();
			return(false);
		}
	}
	outFile.close();

#else
	FILE* fp = fopen(fn_fcr.c_str(), "wb");
	if (!fp) {
		CApplication::GetInstance()->MessageBox((fn_fcr + " openできませんでした").c_str(), "", MB_OK);
		delete[] buffer;
		return false;
	}

	fwrite(buffer, sizeof(unsigned char), head_size, fp);
	delete[] buffer;

	for (int i = 0 ; i < SubPixel ; i++){
		size_t size = fwrite(Dt[i], sizeof(unsigned short int), MainPixel, fp);
		if (size < (unsigned int)MainPixel) {
			CApplication::GetInstance()->MessageBox((fn_fcr + " writeできませんでした").c_str(), "", MB_OK);
			fclose(fp);
			return false;
		}
	}
	fclose(fp);
#endif

	return true;
}
//---------------------------------------------------------------------------
//オフセット自動更新エラー判定における衝撃判定（ttsuji）
bool TSIMARS::CalcShockJudge(int iUpper_thre , int iLower_thre, int iNumThre, int& iUpper_count, int& iLower_count, int* iQLUpper, int* iXUpper, int* iYUpper, int* iQLLower, int* iXLower, int* iYLower, int& iQLmin, int& iQLmax, bool bExcludeEndPixel)
{
	int size = MainPixel*SubPixel;

	iUpper_count = 0;
	iLower_count = 0;

	if( iUpper_thre > DtMax ) iUpper_thre = DtMax;
	else if( iUpper_thre < 0 )  iUpper_thre = 0;

	if( iLower_thre > DtMax ) iLower_thre = DtMax;
	else if( iLower_thre < 0 )  iLower_thre = 0;

	int iMCnt, iSCnt;
	int iPixExclude = (bExcludeEndPixel) ? 2 : 0;//端部から除外するpix数

	for(iSCnt = iPixExclude; iSCnt < SubPixel - iPixExclude; iSCnt++){
		for(iMCnt = iPixExclude; iMCnt < MainPixel - iPixExclude; iMCnt++){
			if( Dt[iSCnt][iMCnt] >= iUpper_thre ){
				if (iUpper_count < 100) {
					iXUpper[iUpper_count] = iMCnt;
					iYUpper[iUpper_count] = iSCnt;
					iQLUpper[iUpper_count] = Dt[iSCnt][iMCnt];
				}
				iUpper_count++;
			}
			if( Dt[iSCnt][iMCnt] <= iLower_thre ){
				if (iLower_count < 100) {
					iXLower[iLower_count] = iMCnt;
					iYLower[iLower_count] = iSCnt;
					iQLLower[iLower_count] = Dt[iSCnt][iMCnt];
				}
				iLower_count++;
			}
			if (iQLmin > Dt[iSCnt][iMCnt]) {
				iQLmin = Dt[iSCnt][iMCnt];
			}
			if (iQLmax < Dt[iSCnt][iMCnt]) {
				iQLmax = Dt[iSCnt][iMCnt];
			}

		}
	}


	if (iUpper_count > iNumThre || iLower_count > iNumThre){
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
//オフセット自動更新エラー判定における散乱線判定(ttsuji)
bool TSIMARS::CalcScatteredXray(int iUpper_thre , int iLower_thre, int iROI_Length, double* dQLscatter, bool* bOK)
{
	int size = MainPixel*SubPixel;
	int hcount = 0, vcount = 0;
	int denom = iROI_Length*iROI_Length;

	int hcount0 = MainPixel * 0.25 - iROI_Length/2;
	int hcount1 = MainPixel * 0.25 + iROI_Length/2;
	int hcount2 = MainPixel * 0.5 - iROI_Length/2;;
	int hcount3 = MainPixel * 0.5 + iROI_Length/2;
	int hcount4 = MainPixel * 0.75 - iROI_Length/2;
	int hcount5 = MainPixel * 0.75 + iROI_Length/2;
	int vcount0 = SubPixel * 0.25 - iROI_Length/2;
	int vcount1 = SubPixel * 0.25 + iROI_Length/2;
	int vcount2 = SubPixel * 0.5 - iROI_Length/2;
	int vcount3 = SubPixel * 0.5 + iROI_Length/2;
	int vcount4 = SubPixel * 0.75 - iROI_Length/2;
	int vcount5 = SubPixel * 0.75 + iROI_Length/2;


	for(int iS = 0 ; iS < SubPixel ; iS++) {
		for(int iM = 0 ; iM < MainPixel ; iM++ ) {
			if ((iS >= vcount0) & (iS < vcount1) ) {
				if ((iM >= hcount0) & (iM < hcount1) ){
					dQLscatter[0] += Dt[iS][iM];
				}else if ( (iM >= hcount2) & (iM < hcount3) ){
					dQLscatter[1] += Dt[iS][iM];
				}else if ( (iM >= hcount4) & (iM < hcount5) ){
					dQLscatter[2] += Dt[iS][iM];
				}
			} else if ( (iS >= vcount2) & (iS < vcount3) ) {
				if ((iM >= hcount0) & (iM < hcount1) ){
					dQLscatter[3] += Dt[iS][iM];
				}else if ( (iM >= hcount2) & (iM < hcount3) ){
					dQLscatter[4] += Dt[iS][iM];
				}else if ( (iM >= hcount4) & (iM < hcount5) ){
					dQLscatter[5] += Dt[iS][iM];
				}
			} else if ( (iS >= vcount4) & (iS < vcount5) ) {
				if ((iM >= hcount0) & (iM < hcount1) ){
					dQLscatter[6] += Dt[iS][iM];
				}else if ( (iM >= hcount2) & (iM < hcount3) ){
					dQLscatter[7] += Dt[iS][iM];
				}else if ( (iM >= hcount4) & (iM < hcount5) ){
					dQLscatter[8] += Dt[iS][iM];
				}
			}
		}
	}

	int i;
	for (i = 0; i < 9; i++) {
		dQLscatter[i] /= denom;
	}

	for (i = 0; i < 9; i++) {
		if ((dQLscatter[i] <= iLower_thre) | (dQLscatter[i] >= iUpper_thre) ) {//散乱線あり
			bOK[i] = false;
		} else{
			bOK[i] = true;
		}

	}
	bool bResult = true;
	for (i = 0; i < 9; i++) {
		bResult &= bOK[i];
	}

	return bResult;
}
//---------------------------------------------------------------------------
bool TSIMARS::CalcEMC(int* iHist, int& iHistBroad, int iEmc_min_thre , int iEmc_broad_thre, int iKasa)
{
	const int	iOffset_addr = 1024;
	const int   size = MainPixel*SubPixel;
	int			iHist_addr = 0;
	int			iHist_dist_addr = 0;
	int			iHist_calc_count = 0;
	int 		iQLAve = Average() - iKasa + iOffset_addr;//平均画素値
	bool		bOK = true;

	for (int iS = 0 ; iS < SubPixel ; iS++) {
		for (int iM = 0 ; iM < MainPixel ; iM++) {
			iHist_addr = Dt[iS][iM] - iKasa + iOffset_addr;
			iHist_addr = CLIP(iHist_addr, 0, 2*iOffset_addr - 1);
			iHist[iHist_addr] += 1;
		}
	}


	for (int i = 0 ; i < 2*iOffset_addr; i++){
		if ((iHist_calc_count < iEmc_min_thre) & ((iHist_calc_count + iHist[i] ) >= iEmc_min_thre)){
			iHist_dist_addr = i;
			break;
		}
		iHist_calc_count += iHist[i];
	}

	iHistBroad = iQLAve - iHist_dist_addr;//ヒストグラムの広がり具合
	if (iHistBroad > iEmc_broad_thre)	bOK = false;//ヒストグラムが広がっている場合エラー
	else  							    bOK = true;

	return bOK;
}
//---------------------------------------------------------------------------
/*
	欠陥補正6b
		CTFチャート領域は周囲の正常画素のエッジ検出してエッジ方向に沿って補正   //210220 ttsuji

string str_img			//input		被補正画像のパス
string str_coord		//input		線欠陥アドレスを記載したテキストファイルのパス
bool bAutoDetect			//input		グリッド有無と向きの自動検出フラグ
int& iGridDirection			//output	グリッドの有無と向き
float& fHonsuu				//output	グリッド本数
double& dA1					//output	第1隣接画素の補正ウェイト
double& dA2					//output	第2隣接画素の補正ウェイト
double& dA3					//output	第3隣接画素の補正ウェイト
double& dA4					//output	第4隣接画素の補正ウェイト
double dTh					//input		とげ判定の閾値
bool bPreCor2Line			//input		仮埋めフラグ

*/

bool TSIMARS::bDefectCorrection6b(string str_img, string str_coord, bool bAutoDetect, int& iGridDirection, float& fHonsuu, double& dA1, double& dA2, double& dA3, double& dA4, double dTh, bool bPreCor2Line, int iIndexEdgeDetect)
{
	TTMA Image, outImage;
	const int iHashi = 3;   // これ以上遠くからは補間しない。


	/*        設定読み込み             */
	fstream fcoord;                                 // 入力座標データ
	fcoord.open( str_coord.c_str(), ios::in );
	if( fcoord.fail() ) {
		CApplication::GetInstance()->MessageBox((str_coord + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}

	// 設定読み込み
	int iSet; fcoord >> iSet;                       // 行数
	int *iOn; iOn = new int[iSet];                  // 縦・横・無視
	int *iLine; iLine = new int[iSet];                  // 座標
	int *iWid; iWid = new int[iSet];                  // 座標

	for( int i=0; i<iSet; i++){
		fcoord >> iOn[i] >> iLine[i] >> iWid[i];
	}
	fcoord.close();

	string Prefix = "GL";
	/**********   Prefix Section  **********/
	int iMs, iMw;  // 何本横から平均初めて何画素までか
	double a, b1, b2, b3, b4;
	double iThres;
	Prefix += "Lin";
	iMs=1;
	iMw=4;
	b1=dA1;
	b2=dA2;
	b3=dA3;
	b4=dA4;
	iThres=dTh;

	Prefix += "R0";

	/**************   補間    **************/
	int iRandom0[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int iRandom1[15] = {1,0,-1,0,1,-1,1,0,0,1,-1,0,-1,1,-1};
	int iRandom2[15] = {0,1,0,-2,2,1,0,-2,-1,2,-1,1,-2,-1,2};
	int *iRandom;
	int iR =0;
//	int iSHashi=0;
//	int iSHashi=1;
	int iSHashi=2;//DifPixelsU,Dの導入により
	iRandom = iRandom0;


	bool Yoko = false;
	int  iGridDirectionByFFT = _GridHor;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
	if( bAutoDetect == true ){
		float hon;
		GridLineDefectF->DetectGridPeak( str_img, hon, iGridDirectionByFFT );

		if( hon>=30.05 && hon <36.65){  // A1.0
			b1=0;  b2=1;  b3=0;  b4=0;
		} else if( hon>=36.65 && hon <37.95){  //A1.25
			b1=0;  b2=1.25;  b3=-0.25;  b4=0;
		} else if( hon>=37.95 && hon <38.55){  //A1.5
			b1=0;  b2=1.5;  b3=-0.5;  b4=0;
		} else if( hon>=38.55 && hon <38.85){  //A1.75
			b1=0;  b2=1.75;  b3=-0.75;  b4=0;
		} else if( hon>=38.85 && hon <39.15){  //A2.0
			b1=0;  b2=2;  b3=-1;  b4=0;
		} else if( hon>=39.15 && hon <40.75){  //B
			b1=0;  b2=1;  b3=0.7;  b4=-0.7;
		} else if( hon>=40.75 && hon <41.25){  //C1.6
			b1=0.5;  b2=-0.5;  b3=1.6;  b4=-0.6;
		} else if( hon>=41.25 && hon <41.75){  //C1.45
			b1=0.5;  b2=-0.5;  b3=1.45;  b4=-0.45;
		} else if( hon>=41.75 && hon <42.55){  //C1.3
			b1=0.5;  b2=-0.5;  b3=1.3;  b4=-0.3;
		} else if( hon>=42.55 && hon <43.75){  //C1.15
			b1=0.5;  b2=-0.5;  b3=1.15;  b4=-0.15;
		} else if( hon>=43.75 && hon <47.75){  //C1.0
			b1=0.5;  b2=-0.5;  b3=1;  b4=0;
		} else if( hon>=47.75 && hon <50.65){  //C0.5
			b1=0.5;  b2=-0.5;  b3=0.5;  b4=0.5;
		} else if( hon>=50.65 && hon <52.95){  //D1.8
			b1=1.8;  b2=-0.8;  b3=0;  b4=0;
		} else if( hon>=52.95 && hon <57.05){  //D1.5
			b1=1.5;  b2=-0.5;  b3=0;  b4=0;
		} else if( hon>=57.05 && hon <66.75){  //D1.4
			b1=1.4;  b2=-0.4;  b3=0;  b4=0;
		} else {  //else
			delete[] iOn;
			delete[] iLine;
			delete[] iWid;
			return false;
		}

		if(_GridVer == iGridDirectionByFFT){//縦グリッド
			Yoko = false;
		} else if (_GridHor == iGridDirectionByFFT){//横グリッド
			Yoko = true;
		} else if (_GridNon == iGridDirectionByFFT){//グリッド検出せず
			//グリッド検出できない場合は隣接画素で補間する
			b1 = 1.0;
			b2 = b3 = b4 = 0.0;
		} else {//エラー
			delete[] iOn;
			delete[] iLine;
			delete[] iWid;
			return false;
		}
		fHonsuu = hon;


		switch (iGridDirectionByFFT) {
		case _GridVer:
			iGridDirection = 0;
			break;
		case _GridHor:
			iGridDirection = 1;
			break;
		case _GridNon:
			iGridDirection = 2;
			break;
		}
	}
	else{//グリッド本数手動指定のときにはグリッド向きも合わせて指定（120807 ttsuji修正）
		Yoko = (bool)iGridDirection;

		switch (iGridDirection){ //ImageToolのGUI(0:V方向、1:H方向、2:無)
		case 0:
			iGridDirectionByFFT = _GridVer;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
			break;
		case 1:
			iGridDirectionByFFT = _GridHor;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
			break;
		case 2:
			iGridDirectionByFFT = _GridNon;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
			break;
		}
	}


	Image = *this;
	outImage = *this;

	const int iToge = 3;

	//エッジ検出用
	DIFFERENTIALARROUNDPIXELS DifPixels[2];//線欠陥の左右それぞれの3x3マスクのエッジ
	DIFFERENTIALARROUNDPIXELS DifPixelsU[2];//線欠陥の下方に1pixシフトさせた拡張ROIのエッジ（[0]は線欠陥をはさんで左側,[1]は右側）
	DIFFERENTIALARROUNDPIXELS DifPixelsD[2];//線欠陥の上方に1pixシフトさせた拡張ROIのエッジ（[0]は線欠陥をはさんで左側,[1]は右側）
	DIFFERENTIALARROUNDPIXELS DifPixelsExp[2];//線欠陥の左右それぞれの3x3マスクを上下に1pixシフトさせたROIも含めたエッジ

	const int iMaskSizeEdgeDetect = 3;
	const int iHalfMaskSizeEdgeDetect = iMaskSizeEdgeDetect / 2;//3x3マスクサイズの半分
	const int iQLOffset = 10000;

	//エッジ画像
	TTMA ImageUp2Down = *this;
	TTMA ImageLeft2Right = *this;
	TTMA ImageUpLeft2DownRight = *this;
	TTMA ImageUpRight2DownLeft = *this;
	ImageUp2Down.FillValue(iQLOffset);
	ImageLeft2Right.FillValue(iQLOffset);
	ImageUpLeft2DownRight.FillValue(iQLOffset);
	ImageUpRight2DownLeft.FillValue(iQLOffset);

	//LOG変換
	const double dMidVal = 948.;
	const double dL = 4.0;
	const int iTh = 60000;
	const int iBit = 14;
//	const double dThEnergy = 50.;//初版
	const double dThEnergy = 25.;//4.0cyc/mmのエッジ検出対策
	const int iHalfMaskItpl = 2;
	TSIMARS LogImage = (TSIMARS)*this;
	LogImage.LogThreshold(dMidVal, dL, iTh);
	LogImage.BitChange(iBit);
	LogImage.WriteTma( AddPrefix(str_img, "BL"), false);

	//ラプラシアンフィルタ（tmp）
	TSIMARS ImgLaplacian = (TSIMARS)LogImage;
	ImgLaplacian.FillValue(iQLOffset);
	ImgLaplacian.CalcLaplacianFilter(iQLOffset);
	//debug
	ImgLaplacian.WriteTma( AddPrefix(str_img, "LaplacianBL"), false);


	//CTFチャートのパターン領域と縞の配置角度を検出する
	TSIMARS CtfPatternImage = LogImage;
	double dCtfStripesAngle;//CTFチャートの縞パターンの配置角度
//	const int iThQLCtfExist = 10000;//CTFチャートが存在するとみなすQL閾値
	const int iThQLCtfExist = 16000;//CTFチャートが存在するとみなすQL閾値
	if (!CtfPatternImage.DetectionCTFChartPattern(str_img, dCtfStripesAngle)){
		delete[] iOn;
		delete[] iLine;
		delete[] iWid;
		return false;
	}


	for( int iL=0; iL<iSet; iL++){
		//すべて縦線欠陥として扱うために、横線欠陥の場合は主副反転させておく（ttsuji）
		if(_LineDefectHor == iOn[iL]) {//線欠陥が横であれば主副反転する
			Image.MainSub();
			outImage.MainSub();
		}

		//120807 ttsuji修正
		double b1tmp(100.), b2tmp(100.), b3tmp(100.), b4tmp(100.);
		double dLeft=0, dRight=0;
		int iM = iLine[iL];//縦線欠陥のｘ座標
		int iLeft=0, iRight=0, iLeft0=0, iRight0=0;

#if 0
		//画像端の線欠陥は処理しない（ttsuji）
		if( iLine[iL] < iHashi || iLine[iL]+iWid[iL]-1 >= Image.MainPixel - iHashi ) {
			if(_LineDefectHor == iOn[iL]) {//線欠陥が横であれば主副反転する
				Image.MainSub();
				outImage.MainSub();
			}
			continue;//主副反転を元に戻してから次の欠陥に移る（ttsuji）
		}
#endif

		if ( iWid[iL] == 1 ){
//			if ((_LineDefectVer == iOn[iL]) ^ (_GridHor == iGridDirectionByFFT)) {//線欠陥が縦とグリッドが横の排他的論理和（グリッドと線欠陥が平行の場合）（←bug! （横グリッドの偽は縦のみならずグリッドなしも含まれるため））
//			if ((_LineDefectVer == iOn[iL]) & (_GridVer == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま
			if ((_LineDefectVer == iOn[iL]) & (_GridVer == iGridDirectionByFFT) ||
				(_LineDefectHor == iOn[iL]) & (_GridHor == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji

				;
			}
			else{//線欠陥とグリッドが直交する場合、もしくはグリッドが検出されなかった場合
				b1tmp = b1;
				b2tmp = b2;
				b3tmp = b3;
				b4tmp = b4;
				//両隣の画素で補間
				b1 = 1.0;
				b2 = b3 = b4 = 0.0;
			}

//			int iM = iLine[iL];
//			int iLeft0=0, iRight0=0;

			//すべて縦線欠陥として扱う（横線欠陥の場合は主副反転させておく）（ttsuji）
			for( int iS=iSHashi; iS<Image.SubPixel-iSHashi; iS++){
				//画像端に線欠陥がある場合
				if (0 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM+1]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM+1]);//隣接する片側の画素値をコピーする
				}
				else if (Image.MainPixel - 1 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM-1]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM-1]);//隣接する片側の画素値をコピーする
				}
				//画像端以外に線欠陥がある場合  (iMs = 1)
				else{
				//ここに新規コードをかく（線欠陥の左右それぞれに3x3マスクでエッジ検出 → 5x5で補間）
 #if 0
					//DifPixelsの調査
					if (1380 == iM) { //Image_20210216-0030
						if (1270 == iS) {//フラットな領域
							int temp = 0;
						}
						else if (1300 == iS) {//チャートの3.0cyc/mm縞領域
							int temp = 0;
						}
					}
					if (1340 == iM) { //Image_20210216-0030
						if (1299 == iS) {//チャートの4.0cyc/mm縞領域
							int temp = 0;
						}
						else if (1308 == iS) {//チャートの4.0cyc/mm縞領域
							int temp = 0;
						}
					}
					if (1480 == iM) { //Image_20210216-0025
						if (1485 == iS) {//チャートの4.0cyc/mm縞領域
							int temp = 0;
						}
						else if (1508 == iS) {//チャートの4.0cyc/mm縞領域
							int temp = 0;
						}
					}
					if (1340 == iM) { //Image_20210216-0025
						if (1360 == iS) {//チャートの2.5cyc/mm縞領域
//						if (1355 == iS) {//チャートの2.5cyc/mm縞領域
							int temp = 0;
						}
						else if (1392 == iS) {//チャートの2.5cyc/mm縞領域
							int temp = 0;
						}
					}
					if (1360 == iM) { //Image_20210216-0025
						if (1314 == iS) {//チャートの3cyc/mm縞領域
							int temp = 0;
						}
						else if (1317 == iS) {//チャートの3cyc/mm縞領域
							int temp = 0;
						}
					}
					if (1440 == iM) { //Image_20210216-0025
						if (1313 == iS) {//チャートの3.0cyc/mm縞領域
							int temp = 0;
						}
					}
					if (1404 == iM) { //Image_20210216-0027
						if (1357 == iS) {//チャートの2.5cyc/mm縞領域
							int temp = 0;
						}
						else if (1364 == iS) {//チャートの2.5cyc/mm縞領域
							int temp = 0;
						}
						else if (1319 == iS) {//チャートの3.0cyc/mm縞領域
							int temp = 0;
						}
						else if (1327 == iS) {//チャートの3.0cyc/mm縞領域
							int temp = 0;
						}
					}
 #endif

					//補正対象画素がCTFチャートの縞の領域内にある場合
					if (CtfPatternImage.Dt[iS][iM] >= iThQLCtfExist)
					{
						double dOrientWeightedByEnergy;//左右のEnergyの重みで計算したエッジ方向
						if (_USING_CTF_AREA == iIndexEdgeDetect)
						{
							dOrientWeightedByEnergy = dCtfStripesAngle;//CTFチャートの領域検出の結果から求めた角度とする
						}
						else if (_USING_SOBEL_FILTER == iIndexEdgeDetect)
						{
							//線欠陥の左右の正常画素3x3マスクでそれぞれエッジ検出
							double dEnergyLRAve = 0.0;
							for (int iMSide = 0; iMSide <= 1; iMSide++)
							{
								int iParity = (0 == iMSide) ? -1 : 1;
								DifPixels[iMSide] = CalcDifferentialArroundPixel_Fast(LogImage, iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1));//線欠陥の左右のROIにおけるエッジ情報を計算する
								DifPixelsU[iMSide] = CalcDifferentialArroundPixel_Fast(LogImage, iS - 1, iM + iParity * (iHalfMaskSizeEdgeDetect + 1));//線欠陥の上方1pixの左右のROIにおけるエッジ情報を計算する
								DifPixelsD[iMSide] = CalcDifferentialArroundPixel_Fast(LogImage, iS + 1, iM + iParity * (iHalfMaskSizeEdgeDetect + 1));//線欠陥の下方1pixの左右のROIにおけるエッジ情報を計算する
 #if 0
								dEnergyLRAve += DifPixels[iMSide].dEnergy;
 #else
								//線欠陥補正対象画素の左右片側ROIにおける上下±1pixシフトさせたROIを含めたEnergyとエッジの方向を計算
								DifPixelsExp[iMSide] = GetOrientWeightedByEnergy(DifPixels[iMSide], DifPixelsU[iMSide], DifPixelsD[iMSide]);
								dEnergyLRAve += DifPixelsExp[iMSide].dEnergy;
 #endif

								//debug
								ImageUp2Down.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].Up2Down.dDiff + iQLOffset);
								ImageLeft2Right.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].Left2Right.dDiff + iQLOffset);
								ImageUpLeft2DownRight.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].UpLeft2DownRight.dDiff + iQLOffset);
								ImageUpRight2DownLeft.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].UpRight2DownLeft.dDiff + iQLOffset);
							}
							dEnergyLRAve /= 2.0;







						}

					}
					else{//補正対象画素がCTFチャートの縞の領域外の場合
						dLeft =   b1/2.*Image.Dt[iS][iM-iMs]
								+ b2/2.*Image.Dt[iS][iM-iMs-1]
								+ b3/2.*Image.Dt[iS][iM-iMs-2]
								+ b4/2.*Image.Dt[iS][iM-iMs-3];
						dRight =  b1/2.*Image.Dt[iS][iM+iMs]
								+ b2/2.*Image.Dt[iS][iM+iMs+1]
								+ b3/2.*Image.Dt[iS][iM+iMs+2]
								+ b4/2.*Image.Dt[iS][iM+iMs+3];
						iLeft0 = Image.Dt[iS][iM-iToge];
						iRight0 = Image.Dt[iS][iM+iToge];

						//トゲ判定
						if (iLeft0*iRight0 == 0 || double(iLeft0)/iRight0 > iThres || double(iRight0)/iLeft0 > iThres){
							dLeft = Image.Dt[iS+iRandom[iR++%15]][iM-iMs]/2;
							dRight = Image.Dt[iS+iRandom[iR++%15]][iM+iMs]/2;
						}
						outImage.SetVal(iS, iM , int(dLeft + dRight) );
					}

/*
					//線欠陥の左右の正常画素3x3マスクでそれぞれエッジ検出
					double dEnergyLRAve = 0.0;
					for (int iMSide = 0; iMSide <= 1; iMSide++) {
						int iParity = (0 == iMSide) ? -1 : 1;
						DifPixels[iMSide] = CalcDifferentialArroundPixel_Fast(LogImage, iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1));//線欠陥の左右のROIにおけるエッジ情報を計算する
						DifPixelsU[iMSide] = CalcDifferentialArroundPixel_Fast(LogImage, iS - 1, iM + iParity * (iHalfMaskSizeEdgeDetect + 1));//線欠陥の上方1pixの左右のROIにおけるエッジ情報を計算する
						DifPixelsD[iMSide] = CalcDifferentialArroundPixel_Fast(LogImage, iS + 1, iM + iParity * (iHalfMaskSizeEdgeDetect + 1));//線欠陥の下方1pixの左右のROIにおけるエッジ情報を計算する
 #if 0
						dEnergyLRAve += DifPixels[iMSide].dEnergy;
 #else
						//線欠陥補正対象画素の左右片側ROIにおける上下±1pixシフトさせたROIを含めたEnergyとエッジの方向を計算
                        DifPixelsExp[iMSide] = GetOrientWeightedByEnergy(DifPixels[iMSide], DifPixelsU[iMSide], DifPixelsD[iMSide]);
						dEnergyLRAve += DifPixelsExp[iMSide].dEnergy;
 #endif

						//debug
						ImageUp2Down.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].Up2Down.dDiff + iQLOffset);
						ImageLeft2Right.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].Left2Right.dDiff + iQLOffset);
						ImageUpLeft2DownRight.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].UpLeft2DownRight.dDiff + iQLOffset);
						ImageUpRight2DownLeft.SetVal(iS, iM + iParity * (iHalfMaskSizeEdgeDetect + 1), DifPixels[iMSide].UpRight2DownLeft.dDiff + iQLOffset);
					}
					dEnergyLRAve /= 2.0;


					//エッジ情報から補間方式を選別
 #if 0
					if (dEnergyLRAve >= dThEnergy) {//Energy閾値よりも大きい場合はエッジ方向に沿った補間
 #else
					if (CtfPatternImage.Dt[iS][iM] >= iThQLCtfExist) {//CTFチャートの縞の領域合は縞の方向に沿った補間
 #endif
						//DifPixels.Orientation の方向に近い画素ほどウェイトを重くした2次元フィルター（任意方向に対応）
						double dSumWeight =0.0;
						double dSum = 0.0;
						double dWeight, dCurOrient, dCos;
						double dDistance;//補正対象画素と参照画素との距離
						double dDistance2;//補正対象画素と参照画素との距離2乗分の１
						double dOrientWeightedByEnergy;//左右のEnergyの重みで計算したエッジ方向
						for (int iSSmth = -iHalfMaskItpl; iSSmth <= iHalfMaskItpl; iSSmth++) {
							for (int iMSmth = -iHalfMaskItpl; iMSmth <= iHalfMaskItpl; iMSmth++) {
								if (0 == iMSmth) {//線欠陥上の画素は補間に用いない
									continue;
								}
								else{//線欠陥上の画素以外
									dCurOrient = atan((double)iSSmth / iMSmth) * 180 / PI;
 #if 0   	//左右それぞれのOrientに応じてCos計算
									if (iMSmth < 0) {//線欠陥に対してx原点側の場合
										dCos = cos((dCurOrient - DifPixels[0].dOrientation) * PI / 180);
									}
									else{//線欠陥に対してx終端側の場合
										dCos = cos((dCurOrient - DifPixels[1].dOrientation) * PI / 180);
									}
 #else    	//Orientは左右のEnergyの重みで計算
  #if 0
//									dOrientWeightedByEnergy = (DifPixels[0].dOrientation * DifPixels[0].dEnergy + DifPixels[1].dOrientation * DifPixels[1].dEnergy) / (DifPixels[0].dEnergy + DifPixels[1].dEnergy);
//									dOrientWeightedByEnergy = GetOrientWeightedByEnergy(DifPixels);
									dOrientWeightedByEnergy = GetOrientWeightedByEnergy(DifPixelsExp);
  #elif 1
									//CTFチャートの領域検出の結果から求めた角度とする
									dOrientWeightedByEnergy = dCtfStripesAngle;
  #else
									//決め打ちの角度として計算してみる
   #if 0
									dOrientWeightedByEnergy = 180.;//Image_20210216-0025
   #elif 0
									dOrientWeightedByEnergy = 172.;//Image_20210216-0026
   #elif 0
									dOrientWeightedByEnergy = 164.;//Image_20210216-0027
   #elif 0
									dOrientWeightedByEnergy = 153.;//Image_20210216-0028
   #elif 0
									dOrientWeightedByEnergy = 142.;//Image_20210216-0029
   #elif 0
									dOrientWeightedByEnergy = 129.;//Image_20210216-0030
   #elif 0
									dOrientWeightedByEnergy = 117.;//Image_20210216-0031
   #elif 0
									dOrientWeightedByEnergy = 107.;//Image_20210216-0032
   #elif 0
									dOrientWeightedByEnergy =  98.;//Image_20210216-0033
   #else
									dOrientWeightedByEnergy =  90.;//Image_20210216-0034
   #endif
  #endif
									dCos = cos((dCurOrient - dOrientWeightedByEnergy) * PI / 180);
 #endif
									dDistance = sqrt((float)iSSmth * iSSmth + iMSmth * iMSmth);

									//距離に関する重み
 #if 0
									dDistance2 = 1 / dDistance / dDistance;//距離の-2乗
 #elif 0
									dDistance2 = 1 / fabs(dDistance);//距離の-1乗
 #else
									dDistance2 = 1.0;//距離の0乗(重みなし)
 #endif

									//周囲のエッジ方向の射影に関する重み
 #if 0
									dWeight = dCos * dCos * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #elif 0
									dWeight = pow(dCos, 4.0) * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #elif 0
									dWeight = pow(dCos, 6.0) * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #elif 0
									dWeight = pow(dCos, 10.0) * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #elif 0
									dWeight = pow(dCos, 20.0) * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #elif 0
									dWeight = pow(dCos, 40.0) * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #else
									dWeight = pow(dCos, 80.0) * dDistance2;//周囲のエッジ方向と異なる方向の重みを抑えてボケを少なくする
 #endif
									dSum += Dt[iS + iSSmth][iM + iMSmth] * dWeight;
									dSumWeight += dWeight;
								}
							}
						}
//						Dt[iS][iM] = (unsigned short int)(dSum / dSumWeight);
						outImage.SetVal(iS, iM , (unsigned short int)(dSum / dSumWeight));

					}
					else{//Energy閾値よりも小さい場合は従来タイプの補間
						dLeft =   b1/2.*Image.Dt[iS][iM-iMs]
								+ b2/2.*Image.Dt[iS][iM-iMs-1]
								+ b3/2.*Image.Dt[iS][iM-iMs-2]
								+ b4/2.*Image.Dt[iS][iM-iMs-3];
						dRight =  b1/2.*Image.Dt[iS][iM+iMs]
								+ b2/2.*Image.Dt[iS][iM+iMs+1]
								+ b3/2.*Image.Dt[iS][iM+iMs+2]
								+ b4/2.*Image.Dt[iS][iM+iMs+3];
//						iLeft0 = Image.Dt[iS][iM-iMs-2];
//						iRight0 = Image.Dt[iS][iM+iMs+2];
						iLeft0 = Image.Dt[iS][iM-iToge];
						iRight0 = Image.Dt[iS][iM+iToge];

						//トゲ判定
						if (iLeft0*iRight0 == 0 || double(iLeft0)/iRight0 > iThres || double(iRight0)/iLeft0 > iThres){
							dLeft = Image.Dt[iS+iRandom[iR++%15]][iM-iMs]/2;
							dRight = Image.Dt[iS+iRandom[iR++%15]][iM+iMs]/2;
						}
						outImage.SetVal(iS, iM , int(dLeft + dRight) );
					}
					Image.SetVal(iS, iM, outImage.Dt[iS][iM]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
*/
				}//end if (画像の左右端以外に線欠陥がある場合)
			}//end for iS
		}//end if (iWid[iL] == 1)
		else if ( iWid[iL] == 2 ){
			for( int iS=iSHashi; iS<Image.SubPixel-iSHashi; iS++){
				double dQL_M0, dQL_M1;//補間対象のQL値

				//画像端に線欠陥がある場合
				if (0 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM+2]);
					outImage.SetVal(iS, iM+1 , Image.Dt[iS][iM+2]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM+2]);//隣接する片側の画素値をコピーする
					Image.SetVal(iS, iM+1, Image.Dt[iS][iM+2]);//隣接する片側の画素値をコピーする
				}
				else if (Image.MainPixel - 2 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM-1]);
					outImage.SetVal(iS, iM+1 , Image.Dt[iS][iM-1]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM-1]);//隣接する片側の画素値をコピーする
					Image.SetVal(iS, iM+1, Image.Dt[iS][iM-1]);//隣接する片側の画素値をコピーする
				}
				//画像端から1～3画素離れたところから線欠陥の場合
				else if (1 == iM || 2 == iM || 3 == iM || Image.MainPixel - 3 == iM || Image.MainPixel - 4 == iM || Image.MainPixel - 5 == iM) {//線形補間とする
					dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
					dQL_M1 =  1/3.*Image.Dt[iS][iM-1] + 2/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
					outImage.SetVal(iS, iM,   dQL_M0);//仮埋め
					outImage.SetVal(iS, iM+1, dQL_M1);//仮埋め
					Image.SetVal(iS, iM,   dQL_M0);//仮埋め
					Image.SetVal(iS, iM+1, dQL_M1);//仮埋め
				}
				//画像端以外に線欠陥がある場合
				else{
					if (bPreCor2Line & ((_LineDefectVer == iOn[iL]) & (_GridVer == iGridDirectionByFFT)) ||
						bPreCor2Line & ((_LineDefectHor == iOn[iL]) & (_GridHor == iGridDirectionByFFT))) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji
  #if 1
						//ここに、欠陥補正7相当アルゴで仮埋めするコードを書く。（欠陥補正4の仮埋めを使わない場合はこれが必須）
						dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
						dQL_M1 =  1/3.*Image.Dt[iS][iM-1] + 2/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
						Image.SetVal(iS, iM,   dQL_M0);//仮埋め
						Image.SetVal(iS, iM+1, dQL_M1);//仮埋め
  #elif 0
						//欠陥補正4相当アルゴで仮埋めする
						dQL_M0 =  1/2.*Image.Dt[iS][iM-2] + 1/2.*Image.Dt[iS][iM+2];//1画素飛ばしで仮埋め
						dQL_M1 =  1/2.*Image.Dt[iS][iM-1] + 1/2.*Image.Dt[iS][iM+3];//1画素飛ばしで仮埋め
						Image.SetVal(iS, iM,   dQL_M0);//仮埋め
						Image.SetVal(iS, iM+1, dQL_M1);//仮埋め
  #endif
					}

					//iMについて(dQL_M0)
					//まずトゲ判定
					iLeft0  = Image.Dt[iS][iM - iToge];//線欠陥から３画素離れた画素で判定
					iRight0 = Image.Dt[iS][iM + iToge];//線欠陥から３画素離れた画素で判定
					if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){//トゲ
						//2ライン連続線欠陥に隣接する正常画素で直線補間
						dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];
					}
					else{//非トゲ
						if ((_LineDefectVer == iOn[iL]) & (_GridVer == iGridDirectionByFFT) ||
							(_LineDefectHor == iOn[iL]) & (_GridHor == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji

							dLeft =
								  b1/2.*Image.Dt[iS][iM-iMs]
								+ b2/2.*Image.Dt[iS][iM-iMs-1]
								+ b3/2.*Image.Dt[iS][iM-iMs-2]
								+ b4/2.*Image.Dt[iS][iM-iMs-3];
							dRight =
								+ b1/2.*Image.Dt[iS][iM+iMs]
								+ b2/2.*Image.Dt[iS][iM+iMs+1]
								+ b3/2.*Image.Dt[iS][iM+iMs+2]
								+ b4/2.*Image.Dt[iS][iM+iMs+3];
							dQL_M0 = dLeft + dRight;
						}
						else{
							//隣接画素にて補間
							dQL_M0 =  1/2.*Image.Dt[iS][iM-1] + 1/2.*Image.Dt[iS][iM+1];
						}
					}
					//iM+1について(dQL_M1)
					//まずトゲ判定
					iLeft0  = Image.Dt[iS][iM + 1 - iToge];//線欠陥から３画素離れた画素で判定
					iRight0 = Image.Dt[iS][iM + 1 + iToge];//線欠陥から３画素離れた画素で判定
					if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){//トゲ
						//2ライン連続線欠陥に隣接する正常画素で直線補間
						dQL_M1 =  1/3.*Image.Dt[iS][iM-1] + 2/3.*Image.Dt[iS][iM+2];
					}
					else{//非トゲ
						if ((_LineDefectVer == iOn[iL]) & (_GridVer == iGridDirectionByFFT) ||
							(_LineDefectHor == iOn[iL]) & (_GridHor == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji

							dLeft =
								  b1/2.*Image.Dt[iS][iM-iMs+1]
								+ b2/2.*Image.Dt[iS][iM-iMs]
								+ b3/2.*Image.Dt[iS][iM-iMs-1]
								+ b4/2.*Image.Dt[iS][iM-iMs-2];
							dRight =
								+ b1/2.*Image.Dt[iS][iM+iMs+1]
								+ b2/2.*Image.Dt[iS][iM+iMs+2]
								+ b3/2.*Image.Dt[iS][iM+iMs+3]
								+ b4/2.*Image.Dt[iS][iM+iMs+4];
							dQL_M1 = dLeft + dRight;
						}
						else{
							//隣接画素にて補間
							dQL_M1 =  1/2.*Image.Dt[iS][iM]   + 1/2.*Image.Dt[iS][iM+2];
						}
					}
					outImage.SetVal(iS, iM,   dQL_M0);
					outImage.SetVal(iS, iM+1, dQL_M1);
					Image.SetVal(iS, iM, outImage.Dt[iS][iM]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
					Image.SetVal(iS, iM+1, outImage.Dt[iS][iM+1]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
				}
  			}
		}

#if 0 	//120807 ttsuji修正
		if( (iOn[iL] == 2) ^ (Yoko==true) ) {//線欠陥が横、もしくはグリッドが横の排他的論理和（両方とも横のときに主副反転してくれない）
#else
//		if(iOn[iL] == 2) {//線欠陥が横であれば主副反転する
		if (_LineDefectHor == iOn[iL]) {//線欠陥が横であれば主副反転する
#endif
			Image.MainSub();
			outImage.MainSub();
		}

		//120807 ttsuji修正
//		if( (iOn[iL] == 2) ^ (Yoko==true) ) {//線欠陥が横、もしくはグリッドが横の排他的論理和（線欠陥とグリッド向きが直交する場合）
		if (b1tmp < 99) {//バッファb1tmpを使っている場合
			//補正係数を元に戻しておく
			b1 = b1tmp;
			b2 = b2tmp;
			b3 = b3tmp;
			b4 = b4tmp;
		}


	}//endfor iL

	delete[] iOn;
	delete[] iLine;
	delete[] iWid;

	dA1 = b1;
	dA2 = b2;
	dA3 = b3;
	dA4 = b4;

	//debug
	ImageUp2Down.WriteTma( AddPrefix(str_img, "SobelUp2Down"), false);
	ImageLeft2Right.WriteTma( AddPrefix(str_img, "SobelLeft2Right"), false);
	ImageUpLeft2DownRight.WriteTma( AddPrefix(str_img, "SobelUpLeft2DownRight"), false);
	ImageUpRight2DownLeft.WriteTma( AddPrefix(str_img, "SobelUpRight2DownLeft"), false);


	Image = outImage;
	*this = (TSIMARS)outImage;

#if 0
	// 横線欠陥は略
	Image.WriteTma( AddPrefix( str_img, Prefix ));
#endif

	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥補正7	ttsuji130331
		大小欠陥をまとめて１つのアルゴで処理
		iGridDirection	縦：０、横：１、なし：２

		線欠陥の仮埋めは従来は４で行っていたが、６の内部の前段処理として行う方がすっきりするのでそうする。
    	端処理どうするか？
*/
bool TSIMARS::bDefectCorrection7(TTMA& bad, const int iGridDirection, const bool bPatternAnalysis, const int iPixelNumPatternAnalysis, const double dThPatternExist, const bool bModifyWeight)
{
	if (0 == iGridDirection) {//縦グリッドならば主副反転
		bad.MainSub();
		MainSub();
	}

	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){

#if 0
			if(bad.Dt[iSCnt][iMCnt] > 0
			&& bad.Dt[iSCnt][iMCnt] <= 0x1919){    //欠陥1補正対象画素
			//&& bad.Dt[iSCnt][iMCnt] <= 65535){    //欠陥1補正対象画素
												 //0x1919
#elif 0
			if (bad.Dt[iSCnt][iMCnt] > 0) {//欠陥画素であれば無条件に処理
#else
			if (bad.Dt[iSCnt][iMCnt] > 0 && bad.Dt[iSCnt][iMCnt] != 0x2000 && bad.Dt[iSCnt][iMCnt] != 0xEEEE) {//サイズオーバーの点欠陥以外の点欠陥を処理（線欠陥も除く）
#endif
				if (2 != iGridDirection) {//グリッドあり
		//欠陥に対して水平方向のみの画素で、かつ欠陥に隣接した画素から補間（横グリッドと想定し、グリッドの向きのみから補間）（要は欠陥補正2改と同じアルゴ）
		//　→　エッジある画素でのトゲなくなり、かつスムーズ領域での良化も保持しており、両領域で従来補正4より改善した！
					const int iMSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
					if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt, iMCnt, iMSearchInterval)){
						CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
						if (0 == iGridDirection) {//縦グリッドならば主副反転
							bad.MainSub();
							MainSub();
						}
						return false;
					}
				}
				else{//グリッドなし
					const int iSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
					if (bPatternAnalysis) {
						if (!bDefectCorrectionWithPatternAnalysis(bad, iSCnt, iMCnt, iSearchInterval, iPixelNumPatternAnalysis, dThPatternExist)) {
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionWithPatternAnalysis()", "Error", MB_OK);
							return false;
						}
					}
					else{
						if (!bDefectCorrectionByHorVer4Pixel(bad, iSCnt, iMCnt, iSearchInterval, bModifyWeight)){
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByHorVer4Pixel()", "Error", MB_OK);
							return false;
						}
					}
				}
			}
		}
	}

	//元に戻す
	if (0 == iGridDirection) {//縦グリッドならば主副反転
		bad.MainSub();
		MainSub();
	}

	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥補正4　　	渡邊殿org(10325)
		欠陥画素の上下左右と斜め8方向の画素を用いて補間
*/


bool TSIMARS::bDefectCorrection4(TTMA& bad)
{
	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] > 0
			&& bad.Dt[iSCnt][iMCnt] <= 0x1919){    //欠陥1補正対象画素
			//&& bad.Dt[iSCnt][iMCnt] <= 65535){    //欠陥1補正対象画素
												 //0x1919

				__int64 iSum_PV = 0;
				//int iSum_PV = 0;
				int iSum_norm = 0;

				if(iSCnt-2 >= 0 && iMCnt-2 >= 0){
					if(bad.Dt[iSCnt-2][iMCnt-2] == 0){
						iSum_PV += Dt[iSCnt-2][iMCnt-2];
						++iSum_norm;
					}
				}
				if(iSCnt-2 >= 0){
					if(bad.Dt[iSCnt-2][iMCnt] == 0){
						iSum_PV += Dt[iSCnt-2][iMCnt];
						++iSum_norm;
					}
				}
				if(iSCnt-2 >= 0 && iMCnt+2 < bad.MainPixel){
					if(bad.Dt[iSCnt-2][iMCnt+2] == 0){
						iSum_PV += Dt[iSCnt-2][iMCnt+2];
						++iSum_norm;
					}
				}
				if(iMCnt-2 >= 0){
					if(bad.Dt[iSCnt][iMCnt-2] == 0){
						iSum_PV += Dt[iSCnt][iMCnt-2];
						++iSum_norm;
					}
				}

				/*
				if(bad.Dt[iSCnt][iMCnt] == 0){
					iSum_PV += img.Dt[iSCnt][iMCnt];
					++iSum_norm;
				}
				*/
				if(iMCnt+2 < bad.MainPixel){
					if(bad.Dt[iSCnt][iMCnt+2] == 0){
						iSum_PV += Dt[iSCnt][iMCnt+2];
						++iSum_norm;
					}
				}
				if(iSCnt+2 < bad.SubPixel && iMCnt-2 >= 0){
					if(bad.Dt[iSCnt+2][iMCnt-2] == 0){
						iSum_PV += Dt[iSCnt+2][iMCnt-2];
						++iSum_norm;
					}
				}
				if(iSCnt+2 < bad.SubPixel){
					if(bad.Dt[iSCnt+2][iMCnt] == 0){
						iSum_PV += Dt[iSCnt+2][iMCnt];
						++iSum_norm;
					}
				}
				if(iSCnt+2 < bad.SubPixel && iMCnt+2 < bad.MainPixel){
					if(bad.Dt[iSCnt+2][iMCnt+2] == 0){
						iSum_PV += Dt[iSCnt+2][iMCnt+2];
						++iSum_norm;
					}
				}

				if(iSum_norm > 0){     // 20100330　正常画素があるときのみ補正する。
					iSum_PV /= iSum_norm;
					Dt[iSCnt][iMCnt] = iSum_PV;
				}
			}
		}
	}


	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥補正4改
　　	渡邊殿org(10325)を修正（120809 ttsuji）
*/


bool TSIMARS::bDefectCorrection4kai(TTMA& bad, const int iAlgoIndex, const int iGridDirection)
{
	if (1 == iAlgoIndex || 2 == iAlgoIndex) {//newアルゴリズム
		if (0 == iGridDirection) {//縦グリッドならば主副反転
			bad.MainSub();
			MainSub();
		}
	}

	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] > 0
			&& bad.Dt[iSCnt][iMCnt] <= 0x1919){    //欠陥1補正対象画素
			//&& bad.Dt[iSCnt][iMCnt] <= 65535){    //欠陥1補正対象画素
												 //0x1919

				__int64 iSum_PV = 0;
				//int iSum_PV = 0;
				int iSum_norm = 0;

				if (0 == iAlgoIndex) {//watababe orgアルゴリズム
					if(iSCnt-2 >= 0 && iMCnt-2 >= 0){
						if(bad.Dt[iSCnt-2][iMCnt-2] == 0){
							iSum_PV += Dt[iSCnt-2][iMCnt-2];
							++iSum_norm;
						}
					}
					if(iSCnt-2 >= 0){
						if(bad.Dt[iSCnt-2][iMCnt] == 0){
							iSum_PV += Dt[iSCnt-2][iMCnt];
							++iSum_norm;
						}
					}
					if(iSCnt-2 >= 0 && iMCnt+2 < bad.MainPixel){
						if(bad.Dt[iSCnt-2][iMCnt+2] == 0){
							iSum_PV += Dt[iSCnt-2][iMCnt+2];
							++iSum_norm;
						}
					}
					if(iMCnt-2 >= 0){
						if(bad.Dt[iSCnt][iMCnt-2] == 0){
							iSum_PV += Dt[iSCnt][iMCnt-2];
							++iSum_norm;
						}
					}

					/*
					if(bad.Dt[iSCnt][iMCnt] == 0){
						iSum_PV += img.Dt[iSCnt][iMCnt];
						++iSum_norm;
					}
					*/
					if(iMCnt+2 < bad.MainPixel){
						if(bad.Dt[iSCnt][iMCnt+2] == 0){
							iSum_PV += Dt[iSCnt][iMCnt+2];
							++iSum_norm;
						}
					}
					if(iSCnt+2 < bad.SubPixel && iMCnt-2 >= 0){
						if(bad.Dt[iSCnt+2][iMCnt-2] == 0){
							iSum_PV += Dt[iSCnt+2][iMCnt-2];
							++iSum_norm;
						}
					}
					if(iSCnt+2 < bad.SubPixel){
						if(bad.Dt[iSCnt+2][iMCnt] == 0){
							iSum_PV += Dt[iSCnt+2][iMCnt];
							++iSum_norm;
						}
					}
					if(iSCnt+2 < bad.SubPixel && iMCnt+2 < bad.MainPixel){
						if(bad.Dt[iSCnt+2][iMCnt+2] == 0){
							iSum_PV += Dt[iSCnt+2][iMCnt+2];
							++iSum_norm;
						}
					}

					if(iSum_norm > 0){     // 20100330　正常画素があるときのみ補正する。
						iSum_PV /= iSum_norm;
						Dt[iSCnt][iMCnt] = iSum_PV;
					}
				}
				else if (1 == iAlgoIndex) {//newアルゴリズム
#if 0 //ttsuji 120725
		//欠陥に対して水平方向のみの画素から補間（横グリッドと想定し、グリッドの向きのみから補間）
		//　→　エッジある画素で補正残差のトゲ目立つ。（スムーズ領域では良化）
					if(iMCnt-2 >= 0){
						if(bad.Dt[iSCnt][iMCnt-2] == 0){
							iSum_PV += Dt[iSCnt][iMCnt-2];
							++iSum_norm;
						}
					}
					if(iMCnt+2 < bad.MainPixel){
						if(bad.Dt[iSCnt][iMCnt+2] == 0){
							iSum_PV += Dt[iSCnt][iMCnt+2];
							++iSum_norm;
						}
					}


					if(iSum_norm > 0){     // 20100330　正常画素があるときのみ補正する。
						iSum_PV /= iSum_norm;
						Dt[iSCnt][iMCnt] = iSum_PV;
					}
#else //ttsuji 120725
		//欠陥に対して水平方向のみの画素で、かつ欠陥に隣接した画素から補間（横グリッドと想定し、グリッドの向きのみから補間）（要は欠陥補正2改と同じアルゴ）
		//　→　エッジある画素でのトゲなくなり、かつスムーズ領域での良化も保持しており、両領域で従来補正4より改善した！
					const int iMSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
					if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt, iMCnt, iMSearchInterval)){
						CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
						if (0 == iGridDirection) {//縦グリッドならば主副反転
							bad.MainSub();
							MainSub();
						}
						return false;
					}
#endif
				}
				else if (2 == iAlgoIndex) {//newアルゴリズム(Multi Resolution読みの画素密度変換前の画像をハンドリングする場合)
					if (0 == iGridDirection) {//縦グリッドならば縦方向の隣接画素で補間（画素密度変換後画像だと縦1画素とばしに相当）
						const int iMSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
						if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt, iMCnt, iMSearchInterval)){
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
							if (0 == iGridDirection) {//縦グリッドならば主副反転
								bad.MainSub();
								MainSub();
							}
							return false;
						}
					}
					else{//横グリッドならば横方向に1画素とばしの画素で補間（画素密度変換後画像だと横隣接画像に相当）
						const int iMSearchInterval = 2;//1画素ずつ欠陥周囲の正常画素を探索
						if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt, iMCnt, iMSearchInterval)){
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
							if (0 == iGridDirection) {//縦グリッドならば主副反転
								bad.MainSub();
								MainSub();
							}
							return false;
						}
					}

				}
				else if (3 == iAlgoIndex) {//newアルゴリズム(Multi Resolution読みの画素密度変換前の画像をハンドリングする場合)
					//tmp
					if (iMCnt == 6) {
						int aa = 1;
						if (iSCnt == 500) {
							int a = 1;
						}
						if (iSCnt == 762) {
							int a = 1;
						}
					}
					if (iMCnt == 7) {
						int aa = 1;
						if (iSCnt == 760) {
							int a = 1;
						}
						if (iSCnt == 761) {
							int a = 1;
						}
						if (iSCnt == 762) {
							int a = 1;
						}
					}
					if (iMCnt == 8) {
						int aa = 1;
					}
					if (iMCnt == 9) {
						int aa = 1;
					}
					if (iMCnt > 90) {
						int aa = 1;
					}


					if (0 == iGridDirection) {//縦グリッドならば縦方向の隣接画素で補間（画素密度変換後画像だと斜め上2画素に相当）
						const int iSearchMode = 0;//縦方向の正常画素のみから補間
						if (!bDefectCorrectionByMultiResoPixel(bad, iSCnt, iMCnt, iSearchMode)){
								CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByMultiResoPixel()", "Error", MB_OK);
								return false;
						}
					}
					else{//横グリッドならば横方向に1画素とばしの画素で補間（画素密度変換後画像だと横隣接画像に相当）
						const int iSearchMode = 1;//横方向の正常画素のみから補間
						if (!bDefectCorrectionByMultiResoPixel(bad, iSCnt, iMCnt, iSearchMode)){
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByMultiResoPixel()", "Error", MB_OK);
							return false;
						}
					}

				}
				else if (4 == iAlgoIndex) {//newアルゴリズム(主副トゲからパターン認識してパターンがグリッドと直交するならパターンに平行な正常画素で補間)


				}
				else if (5 == iAlgoIndex) {//newアルゴリズム(ビニング時に2画素はなれた画素だと補正残差ひどくなるので隣接画素から補間（欠陥補正2ライク））
/*
					if(iSCnt-1 >= 0 && iMCnt-1 >= 0){
						if(bad.Dt[iSCnt-1][iMCnt-1] == 0){
							iSum_PV += Dt[iSCnt-1][iMCnt-1];
							++iSum_norm;
						}
					}*/
					if(iSCnt-1 >= 0){
						if(bad.Dt[iSCnt-1][iMCnt] == 0){
							iSum_PV += Dt[iSCnt-1][iMCnt];
							++iSum_norm;
						}
					}
/*
					if(iSCnt-1 >= 0 && iMCnt+1 < bad.MainPixel){
						if(bad.Dt[iSCnt-1][iMCnt+1] == 0){
							iSum_PV += Dt[iSCnt-1][iMCnt+1];
							++iSum_norm;
						}
					}*/
					if(iMCnt-1 >= 0){
						if(bad.Dt[iSCnt][iMCnt-1] == 0){
							iSum_PV += Dt[iSCnt][iMCnt-1];
							++iSum_norm;
						}
					}

					if(iMCnt+1 < bad.MainPixel){
						if(bad.Dt[iSCnt][iMCnt+1] == 0){
							iSum_PV += Dt[iSCnt][iMCnt+1];
							++iSum_norm;
						}
					}
/*					if(iSCnt+1 < bad.SubPixel && iMCnt-1 >= 0){
						if(bad.Dt[iSCnt+1][iMCnt-1] == 0){
							iSum_PV += Dt[iSCnt+1][iMCnt-1];
							++iSum_norm;
						}
					}*/
					if(iSCnt+1 < bad.SubPixel){
						if(bad.Dt[iSCnt+1][iMCnt] == 0){
							iSum_PV += Dt[iSCnt+1][iMCnt];
							++iSum_norm;
						}
					}
/*					if(iSCnt+1 < bad.SubPixel && iMCnt+1 < bad.MainPixel){
						if(bad.Dt[iSCnt+1][iMCnt+1] == 0){
							iSum_PV += Dt[iSCnt+1][iMCnt+1];
							++iSum_norm;
						}
					}*/

					if(iSum_norm > 0){     // 20100330　正常画素があるときのみ補正する。
						iSum_PV /= iSum_norm;
						Dt[iSCnt][iMCnt] = iSum_PV;
					}

				}
			}
		}
	}

	//元に戻す
	if (1 == iAlgoIndex || 2 == iAlgoIndex) {//newアルゴリズム
		if (0 == iGridDirection) {//縦グリッドならば主副反転
			bad.MainSub();
			MainSub();
		}
	}

	return true;
}
//---------------------------------------------------------------------------
/*
欠陥に対して水平方向のみの画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
（横グリッドと想定し、グリッドの向きのみから補間）
*/
bool TSIMARS::bDefectCorrectionByOnlyHor2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iMSearchInterval)
{

}

void TSIMARS::SearchNormal4PixelsArroundBadPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, bool& bLeft, bool& bRight, bool& bUp, bool& bDown, int& iM1, int& iM2, int& iS1, int& iS2, int& iQLleft, int& iQLright, int& iQLup, int& iQLdown)
{


}
//---------------------------------------------------------------------------
/*
 欠陥に対して水平方向のみの画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間

 */
bool TSIMARS::bDefectCorrectionByMultiResoPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchMode)
{
	int QLHor, QLVer;
	int iQLleft = 0;
	int iQLright = 0;
	int iQLup = 0;
	int iQLdown = 0;
	int iM1 = 0;
	int iM2 = 0;
	int iS1 = 0;
	int iS2 = 0;
	bool bLeft = false;
	bool bRight = false;
	bool bUp = false;
	bool bDown = false;
	int iMShift = 0;
	int iSShift = 0;
	int iMSearchInterval = 2;//マルチレゾの場合は正方画像の左右隣接画素は1画素とばしの左右画素

	//左右方向の最近接画素を探索
	if(iMCor - iMSearchInterval >= 0){ //要素検討なのでとりあえず端2画素は補正から除外する（後ほど端処理を入れる）
		iMShift = iMSearchInterval;
		while (0 != bad.Dt[iSCor][iMCor - iMShift] && (iMCor - iMShift) > 0){//横線欠陥の場合に画像幅の分までiMShiftがインクリメントされる無駄処理が入ってしまう。
//		while (0 != bad.Dt[iSCor][iMCor - iMShift] && (iMCor - iMShift) > 0 && iMShift <= 2){//大サイズ欠陥の尾の部分が対応できないので補正画質NG
			iMShift += iMSearchInterval;
		}
		iM1 = iMCor - iMShift;
		if (iM1 >= 0 && iM1 < bad.MainPixel) {
			iQLleft = Dt[iSCor][iM1];
			bLeft = true;
		}
		else{
			bLeft = false;
		}
	}
	if(iMCor + iMSearchInterval < bad.MainPixel){  //要素検討なのでとりあえず端2画素は補正から除外する（後ほど端処理を入れる）
		iMShift = iMSearchInterval;
		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel){//横線欠陥の場合に画像幅の分までiMShiftがインクリメントされる無駄処理が入ってしまう。
//		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel && iMShift <= 2){//大サイズ欠陥の尾の部分が対応できないので補正画質NG
			iMShift += iMSearchInterval;
		}
		iM2 = iMCor + iMShift;
		if (iM2 >= 0 && iM2 < bad.MainPixel) {
			iQLright = Dt[iSCor][iM2];
			bRight = true;
		}
		else{
			bRight = false;
		}
	}

	if (bLeft && bRight) {
		if (iM2 - iM1) {
			QLHor = (int)((iQLleft * (iM2 - iMCor) + iQLright * (iMCor - iM1)) / (float)(iM2 - iM1));
		}
		else{
			QLHor = iQLleft;
		}
	}
	else if (bLeft) {
		QLHor = iQLleft;
	}
	else if (bRight) {
		QLHor = iQLright;
	}


	//上下方向の最近接画素を探索
	if (!(iMCor % 2)) {//偶数列のとき
		if (iMCor - 1 >= 0 && iMCor + 1 < bad.MainPixel && iSCor - 1 >= 0 && iSCor < bad.SubPixel) {
			//上方探索
			iSShift = 0;
			//斜め上が正常画素か調査
			while (0 != bad.Dt[iSCor - iSShift - 1][iMCor - 1] && 0 != bad.Dt[iSCor - iSShift - 1][iMCor + 1] && (iSCor - iSShift - 1 >= 0)){
				if (0 != bad.Dt[iSCor - iSShift - 1][iMCor]) {//斜め上がともに欠陥画素ならば真上画素が正常画素か調査
					iSShift++;
				}
				else{//真上が正常画素ならば探索終了
					bUp = true;
					iQLup = Dt[iSCor - iSShift - 1][iMCor];
					iS1 = iSCor - 2*iSShift - 2;
					break;
				}
			}
			if (!bUp && iSCor - iSShift - 1 >= 0) {//斜め上に正常画素があった場合
				bUp = true;
				int iNum = 0;
				if (!bad.Dt[iSCor - iSShift - 1][iMCor - 1]) {
					iQLup += Dt[iSCor - iSShift - 1][iMCor - 1];
					iNum++;
				}
				if (!bad.Dt[iSCor - iSShift - 1][iMCor + 1]) {
					iQLup += Dt[iSCor - iSShift - 1][iMCor + 1];
					iNum++;
				}
				iQLup /= iNum;//切捨て
				iS1 = iSCor - 2*iSShift - 1;
			}

			//下方探索
			iSShift = 0;
			//真横が正常画素か調査
			while (0 != bad.Dt[iSCor + iSShift][iMCor - 1] && 0 != bad.Dt[iSCor + iSShift][iMCor + 1] && (iSCor + iSShift + 1 < bad.SubPixel)){
				if (0 != bad.Dt[iSCor + iSShift + 1][iMCor]) {//真横がともに欠陥画素ならば真下画素が正常画素か調査
					iSShift++;
				}
				else{//真下が正常画素ならば探索終了
					bDown = true;
					iQLdown = Dt[iSCor + iSShift + 1][iMCor];
					iS2 = iSCor + 2*iSShift + 2;
					break;
				}
			}
			if (!bDown && iSCor + iSShift + 1 < bad.SubPixel) {//真横に正常画素があった場合
				bDown = true;
				int iNum = 0;
				if (!bad.Dt[iSCor + iSShift][iMCor - 1]) {
					iQLdown += Dt[iSCor + iSShift][iMCor - 1];
					iNum++;
				}
				if (!bad.Dt[iSCor + iSShift][iMCor + 1]) {
					iQLdown += Dt[iSCor + iSShift][iMCor + 1];
					iNum++;
				}
				iQLdown /= iNum;//切捨て
				iS2 = iSCor + 2*iSShift + 1;
			}
		}
	}
	else{//奇数列のとき
		if (iMCor - 1 >= 0 && iMCor + 1 < bad.MainPixel && iSCor >= 0 && iSCor + 1 < bad.SubPixel) {
			//上方探索
			iSShift = 0;
			//真横が正常画素か調査
			while (0 != bad.Dt[iSCor - iSShift][iMCor - 1] && 0 != bad.Dt[iSCor - iSShift][iMCor + 1] && (iSCor - iSShift - 1 >= 0)){
				if (0 != bad.Dt[iSCor - iSShift - 1][iMCor]) {//真横がともに欠陥画素ならば真上画素が正常画素か調査
					iSShift++;
				}
				else{//真上が正常画素ならば探索終了
					bUp = true;
					iQLup = Dt[iSCor - iSShift - 1][iMCor];
					iS1 = iSCor - 2*iSShift - 2;
					break;
				}
			}
			if (!bUp && iSCor - iSShift - 1 >= 0) {//真横に正常画素があった場合
				bUp = true;
				int iNum = 0;
				if (!bad.Dt[iSCor - iSShift][iMCor - 1]) {
					iQLup += Dt[iSCor - iSShift][iMCor - 1];
					iNum++;
				}
				if (!bad.Dt[iSCor - iSShift][iMCor + 1]) {
					iQLup += Dt[iSCor - iSShift][iMCor + 1];
					iNum++;
				}
				iQLup /= iNum;//切捨て
				iS1 = iSCor - 2*iSShift - 1;
			}

			//下方探索
			iSShift = 0;
			//斜め下が正常画素か調査
			while (0 != bad.Dt[iSCor + iSShift + 1][iMCor - 1] && 0 != bad.Dt[iSCor + iSShift + 1][iMCor + 1] && (iSCor + iSShift + 1 < bad.SubPixel - 1)){
				if (0 != bad.Dt[iSCor + iSShift + 1][iMCor]) {//斜め下がともに欠陥画素ならば真下画素が正常画素か調査
					iSShift++;
				}
				else{//真下が正常画素ならば探索終了
					bDown = true;
					iQLdown = Dt[iSCor + iSShift + 1][iMCor];
					iS2 = iSCor + 2*iSShift + 2;
					break;
				}
			}
			if (!bDown && iSCor + iSShift + 1 < bad.SubPixel - 1) {//斜め下に正常画素があった場合
				bDown = true;
				int iNum = 0;
				if (!bad.Dt[iSCor + iSShift + 1][iMCor - 1]) {
					iQLdown += Dt[iSCor + iSShift + 1][iMCor - 1];
					iNum++;
				}
				if (!bad.Dt[iSCor + iSShift + 1][iMCor + 1]) {
					iQLdown += Dt[iSCor + iSShift + 1][iMCor + 1];
					iNum++;
				}
				iQLdown /= iNum;//切捨て
				iS2 = iSCor + 2*iSShift + 1;
			}
		}

	}


	if (bUp && bDown) {
		if (iS2 - iS1) {
			QLVer = (int)((iQLup * (iS2 - iSCor) + iQLdown * (iSCor - iS1)) / (float)(iS2 - iS1));
		}
		else{
			QLVer = iQLup;
		}
	}
	else if (bUp) {
		QLVer = iQLup;
	}
	else if (bDown) {
		QLVer = iQLdown;
	}


	switch (iSearchMode) {//探索モード
	case 0://上下方向探索
		if (bUp || bDown) {
			Dt[iSCor][iMCor] = QLVer;
		}
		break;
	case 1://左右方向探索
		if (bLeft || bRight) {
			Dt[iSCor][iMCor] = QLHor;
		}
		break;
	case 2://上下左右の4方向探索
		if ((bUp || bDown) && (bLeft || bRight)) {
			Dt[iSCor][iMCor] = (QLHor + QLVer) / 2;//切捨て
		}
		break;
	default:
		break;
	}


	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥補正２改
		補正アルゴ簡易化（グリッドの縞方向の画素のみを使用） //ttsuji120723
*/
bool TSIMARS::bDefectCorrection2kai(TTMA& bad, const int iAlgoIndex, const int iGridDirection)
{
	const int iSIZE_MAX = 100;      //重み付け補正する欠陥の最大の大きさ

	int iSearchCnt;
	int iCnt, iSum;
	int iMCnt,  iSCnt;
	int iMCnt2, iSCnt2;
	int iMCnt3, iSCnt3;
	int iMBad1, iMBad2, iSBad1, iSBad2;
	bool bBadExist;
	float fDistance;
	float fSum;
	float fSumWeight;
	float fWeight[iSIZE_MAX + 2][iSIZE_MAX + 2];
	unsigned short int nPer = (unsigned short int)(bad.DtMax / 2);

	if(MainPixel != bad.MainPixel || SubPixel != bad.SubPixel){
		return(false);
	}


	//新規アルゴ（グリッドのリス目方向の画素のみから補間）の場合は、縦グリッドならば主副反転
	if (1 == iAlgoIndex || 3 == iAlgoIndex) {//newアルゴリズム
		if (0 == iGridDirection) {//縦グリッドならば主副反転
			bad.MainSub();
			MainSub();
		}
	}


	TTMA bad2;
	bad2.NewDt(bad.SubPixel, bad.MainPixel, bad.Bit);


//　　　重み付けtableの作成
//      L <  2 の時　距離の3乗
//      L >= 2 の時　距離の2乗
//      距離の3乗とcase by case
	if (0 == iAlgoIndex || 5 == iAlgoIndex) {//従来アルゴリズム
		for(iSCnt = 0; iSCnt <= iSIZE_MAX + 1; ++iSCnt){        //090408修正
			for(iMCnt = 0; iMCnt <= iSIZE_MAX + 1; ++iMCnt){    //090408修正
				fDistance = sqrt((float)iSCnt * iSCnt + iMCnt * iMCnt);
				if(fDistance == 0.0){
					fWeight[iSCnt][iMCnt] = 0.0;
				}else if(fDistance < 2.0){
					fDistance = fDistance * fDistance * fDistance;
					fWeight[iSCnt][iMCnt] = 1.0 / fDistance;
				}else{
					fWeight[iSCnt][iMCnt] = 1.0 / 2.0 / fDistance / fDistance;
				}
			}
		}
#if 0
		FILE* fp = fopen("G:\\Theme\\動画\\FPGA補正実装\\欠陥補正2と5の統合\\5×5の欠陥マップ\\fWeight.txt", "wt");
		if (!fp) {
			return false;
		}
		for(iSCnt = 0; iSCnt <= iSIZE_MAX + 1; ++iSCnt){        //090408修正
			for(iMCnt = 0; iMCnt <= iSIZE_MAX + 1; ++iMCnt){    //090408修正
				fprintf(fp, "%f	", fWeight[iSCnt][iMCnt]);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
#endif
	}



	//
	// 欠陥塊分断のため幅2以下を0xA000とし、3以上を0xDDDDとする。
	//      090410

	//横の尾の処理
	for(int iMCnt = 0; iMCnt < bad.MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < bad.SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] == bad.DtMax){
				if(iSCnt + 1 < bad.SubPixel){
					if(bad.Dt[iSCnt + 1][iMCnt] != bad.DtMax){ //次の画素が欠陥でないとき
						bad.Dt[iSCnt][iMCnt] = 0xBBBB;
					}
					else{                                      //次の画素が欠陥であるとき
						if(iSCnt + 2 < bad.SubPixel){
							if(bad.Dt[iSCnt + 2][iMCnt] != bad.DtMax){
								bad.Dt[iSCnt][iMCnt] = 0xBBBB;
								bad.Dt[iSCnt + 1][iMCnt] = 0xBBBB;
							}
							else{
								int iX = 0;
								while(iSCnt + iX < bad.SubPixel){
									if(bad.Dt[iSCnt + iX][iMCnt] == bad.DtMax){
										bad.Dt[iSCnt + iX][iMCnt] = 0xDDDD;
										++iX;
									}else{
										iX = bad.SubPixel; //whileを抜けるため
									}
								}
							}
						}else{  //端画素のとき  090409
							bad.Dt[iSCnt][iMCnt] = 0xBBBB;  //090409
						}
					}
				}else{  //端画素のとき  090409
					bad.Dt[iSCnt][iMCnt] = 0xBBBB;  //090409
				}
			}
		}
	}
	for(int iMCnt = 0; iMCnt < bad.MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < bad.SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] == 0xDDDD){
				bad.Dt[iSCnt][iMCnt] = bad.DtMax;
			}
		}
	}

	//縦の尾の処理
	for(int iSCnt = 0; iSCnt < bad.SubPixel; ++iSCnt){
		for(int iMCnt = 0; iMCnt < bad.MainPixel; ++iMCnt){
			if(bad.Dt[iSCnt][iMCnt] >= 0xBBBB){
				if(iMCnt + 1 < bad.MainPixel){
					if(bad.Dt[iSCnt][iMCnt + 1] < 0xBBBB){ //次の画素が欠陥でないとき
						bad.Dt[iSCnt][iMCnt] = 0x1111;
					}
					else if(iMCnt + 1 < bad.MainPixel){
						if(bad.Dt[iSCnt][iMCnt + 1] >= 0xBBBB){                                      //次の画素が欠陥であるとき
							if(iMCnt + 2 < bad.MainPixel){
								if(bad.Dt[iSCnt][iMCnt + 2] < 0xBBBB){
									bad.Dt[iSCnt][iMCnt] = 0x1111;
									bad.Dt[iSCnt][iMCnt + 1] = 0x1111;
								}
								else{        //2連続欠陥の場合
									int iY = 0;
									while(iMCnt + iY < bad.MainPixel){
										if(bad.Dt[iSCnt][iMCnt + iY] >= 0xBBBB){
											if(bad.Dt[iSCnt][iMCnt + iY] == bad.DtMax){
												bad.Dt[iSCnt][iMCnt + iY] = 0x2222;
											}else{
												bad.Dt[iSCnt][iMCnt + iY] = 0x1111;
											}
											++iY;
										}else{
											iY = bad.MainPixel;  //whileを抜けるため
										}
									}
								}
							}else{  //端画素のとき
								bad.Dt[iSCnt][iMCnt] = 0x1111; //090409
							}
						}
					}
				}else{  //端画素のとき
					bad.Dt[iSCnt][iMCnt] = 0x1111; //090409
				}
			}
		}
	}

	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			if(bad.Dt[iSCnt][iMCnt] == 0x2222){
				bad.Dt[iSCnt][iMCnt] = 0xDDDD;
			}else if(bad.Dt[iSCnt][iMCnt] == 0x1111){
				bad.Dt[iSCnt][iMCnt] = 0xA000;
			}
		}
	}



	/*  //デバッグ用
	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			bad2.Dt[iSCnt][iMCnt] = bad.Dt[iSCnt][iMCnt];
		}
	}
	bad2.WriteTma("Debug_TestImg");
	*/

/*	//test
	bad.WriteTma("bad_debug_output");
	bad2.WriteTma("bad2_debug_output");
*/

	//
	//
	// 欠陥補正2の本体部分
	//
	//

	vector<int> MPos, SPos, MSize, SSize;
	vector<int> MCPos, SCPos;

	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			if(bad.Dt[iSCnt][iMCnt] == 0xDDDD){          // bad.DtMax からの修正
//			if(bad.Dt[iSCnt][iMCnt] <= 0xDDDD || bad.Dt[iSCnt][iMCnt] > 0){//大小サイズ欠陥によらず同じ処理
//			if(bad.Dt[iSCnt][iMCnt] != 0){//大小サイズ欠陥によらず同じ処理

				//
				// 幅2で欠陥塊を分断する
				//
				MPos.push_back(iMCnt);            //最初に見つけた主画素位置
				SPos.push_back(iSCnt);            //最初に見つけた副画素位置

				int iM0 = iMCnt;
				int iM1 = iMCnt;
				int iS0 = iSCnt;
				int iS1 = iSCnt;

				stack<int, vector<int> > stkM, stkS;
				stkM.push(iMCnt);
				stkS.push(iSCnt);
				bad.Dt[iSCnt][iMCnt] = 0xBBBB;   //最初に見つけた画素に0xBBBBフラグ
				bad2.Dt[iSCnt][iMCnt] = 0xBBBB;   //最初に見つけた画素に0xBBBBフラグ

				do{
					int iM = stkM.top();
					stkM.pop();
					int iS = stkS.top();
					stkS.pop();

					int iSPos = iS - 1;
					if(iSPos >= 0){
						if(bad.Dt[iSPos][iM] == 0xDDDD){
							stkM.push(iM);
							stkS.push(iSPos);
							bad.Dt[iSPos][iM] = 0xBBBB;
							bad2.Dt[iSPos][iM] = 0xBBBB;
							if(iS0 > iSPos){
								iS0 = iSPos;
							}
						}else if(bad.Dt[iSPos][iM] == 0xA000){
							if(bad.Dt[iS][iM] == 0xBBBB){
								stkM.push(iM);
								stkS.push(iSPos);
								bad.Dt[iSPos][iM] = 0xCCCC;
								bad2.Dt[iSPos][iM] = 0xCCCC;
								if(iS0 > iSPos){
									iS0 = iSPos;
								}
							}
						}
					}

					iSPos = iS + 1;
					if(bad.SubPixel > iSPos){
						if(bad.Dt[iSPos][iM] == 0xDDDD){
							stkM.push(iM);
							stkS.push(iSPos);
							bad.Dt[iSPos][iM] = 0xBBBB;
							bad2.Dt[iSPos][iM] = 0xBBBB;
							if(iS1 < iSPos){
								iS1 = iSPos;
							}
						}else if(bad.Dt[iSPos][iM] == 0xA000){
							if(bad.Dt[iS][iM] == 0xBBBB){
								stkM.push(iM);
								stkS.push(iSPos);
								bad.Dt[iSPos][iM] = 0xCCCC;
								bad2.Dt[iSPos][iM] = 0xCCCC;
								if(iS1 < iSPos){
									iS1 = iSPos;
								}
							}
						}
					}

					int iMPos = iM - 1;
					if(0 <= iMPos){
						if(bad.Dt[iS][iMPos] == 0xDDDD){
							stkM.push(iMPos);
							stkS.push(iS);
							bad.Dt[iS][iMPos] = 0xBBBB;
							bad2.Dt[iS][iMPos] = 0xBBBB;
							if(iM0 > iMPos){
								iM0 = iMPos;
							}
						}else if(bad.Dt[iS][iMPos] == 0xA000){
							if(bad.Dt[iS][iM] == 0xBBBB){
								stkM.push(iMPos);
								stkS.push(iS);
								bad.Dt[iS][iMPos] = 0xCCCC;
								bad2.Dt[iS][iMPos] = 0xCCCC;
								if(iM0 > iMPos){
									iM0 = iMPos;
								}
							}
						}
					}

					iMPos = iM + 1;
					if(bad.MainPixel > iMPos){
						if(bad.Dt[iS][iMPos] == 0xDDDD){
							stkM.push(iMPos);
							stkS.push(iS);
							bad.Dt[iS][iMPos] = 0xBBBB;
							bad2.Dt[iS][iMPos] = 0xBBBB;
							if(iM1 < iMPos){
								iM1 = iMPos;
							}
						}else if(bad.Dt[iS][iMPos] == 0xA000){
							if(bad.Dt[iS][iM] == 0xBBBB){
								stkM.push(iMPos);
								stkS.push(iS);
								bad.Dt[iS][iMPos] = 0xCCCC;
								bad2.Dt[iS][iMPos] = 0xCCCC;
								if(iM1 < iMPos){
								    iM1 = iMPos;
								}
							}
						}
					}
				}while(!stkM.empty());



				//
				// 欠陥大きさ決定
				//   iMBad1 が主の欠陥位置の-1、iMBad2 が主の欠陥位置の+1になる
				//

                if(iMCnt > 0){
					iMBad1 = iMCnt - 1;
				}else{
					iMBad1 = 0;
				}
				if(iSCnt > 0){
					iSBad1 = iSCnt - 1;
				}else{
					iSBad1 = 0;
				}
				iMBad2 = iMCnt;
				while(iMBad2 < MainPixel - 1 && (bad.Dt[iSCnt][iMBad2] == 0xBBBB || bad.Dt[iSCnt][iMBad2] == 0xCCCC)){
					++iMBad2;
				}
				iSBad2 = iSCnt;
				while(iSBad2 < SubPixel - 1 && (bad.Dt[iSBad2][iMCnt] == 0xBBBB || bad.Dt[iSBad2][iMCnt] == 0xCCCC)){
					++iSBad2;
				}
				do{
					bBadExist = false;
					for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
						if((bad.Dt[iSBad1][iMCnt2] == 0xBBBB || bad.Dt[iSBad1][iMCnt2] == 0xCCCC) && iSBad1 > 0){
							bBadExist = true;
							--iSBad1;
							iMCnt2 = iMBad1 - 1;    //resetして最初からcheck
						}
					}
					for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
						if((bad.Dt[iSBad2][iMCnt2] == 0xBBBB || bad.Dt[iSBad2][iMCnt2] == 0xCCCC) && iSBad2 < SubPixel - 1){
							bBadExist = true;
							++iSBad2;
							iMCnt2 = iMBad1 - 1;    //resetして最初からcheck
						}
					}
					for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
						if((bad.Dt[iSCnt2][iMBad1] == 0xBBBB || bad.Dt[iSCnt2][iMBad1] == 0xCCCC) && iMBad1 > 0){
							bBadExist = true;
							--iMBad1;
							iSCnt2 = iSBad1 - 1;    //resetして最初からcheck
						}
					}
					for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
						if((bad.Dt[iSCnt2][iMBad2] == 0xBBBB || bad.Dt[iSCnt2][iMBad2] == 0xCCCC) && iMBad2 < MainPixel - 1){
							bBadExist = true;
							++iMBad2;
							iSCnt2 = iSBad1 - 1;    //resetして最初からcheck
						}
					}
				}while(bBadExist);

				for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
					for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
						if(bad.Dt[iSCnt2][iMCnt2] == 0xBBBB
								|| bad.Dt[iSCnt2][iMCnt2] == 0xCCCC
								|| bad.Dt[iSCnt2][iMCnt2] == 0xA000){
							bad.Dt[iSCnt2][iMCnt2] = bad.DtMax;
							bad2.Dt[iSCnt2][iMCnt2] = bad2.DtMax;
						}
					}
				}


				//
				// 欠陥補正   ※欠陥の周辺1画素の画素値をnPerとする。
				//
				if(bad.Dt[iSBad1][iMBad1] == 0
						&& ((iMBad1 + 1 < bad.MainPixel && bad.Dt[iSBad1][iMBad1 + 1] == bad.DtMax)
							|| (iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad1] == bad.DtMax)
							|| (iMBad1 + 1 < bad.MainPixel && iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad1 + 1] == bad.DtMax))){
					bad2.Dt[iSBad1][iMBad1] = nPer;
					//bad.Dt[iSBad1][iMBad1] = nPer;
				}
				if(bad.Dt[iSBad1][iMBad2] == 0
						&& ((iMBad2 - 1 >= 0 && bad.Dt[iSBad1][iMBad2 - 1] == bad.DtMax)
							|| (iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad2] == bad.DtMax)
							|| (iMBad2 - 1 >= 0 && iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad2 - 1] == bad.DtMax))){
					bad2.Dt[iSBad1][iMBad2] = nPer;
				}
				if(bad.Dt[iSBad2][iMBad1] == 0
						&& ((iMBad1 + 1 < bad.MainPixel && bad.Dt[iSBad2][iMBad1 + 1] == bad.DtMax)
							|| (iSBad2 - 1 >= 0 && bad.Dt[iSBad2 - 1][iMBad1] == bad.DtMax)
							|| (iSBad2 - 1 >= 0 && iMBad1 + 1 < bad.MainPixel && bad.Dt[iSBad2 - 1][iMBad1 + 1] == bad.DtMax))){
					bad2.Dt[iSBad2][iMBad1] = nPer;
				}
				if(bad.Dt[iSBad2][iMBad2] == 0
						&& ((iMBad2 - 1 >= 0 && bad.Dt[iSBad2][iMBad2 - 1] == bad.DtMax)
							|| (iSBad2 - 1 >= 0 &&bad.Dt[iSBad2 - 1][iMBad2] == bad.DtMax)
							|| (iSBad2 - 1 >= 0 && iMBad2 - 1 >= 0 &&bad.Dt[iSBad2 - 1][iMBad2 - 1] == bad.DtMax))){
					bad2.Dt[iSBad2][iMBad2] = nPer;
				}
				for(iSCnt2 = iSBad1 + 1; iSCnt2 < iSBad2; ++iSCnt2){
					if(bad.Dt[iSCnt2][iMBad1] == 0
							&& (bad.Dt[iSCnt2 - 1][iMBad1] == bad.DtMax || (iMBad1 + 1 < bad.MainPixel && bad.Dt[iSCnt2 - 1][iMBad1 + 1] == bad.DtMax)
							|| (iMBad1 + 1 < bad.MainPixel && bad.Dt[iSCnt2][iMBad1 + 1] == bad.DtMax)
							|| bad.Dt[iSCnt2 + 1][iMBad1] == bad.DtMax || (iMBad1 + 1 < bad.MainPixel && bad.Dt[iSCnt2 + 1][iMBad1 + 1] == bad.DtMax))){
						bad2.Dt[iSCnt2][iMBad1] = nPer;
					}
					if(bad.Dt[iSCnt2][iMBad2] == 0
							&& ((iMBad2 - 1 >= 0 && bad.Dt[iSCnt2 - 1][iMBad2 - 1] == bad.DtMax) || bad.Dt[iSCnt2 - 1][iMBad2] == bad.DtMax
							|| (iMBad2 - 1 >= 0 && bad.Dt[iSCnt2][iMBad2 - 1] == bad.DtMax)
							|| (iMBad2 - 1 >= 0 && bad.Dt[iSCnt2 + 1][iMBad2 - 1] == bad.DtMax) || bad.Dt[iSCnt2 + 1][iMBad2] == bad.DtMax)){
						bad2.Dt[iSCnt2][iMBad2] = nPer;
					}
				}
				for(iMCnt2 = iMBad1 + 1; iMCnt2 < iMBad2; ++iMCnt2){
					if(bad.Dt[iSBad1][iMCnt2] == 0
							&& (bad.Dt[iSBad1][iMCnt2 - 1] == bad.DtMax || (iSBad1 - 1 >= 0 && bad.Dt[iSBad1 - 1][iMCnt2 + 1] == bad.DtMax)
							|| (iSBad1 + 1 < bad.SubPixel && (bad.Dt[iSBad1 + 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSBad1 + 1][iMCnt2] == bad.DtMax || bad.Dt[iSBad1 + 1][iMCnt2 + 1] == bad.DtMax)))){
						bad2.Dt[iSBad1][iMCnt2] = nPer;
					}
					if(bad.Dt[iSBad2][iMCnt2] == 0
							&& ((iSBad2 - 1 >= 0 && (bad.Dt[iSBad2 - 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSBad2 - 1][iMCnt2] == bad.DtMax || bad.Dt[iSBad2 - 1][iMCnt2 + 1] == bad.DtMax))
							|| bad.Dt[iSBad2][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSBad2][iMCnt2 + 1] == bad.DtMax)){
						bad2.Dt[iSBad2][iMCnt2] = nPer;
					}
				}
				for(iSCnt2 = iSBad1 + 1; iSCnt2 < iSBad2; ++iSCnt2){
					for(iMCnt2 = iMBad1 + 1; iMCnt2 < iMBad2; ++iMCnt2){
						if(bad.Dt[iSCnt2][iMCnt2] == 0
								&& (bad.Dt[iSCnt2 - 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSCnt2 - 1][iMCnt2] == bad.DtMax || bad.Dt[iSCnt2 - 1][iMCnt2 + 1] == bad.DtMax
								|| bad.Dt[iSCnt2][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSCnt2][iMCnt2 + 1] == bad.DtMax
								|| bad.Dt[iSCnt2 + 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSCnt2 + 1][iMCnt2] == bad.DtMax || bad.Dt[iSCnt2 + 1][iMCnt2 + 1] == bad.DtMax)){
							bad2.Dt[iSCnt2][iMCnt2] = nPer;
						}
					}
				}





				//
				//欠陥補正値の計算(１つの欠陥塊の中でループをまわす)
				//
				for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
					for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
						if(bad.Dt[iSCnt2][iMCnt2] == bad.DtMax){
							//tmp
							if (2271 == iMCnt2 && 838 == iSCnt2) {
								int aaa = 1;
							}
							if (625 == iMCnt2 && 2155 == iSCnt2) {
								int aaa = 1;
							}

							if(iMBad2 - iMBad1 < iSIZE_MAX + 2 && iSBad2 - iSBad1 < iSIZE_MAX + 2){  // 090408に+2を追加

								if (0 == iAlgoIndex) {//従来アルゴリズム
									fSum = 0.0;
									fSumWeight = 0.0;
									for(iSCnt3 = iSBad1; iSCnt3 <= iSBad2; ++iSCnt3){
										for(iMCnt3 = iMBad1; iMCnt3 <= iMBad2; ++iMCnt3){
											if(bad2.Dt[iSCnt3][iMCnt3] == nPer){
												fSum += Dt[iSCnt3][iMCnt3] * fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];
												fSumWeight += fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];
											}
										}
									}
									if(fSumWeight > 0){  //エラー処理090330追加
										SetVal(iSCnt2, iMCnt2, fSum / fSumWeight);
									}
								}
								else if (1 == iAlgoIndex) {//欠陥の左右の正常画素で補間(120723 ttsuji)
 #if 0
									SetVal(iSCnt2, iMCnt2, 0);//欠陥画素の左右方向の正常画素で補間(補正対象画素を確認するためあえて白く補正)

 #elif 0   			//欠陥の左の正常画素で補間
									int iMtmp = iMBad1;
									while (nPer != bad2.Dt[iSCnt2][iMtmp]){
										iMtmp++;
									}
									//正常画素が１つ内側にもないか調べる
									if (nPer == bad2.Dt[iSCnt2][iMtmp + 1]) {                     14										iMtmp++;
									}
									SetVal(iSCnt2, iMCnt2, Dt[iSCnt2][iMtmp]);//欠陥画素の左右方向の正常画素で補間
 #elif 1			//欠陥の左右の正常画素で補間（画質的には素性が最も良い（従来補正よりも良い））

									const int iMSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
									if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt2, iMCnt2, iMSearchInterval)){
										CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
										if (0 == iGridDirection) {//縦グリッドならば主副反転
											bad.MainSub();
											MainSub();
										}
										return false;
									}
 #endif
								}
								else if (2 == iAlgoIndex) {//欠陥の上下左右の正常画素で補間（なぜか上下方向の補間を行なうと計算が無限に続く）
 #if 0
									int iMtmp1 = iMBad1;
									int iMtmp2 = iMBad2;
									int iStmp1 = iSBad1;
									int iStmp2 = iSBad2;
									int iSShift = 0;

									//欠陥の左側
//									while (nPer != bad2.Dt[iSCnt2][iMtmp1] && iMtmp1 < bad2.MainPixel){
									while (nPer != bad2.Dt[iSCnt2][iMtmp1]){
										iMtmp1++;                                                   									}
									//正常画素が１つ内側にもないか調べる           +++++++++
									if (nPer == bad2.Dt[iSCnt2][iMtmp1 + 1]) {
										iMtmp1++;
									}
									//欠陥の右側
//									while (nPer != bad2.Dt[iSCnt2][iMtmp2] && iMtmp2 > 0){
									while (nPer != bad2.Dt[iSCnt2][iMtmp2]){
										iMtmp2--;
									}
									//正常画素が１つ内側にもないか調べる
									if (nPer == bad2.Dt[iSCnt2][iMtmp2 - 1]) {
										iMtmp2--;
									}


									if (iSCnt2 > 75) {
										int a = 1;
									}
									if (iSCnt2 > 76) {
										int a = 1;
									}

									//欠陥の上側
/*									while (nPer != bad2.Dt[iStmp1][iMCnt2] && iStmp1 < bad2.SubPixel){
									while (nPer != bad2.Dt[iStmp1][iMCnt2]){
										iStmp1++;
									}
									//正常画素が１つ内側にもないか調べる
									if (nPer == bad2.Dt[iStmp1 + 1][iMCnt2]) {
										iStmp1++;
									}
*/
									iSShift = 1;
									while (0 != bad.Dt[iSCnt2 - iSShift][iMCnt2] && (iSCnt2 - iSShift) >= 0){
										iSShift++;
									}
									iStmp1 = iSCnt2 - iSShift;



									//欠陥の下側
/*//								while (nPer != bad2.Dt[iStmp2][iMCnt2] && iStmp2 > 0){
									while (nPer != bad2.Dt[iStmp2][iMCnt2]){
										iStmp2--;
									}
									//正常画素が１つ内側にもないか調べる
									if (nPer == bad2.Dt[iStmp2 - 1][iMCnt2]) {
										iStmp2--;
									}
*/
									iSShift = 1;
									while (0 != bad.Dt[iSCnt2 + iSShift][iMCnt2] && (iSCnt2 + iSShift) < bad.SubPixel){
										iSShift++;
									}
									iStmp2 = iSCnt2 + iSShift;

									float QLtate, QLyoko;

									//正常画素の左右の画素で補間
									if (iMtmp2 - iMtmp1) {
										QLyoko = (Dt[iSCnt2][iMtmp1]*(iMtmp2 - iMCnt2) + Dt[iSCnt2][iMtmp2]*(iMCnt2 - iMtmp1))/(float)(iMtmp2 - iMtmp1);
									}
									else{
										QLyoko = Dt[iSCnt2][iMtmp1];
									}

									if (iStmp2 - iStmp1) {
										QLtate = (Dt[iStmp1][iMCnt2]*(iStmp2 - iSCnt2) + Dt[iStmp2][iMCnt2]*(iSCnt2 - iStmp1))/(float)(iStmp2 - iStmp1);
									}
									else{
										QLtate = Dt[iStmp1][iMCnt2];
									}
									SetVal(iSCnt2, iMCnt2, 0.5*(QLtate +QLyoko));//欠陥画素の左右方向の正常画素で補間
//									SetVal(iSCnt2, iMCnt2, QLyoko);//欠陥画素の左右方向の正常画素で補間
//									SetVal(iSCnt2, iMCnt2, QLtate);//欠陥画素の左右方向の正常画素で補間
 #else
									//欠陥の上下左右の正常画素で補間（なぜか上下方向の補間を行なうと計算が無限に続く）
									int iMtmp1 = iMBad1;
									int iMtmp2 = iMBad2;
									int iStmp1 = iSBad1;
									int iStmp2 = iSBad2;

									//欠陥の左側
//									while (nPer != bad2.Dt[iSCnt2][iMtmp1] && iMtmp1 < bad2.MainPixel){
									while (nPer != bad2.Dt[iSCnt2][iMtmp1]){
										iMtmp1++;
									}
									//正常画素が１つ内側にもないか調べる
									if (nPer == bad2.Dt[iSCnt2][iMtmp1 + 1]) {
										iMtmp1++;
									}
									//欠陥の右側
//									while (nPer != bad2.Dt[iSCnt2][iMtmp2] && iMtmp2 > 0){
									while (nPer != bad2.Dt[iSCnt2][iMtmp2]){
										iMtmp2--;
									}
									//正常画素が１つ内側にもないか調べる
									if (nPer == bad2.Dt[iSCnt2][iMtmp2 - 1]) {
										iMtmp2--;
									}
/*
									//欠陥の上側
									for(iSCnt3 = iSBad1; iSCnt3 <= iSBad2; ++iSCnt3){
										for(iMCnt3 = iMBad1; iMCnt3 <= iMBad2; ++iMCnt3){
											if(iMCnt2 == iMCnt3 && bad2.Dt[iSCnt3][iMCnt3] == nPer){
												iStmp1 = iSCnt3;
												if (bad2.Dt[iSCnt3 + 1][iMCnt3] == nPer) {
													iStmp1 = iSCnt3 + 1;
												}
												iSCnt3 = iSBad2;
												break;
											}
										}
									}
									//欠陥の下側
									for(iSCnt3 = iSBad2; iSCnt3 >= iSBad1; --iSCnt3){
										for(iMCnt3 = iMBad1; iMCnt3 <= iMBad2; ++iMCnt3){
											if(iMCnt2 == iMCnt3 && bad2.Dt[iSCnt3][iMCnt3] == nPer){
												iStmp2 = iSCnt3;
												if (bad2.Dt[iSCnt3 - 1][iMCnt3] == nPer) {
													iStmp2 = iSCnt3 - 1;
												}
												iSCnt3 = iSBad1;
												break;
											}
										}
									}
*/

									float QLtate, QLyoko;

									//正常画素の左右の画素で補間
									if (iMtmp2 - iMtmp1) {
										QLyoko = (Dt[iSCnt2][iMtmp1]*(iMtmp2 - iMCnt2) + Dt[iSCnt2][iMtmp2]*(iMCnt2 - iMtmp1))/(float)(iMtmp2 - iMtmp1);
									}
									else{
										QLyoko = Dt[iSCnt2][iMtmp1];
									}

									if (iStmp2 - iStmp1) {
										QLtate = (Dt[iStmp1][iMCnt2]*(iStmp2 - iSCnt2) + Dt[iStmp2][iMCnt2]*(iSCnt2 - iStmp1))/(float)(iStmp2 - iStmp1);
									}
									else{
										QLtate = Dt[iStmp1][iMCnt2];
									}
									SetVal(iSCnt2, iMCnt2, 0.5*(QLtate +QLyoko));//欠陥画素の左右方向の正常画素で補間
//									SetVal(iSCnt2, iMCnt2, QLyoko);//欠陥画素の左右方向の正常画素で補間
//									SetVal(iSCnt2, iMCnt2, QLtate);//欠陥画素の左右方向の正常画素で補間
 #endif
								}
								else if (3 == iAlgoIndex) {//欠陥の左右の正常画素で補間(120823 ttsuji)
									if (0 == iGridDirection) {//縦グリッドならば縦方向の隣接画素で補間（画素密度変換後画像だと縦1画素とばしに相当）
										const int iMSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
										if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt2, iMCnt2, iMSearchInterval)){
											CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
											bad.MainSub();
											MainSub();
											return false;
										}
									}
									else{//横グリッドならば横方向に1画素とばしの画素で補間（画素密度変換後画像だと横隣接画像に相当）
										const int iMSearchInterval = 2;//1画素ずつ欠陥周囲の正常画素を探索
										if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt2, iMCnt2, iMSearchInterval)){
											CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
											return false;
										}
									}
								}
								else if (4 == iAlgoIndex) {//newアルゴリズム(Multi Resolution読みの画素密度変換前の画像をハンドリングする場合)
									if (0 == iGridDirection) {//縦グリッドならば縦方向の隣接画素で補間（画素密度変換後画像だと斜め上2画素に相当）
										const int iSearchMode = 0;//縦方向の正常画素のみから補間
										if (!bDefectCorrectionByMultiResoPixel(bad, iSCnt2, iMCnt2, iSearchMode)){
											CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByMultiResoPixel()", "Error", MB_OK);
											return false;
										}
									}
									else{//横グリッドならば横方向に1画素とばしの画素で補間（画素密度変換後画像だと横隣接画像に相当）
										const int iSearchMode = 1;//横方向の正常画素のみから補間
										if (!bDefectCorrectionByMultiResoPixel(bad, iSCnt2, iMCnt2, iSearchMode)){
											CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByMultiResoPixel()", "Error", MB_OK);
											return false;
										}
									}

								}
								else if (5 == iAlgoIndex) {//従来アルゴリズム(周辺の矩形画素で埋める)
									fSum = 0.0;
									fSumWeight = 0.0;
									for(iSCnt3 = iSBad1; iSCnt3 <= iSBad2; ++iSCnt3){
										for(iMCnt3 = iMBad1; iMCnt3 <= iMBad2; ++iMCnt3){
#if 1
											//周辺矩形の正常画素のみを用いて補正(異常画素は除外する場合)
											if ((iSBad1 == iSCnt3 && (iMBad1 <= iMCnt3 && iMCnt3 <= iMBad2))
											 || (iSBad2 == iSCnt3 && (iMBad1 <= iMCnt3 && iMCnt3 <= iMBad2))
											 || (iMBad1 == iMCnt3 && (iSBad1 <= iSCnt3 && iSCnt3 <= iSBad2))
											 || (iMBad2 == iMCnt3 && (iSBad1 <= iSCnt3 && iSCnt3 <= iSBad2))){
//												if (bad.Dt[iSCnt3][iMCnt3] == bad.DtMax) {//周辺矩形が欠陥画素であった場合
//												if (bad.Dt[iSCnt3][iMCnt3] == 0xDDDD) {//周辺矩形が欠陥画素であった場合
												if (bad.Dt[iSCnt3][iMCnt3] == 0xDDDD || bad.Dt[iSCnt3][iMCnt3] == 0xFFFF) {//周辺矩形が欠陥画素であった場合
													continue;//とりあえず補間に用いる画素からは除外する
												}
												fSum += Dt[iSCnt3][iMCnt3] * fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];
												fSumWeight += fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];

											}
#else
											//周辺矩形の正常画素のみを用いて補正（異常画素は隣接の正常画素で代替する場合）
											if (iSBad1 == iSCnt3 && (iMBad1 <= iMCnt3 && iMCnt3 <= iMBad2))
											|| (iSBad2 == iSCnt3 && (iMBad1 <= iMCnt3 && iMCnt3 <= iMBad2)){
												int iMtmp = iMCnt3;
												int iDist = 1;
												//周辺矩形が欠陥画素の場合は隣の正常画素で補間
												while ((bad.Dt[iSCnt3][iMtmp] == 0xDDDD || bad.Dt[iSCnt3][iMtmp] == 0xFFFF) && iMtmp < bad.MainPixel-2){
 #if 0
//													iMtmp++;
 #else	//実装中

													if (bad.Dt[iSCnt3][iMtmp + iDist] == 0xDDDD || bad.Dt[iSCnt3][iMtmp + iDist] == 0xFFFF) {
														if (bad.Dt[iSCnt3][iMtmp - iDist] == 0xDDDD || bad.Dt[iSCnt3][iMtmp - iDist] == 0xFFFF) {
															iDist++;
															continue;
													}
													else{
                                                    	break;
													}
 #endif
												}
												fSum += Dt[iSCnt3][iMtmp] * fWeight[abs(iSCnt3 - iSCnt2)][abs(iMtmp - iMCnt2)];
												fSumWeight += fWeight[abs(iSCnt3 - iSCnt2)][abs(iMtmp - iMCnt2)];
											}
											else if (iMBad1 == iMCnt3 && (iSBad1 <= iSCnt3 && iSCnt3 <= iSBad2)){
											}
											else if (iMBad2 == iMCnt3 && (iSBad1 <= iSCnt3 && iSCnt3 <= iSBad2)){
											}
												fSum += Dt[iSCnt3][iMCnt3] * fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];
												fSumWeight += fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];

											 }
#endif
										}
									}
									if(fSumWeight > 0){  //エラー処理090330追加
										SetVal(iSCnt2, iMCnt2, fSum / fSumWeight);
									}
								}

							}
							else{


								/*   //090408 else文には入らないアルゴに修正した
								iSearchCnt = 1;
								iCnt = 0;
								iSum = 0;
								for(;;){
									for(iSCnt3 = - iSearchCnt; iSCnt3 <= iSearchCnt; ++iSCnt3){
										for(iMCnt3 = - iSearchCnt; iMCnt3 <= iSearchCnt; ++iMCnt3){
											if(iSCnt2 + iSCnt3 >= 0 && iSCnt2 + iSCnt3 < SubPixel
													&& iMCnt2 + iMCnt3 >= 0 && iMCnt2 + iMCnt3 < MainPixel
													&& bad.Dt[iSCnt2 + iSCnt3][iMCnt2 + iMCnt3] != bad.DtMax){  //

												iSum += Dt[iSCnt2 + iSCnt3][iMCnt2 + iMCnt3];
												++iCnt;
											}
										}
									}
									if(iCnt != 0){
										SetVal(iSCnt2, iMCnt2, float(iSum) / iCnt);
										break;
									}else{
										++iSearchCnt;
									}
								}
								*/

							}

						}
					}
				}



				//欠陥のreset
				for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
					for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
						if(bad.Dt[iSCnt2][iMCnt2] == bad.DtMax){
							bad.Dt[iSCnt2][iMCnt2] = 0xAABB;   //欠陥補正終了のためMax値でなくした。
						}
						bad2.Dt[iSCnt2][iMCnt2] = 0;
					}
				}



			}
		}
	}


	/*  //デバッグ用
	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			bad2.Dt[iSCnt][iMCnt] = bad.Dt[iSCnt][iMCnt];
		}
	}
	bad2.WriteTma("Debug_TestImg");
	*/


	//デバッグ用
//	bad2.WriteTma("Debug_TestImg");


	//新規アルゴ（グリッドのリス目方向の画素のみから補間）の場合は、縦グリッドならば主副反転していたのを元に戻す
	if (1 == iAlgoIndex || 3 == iAlgoIndex) {//newアルゴリズム
		if (0 == iGridDirection) {//縦グリッドならば主副反転
			bad.MainSub();
			MainSub();
		}
	}

	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥補正5　　	渡邊殿org(100326)
		大サイズ欠陥補正（欠陥補正2が既にかかっていることが前提）
		大サイズ欠陥にグリッドリス目パターンを載せる
		欠陥画素の上下左右４方向の画素を用いて補間
*/
bool TSIMARS::bDefectCorrection5(TTMA& bad)
{
	int iMCnt, iSCnt;
	for(int iMCnt = 0; iMCnt < bad.MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < bad.SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] == 0xAABB){
				bad.Dt[iSCnt][iMCnt] = 0xFFFF;
			}
		}
	}


	TSIMARS img2 = *this;  // 20100329 補正用データ

	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			/*
			if(bad.Dt[iSCnt][iMCnt] > 0x1919
					&& bad.Dt[iSCnt][iMCnt] <= 65535){
			*/
			if(bad.Dt[iSCnt][iMCnt] == 0xFFFF){
				int iData = 0;
				int iDataCnt = 0;
				int iCnt_Bad = 1;
				//int iAve_Data = 0;
				int iAve_Data =  Dt[iSCnt][iMCnt];
				int iAve_DataS = Dt[iSCnt][iMCnt];
				int iAve_DataM = Dt[iSCnt][iMCnt];
				int iCnt_BadS = 1;
				int iCnt_BadM = 1;
				int iY = iSCnt - 1;

				while(iY >= 0 && bad.Dt[iY][iMCnt] > 0x1919){
					iAve_Data += Dt[iY][iMCnt];
					++iCnt_Bad;
					iAve_DataS += Dt[iY][iMCnt];    //
					++iCnt_BadS;                        //
					--iY;
				}
				//iData += img.Dt[iY][iMCnt];
				if(iY - 1 >= 0 && bad.Dt[iY - 1][iMCnt] >= 0 && bad.Dt[iY - 1][iMCnt] <= 0x1919){
					iData += Dt[iY][iMCnt];
					iData += Dt[iY - 1][iMCnt];
					iDataCnt += 2;
				}
				iY = iSCnt + 1;
				while(iY < SubPixel && bad.Dt[iY][iMCnt] > 0x1919){
					iAve_Data += Dt[iY][iMCnt];
					++iCnt_Bad;
					iAve_DataS += Dt[iY][iMCnt];     //
					++iCnt_BadS;                         //
					++iY;
				}
				//iData += img.Dt[iY][iMCnt];
				if(iY + 1 < bad.SubPixel && bad.Dt[iY + 1][iMCnt] >= 0 && bad.Dt[iY + 1][iMCnt] <= 0x1919){
					iData += Dt[iY][iMCnt];
					iData += Dt[iY + 1][iMCnt];
					iDataCnt += 2;
				}
				int iX = iMCnt - 1;
				while(iX >= 0 && bad.Dt[iSCnt][iX] > 0x1919){
					iAve_Data += Dt[iSCnt][iX];
					++iCnt_Bad;
					iAve_DataM += Dt[iSCnt][iX];
					++iCnt_BadM;
					--iX;
				}
				//iData += img.Dt[iSCnt][iX];
				if(iX - 1 >= 0 && bad.Dt[iSCnt][iX - 1] >= 0 && bad.Dt[iSCnt][iX - 1] <= 0x1919){
					iData += Dt[iSCnt][iX];
					iData += Dt[iSCnt][iX - 1];
					iDataCnt += 2;
				}
				iX = iMCnt + 1;
				while(iX < MainPixel && bad.Dt[iSCnt][iX] > 0x1919){
					iAve_Data += Dt[iSCnt][iX];
					++iCnt_Bad;
					iAve_DataM += Dt[iSCnt][iX];      //
					++iCnt_BadM;                          //
					++iX;
				}
				//iData += img.Dt[iSCnt][iX];
				if(iX + 1 < bad.MainPixel && bad.Dt[iSCnt][iX + 1] >= 0 && bad.Dt[iSCnt][iX + 1] <= 0x1919){
					iData += Dt[iSCnt][iX];
					iData += Dt[iSCnt][iX + 1];
					iDataCnt += 2;
				}
				if(iCnt_Bad > 0){
					iAve_Data /= iCnt_Bad;
				}

			   ////////
				/*
				if(iCnt_BadS & 1 == 1 && iCnt_BadS > 1){
					iAve_DataS -= img.Dt[iY-1][iMCnt];
					--iCnt_BadS;
				}
				if(iCnt_BadS > 0){
					iAve_DataS /= iCnt_BadS;
				}
				if(iCnt_BadM & 1 == 1 && iCnt_BadM > 1){
					iAve_DataM -= img.Dt[iSCnt][iX-1];
					--iCnt_BadM;
				}
				if(iCnt_BadM > 0){
					iAve_DataM /= iCnt_BadM;
				}
				iAve_Data = (iAve_DataS + iAve_DataM) / 2;
					  */
			  //////////
				//if(iDataCnt > 0){

				if(iDataCnt > 0 && iCnt_Bad > 0){
					int iDt = (Dt[iSCnt][iMCnt] + iAve_Data) / 2 + (iData / iDataCnt - iAve_Data);

					//低周波(欠陥補正2結果) +　高周波（グリッドリス目）

					//img.SetVal(iSCnt, iMCnt, iDt);
					img2.SetVal(iSCnt, iMCnt, iDt);  // 20100329
				}
			}
		}
	}

	*this = img2;  // 20100329

	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥補正1　　	渡邊殿org
		小サイズ欠陥補正（サイズ3画素以内）
		欠陥画素の上下左右の正常画素のうち、ランダムに選んだ2画素の平均値で埋める
*/
bool TSIMARS::bDefectCorrection1(TTMA& bad)
{
	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] > 0
					&& bad.Dt[iSCnt][iMCnt] <= 0x1919){    //欠陥1補正対象画素

				int iPos1 = int(bad.Dt[iSCnt][iMCnt] / 0x0100);//16進数で2桁小さくシフト
				int iPos2 = int(bad.Dt[iSCnt][iMCnt] % 0x0100);//16進数で下2桁のみ抽出

				int iPos1S = iSCnt - 2 + (iPos1 - 1) / 5;
				int iPos1M = iMCnt - 2 + (iPos1 - 1) % 5;
				int iPos2S = iSCnt - 2 + (iPos2 - 1) / 5;
				int iPos2M = iMCnt - 2 + (iPos2 - 1) % 5;

				/*
				if((iPos1S >= 0 && iPos1S < bad.SubPixel)
						&& (iPos1M >= 0 && iPos1M < bad.MainPixel)
						&& (iPos2S >= 0 && iPos2S < bad.SubPixel)
						&& (iPos2M >= 0 && iPos2M < bad.MainPixel)){

					int iData1 = img.Dt[iPos1S][iPos1M];
					int iData2 = img.Dt[iPos2S][iPos2M];
					img.Dt[iSCnt][iMCnt] = (iData1 + iData2) / 2;   //切捨て

				}
				*/

				//エラー処理付き
				if((iPos1S >= 0 && iPos1S < bad.SubPixel)
						&& (iPos1M >= 0 && iPos1M < bad.MainPixel)
						&& (iPos2S >= 0 && iPos2S < bad.SubPixel)
						&& (iPos2M >= 0 && iPos2M < bad.MainPixel)){

					int iData1 = Dt[iPos1S][iPos1M];
					int iData2 = Dt[iPos2S][iPos2M];

					if(bad.Dt[iPos1S][iPos1M] > 0 || bad.Dt[iPos2S][iPos2M] > 0
							|| iPos1 <= 0 || iPos1 == 13 || iPos1 > 25
							|| iPos2 <= 0 || iPos2 == 13 || iPos2 > 25){

						//エラー処理（何もしない）
					}
					else{
						Dt[iSCnt][iMCnt] = (iData1 + iData2) / 2;   //切捨て
					}
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//欠陥補正3

bool TSIMARS::bDefectCorrection3(const int iMainMed, const int iSubMed, const float fTh)
{
	TTMA ImageMedian;
	ImageMedian = *this;
	ImageMedian.Median(iSubMed, iMainMed);

	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			float fNum;
			fNum = (float)ImageMedian.Dt[iSCnt][iMCnt] - (float)Dt[iSCnt][iMCnt];
			if(fNum >= fTh || fNum <= - fTh){//メディアンフィルター後の差分値が閾値を越える場合はメディアン後の値とする
				Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
//欠陥補正3改（副方向の1Dメディアン差分の閾値判定に修正）

bool TSIMARS::bDefectCorrection3by1DMedianDiff(const int iSubMed, const float fTh)
{
	TTMA ImageMedian;
	ImageMedian = *this;
	ImageMedian.SubMedian(iSubMed);

	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			float fNum;
			fNum = (float)ImageMedian.Dt[iSCnt][iMCnt] - (float)Dt[iSCnt][iMCnt];
			if(fNum >= fTh || fNum <= - fTh){//メディアンフィルター後の差分値が閾値を越える場合はメディアン後の値とする
				Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
//欠陥補正3改（差分の閾値判定を比の閾値判定に修正）
//   fTh > 1　とする。

bool TSIMARS::bDefectCorrection3byMedianRatio(const int iMainMed, const int iSubMed, const float fTh)
{
	if (fTh <= 1.) {
		CApplication::GetInstance()->MessageBox("fThは1より大きい値としてください.", "Error", MB_OK);
		return false;
	}

	TTMA ImageMedian;
	ImageMedian = *this;
	ImageMedian.Median(iSubMed, iMainMed);
	float fThInv = 1 / fTh;

	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			float fRate;
			if (!Dt[iSCnt][iMCnt]) {//アンダーフローしている点は異常画素の可能性あるため、メディアン値で置換する
				Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
			}
			else{
				fRate = (float)ImageMedian.Dt[iSCnt][iMCnt] / (float)Dt[iSCnt][iMCnt];
				if(fRate > fTh || fRate < fThInv){//メディアンフィルター後の比が閾値を越える場合はメディアン後の値とする
					Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//欠陥補正3改（差分の閾値判定を比の閾値判定に修正）
//   fTh > 1　とする。

bool TSIMARS::bDefectCorrection3by1DMedianRatio(const int iSubMed, const float fTh)
{
	if (fTh <= 1.) {
		CApplication::GetInstance()->MessageBox("fThは1より大きい値としてください.", "Error", MB_OK);
		return false;
	}

	TTMA ImageMedian;
	ImageMedian = *this;
	ImageMedian.SubMedian(iSubMed);
	float fThInv = 1 / fTh;

	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			float fRate;
			if (!Dt[iSCnt][iMCnt]) {//アンダーフローしている点は異常画素の可能性あるため、メディアン値で置換する
				Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
			}
			else{
				fRate = (float)ImageMedian.Dt[iSCnt][iMCnt] / (float)Dt[iSCnt][iMCnt];
				if(fRate > fTh || fRate < fThInv){//メディアンフィルター後の比が閾値を越える場合はメディアン後の値とする
					Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//欠陥補正3改（差分の閾値判定を比の閾値判定に修正）
//   fTh > 1　とする。

bool TSIMARS::bDefectCorrection3byMain1DMedianRatio(const int iMainMed, const float fTh)
{
	if (fTh <= 1.) {
		CApplication::GetInstance()->MessageBox("fThは1より大きい値としてください.", "Error", MB_OK);
		return false;
	}

	TTMA ImageMedian;
	ImageMedian = *this;
	ImageMedian.MainMedian(iMainMed);
	ImageMedian.MainPixelExpand(iMainMed / 2);//画素延長処理

	float fThInv = 1 / fTh;

	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			float fRate;
			if (!Dt[iSCnt][iMCnt]) {//アンダーフローしている点は異常画素の可能性あるため、メディアン値で置換する
				Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
			}
			else{
				fRate = (float)ImageMedian.Dt[iSCnt][iMCnt] / (float)Dt[iSCnt][iMCnt];
				if(fRate > fTh || fRate < fThInv){//メディアンフィルター後の比が閾値を越える場合はメディアン後の値とする
					Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//欠陥補正3改（メディアン比が閾値以上であり、かつベースQLが別の閾値以上であれば、メディアン値に置換）
//   fTh1 > 1　とする。

bool TSIMARS::bDefectCorrection3by1DMedianRatioAndBaseQL(const int iSubMed, const float fTh, const int iThBaseQL)
{
	if (fTh <= 1.) {
		CApplication::GetInstance()->MessageBox("fThは1より大きい値としてください.", "Error", MB_OK);
		return false;
	}
	if (iThBaseQL < 0) {
		CApplication::GetInstance()->MessageBox("ベースQL閾hは0以上の値としてください.", "Error", MB_OK);
		return false;
	}

	TTMA ImageMedian;
	ImageMedian = *this;
	ImageMedian.SubMedian(iSubMed);
	float fThInv = 1 / fTh;

	for(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for(int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			float fRate;
			if (!Dt[iSCnt][iMCnt]) {//アンダーフローしている点は異常画素の可能性あるため、メディアン値で置換する
				Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
			}
			else{
				fRate = (float)ImageMedian.Dt[iSCnt][iMCnt] / (float)Dt[iSCnt][iMCnt];
//				if (fRate > fTh || fRate < fThInv){//メディアンフィルター後の比が閾値を越える場合はメディアン後の値とする
				if ((fRate > fTh || fRate < fThInv) && ImageMedian.Dt[iSCnt][iMCnt] > iThBaseQL){//メディアン比が閾値以上であり、かつベースQLが別の閾値以上であれば、メディアン値に置換
					Dt[iSCnt][iMCnt] = ImageMedian.Dt[iSCnt][iMCnt];
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//	FF欠陥補正
//補正非対象画素も補正する(上下左右の簡易補正)。線欠陥補正評価のため
//

bool TSIMARS::bDefectCorrectionFFFF(TTMA& bad)
{

	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			if(bad.Dt[iSCnt][iMCnt] == 0xABCD){    //補正非対象
				int iData = 0;
				int iDCnt = 0;

				int iPos1S = iSCnt - 1;
				int XA = 1;
				while(XA == 1){
					if(iPos1S >= 0 && iPos1S < bad.SubPixel){
						if(bad.Dt[iPos1S][iMCnt] != 0xABCD){
							iData += Dt[iPos1S][iMCnt];
							++iDCnt;
							XA = 0;
						}else{
							--iPos1S;
						}
					}else{
						XA = 0;
					}
				}

				int iPos2S = iSCnt + 1;
				int XB = 1;
				while(XB == 1){
					if(iPos2S >= 0 && iPos2S < bad.SubPixel){
						if(bad.Dt[iPos2S][iMCnt] != 0xABCD){
							iData += Dt[iPos2S][iMCnt];
							++iDCnt;
							XB = 0;
						}else{
							++iPos2S;
						}
					}else{
						XB = 0;
					}
				}

				int iPos1M = iMCnt - 1;
				int XC = 1;
				while(XC == 1){
					if(iPos1M >= 0 && iPos1M < bad.MainPixel){
						if(bad.Dt[iSCnt][iPos1M] != 0xABCD){
							iData += Dt[iSCnt][iPos1M];
							++iDCnt;
							XC = 0;
						}else{
							--iPos1M;
						}
					}else{
						XC = 0;
					}
				}

				int iPos2M = iMCnt + 1;
				int XD = 1;
				while(XD == 1){
					if(iPos2M >= 0 && iPos2M < bad.MainPixel){
						if(bad.Dt[iSCnt][iPos2M] != 0xABCD){
							iData += Dt[iSCnt][iPos2M];
							++iDCnt;
							XD = 0;
						}else{
							++iPos2M;
						}
					}else{
						XD = 0;
					}
				}

				if(iDCnt != 0){
					Dt[iSCnt][iMCnt] = (iData + 0.5) / iDCnt;
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//SPC実装のゲイン補正（Bmapを用いた乗算処理（SELENE,ROOTS用））
	//
	//
	//               U16.0           U20.12->U20.0  ------------    U16.0
	//     in_data[] ------------------(x)----------| limiter  |--- out_data[]
	//                                  |           ------------
	//                                  | U4.12
	//                                  |
	//           U3.13   U4.28->U4.12   |
	//     hosei_data[]-----(x)---------|
	//                       |
	//                       | U1.15 kando
	//

bool  TSIMARS::bGainCorrectionSPC(TTMA& ShadingImg, int iCoeSense, int iShosuKirisute)
{
	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != MainPixel || ShadingImg.SubPixel != SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

#if 0
	for (int i = 0 ; i < SubPixel ; i++){
		for (int j = 0 ; j < MainPixel ; j++){
			float fDiv = 1 << 15;//2^15 = 32768
			float fDt = (float)Dt[i][j] * ShadingImg.Dt[i][j] / fDiv;
			SetVal(i, j, fDt);
		}
	}
#else
	//感度補正も同時に実行（SPC対応）
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U1.15(U3.13) 　()内はゲイン1倍が8192のとき
			i64 *= iCoeSense;//U2.30(U4.28) 　()内はゲイン1倍が8192のとき
			i64 /= 65536;//U2.14(U4.12) 　()内はゲイン1倍が8192のとき
			ShadingImg.Dt[i][j] = (int)i64;
		}
	}
//	ShadingImg.SaveTmaTdt(ExtractFilePath(in_str) + "Kando_Inverse_o_GainBeta.tma");


	TIIMG img_tmp = (TIIMG)*this;
	for (int i = 0 ; i < ShadingImg.SubPixel; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel; j++){
			__int64 i64 = Dt[i][j];//U16.0
			i64 *= ShadingImg.Dt[i][j];//U18.14(U20.12) 　()内はゲイン1倍が8192のとき
//			i64 /= 16384;//U18.0
			i64 /= iShosuKirisute;//U18.0(U20.0) 　()内はゲイン1倍が8192のとき
			img_tmp.Dt[j][i] = (int)i64;
		}
	}

	for (int i = 0 ; i < SubPixel; i++){
		for (int j = 0 ; j < MainPixel; j++){
			Dt[i][j] = CLIP(img_tmp.Dt[j][i], 0, DtMax);//U16.0
		}
	}
#endif

	return true;
}
//---------------------------------------------------------------------------
//SEのFPGA実装のゲイン補正（Bmapを用いた乗算処理（AXIS,47C,77G用）。Bmapは8192QLが1倍で固定）
	//
	//
	//               U16.0           U20.16->U20.0  ------------    U16.0
	//     in_data[] ------------------(x)----------| limiter  |--- out_data[]
	//                                  |           ------------
	//                                  | U4.16
	//                                  |
	//           U3.13   U4.28->U4.16   |
	//     hosei_data[]-----(x)---------|
	//                       |
	//                       | U1.15 kando
	//
bool TSIMARS::bGainCorrectionAXIS(TTMA& ShadingImg, int iCoeSense, int iPanel_Kind)
{
	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != MainPixel || ShadingImg.SubPixel != SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

	//Mayoスジ対策
	unsigned int limit;
	if (iPanel_Kind == _PANEL_47C || iPanel_Kind == _PANEL_77C){
		limit = iCoeSense * 55000;//47C
	}
	else if (iPanel_Kind == _PANEL_AXIS || iPanel_Kind == _PANEL_77G) {
		limit = iCoeSense * 65535;//AXIS/77G
	}
	else{
		CApplication::GetInstance()->MessageBox("指定したパネル種が不正です。", "Error", MB_OK);
		return false;
	}

	limit >>= 15;
	if (limit > 65535){
		limit = 65535;
	}


	//感度補正も同時に実行
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U3.13 　ゲイン1倍は8192固定
			i64 *= iCoeSense;//U4.28

			i64 >>= 12;//U4.16
			i64 *= Dt[i][j];//U20.16
			i64 >>= 16;//U20.0
/*
			i64 >>= 16;//U4.12
			i64 *= Dt[i][j];//U20.12
			i64 >>= 12;//U20.0
*/
			if (i64 > limit){
				i64 = limit;
			}
			Dt[i][j] = CLIP(i64, 0, limit);//U16.0
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//SEのFPGA実装のゲイン補正（Bmapを用いた乗算処理（2430,HM,DEVO2用）。Bmapは8192QLが1倍で固定）
	//
	//
	//               U16.0           U20.16->U20.0  ------------    U16.0
	//     in_data[] ------------------(x)----------| limiter  |--- out_data[]
	//                                  |           ------------
	//                                  | U4.16
	//                                  |
	//           U3.13   U4.28->U4.16   |
	//     hosei_data[]-----(x)---------|
	//                       |
	//                       | U1.15 kando
	//

//#define _LP_FORWARD_GAIN_HOSEI
//#define _RANDOM_OUTPUT

bool TSIMARS::bGainCorrection2430(TTMA& ShadingImg, int iCoeSense, int iPanel_Kind, int iClipQL)
{
	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != MainPixel || ShadingImg.SubPixel != SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

	//Mayoスジ対策
	unsigned int limit;
	if (iPanel_Kind == _PANEL_2430){
		limit = iCoeSense * iClipQL;//2430 49700
	}
	else if (iPanel_Kind == _PANEL_HM){
		limit = iCoeSense * iClipQL;//HM 51900
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1417_GOS){
		limit = iCoeSense * iClipQL;//52000
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1417_CSI){
		limit = iCoeSense * iClipQL;//52000
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1717_GOS){
		limit = iCoeSense * iClipQL;//52000
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1717_CSI){
		limit = iCoeSense * iClipQL;//52000
	}
	else{
		CApplication::GetInstance()->MessageBox("指定したパネル種が不正です。", "Error", MB_OK);
		return false;
	}

	limit >>= 15;
	if (limit > 65535){
		limit = 65535;
	}

#ifdef _RANDOM_OUTPUT
	TTMA RandImg, RandImg6Bit;
	RandImg.NewDt(SubPixel, MainPixel, Bit);
	RandImg.FillValue(0);
	RandImg6Bit.NewDt(SubPixel, MainPixel, Bit);
	RandImg6Bit.FillValue(0);
#endif

	unsigned short random = 0x1234;//1 0010 0011 0100(2進数)
	unsigned short random_tmp;

	//感度補正も同時に実行
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U3.13 　ゲイン1倍は8192固定
			i64 *= iCoeSense;//U4.28
			i64 >>= 12;//U4.16

			__int64 i64tmp = Dt[i][j];//U16.0
			i64tmp <<= 4;//U16.4
#ifdef _LP_FORWARD_GAIN_HOSEI
			i64tmp += ( random & 0x003F );//U16.4  小数部に乱数加算(0x000F -> 111111(2進数)であり、下6bitだけを抜き出して乱数としている)
#else
			i64tmp += ( random & 0x000F );//U16.4  小数部に乱数加算(0x000F -> 1111(2進数)であり、下4bitだけを抜き出して乱数としている)
#endif
			i64 *= i64tmp;//U20.20
			i64 >>= 20;//U20


			if (i64 > limit){
				i64 = limit;
			}
			Dt[i][j] = CLIP(i64, 0, limit);//U16.0


#ifdef _RANDOM_OUTPUT
            RandImg.SetVal(i, j, random);
            RandImg6Bit.SetVal(i, j, random & 0x003F);
#endif

			//乱数を更新
			random_tmp =  (( random & 0x0001 ) >> 0 ) << 15;
			random_tmp += (( random & 0x8000 ) >> 15 ) << 14;
			random_tmp += (((random & 0x4000 ) >> 14 ) ^ (( random & 0x0001 ) >> 0 ) ) << 13;
			random_tmp += (((random & 0x2000 ) >> 13 ) ^ (( random & 0x0001 ) >> 0 ) ) << 12;
			random_tmp += (( random & 0x1000 ) >> 12 ) << 11;
			random_tmp += (((random & 0x0800 ) >> 11 ) ^ (( random & 0x0001 ) >> 0 ) ) << 10;
			random_tmp += (( random & 0x0400 ) >> 10 ) << 9;
			random_tmp += (( random & 0x0200 ) >> 9 ) << 8;
			random_tmp += (( random & 0x0100 ) >> 8 ) << 7;
			random_tmp += (( random & 0x0080 ) >> 7 ) << 6;
			random_tmp += (( random & 0x0040 ) >> 6 ) << 5;
			random_tmp += (( random & 0x0020 ) >> 5 ) << 4;
			random_tmp += (( random & 0x0010 ) >> 4 ) << 3;
			random_tmp += (( random & 0x0008 ) >> 3 ) << 2;
			random_tmp += (( random & 0x0004 ) >> 2 ) << 1;
			random_tmp += (( random & 0x0002 ) >> 1 ) << 0;
			random = random_tmp;
		}
	}

#ifdef _RANDOM_OUTPUT
    RandImg.SaveTmaTdt("C:\\RandomImage.tma");
    RandImg6Bit.SaveTmaTdt("C:\\RandomImage6Bit.tma");
#endif

	return true;
}
//---------------------------------------------------------------------------


bool TSIMARS::bForwardGainCorrectionLP(TTMA& ShadingImg, int iCoeSense, int iClipQL, bool bExeForwardSensCor, double dKando32768, int iNormBitSense)
{
	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != MainPixel || ShadingImg.SubPixel != SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

	//Mayoスジ対策
	unsigned int limit = iCoeSense * iClipQL;//52000　（DEVO2　141021情報）

	limit >>= 15;
	if (limit > 65535){
		limit = 65535;
	}

#ifdef _RANDOM_OUTPUT
	TTMA RandImg, RandImg6Bit;
	RandImg.NewDt(SubPixel, MainPixel, Bit);
	RandImg.FillValue(0);
	RandImg6Bit.NewDt(SubPixel, MainPixel, Bit);
	RandImg6Bit.FillValue(0);
#endif

	unsigned short random = 0x1234;//1 0010 0011 0100(2進数)
	unsigned short random_tmp;

	//感度補正も同時に実行
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U3.13 　ゲイン1倍は8192固定
			i64 *= iCoeSense;//U4.28
			i64 >>= 12;//U4.16

			__int64 i64tmp = Dt[i][j];//U16.0
			i64tmp <<= 4;//U16.4
			i64tmp += ( random & 0x003F );//U16.4  小数部に乱数加算(0x003F -> 111111(2進数)であり、下6bitだけを抜き出して乱数としている)

			switch (bExeForwardSensCor) {
			case false://順感度補正OFF
				i64 *= i64tmp;//U20.20
				i64 >>= 20;//U20
				break;
			case true://順感度補正ON
				i64 *= i64tmp;//U20.20
				i64 *= round(dKando32768);// U21.35  四捨五入して規格値32768の整数に戻す
				i64 >>= (20 + iNormBitSense);//U20
				break;
			}


			if (i64 > limit){
				i64 = limit;
			}
			Dt[i][j] = CLIP(i64, 0, limit);//U16.0


#ifdef _RANDOM_OUTPUT
            RandImg.SetVal(i, j, random);
            RandImg6Bit.SetVal(i, j, random & 0x003F);
#endif

			//乱数を更新
			random_tmp =  (( random & 0x0001 ) >> 0 ) << 15;
			random_tmp += (( random & 0x8000 ) >> 15 ) << 14;
			random_tmp += (((random & 0x4000 ) >> 14 ) ^ (( random & 0x0001 ) >> 0 ) ) << 13;
			random_tmp += (((random & 0x2000 ) >> 13 ) ^ (( random & 0x0001 ) >> 0 ) ) << 12;
			random_tmp += (( random & 0x1000 ) >> 12 ) << 11;
			random_tmp += (((random & 0x0800 ) >> 11 ) ^ (( random & 0x0001 ) >> 0 ) ) << 10;
			random_tmp += (( random & 0x0400 ) >> 10 ) << 9;
			random_tmp += (( random & 0x0200 ) >> 9 ) << 8;
			random_tmp += (( random & 0x0100 ) >> 8 ) << 7;
			random_tmp += (( random & 0x0080 ) >> 7 ) << 6;
			random_tmp += (( random & 0x0040 ) >> 6 ) << 5;
			random_tmp += (( random & 0x0020 ) >> 5 ) << 4;
			random_tmp += (( random & 0x0010 ) >> 4 ) << 3;
			random_tmp += (( random & 0x0008 ) >> 3 ) << 2;
			random_tmp += (( random & 0x0004 ) >> 2 ) << 1;
			random_tmp += (( random & 0x0002 ) >> 1 ) << 0;
			random = random_tmp;
		}
	}

#ifdef _RANDOM_OUTPUT
    RandImg.SaveTmaTdt("C:\\RandomImage.tma");
	RandImg6Bit.SaveTmaTdt("C:\\RandomImage6Bit.tma");
#endif

	return true;
}
//---------------------------------------------------------------------------
//LongPanelのシンチ段差、ガラス段差検出のための副微分処理

#define _SUNUKE_NIJUUDANSA //素抜け二重段差対策

void TSIMARS::DetectLPSintiGlassEdgePosition(string FileName, int iY0, int iYE, int iX0, int iXE, int iSMedBeforeSubDiff, int iMMedBeforeSubDiff, int iMMedAfterSubDiff, int iThBaseQLSubDiff, bool bUpperEdge, bool bGainCalib)
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//LongPanelのシンチ段差補正

//#define _ONLY_NO_OVERLAP_COR//非重複領域の補正のみ行いたい場合に定義する（さらに、左右位置ずれ量、白帯長さ、パネル間感度補正係数も設定する）
#define _HEIKOHSHIHENKEI_HOKAN//平行四辺形補間

//#define _TIME1 //シンチ段差補正の準備補正（A,B,C）のスループット計測
//#define _TIME2 //シンチ段差補正本体（D,E）のスループット計測


void TSIMARS::LPSintiEdgeCorrection(TTMA& ImageUp, string FileNameImageUp, string& FileName, bool bCorHorShift, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch, bool bGPRMatch,
							int iMainMedianDiffImg, int iSubMedianDiffImg, int iLimitSearchRange, bool bUetyousei, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio,
							int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float fThQLRateUD, float fMagicPara, bool bDetectKanajakuMemori, int iMedianHasiSyori, int iThQLSubDiffForScaleSearch, int iSubAveNum, int iMarginXMed, double dKandoTop, double dKandoCenter, int iYSearchEnd, bool& bExistPbScale, bool bUpperEdge)
{
#ifdef _TIME1
			//計算時間計測
		clock_t	startTime, stopTime;
		startTime = clock();
#endif

	string StrPre = "";//接頭辞
	double* delta = new double[MainPixel];
	int* edgeY = new int[MainPixel];
	TFIMG ImageRateOfUpToDown;
	LPCalcSintiEdge(delta, edgeY, FileName, bUpperEdge);//シンチ段差の段差量を計算する
	int iYSintiEnd = round((bUpperEdge ? m_SintiEdgeU.ymax : m_SintiEdgeD.ymax) / 10.);//シンチ段差の下端
//	int iYSintiEnd = round((bUpperEdge ? m_SintiEdgeU.ymax : m_SintiEdgeD.ymax) / 10.) + 1;//シンチ段差の下端
	int iMShiftDetect;//上画像と中画像の左右位置ずれ
	int iWidthROI = 500;//とりあえず
	int iWidthOverlap = 100;//とりあえず
	float fSensitivityRateDtoU;//tmp
	TTMA ImageUpBeforeLRShift = ImageUp;//左右シフト前のImageUpを保存
	TTMA ImageUpAfterLRShift = ImageUp;//左右シフト後のImageUpを保存

#ifdef _ONLY_NO_OVERLAP_COR   //非重複領域の補正のみ行いたい場合(MCデバッグ用)
	iMShiftDetect = 1;//LongPanel.logより抜粋
	iSitaOffset = 0;//LongPanel.logより抜粋
	fSensitivityRateDtoU = 0.993656;//LongPanel.logより抜粋

#else //通常はこちら
	//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主Medianをかけて、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする（ImageUpは左右シフト後の画像に変換される）
	if (bCorHorShift) {
//		LPDetectHorizontalMisalignmentAndShiftImageUp(ImageUp, FileNameImageUp, iMShiftDetect, FileName, iMainShiftMax, iSubNumMatch, iMainMedianMatch, iSubRangeOnlyMidPanel, iYSintiEnd);
		LPDetectHorizontalMisalignmentAndShiftImageUpMultiROI(ImageUp, FileNameImageUp, iMShiftDetect, FileName, iMainShiftMax, iSubNumMatch, iMainMedianMatch, iSubRangeOnlyMidPanel, iYSintiEnd, iWidthROI, iWidthOverlap, dKandoTop, dKandoCenter, bGPRMatch);

//		iMShiftDetect = 0;//強制上書きテスト(150529 ttsuji)
	}



	//上画像と中画像の感度の違いを検出して上画像の感度を修正する
//	float fSensitivityRateDtoU;
	if (bUetyousei) {
		ImageRateOfUpToDown.NewDt(MainPixel, 1);//M,Sの順
//		LPAdjustEndPanelSensitivity(ImageRateOfUpToDown, ImageUp, edgeY, FileName, iLimitSearchRange, iUeOffset, iSitaOffset, fUpperLimitOfPanelSensRatio, fLowerLimitOfPanelSensRatio, iMainShiftMax, fSensitivityRateDtoU);//上画像ImageUpの感度を中画像に合うように調整
		LPAdjustEndPanelSensitivity2(ImageRateOfUpToDown, ImageUp, edgeY, FileName, iLimitSearchRange, iUeOffset, iSitaOffset, fUpperLimitOfPanelSensRatio, fLowerLimitOfPanelSensRatio, iMainShiftMax, fSensitivityRateDtoU, iYSearchEnd);//上画像ImageUpの感度を中画像に合うように調整

		//シンチ段差プロファイルを主Median11
		for (int iM = 0; iM < MainPixel; iM++){
			ImageRateOfUpToDown.Dt[iM][0] = delta[iM];
		}
		const int iPMedian = 11;
		ImageRateOfUpToDown.MainMedian(iPMedian);
 #ifdef _LP_DEBUG_OUTPUT
		ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "Med_delta.txt", true);
 #endif
		for (int iM = 0; iM < MainPixel; iM++){
			delta[iM] = ImageRateOfUpToDown.Dt[iM][0];
		}
// #ifdef _LP_DEBUG_OUTPUT
		//MCコンペア用にTop画像の処理を追加（上画像（左右位置ずらしなし）にクリッピング処理を入れて出力）(151004 ttsuji)
		//Topパネルに感度補正係数を乗算
		for (int iS = 0; iS < ImageUpBeforeLRShift.SubPixel; iS++){
			for (int iM = 0; iM < ImageUpBeforeLRShift.MainPixel; iM++){
				ImageUpBeforeLRShift.SetVal(iS, iM, ImageUpBeforeLRShift.Dt[iS][iM] * fSensitivityRateDtoU);
			}
		}
		string strImageUpPanelSensTC = "A(" + FloatToStrF(fSensitivityRateDtoU,ffFixed,7,6) + ")ImageUp.tma";//有効数字7桁、小数点以下6桁を表示
		string strImageUpPanelSensTCBeforeLRShift = "A(" + FloatToStrF(fSensitivityRateDtoU,ffFixed,7,6) + ")ImageUpBeforeLRShift.tma";//有効数字7桁、小数点以下6桁を表示
		ImageUpBeforeLRShift.WriteTma(ExtractFilePath(FileName) + strImageUpPanelSensTCBeforeLRShift);

		//S値適正化補正
		int iSValueMod = 4;//４倍乗算
		ImageUpAfterLRShift  *= iSValueMod;
		ImageUpBeforeLRShift *= iSValueMod;
		ImageUpAfterLRShift.WriteTma(ExtractFilePath(FileName) + "A" + strImageUpPanelSensTC);
		ImageUpBeforeLRShift.WriteTma(ExtractFilePath(FileName) + "A" + strImageUpPanelSensTCBeforeLRShift);

		//クリップ処理
		int iClipValue = 52000;
		for (int iS = 0; iS < ImageUpAfterLRShift.SubPixel; iS++){
			for (int iM = 0; iM < ImageUpAfterLRShift.MainPixel; iM++){
				//非補正画像QLが飽和しているときの対策（clip処理）
				if (ImageUpAfterLRShift.Dt[iS][iM] <= iClipValue * dKandoTop * fSensitivityRateDtoU) {//QL値がiClipValue * dKando * fSensitivityRateDtoU 以下であれば、QLを変えない。
					continue;
				}
				else{
					ImageUpAfterLRShift.SetVal(iS, iM, iClipValue * dKandoTop * fSensitivityRateDtoU);
				}
			}
		}
		ImageUpAfterLRShift.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(round(iClipValue * dKandoTop * fSensitivityRateDtoU)) + ")A" + strImageUpPanelSensTC);

		for (int iS = 0; iS < ImageUpBeforeLRShift.SubPixel; iS++){
			for (int iM = 0; iM < ImageUpBeforeLRShift.MainPixel; iM++){
				//非補正画像QLが飽和しているときの対策（clip処理）
				if (ImageUpBeforeLRShift.Dt[iS][iM] <= iClipValue * dKandoTop * fSensitivityRateDtoU) {//QL値がiClipValue * dKando * fSensitivityRateDtoU 以下であれば、QLを変えない。
					continue;
				}
				else{
					ImageUpBeforeLRShift.SetVal(iS, iM, iClipValue * dKandoTop * fSensitivityRateDtoU);
				}
			}
		}
		ImageUpBeforeLRShift.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(round(iClipValue * dKandoTop * fSensitivityRateDtoU)) + ")A" + strImageUpPanelSensTCBeforeLRShift);
// #endif
	}
#endif

#ifdef _TIME1
	stopTime = clock();
	double time = (double)(stopTime-startTime)/CLOCKS_PER_SEC;
	CApplication::GetInstance()->MessageBox(("シンチ段差補正A,B,C：\n" + FloatToStr(time) + " sec").c_str(), "OK", MB_OK);
#endif


#ifdef _TIME2
	//計算時間計測
	clock_t	startTime2, stopTime2;
	startTime2 = clock();
#endif


 #if 0 //tmp
	iSitaOffset = 100;//強制的に白帯長さを100にする
 #endif

	const int iKasa = 30000;//CorrImgのアンダーフローを防ぐため導入（ttsuji 131003）
//	const int iKasa = 50000;//CorrImgのアンダーフローを防ぐため導入（ttsuji 131003）
//	const int iKasa = 4000;//CorrImgのアンダーフロー、オーバーフローを防ぐため修正（ttsuji 140407）
	TTMA CorrImg;

 #if 0 //検出した白帯長さをDC成分補間に反映するのをやめた。（DC補間長さを長くしても完全にはキレイにならない画像もあるため、DC補間はできるだけ短く抑え、白帯は短冊横傘を強くかけて落とすとする）140523
	iItplSubOffset = iSitaOffset;//感度比算出で計算した白帯の長さをDC成分補間の補間長さに反映
 #elif 0
	iItplSubOffset = 3;
 //	iItplSubOffset = 50;//150622ttsuji 決めうち（VG対応でグリッドなし腰椎側面の白帯対策）
//	iItplSubOffset = 46;//150622ttsuji 決めうち（VG対応でグリッドなし腰椎側面の白帯対策）
//	iItplSubOffset = 30;//150622ttsuji 決めうち（VG対応でグリッドなし腰椎側面の白帯対策）
 #elif 1
	if (iSitaOffset >= 100) {
//	if (iSitaOffset >= 0) {//強制的にDC補間長さを50pixとするため
//		iItplSubOffset = 50;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
//		iItplSubOffset = 30;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
//		iItplSubOffset = 20;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
		iItplSubOffset = 10;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
//		iItplSubOffset = 3;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
	}
 #endif
	CorrImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
//	CorrImg.NewDt(iYSintiEnd + iItplSubOffset + 2, MainPixel, Bit);//マージン込み確保
//	CorrImg.NewDt(iYSintiEnd + iItplSubOffset + 3, MainPixel, Bit);//マージン込み確保(140816に iYSintiEnd + iItplSubOffset + 1　に戻した。ttsuji )
	CorrImg.FillValue(iKasa);

	TTMA QLRateImg;//上下の画像のQL比（左右位置ずれ検出のため） ←結局アルゴが複雑になりそうだったので使わなかった。
	const double dNormQL = 8192.;//8192QLが1倍
	QLRateImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
	QLRateImg.FillValue(0);

#ifndef _ONLY_NO_OVERLAP_COR
	//上画像のうち、中画像と同じ絵が存在する領域に対して中画像との差分をとって、主Median、副Medianして上画像との差分画像の低周波情報を取得
	LPCorrectOverlapAreaReferToImageUp(ImageUp, CorrImg, QLRateImg, FileName, edgeY, iKasa, iYSintiEnd, dNormQL, iSubRangeOnlyMidPanel, iMainMedianDiffImg, iSubMedianDiffImg, iMShiftDetect);
#endif

	//上画像低周波情報で補正した中画像のうち、上画像情報が存在しない補正領域は中画像のDC成分を線形補間にて計算する。その際、DC成分線形補間の端点のQL比がある閾値以上であれば、濃度の濃い画素の濃度を下げた上で線形補間する。
	//DC成分線形補間した画像を中画像から減算して補正画像を作成し、これに主Medianをかけて、補正量の主方向スムージングを行う。これを最終的な補正画像として、中画像から減算する。
//	LPCorrectNonOverlapAreaByItplDC(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD, fMagicPara);
//	LPCorrectNonOverlapAreaByItplDC2(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD, fMagicPara);
#ifndef _HEIKOHSHIHENKEI_HOKAN
	LPCorrectNonOverlapAreaByItplDC3(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD);
#else
	LPCorrectNonOverlapAreaByItplDC4(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, bDetectKanajakuMemori, iMedianHasiSyori, iThQLSubDiffForScaleSearch, iSubAveNum, iMarginXMed, bExistPbScale);
#endif


#ifdef _TIME2
	stopTime2 = clock();
	double time2 = (double)(stopTime2-startTime2)/CLOCKS_PER_SEC;
	CApplication::GetInstance()->MessageBox(("シンチ段差補正D,E：\n" + FloatToStr(time2) + " sec").c_str(), "OK", MB_OK);
#endif

	if(bUetyousei){
//		StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + ",-2,3," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ")";
		if (bCorHorShift) {
//			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "NCC"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
			if (bDetectKanajakuMemori) {
#if 0
				StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "K" + IntToStr(iThQLSubDiffForScaleSearch) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "N"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
#else			//矩形アーチファクト対策
				StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "K(-1,-3," + IntToStr(iThQLSubDiffForScaleSearch) + ")" + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "N"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
#endif
			}
			else{
				StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "N"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
			}
		}
		else{
			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) +  ")";
		}
	}
	else{
		if (bCorHorShift) {
			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",ANCC"  + IntToStr(iMShiftDetect) + ")";
		}
		else{
			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A)";
		}
	}
#if 0
	FileName = AddPrefix(FileName, StrPre);
#else
	FileName = ExtractFilePath(FileName) + StrPre + ".tma";
#endif

	delete[] edgeY;
	delete[] delta;
}
//---------------------------------------------------------------------------
//シンチ段差の段差量を計算する

void TSIMARS::LPCalcSintiEdge(double* delta, int* edgeY, string FileName, bool bUpperEdge)
{
	string strUD;

	// エッジ位置と段差量
	for (int iM = 0; iM < MainPixel; iM++){
		switch (bUpperEdge) {
		case true:
#if 0		//段差が直線と仮定
			edgeY[0]           = round(m_SintiEdgeU.ystart / 10.);
			edgeY[MainPixel-1] = round(m_SintiEdgeU.yend / 10.);
			edgeY[iM] = edgeY[0] + (edgeY[MainPixel-1]-edgeY[0]) * float(iM) / (MainPixel-1);
#else       //段差が放物線と仮定
			edgeY[iM] = round(m_SintiEdgeU.y10[iM] / 10.);
#endif
			strUD = "u";
			break;
		case false:
			edgeY[iM] = round(m_SintiEdgeD.y10[iM] / 10.);
			strUD = "d";
			break;
		}
		delta[iM] = (Dt[edgeY[iM]-4][iM] + Dt[edgeY[iM]-3][iM] - Dt[edgeY[iM]+4][iM] - Dt[edgeY[iM]+3][iM]) / 2.;
	}

#ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "edgeY" + strUD + "_delta.txt").c_str(), "wt");
	for (int iM = 0; iM < MainPixel; iM++){
		fprintf(fp, "%d	%d	%lf\n", iM, edgeY[iM], delta[iM]);
	}
	fclose(fp);
#endif

}
//---------------------------------------------------------------------------
//上画像の感度を中画像に合うように調整（同一箇所から20pix程度ずれたところで合わせるため精度に限界あり）

//#define _AUTOADJUSTSENSITIVITY_TK
#define  _SOFFSET
#define  _GRADIENT_MARGIN
#define  _DA_OFFSET
#define  _RATE1
#define  _DETECT_RADIATION_FIELD
#define  _DA_ROUND3
#define  _HIST_CENTER //感度比を計算する際に、Top(or Bottom)のみならずCenterのHistgramも解析する


void TSIMARS::LPAdjustEndPanelSensitivity(TFIMG& ImageRateOfUpToDown, TTMA& ImageUp, int* edgeY, string FileName, int iLimitSearchRange, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iMainShiftMax, float& fSensitivityRateDtoU)
{
 #ifndef _DETECT_RADIATION_FIELD
	//ヒストグラムをとって最大QL、最小QLを計算  (左右は画像端から位置ずれ分30pixを除外し、上下は3pix平均とする)
	ImageUp.Histogram(ImageUp.SubPixel - iUeOffset - 1, ImageUp.SubPixel - iUeOffset + 1, iMainShiftMax, ImageUp.MainPixel - 1 - iMainShiftMax);
//	float fThRate = 0.7;//ヒストの全面積の70％（感度比を計算する画素の判定に使用） ←多すぎて素抜けを含んでしまいNG
//	float fThRate = 0.5;//ヒストの全面積の50％（感度比を計算する画素の判定に使用）
	float fThRate = 0.3;//ヒストの全面積の30％（感度比を計算する画素の判定に使用）
	int iThMaxQL = ImageUp.HistRuisekiDosuuPercent(fThRate, true);
//	int iThMinQL = ImageUp.HistRuisekiDosuuPercent(fThRate, false);
	int iThMinQL = 50;//50QL以下は感度比計算から除外する
 #endif

	//シンチ段差下方の散乱線の有無を検出し、感度比計算に適切なｙ座標を求める
	//（ImageUpの下端のQL値の副方向の傾きと合致するCenterパネルのシンチ段差下方のｙ座標を計算）
	const int iMainMoveAve = 51;//とりあえず
	const int iSubMoveAve = 11;//とりあえず
	const int iMDiff = 50;//50pix間隔で調査
	const int iMCut = iMainShiftMax;//除外するpix(=30)
	TTMA SmoothImageUp = ImageUp;
	SmoothImageUp.MainMoveAve(iMainMoveAve);
	SmoothImageUp.SubMoveAve(iSubMoveAve);

	TTMA SmoothImageDown = *this;
	SmoothImageDown.MainMoveAve(iMainMoveAve);
	SmoothImageDown.SubMoveAve(iSubMoveAve);
 #ifdef _LP_DEBUG_OUTPUT
	SmoothImageUp.WriteTma(ExtractFilePath(FileName) + "SmoothImageUp(" + IntToStr(iMainMoveAve) + "," + IntToStr(iSubMoveAve) + ").tma");
	SmoothImageDown.WriteTma(ExtractFilePath(FileName) + "SmoothImageDown(" + IntToStr(iMainMoveAve) + "," + IntToStr(iSubMoveAve) + ").tma");
 #endif

 #ifdef _DETECT_RADIATION_FIELD
	//左右の照射野絞りを検出することで感度比のロバスト向上(140823 ttsuji)
	const int iThQLMDif = 100;//14bitログ画像の主微分のQL閾値
//	const int iThQLMDif = 1000;//14bitログ画像の主微分のQL閾値
//	const int iThQLMDif = 70;//14bitログ画像の主微分のQL閾値
	bool bExist0 = false;//照射野境界の検出フラグ
	bool bExistE = false;//照射野境界の検出フラグ
//	int iSSearchRadiationField0   = SmoothImageUp.SubPixel - 100; old
	int iSSearchRadiationField0   = SmoothImageUp.SubPixel - 101;//アルゴ仕様書記載にあわせる（141022 ttsuji)
//	int iSSearchRadiationField0   = SmoothImageUp.SubPixel - 501;//アルゴ仕様書記載にあわせる（150402 ttsuji  tmp)
	int iSSearchRadiationFieldEnd = SmoothImageUp.SubPixel - 1;//画像の下端
//	int iSSearchRadiationFieldEnd = SmoothImageUp.SubPixel - 4;//画像の下端 (Topの下端が黒塗りされているため、照射野境界検出時のｙレンジには下端を含まないように修正。)(tmp 150402  日赤のRoutine908解析のため)
	int iMBorder0 = 0;//照射野境界のｘ座標
	int iMBorderEnd = SmoothImageUp.MainPixel - 1;//照射野境界のｘ座標
	LPDetectBorderRadiationField(FileName, SmoothImageUp, iThQLMDif, bExist0, bExistE, iMBorder0, iMBorderEnd, iSSearchRadiationField0, iSSearchRadiationFieldEnd, iLimitSearchRange);

	//ヒストグラムをとって最大QL、最小QLを計算  (左右は画像端から位置ずれ分30pixを除外し、上下は3pix平均とする)
	int iMStart = (iMBorder0 > iMainShiftMax) ? iMBorder0 + 1 : iMainShiftMax;
	int iMEnd = (iMBorderEnd < ImageUp.MainPixel - iMainShiftMax) ? iMBorderEnd - 1 : ImageUp.MainPixel - 1 - iMainShiftMax;//照射野境界のｘ座標
	ImageUp.Histogram(ImageUp.SubPixel - iUeOffset - 1, ImageUp.SubPixel - iUeOffset + 1, iMStart, iMEnd);
//	ImageUp.Histogram(ImageUp.SubPixel - iUeOffset - 1, ImageUp.SubPixel - iUeOffset + 1, iMainShiftMax, ImageUp.MainPixel - 1 - iMainShiftMax);
	float fThRate = 0.3;//ヒストの全面積の30％（感度比を計算する画素の判定に使用）
	int iThMaxQL = ImageUp.HistRuisekiDosuuPercent(fThRate, true);
 #endif


//	int iNumOfSample = 80;//上パネルの最小二乗法のサンプル点の個数
//	int iNumOfSample = 100;//上パネルの最小二乗法のサンプル点の個数
	int iNumOfSample = 30;//上パネルの最小二乗法のサンプル点の個数(default)
//	int iNumOfSample = 20;//上パネルの最小二乗法のサンプル点の個数
//	int iNumOfSample = 10;//上パネルの最小二乗法のサンプル点の個数
	double* dS = new double[iNumOfSample];
	double* dQL = new double[iNumOfSample];
	double dA, dB;
	Vct v;

#ifdef _LP_DEBUG_OUTPUT
	FILE* fp2 = fopen((ExtractFilePath(FileName) + "SearchSironukeRyoiki(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "," + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
/*
 #ifdef _HIST_CENTER
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + "," + IntToStr(iThMaxQL_Center) + ").txt").c_str(), "wt");
 #else
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
 #endif
	fprintf(fp3, "iM	iCounter\n");
*/
#endif
	double dSum = 0.;
	int iCounter = 0;//関心領域内で検出した白帯位置の検出数
	int iSAve(0), iSMax(0);//関心領域内で検出した白帯位置の主方向の統計値


#if 0
	fprintf(fp2, "iM	dA	dB	iSSearch	dQL10Dif\n");
	for (int iM = iMCut; iM < MainPixel - iMCut; iM += iMDiff){
		if (iThMaxQL <= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM]) {//iMが素抜けの場合は排除
			continue;
		}

		for (int iS = 0; iS < iNumOfSample; iS++) {
			dS[iS]  = iS;
			dQL[iS] = SmoothImageUp.Dt[ImageUp.SubPixel - iNumOfSample + iS][iM];
		}
		CLeastSquare LSItpl(iNumOfSample, dS, dQL, _LINEAR);
		if (!LSItpl.Calc()){
			CApplication::GetInstance()->MessageBox("補間プロファイルの最小二乗法計算でエラーが発生しました。", "Error", MB_OK);
			delete[] dS;
			delete[] dQL;
			fclose(fp);
			fclose(fp2);
			return;
		}
		v = LSItpl.GetSolve();
		dA = v[1];
		dB = v[0];

		//iMにおけるシンチ段差位置
		const int iSDif = 10;//10pix刻みで副微分をとる
//		const int iSDif = 5;//10pix刻みで副微分をとる
		int iSDifHalf = iSDif / 2;
//		const int iSSearchEnd = 300;//副微分値を探索する終点
		const int iSSearchEnd = 400;//副微分値を探索する終点
		double dQL10dif;//10pix刻みの副微分値
		int iSSearch = 0;//白帯がシンチ段差下方どこまで続いているか

		//白帯起因の上凸カーブであるかをまずチェック（上凸カーブがなければ白帯なしとしてiSSearch=0とする）


		//dAに近い傾きをもつiSを探索
		iSSearch = 0;
//		for (int iS = edgeY[iM] + iSDif; iS < iSSearchEnd; iS += iSDif) {
		for (int iS = edgeY[iM] + 2*iSDif; iS < iSSearchEnd; iS += iSDif) {
			dQL10dif = (double)(SmoothImageDown.Dt[iS + iSDifHalf][iM] - SmoothImageDown.Dt[iS - iSDifHalf][iM]) / iSDif;
//			if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
			if (dA >= dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
				iSSearch = iS;
				dSum += iSSearch;
				iCounter++;

				if (iSMax < iSSearch) {
					iSMax = iSSearch;//iSMaxを更新
				}
				break;
			}
		}
		fprintf(fp2, "%d	%lf	%lf	%d	%lf\n", iM, dA, dB, iSSearch, dQL10dif);
	}
	iSAve = (int)(dSum / iCounter + 0.5);

	//シンチ段差より下方の白帯の長さ
	int iShiroobiLen = iSAve - edgeY[0];
//	int iShiroobiLen = iSMax - edgeY[0];
	iSitaOffset = iShiroobiLen;//引数を更新


	fprintf(fp2, "\niSAve	iSMax\n");
	fprintf(fp2, "%d	%d\n", iSAve, iSMax);
	fclose(fp2);
	delete[] dS;
	delete[] dQL;

#else     	//関心領域で平均化した副プロファイルをまず計算し、次にそのプロファイルを分析する（白帯は主方向全域に渡る＆ノイズが少ないのでこっちが精度良いと考える）
	TTMA SubProfileU, SubProfileD;
	SubProfileU.NewDt(SubPixel, 1, Bit);//関心領域で平均化した副プロファイル
	SubProfileD.NewDt(SubPixel, 1, Bit);//関心領域で平均化した副プロファイル
	float fSumU = 0.0;
	float fSumD = 0.0;
	int iCounter2 = 0;
	for (int iS = 0; iS < SubPixel; iS++){
		fSumU = fSumD = 0.0;
		iCounter2 = 0;
/*
 #ifdef _DETECT_RADIATION_FIELD
		for (int iM = iMStart; iM < iMEnd; iM++){//照射野境界を除外して集計
 #else
		for (int iM = iMCut; iM < MainPixel - iMCut; iM++){
 #endif
*/
		for (int iM = iMCut; iM < MainPixel - iMCut; iM++){//白帯検出については、照射野外も含む、従来のものとする
			if (iThMaxQL <= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM]) {//iMが素抜けの場合は排除
//			if (iThMaxQL <= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] || iThMinQL >= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM]) {//iMが素抜けの場合は排除
				continue;
			}

			//主方向に平均化
			fSumU += SmoothImageUp.Dt[iS][iM];
			fSumD += SmoothImageDown.Dt[iS][iM];
			iCounter2++;
/*
 #ifdef _LP_DEBUG_OUTPUT
			if (iS == SubPixel - 1) {
				fprintf(fp3, "%d	%d\n", iM, iCounter2);
			}
 #endif
*/
		}
		fSumU /= iCounter2;
		fSumD /= iCounter2;
		SubProfileU.SetVal(iS, 0, fSumU);
		SubProfileD.SetVal(iS, 0, fSumD);
	}
 #ifdef _LP_DEBUG_OUTPUT
	SubProfileU.WriteTma(ExtractFilePath(FileName) + "SubProfileU(" + IntToStr(iThMaxQL) + ").tma");
	SubProfileD.WriteTma(ExtractFilePath(FileName) + "SubProfileD(" + IntToStr(iThMaxQL) + ").tma");
//	fclose(fp3);
	fprintf(fp2, "iIndex	iS	dDeltaQL[iIndex]\n");
 #endif

	//最小二乗法近似で上パネルの傾きを計算
	for (int iS = 0; iS < iNumOfSample; iS++) {
		dS[iS]  = iS;
		dQL[iS] = SubProfileU.Dt[SubProfileU.SubPixel - iNumOfSample + iS][0];
	}
	CLeastSquare LSItpl(iNumOfSample, dS, dQL, _LINEAR);
	if (!LSItpl.Calc()){
		CApplication::GetInstance()->MessageBox("補間プロファイルの最小二乗法計算でエラーが発生しました。", "Error", MB_OK);
		delete[] dS;
		delete[] dQL;
 #ifdef _LP_DEBUG_OUTPUT
		fclose(fp2);
 #endif
		return;
	}
	v = LSItpl.GetSolve();
	dA = v[1];
	dB = v[0];
#ifdef _DA_ROUND3
	dA = round(dA*1000) / 1000.;//小数第3位まで四捨五入で計算 (浮動小数点の不等式でMCと不整合でたため　141009)
#endif

//	const int iSDif = 5;//10pix刻みで副微分をとる
//	const int iSDif = 10;//10pix刻みで副微分をとる
	const int iSDif = 20;//20pix刻みで副微分をとる（白帯検出不可案件対応のため　140802）
	int iSDifHalf = iSDif / 2;
//	const int iSSearchEnd = 300;//副微分値を探索する終点
	const int iSSearchEnd = 400;//副微分値を探索する終点
//	const int iSSearchEnd = 500;//副微分値を探索する終点
	double dQL10dif;//10pix刻みの副微分値
	int iSSearch = 0;//白帯がシンチ段差下方どこまで続いているか
#ifdef _SOFFSET
	const int iSOffset = 20;//シンチ段差位置から、副微分値を調査するスタート点までのオフセット（白帯検出不可案件対応のため　140802）
#endif
#ifdef _GRADIENT_MARGIN
//	const double dGradMarginRate = 1.1;//上凸検出のときの傾きのマージンの％（白帯検出不可案件対応のため　140802）
	const double dGradMarginRate = 1.2;//上凸検出のときの傾きのマージンの％（白帯検出不可案件対応のため　140802）
#endif
#ifdef _DA_OFFSET
	const double dAOffset = 0.3;//dAが小さすぎてサーチ範囲の中にdAを下回るdQL10difがない場合はdAをオフセットする（白帯検出不可案件対応のため　140811）
#endif

	//白帯起因の上凸カーブであるかをまずチェック（上凸カーブがなければ白帯なしとしてiSSearch=0とする）
//	double dDeltaQL[4];
	double dDeltaQL[3];
	int iIndex = 0;
	bool bExistShiroobi = false;
	//シンチ段差より下方の副プロファイルの接線の傾きを求める
//	for (int iIndex = 0; iIndex < 4; iIndex++) {
	for (int iIndex = 0; iIndex < 3; iIndex++) {
 #ifdef _SOFFSET
		int iS = edgeY[0] + iSOffset;//iSDifによらずスタート点は固定とする。（白帯検出不可案件対応のため　140802）
 #else
		int iS = edgeY[0] + 2*iSDif;//iSDifに応じてスタート点も変動してしまう。
 #endif
		dDeltaQL[iIndex] = (double)(SubProfileD.Dt[iS + iIndex*iSDif + iSDifHalf][0] - SubProfileD.Dt[iS + iIndex*iSDif - iSDifHalf][0]) / iSDif;
 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp2, "%d	%d	%lf\n", iIndex, iS + iIndex*iSDif, dDeltaQL[iIndex]);
 #endif
	}
 #ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\niS	dQL10Dif	dA\n");
 #endif
	//シンチ段差下方の接線の傾きと上パネル下端の接線の傾きの関係から白帯あるか判定
//	if (dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] > dDeltaQL[1] && dDeltaQL[1] > dDeltaQL[2] && dDeltaQL[2] > dDeltaQL[3]) {
//	if (dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] > dDeltaQL[1] && dDeltaQL[1] > dDeltaQL[2]) {
 #ifdef _GRADIENT_MARGIN
	if (dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] * dGradMarginRate >= dDeltaQL[1] && dDeltaQL[1] * dGradMarginRate >= dDeltaQL[2]) {//上画像の下端の傾きを中画像のシンチ段差直下と比較するように修正（白帯検出不可案件対応のため　140802）
 #else
  #if 0
	if (dA < dDeltaQL[2] && 0 < dDeltaQL[0] && dDeltaQL[0] > dDeltaQL[1] && dDeltaQL[1] > dDeltaQL[2]) {//傾きが確実に増加している場合に白帯と検出
  #elif 0
	if (dA < dDeltaQL[2] && 0 < dDeltaQL[0] && dDeltaQL[0] >= dDeltaQL[1] && dDeltaQL[1] >= dDeltaQL[2]) {//傾きが減少してさえいなければ、白帯と検出するよう修正（白帯検出不可案件対応のため）
  #elif 0
	if (dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] >= dDeltaQL[1] && dDeltaQL[1] >= dDeltaQL[2]) {//上画像の下端の傾きを中画像のシンチ段差直下と比較するように修正（白帯検出不可案件対応のため　140802）
  #endif
 #endif

 #ifdef _SOFFSET  //白帯検出不可案件対応のため　140802
		for (int iS = edgeY[0] + iSOffset; iS < iSSearchEnd; iS += iSDif) {
 #else
		for (int iS = edgeY[0] + 2*iSDif; iS < iSSearchEnd; iS += iSDif) {
 #endif
			dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
 #ifdef _LP_DEBUG_OUTPUT
			fprintf(fp2, "%d	%lf	%lf\n", iS, dQL10dif, dA);
 #endif
			if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
				iSSearch = iS;
				bExistShiroobi = true;
				break;
			}
		}

		//シンチ段差より下方の白帯の長さ
		if (bExistShiroobi) {
			int iShiroobiLen = iSSearch - edgeY[0];
			iSitaOffset = iShiroobiLen;//引数を更新
		}
 #ifdef _DA_OFFSET
		else{//dAが小さすぎてサーチ範囲の中にdAを下回るdQL10difがない場合はdAをオフセットする（白帯検出不可案件対応のため　140811）
			dA += dAOffset;

  #ifdef _SOFFSET  //白帯検出不可案件対応のため　140802
			for (int iS = edgeY[0] + iSOffset; iS < iSSearchEnd; iS += iSDif) {
  #else
			for (int iS = edgeY[0] + 2*iSDif; iS < iSSearchEnd; iS += iSDif) {
  #endif
				dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
  #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp2, "%d	%lf	%lf\n", iS, dQL10dif, dA);
  #endif
				if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
					iSSearch = iS;
					bExistShiroobi = true;
					break;
				}
			}

			//シンチ段差より下方の白帯の長さ
			if (bExistShiroobi) {
				int iShiroobiLen = iSSearch - edgeY[0];
				iSitaOffset = iShiroobiLen;//引数を更新
			}
		}
 #endif
	}

 #ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\ndA	dB	iSSearch	dQL10Dif	bExistShiroobi\n");
	fprintf(fp2, "%lf	%lf	%d	%lf	%d\n", dA, dB, iSSearch, dQL10dif, bExistShiroobi);
	fprintf(fp2, "\niSitaOffset\n");
	fprintf(fp2, "%d\n", iSitaOffset);
	fclose(fp2);
 #endif
	delete[] dS;
	delete[] dQL;


#endif

 #if 0 //tmp あとでけすこと！
	ImageUp.MainMoveAve(iMainMoveAve);
	this->MainMoveAve(iMainMoveAve);
 #endif


 #if 0
 //test
//	iSitaOffset = 3;//白帯を無視してパネル間感度補正係数を計算（150317 ttsuji）
	iSitaOffset = 60;//白帯長さを決めうちにしてパネル間感度補正係数を計算
 #endif


 #ifdef _HIST_CENTER
	//感度比を計算する際に、Top(or Bottom)のみならずCenterのHistgramも解析する
	Histogram(edgeY[0] + iSitaOffset - 1, edgeY[0] + iSitaOffset + 1, iMStart, iMEnd);
	int iThMaxQL_Center = HistRuisekiDosuuPercent(fThRate, true);
 #endif

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "AdjustEndPanelSensitivity(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + ").txt").c_str(), "wt");
	fprintf(fp, "iM	ImageU	ImageD	RateOfUToD\n");
  #ifdef _HIST_CENTER
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + "," + IntToStr(iThMaxQL_Center) + ").txt").c_str(), "wt");
  #else
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
  #endif
	fprintf(fp3, "iM	iCounter\n");
 #endif
	//感度の比を各ｘごとに計算し、ｘ方向に集計
	for (int iM = 0; iM < MainPixel; iM++){
//		if (0 == Dt[edgeY[iM] + 3][iM]) {
		if (0 == Dt[edgeY[iM] + iSitaOffset][iM]) {
//			ImageRateOfUpToDown.Dt[iM][0] = 1;
			ImageRateOfUpToDown.Dt[iM][0] = 0.; //集計時に除外するように値を設定
		}
		else{
//			ImageRateOfUpToDown.Dt[iM][0] = ImageUp.Dt[ImageUp.SubPixel-2][iM] / (float)Dt[edgeY[iM]+3][iM];
			ImageRateOfUpToDown.Dt[iM][0] = ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] / (float)Dt[edgeY[iM] + iSitaOffset][iM];
		}
 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp, "%d	%d	%d	%lf\n", iM, ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM], Dt[edgeY[iM] + iSitaOffset][iM], ImageRateOfUpToDown.Dt[iM][0]);
 #endif
	}
 #ifdef _LP_DEBUG_OUTPUT
	ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "RateOfUpToDown.txt", true);
 #endif
	ImageRateOfUpToDown.MainMedian(201);
 #ifdef _LP_DEBUG_OUTPUT
	ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "Med_RateOfUpToDown.txt", true);
 #endif

 #ifndef _AUTOADJUSTSENSITIVITY_TK
	float fSensitivityRate;//主方向に平均した感度比
	float fSum = 0.0;
	const int iCut = iMainShiftMax;//平均値算出から除外する端画素
/*
	const float fLowerTh = 0.7;
//	const float fUpperTh = 1.3;//厳しすぎる（もっと感度比が大きくなりうる）
	const float fUpperTh = 1.5;
*/
	const float fLowerTh = fLowerLimitOfPanelSensRatio;
	const float fUpperTh = fUpperLimitOfPanelSensRatio;

 #ifdef _DETECT_RADIATION_FIELD
	for (int iMCnt = iMStart; iMCnt <= iMEnd; iMCnt++){//照射野境界を除外して集計
 #else
	for (int iMCnt = iCut; iMCnt < ImageRateOfUpToDown.MainPixel - iCut; iMCnt++){
//	for (int iMCnt = iCut; iMCnt < 2000; iMCnt++){//胸骨30度画像に特化（腕の境界部がシンチ段差と同じくらいの位置にあり、感度比を乱すため、腕領域を避ける）
//	for (int iMCnt = iCut; iMCnt < 2400; iMCnt++){//胸骨30度画像に特化（腕の境界部がシンチ段差と同じくらいの位置にあり、感度比を乱すため、腕領域を避ける）
 #endif

 #ifdef _HIST_CENTER
    	//感度比を計算する際に、Top(or Bottom)のみならずCenterの画素値も考慮する(150316 ttsuji)
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt] && iThMaxQL_Center > Dt[edgeY[iMCnt] + iSitaOffset][iMCnt]) {
 #else
//		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] || fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] || fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0]) {//異常値は排除
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt]) {//異常値は排除
 #endif
			fSum += ImageRateOfUpToDown.Dt[iMCnt][0];
			iCounter++;
 #ifdef _LP_DEBUG_OUTPUT
			fprintf(fp3, "%d	%d\n", iMCnt, iCounter);
 #endif
		}
	}
	if (!iCounter) {
 #ifdef _RATE1 //感度比の上限、下限の閾値以内となるｘがない場合は、エラーとせずに感度比１とする。（感度比エラー対応のため　140812）
        fSensitivityRate = fSensitivityRateDtoU = 1.0;
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp, "\n%d	%lf	%lf\n", iCounter, fSensitivityRate, fSensitivityRateDtoU);
  #endif
 #else
		CApplication::GetInstance()->MessageBox("感度比の計算でエラーが発生しました。iCounter = 0", "Error", MB_OK);
  #ifdef _LP_DEBUG_OUTPUT
		fclose(fp);
  #endif
		return;
 #endif
	}
	else{
		fSensitivityRate = fSum / iCounter;
		fSensitivityRateDtoU = 1. / fSensitivityRate;
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp, "\n%d	%lf	%lf\n", iCounter, fSensitivityRate, fSensitivityRateDtoU);
  #endif
	}
 #endif

	for (int iS = 0; iS < SubPixel; iS++){
		for (int iM = 0; iM < MainPixel; iM++){
			if (ImageRateOfUpToDown.Dt[iM][0]) {
 #ifdef _AUTOADJUSTSENSITIVITY_TK
				ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM]/ImageRateOfUpToDown.Dt[iM][0]);//桑原さんアルゴ（斜めエッジのときなど、上パネルと中パネルの比が大きくなるところで縦帯が発生する副作用あり）
 #else
				ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM]/fSensitivityRate);
 #endif
			}
			else{
				ImageUp.SetVal(iS, iM, 0);
			}
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	fclose(fp);
	fclose(fp3);
	ImageUp.WriteTma(ExtractFilePath(FileName) + "U(" + FloatToStrF(fSensitivityRateDtoU,ffFixed,7,6) + ").tma");
 #endif

}
//---------------------------------------------------------------------------
//上画像の感度を中画像に合うように調整（同一箇所から20pix程度ずれたところで合わせるため精度に限界あり）
//(150326 ttsuji 日赤の臨床画像を受けて修正)


//#define  _DA_OFFSET
#define _GAISOH  //感度比を計算する際に、Top下端の副プロファイルをCenterパネルの白帯終点まで外挿することで感度比を計算
#define _HENKYOKUTEN //白帯終点を求める際に、Centerパネルの変曲点でも定義する。
#define _GRAD_MARGIN_OFFSET //傾きのオフセットマージンを設定する。（定数倍のみならず）
#define _ATH //腰椎＋アクリルのように白帯が非常に強く、長い場合の対策
//#define _SUBPROFILE_BOSYUUDAN_MOD //白帯長さを求めるときの副プロファイルの母集団の計算を変更する。←　効果あったが別画像で副作用あり、NG.


void TSIMARS::LPAdjustEndPanelSensitivity2(TFIMG& ImageRateOfUpToDown, TTMA& ImageUp, int* edgeY, string FileName, int iLimitSearchRange, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iMainShiftMax, float& fSensitivityRateDtoU, int iSSearchEnd)
{
	//シンチ段差下方の散乱線の有無を検出し、感度比計算に適切なｙ座標を求める
	//（ImageUpの下端のQL値の副方向の傾きと合致するCenterパネルのシンチ段差下方のｙ座標を計算）
	const int iMainMoveAve = 51;//とりあえず
	const int iSubMoveAve = 11;//とりあえず
	const int iMDiff = 50;//50pix間隔で調査
	const int iMCut = iMainShiftMax;//除外するpix(=30)
	TTMA SmoothImageUp = ImageUp;
	SmoothImageUp.MainMoveAve(iMainMoveAve);
	SmoothImageUp.SubMoveAve(iSubMoveAve);

	TTMA SmoothImageDown = *this;
	SmoothImageDown.MainMoveAve(iMainMoveAve);
	SmoothImageDown.SubMoveAve(iSubMoveAve);
 #ifdef _LP_DEBUG_OUTPUT
	SmoothImageUp.WriteTma(ExtractFilePath(FileName) + "SmoothImageUp(" + IntToStr(iMainMoveAve) + "," + IntToStr(iSubMoveAve) + ").tma");
	SmoothImageDown.WriteTma(ExtractFilePath(FileName) + "SmoothImageDown(" + IntToStr(iMainMoveAve) + "," + IntToStr(iSubMoveAve) + ").tma");
 #endif

	//左右の照射野絞りを検出することで感度比のロバスト向上(140823 ttsuji)
	const int iThQLMDif = 100;//14bitログ画像の主微分のQL閾値
//	const int iThQLMDif = 1000;//14bitログ画像の主微分のQL閾値
//	const int iThQLMDif = 70;//14bitログ画像の主微分のQL閾値
	bool bExist0 = false;//照射野境界の検出フラグ
	bool bExistE = false;//照射野境界の検出フラグ
//	int iSSearchRadiationField0   = SmoothImageUp.SubPixel - 100;
	int iSSearchRadiationField0   = SmoothImageUp.SubPixel - 101;//アルゴ仕様書記載にあわせる（141022 ttsuji)
	int iSSearchRadiationFieldEnd = SmoothImageUp.SubPixel - 1;//画像の下端
	int iMBorder0 = 0;//照射野境界のｘ座標
	int iMBorderEnd = SmoothImageUp.MainPixel - 1;//照射野境界のｘ座標
	LPDetectBorderRadiationField(FileName, SmoothImageUp, iThQLMDif, bExist0, bExistE, iMBorder0, iMBorderEnd, iSSearchRadiationField0, iSSearchRadiationFieldEnd, iLimitSearchRange);

	//ヒストグラムをとって最大QL、最小QLを計算  (左右は画像端から位置ずれ分30pixを除外し、上下は3pix平均とする)
	int iMStart = (iMBorder0 > iMainShiftMax) ? iMBorder0 + 1 : iMainShiftMax;
	int iMEnd = (iMBorderEnd < ImageUp.MainPixel - iMainShiftMax) ? iMBorderEnd - 1 : ImageUp.MainPixel - 1 - iMainShiftMax;//照射野境界のｘ座標
	ImageUp.Histogram(ImageUp.SubPixel - iUeOffset - 1, ImageUp.SubPixel - iUeOffset + 1, iMStart, iMEnd);
//	ImageUp.Histogram(ImageUp.SubPixel - iUeOffset - 1, ImageUp.SubPixel - iUeOffset + 1, iMainShiftMax, ImageUp.MainPixel - 1 - iMainShiftMax);
	float fThRate = 0.3;//ヒストの全面積の30％（感度比を計算する画素の判定に使用）
	int iThMaxQL = ImageUp.HistRuisekiDosuuPercent(fThRate, true);


//	int iNumOfSample = 100;//上パネルの最小二乗法のサンプル点の個数
	int iNumOfSample = 30;//上パネルの最小二乗法のサンプル点の個数(default)
	double* dS = new double[iNumOfSample];
	double* dQL = new double[iNumOfSample];
	Vct v;

#ifdef _LP_DEBUG_OUTPUT
	FILE* fp2 = fopen((ExtractFilePath(FileName) + "SearchSironukeRyoiki(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "," + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
	FILE* fpSubProfU = fopen((ExtractFilePath(FileName) + "SubProfileU(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
	FILE* fpSubProfD = fopen((ExtractFilePath(FileName) + "SubProfileD(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
	fprintf(fpSubProfU, "SubProfileU(%d)\n", iThMaxQL);
	fprintf(fpSubProfD, "SubProfileD(%d)\n", iThMaxQL);
#endif
	double dSum = 0.;
	int iCounter = 0;//関心領域内で検出した白帯位置の検出数
	int iSAve(0), iSMax(0);//関心領域内で検出した白帯位置の主方向の統計値


	//関心領域で平均化した副プロファイルをまず計算し、次にそのプロファイルを分析する（白帯は主方向全域に渡る＆ノイズが少ないのでこっちが精度良いと考える）
	TTMA SubProfileU, SubProfileD;
	SubProfileU.NewDt(SubPixel, 1, Bit);//関心領域で平均化した副プロファイル
	SubProfileD.NewDt(SubPixel, 1, Bit);//関心領域で平均化した副プロファイル
	float fSumU = 0.0;
	float fSumD = 0.0;
	int iCounter2 = 0;
#ifdef _LP_DEBUG_OUTPUT
	FILE* fp5 = fopen((ExtractFilePath(FileName) + "KansinXRyoikiShiroobiKensyutu(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
	fprintf(fp5, "iM	iCounter\n");
#endif

	for (int iS = 0; iS < SubPixel; iS++){
		fSumU = fSumD = 0.0;
		iCounter2 = 0;
/*
 #ifdef _DETECT_RADIATION_FIELD
		for (int iM = iMStart; iM < iMEnd; iM++){//照射野境界を除外して集計
 #else
		for (int iM = iMCut; iM < MainPixel - iMCut; iM++){
 #endif
*/
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!! 本当に主方向の範囲はこれでよいか？照射野境界を除去すべきでは？
  #if 0
		for (int iM = iMCut; iM < MainPixel - iMCut; iM++){//白帯検出については、照射野外も含む、従来のものとする
  #else //射野境界は除去した上で集計するように修正(150331 ttsuji)
//		for (int iM = iMStart; iM < iMEnd; iM++){//照射野境界を除外して集計
		for (int iM = iMStart; iM <= iMEnd; iM++){//照射野境界を除外して集計 (150916 iMEndも含めるよう修正)
  #endif

  #if 0		//tmp 150915 ttsuji
			if (1300 < iM && iM < 1550) {//VG評価画像で白帯検出ができなかった腰椎側面画像のガス組織を除外して副プロ計算（20150911-0006）
            	continue;
			}
  #endif

  #ifndef _SUBPROFILE_BOSYUUDAN_MOD
			if (iThMaxQL <= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM]) {//iMが素抜けの場合は排除
//			if (iThMaxQL <= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] || iThMinQL >= ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM]) {//iMが素抜けの場合は排除
				continue;
			}
  #else 	//最小自乗近似するｙの領域については、QL値がヒストグラムで計算した閾値以下の条件を満たすｘを選別する(150811 ttsuji 20150714-0004でパネル間濃度が合わない不具合を受けて)
			int iSubScanRange = iNumOfSample;//画像下端から最小自乗近似する30pixの範囲のｙ座標については、画素値をチェックする.
//			int iSubScanRange = 50;
			bool bFlagNGx = false;
			for (int iSCnt = 0; iSCnt < iSubScanRange; iSCnt++) {
				if (iThMaxQL <= ImageUp.Dt[ImageUp.SubPixel - iUeOffset - iSCnt][iM]) {
					bFlagNGx = true;//ｙ領域のうち、１つでも閾値を越える画素があれば、そのｘは副プロファイル計算の母集団に採用しない
					break;
				}
			}
			if (bFlagNGx) {
				continue;//次のｘ座標へ
			}
  #endif

  #ifdef _LP_DEBUG_OUTPUT
			if (0 == iS) {
				fprintf(fp5, "%d	%d\n", iM, iCounter2);
			}
  #endif

			//主方向に平均化
  #if 1
			fSumU += SmoothImageUp.Dt[iS][iM];
			fSumD += SmoothImageDown.Dt[iS][iM];
  #else     //段差部に金属を配置した場合に移動平均すると金属エッジがぼけて広がり、シンチ段差位置におけるQL値が変わり、パネル間感度補正係数が狂うため、移動平均を削除(150331 ttsuji)
			//白帯長さの計算時には上記の金属エッジの広がりは悪影響ないため、むしろスムージングして凸凹をなくした方が精度が良いため、元に戻す。(150406 ttsuji)
			fSumU += ImageUp.Dt[iS][iM];
			fSumD += Dt[iS][iM];
  #endif
			iCounter2++;
		}
		fSumU /= iCounter2;
		fSumD /= iCounter2;
		SubProfileU.SetVal(iS, 0, fSumU);
		SubProfileD.SetVal(iS, 0, fSumD);
 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpSubProfU, "%d\n", SubProfileU.Dt[iS][0]);
		fprintf(fpSubProfD, "%d\n", SubProfileD.Dt[iS][0]);
 #endif
	}
 #ifdef _LP_DEBUG_OUTPUT
//	SubProfileU.WriteTma(ExtractFilePath(FileName) + "SubProfileU(" + IntToStr(iThMaxQL) + ").tma");
//	SubProfileD.WriteTma(ExtractFilePath(FileName) + "SubProfileD(" + IntToStr(iThMaxQL) + ").tma");
	fclose(fpSubProfU);
	fclose(fpSubProfD);
	fclose(fp5);
	fprintf(fp2, "iIndex	iS	dDeltaQL[iIndex]\n");
 #endif

	//最小二乗法近似で上パネルの傾きを計算
 #ifdef _GAISOH
//	int iLen_PanelOverlap = 137;//TopとCenterとで同じ絵が写る重複領域長さ
	int iLen_PanelOverlap = 137 + round(m_EdgeShiftU.c / 10.);//TopとCenterとで同じ絵が写る重複領域長さ(150810 ttsuji 長手斜入を考慮できていない不具合対応)
 #endif
	for (int iS = 0; iS < iNumOfSample; iS++) {
 #ifdef _GAISOH
		dS[iS]  = iS + iLen_PanelOverlap - iNumOfSample;
 #else
		dS[iS]  = iS;
 #endif
		dQL[iS] = SubProfileU.Dt[SubProfileU.SubPixel - iNumOfSample + iS][0];
	}

	//直線fitting
	CLeastSquare LSItpl(iNumOfSample, dS, dQL, _LINEAR);
	if (!LSItpl.Calc()){
		CApplication::GetInstance()->MessageBox("補間プロファイルの直線fittingでエラーが発生しました。", "Error", MB_OK);
		delete[] dS;
		delete[] dQL;
  #ifdef _LP_DEBUG_OUTPUT
		fclose(fp2);
  #endif
		return;
	}
	v = LSItpl.GetSolve();
	//直線fittingの係数Ax+B
	double dA = v[1];
	double dB = v[0];
	dA = round(dA*1000) / 1000.;//小数第3位まで四捨五入で計算 (浮動小数点の不等式でMCと不整合でたため　141009)

/*
	//冪乗fitting
	CLeastSquare LSPower(iNumOfSample, dS, dQL, _POWER);
	if (!LSPower.Calc()){
		CApplication::GetInstance()->MessageBox("補間プロファイルの冪乗fittingでエラーが発生しました。", "Error", MB_OK);
		delete[] dS;
		delete[] dQL;
		fclose(fp2);
		return;
	}
	v = LSPower.GetSolve();
	//冪乗fittingの係数Bx^(A)
	double dApower = v[1];
	double dBpower = exp(v[0]);
*/


#if 0
//	const int iSDif = 5;//10pix刻みで副微分をとる
//	const int iSDif = 10;//10pix刻みで副微分をとる
	const int iSDif = 20;//20pix刻みで副微分をとる（白帯検出不可案件対応のため　140802）
	const int iSOffset = 20;//シンチ段差位置から、副微分値を調査するスタート点までのオフセット（白帯検出不可案件対応のため　140802）
#elif 0
	const int iSDif = 30;//20pix刻みで副微分をとる（白帯検出不可案件20150901-0006対応のため　150905）
	const int iSOffset = 25;//シンチ段差位置から、副微分値を調査するスタート点までのオフセット（白帯検出不可案件対応のため　140802）
#elif 0
	const int iSDif = 40;//20pix刻みで副微分をとる（白帯検出不可案件20150901-0006対応のため　150905）
	const int iSOffset = 30;//シンチ段差位置から、副微分値を調査するスタート点までのオフセット（白帯検出不可案件対応のため　140802）
#elif 1
	const int iSDif = 20;//20pix刻みで副微分をとる（白帯検出不可案件対応のため　150929）
	const int iSOffset = 30;//シンチ段差位置から、副微分値を調査するスタート点までのオフセット（白帯検出不可案件対応のため　150929）
#endif
	int iSDifHalf = iSDif / 2;

/*
	//iSSearchEndを引数にしたためコメントアウト(150916 ttsuji)
#if 1
//	const int iSSearchEnd = 300;//副微分値を探索する終点
	const int iSSearchEnd = 400;//副微分値を探索する終点
#else
//	const int iSSearchEnd = 420;//副微分値を探索する終点(20150901-0012白帯長さ検出失敗を受けて400より長く)
	const int iSSearchEnd = 500;//副微分値を探索する終点
#endif
*/
	double dQL10dif;//10pix刻みの副微分値
	int iSSearch = 0;//白帯がシンチ段差下方どこまで続いているか
#if 1
//	const double dGradMarginRate = 1.1;//上凸検出のときの傾きのマージンの％（白帯検出不可案件対応のため　140802）
//	const double dGradMarginRate = 1.2;//上凸検出のときの傾きのマージンの％（白帯検出不可案件対応のため　140802）
	const double dGradMarginRate = 1.25;//上凸検出のときの傾きのマージンの％（腰椎側面＋アクリル5cmグリッドなしの白帯検出不可案件対応のため　150830）
#else
	const double dGradMarginRate = 1.5;//上凸検出のときの傾きのマージンの％（腰椎側面＋アクリル5cm、胸部正面肉襦袢＋アクリル6cm、グリッドなしの白帯検出不可案件対応のため　150926）
#endif

#ifdef _DA_OFFSET
	const double dAOffset = 0.3;//dAが小さすぎてサーチ範囲の中にdAを下回るdQL10difがない場合はdAをオフセットする（白帯検出不可案件対応のため　140811）
#endif

	//白帯起因の上凸カーブであるかをまずチェック（上凸カーブがなければ白帯なしとしてiSSearch=0とする）
	double dDeltaQL[3];
	int iIndex = 0;
	bool bExistShiroobi = false;
 #ifdef _ATH
  #if 0
//	double dATh = 3.0;//傾きの差分の閾値（白帯起因の副プロファイルの傾きが強いときのみ白帯終点とならないよう大きめに設定）
//	double dATh = 2.25;//傾きの差分の閾値（白帯終点のときとそうでないときの統計結果から閾値を修正(150709 ttsuji))
	double dATh = 2.75;//斜めスキンラインの腰椎側面のデータから閾値を修正(150715 ttsuji))
//	double dATh = 1.9;//3R-16尺骨（長手45度）データから閾値を修正(150810 ttsuji))
  #else
//	double dATh = 3.0;//傾きの差分の閾値（腰椎側面＋アクリル5cm、胸部正面肉襦袢＋アクリル6cm、グリッドなしの白帯検出不可案件対応のため　150926）
///	double dATh = 7.5;//傾きの差分の閾値（腰椎側面+グリッドなし(20151201-0021TC)の白帯検出不可案件対応のため　151201）
	double dATh = 3.5;//傾きの差分の閾値（胸部正面アクリル6cm_20150925-0004、グリッドなしの白帯検出不可案件対応のため　151008）
  #endif
 #endif

	int iYMaxSintiEdge = round(m_SintiEdgeU.ymax / 10);//シンチ段差のｙ座標最下点

	//シンチ段差より下方の副プロファイルの接線の傾きを求める
	for (int iIndex = 0; iIndex < 3; iIndex++) {
 #if 0
		int iS = edgeY[0] + iSOffset;//iSDifによらずスタート点は固定とする。（白帯検出不可案件対応のため　140802）
 #else 	//白帯誤検出（上凸検出失敗）を受けて変更（151006 ttsuji)
		int iS = iYMaxSintiEdge + iSOffset;//iSDifによらずスタート点は固定とする。（白帯検出不可案件対応のため　140802）
 #endif
//		dDeltaQL[iIndex] = (double)(SubProfileD.Dt[iS + iIndex*iSDif + iSDifHalf][0] - SubProfileD.Dt[iS + iIndex*iSDif - iSDifHalf][0]) / iSDif;
		dDeltaQL[iIndex] = (double)(SubProfileD.Dt[iS + iIndex*iSDif + iSDifHalf][0] - SubProfileD.Dt[iS + iIndex*iSDif - iSDifHalf][0]) / iSDif;
 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp2, "%d	%d	%lf\n", iIndex, iS + iIndex*iSDif, dDeltaQL[iIndex]);
 #endif
	}
 #ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\niS	dQL10Dif	dA	dQL10DifDif	dQL10Dif-dA\n");
 #endif
	//シンチ段差下方の接線の傾きと上パネル下端の接線の傾きの関係から白帯あるか判定
	double dQL10difdif(-1000000.);//二階微分（凸性）
	double dPreQL10dif;//ｙ座標の1個前の傾き（二階微分を計算するため）
 #ifdef _GRAD_MARGIN_OFFSET
	//傾きの定数倍のマージン　or 傾きのオフセット加算マージンの　いずれかを満たすときに白帯ありとする(150409 ttsuji  日赤20150223-0011CBを受けて)
	const double dGradMarginOffset = 0.05;//1QLの1/iSDif = 1/20で0.05とする

  #if 0 //double型の比較のバグあり
	if ((dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] * dGradMarginRate >= dDeltaQL[1] && dDeltaQL[1] * dGradMarginRate >= dDeltaQL[2]) ||
		(dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] + dGradMarginOffset >= dDeltaQL[1] && dDeltaQL[1] + dGradMarginOffset >= dDeltaQL[2])){
  #else  //double型の比較の誤差を考慮(151002 ttsuji)
	double dError = 1e-4;
	if ((dA < dDeltaQL[0] + dError && 0 < dDeltaQL[0] + dError && dDeltaQL[0] * dGradMarginRate >= dDeltaQL[1] - dError && dDeltaQL[1] * dGradMarginRate >= dDeltaQL[2] - dError) ||
		(dA < dDeltaQL[0] + dError && 0 < dDeltaQL[0] + dError && dDeltaQL[0] + dGradMarginOffset >= dDeltaQL[1] - dError && dDeltaQL[1] + dGradMarginOffset >= dDeltaQL[2] - dError)){
//	if (1) {

  #endif

 #else
	//上画像の下端の傾きを中画像のシンチ段差直下と比較するように修正（白帯検出不可案件対応のため　140802）
	if (dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] * dGradMarginRate >= dDeltaQL[1] && dDeltaQL[1] * dGradMarginRate >= dDeltaQL[2]) {
 #endif
//		for (int iS = edgeY[0] + iSOffset; iS < iSSearchEnd; iS += iSDif) {
		for (int iS = iYMaxSintiEdge + iSOffset; iS < iSSearchEnd; iS += iSDif) {
 #ifdef _HENKYOKUTEN
			dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;

//			if (iS > edgeY[0] + iSOffset) {
			if (iS > iYMaxSintiEdge + iSOffset) {
				dQL10difdif = dQL10dif - dPreQL10dif;//二階微分（凸性）
			}

  #ifdef _LP_DEBUG_OUTPUT
//			fprintf(fp2, "%d	%lf	%lf	%lf\n", iS, dQL10dif, dA, dQL10difdif);
			fprintf(fp2, "%d	%lf	%lf	%lf	%lf\n", iS, dQL10dif, dA, dQL10difdif, dQL10dif - dA);
  #endif

			//目標の傾きdAよりも大きい場合、または二階微分が0以上となる場合は白帯終点とする
			if (dA > dQL10dif) {
				iSSearch = iS;
				bExistShiroobi = true;
				break;
			}
  #ifdef _ATH
			//腰椎＋アクリルのように、白帯が非常に強い場合は、変曲点がシンチ段差近傍となる不具合対策(150604 ttsuji)
//			else if (iS > edgeY[0] + iSOffset && dQL10difdif >= 0 && dQL10dif - dA < dATh) {
			else if (iS > iYMaxSintiEdge + iSOffset && dQL10difdif >= 0 && dQL10dif - dA < dATh) {
  #else
//			else if (iS > edgeY[0] + iSOffset && dQL10difdif >= 0) {
			else if (iS > iYMaxSintiEdge + iSOffset && dQL10difdif >= 0) {
  #endif
				iSSearch = iS - iSDifHalf;
				bExistShiroobi = true;
				break;
			}
			dPreQL10dif = dQL10dif;//次のｙ座標で二階微分計算用に規定

 #else
			dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
  #ifdef _LP_DEBUG_OUTPUT
			fprintf(fp2, "%d	%lf	%lf\n", iS, dQL10dif, dA, dQL10difdif);
  #endif
			if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
				iSSearch = iS;
				bExistShiroobi = true;
				break;
			}
 #endif
		}

		//シンチ段差より下方の白帯の長さ
		if (bExistShiroobi) {
//			int iShiroobiLen = iSSearch - edgeY[0];
			int iShiroobiLen = iSSearch - iYMaxSintiEdge;
			iSitaOffset = iShiroobiLen;//引数を更新
		}
 #ifdef _DA_OFFSET
		else{//dAが小さすぎてサーチ範囲の中にdAを下回るdQL10difがない場合はdAをオフセットする（白帯検出不可案件対応のため　140811）
			dA += dAOffset;

//			for (int iS = edgeY[0] + iSOffset; iS < iSSearchEnd; iS += iSDif) {
			for (int iS = iYMaxSintiEdge + iSOffset; iS < iSSearchEnd; iS += iSDif) {
				dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
  #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp2, "%d	%lf	%lf\n", iS, dQL10dif, dA);
  #endif
				if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
					iSSearch = iS;
					bExistShiroobi = true;
					break;
				}
			}

			//シンチ段差より下方の白帯の長さ
			if (bExistShiroobi) {
//				int iShiroobiLen = iSSearch - edgeY[0];
				int iShiroobiLen = iSSearch - iYMaxSintiEdge;
				iSitaOffset = iShiroobiLen;//引数を更新
			}
		}
 #endif
	}

 #ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\ndA	dB	iSSearch	dQL10Dif	bExistShiroobi\n");
	fprintf(fp2, "%lf	%lf	%d	%lf	%d\n", dA, dB, iSSearch, dQL10dif, bExistShiroobi);
	fprintf(fp2, "\niSitaOffset\n");
	fprintf(fp2, "%d\n", iSitaOffset);
	fclose(fp2);
 #endif
//	delete[] dS;
//	delete[] dQL;



 #if 0
 //test
//	iSitaOffset = 3;//白帯を無視してパネル間感度補正係数を計算（150317 ttsuji）
//	iSitaOffset = 160;//白帯を無視してパネル間感度補正係数を計算
//	iSitaOffset = 100;//白帯長さを決めうちにしてパネル間感度補正係数を計算
	iSitaOffset = 250;//白帯長さを決めうちにしてパネル間感度補正係数を計算
 #endif


#ifdef _GAISOH
 #if 1  //外挿する副プロファイルの計算に、ｘごとの感度比の異常値検出を導入（人工骨混入による感度ずれ対策）

  #ifdef _HIST_CENTER
	//感度比を計算する際に、Top(or Bottom)のみならずCenterのHistgramも解析する
//	Histogram(edgeY[0] + iSitaOffset - 1, edgeY[0] + iSitaOffset + 1, iMStart, iMEnd);
	Histogram(iYMaxSintiEdge + iSitaOffset - 1, iYMaxSintiEdge + iSitaOffset + 1, iMStart, iMEnd);
	int iThMaxQL_Center = HistRuisekiDosuuPercent(fThRate, true);
  #endif


  #ifdef _LP_DEBUG_OUTPUT
	FILE* fpSubProfU2 = fopen((ExtractFilePath(FileName) + "SubProfileU2(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
	FILE* fpSubProfD2 = fopen((ExtractFilePath(FileName) + "SubProfileD2(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
	fprintf(fpSubProfU2, "SubProfileU2(%d)\n", iThMaxQL);
	fprintf(fpSubProfD2, "SubProfileD2(%d)\n", iThMaxQL);

	FILE* fp = fopen((ExtractFilePath(FileName) + "AdjustEndPanelSensitivity(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + ").txt").c_str(), "wt");
	fprintf(fp, "iM	ImageU	ImageD	RateOfUToD\n");
   #ifdef _HIST_CENTER
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + "," + IntToStr(iThMaxQL_Center) + ").txt").c_str(), "wt");
   #else
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
   #endif
	fprintf(fp3, "iM	iCounter\n");
  #endif
	//感度の比を各ｘごとに計算し、ｘ方向に集計
	for (int iM = 0; iM < MainPixel; iM++){
		if (0 == Dt[edgeY[iM] + iSitaOffset][iM]) {
			ImageRateOfUpToDown.Dt[iM][0] = 0.; //集計時に除外するように値を設定
		}
		else{
			ImageRateOfUpToDown.Dt[iM][0] = ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] / (float)Dt[edgeY[iM] + iSitaOffset][iM];
		}
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp, "%d	%d	%d	%lf\n", iM, ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM], Dt[edgeY[iM] + iSitaOffset][iM], ImageRateOfUpToDown.Dt[iM][0]);
  #endif
	}
  #ifdef _LP_DEBUG_OUTPUT
	ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "RateOfUpToDown.txt", true);
  #endif

  #if 0
	//主方向にスムージングすると人工骨起因で感度比が異常となっているのに、正常と判断してしまうためスムージングをはずす（150331 ttsuji）
	ImageRateOfUpToDown.MainMedian(201);
  #endif

  #ifdef _LP_DEBUG_OUTPUT
	ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "Med_RateOfUpToDown.txt", true);
  #endif

	float fSensitivityRate;//主方向に平均した感度比
	float fSum = 0.0;
		const int iCut = iMainShiftMax;//平均値算出から除外する端画素

/*
	for (int iMCnt = iMStart; iMCnt <= iMEnd; iMCnt++){//照射野境界を除外して集計
  #ifdef _HIST_CENTER
		//感度比を計算する際に、Top(or Bottom)のみならずCenterの画素値も考慮する(150316 ttsuji)
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt] && iThMaxQL_Center > Dt[edgeY[iMCnt] + iSitaOffset][iMCnt]) {
  #else
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt]) {//異常値は排除
  #endif
			fSum += ImageRateOfUpToDown.Dt[iMCnt][0];
			iCounter++;
  #ifdef _LP_DEBUG_OUTPUT
			fprintf(fp3, "%d	%d\n", iMCnt, iCounter);
  #endif
		}
	}
	if (!iCounter) {
		fSensitivityRate = fSensitivityRateDtoU = 1.0;
		fprintf(fp, "\n%d	%lf	%lf\n", iCounter, fSensitivityRate, fSensitivityRateDtoU);
	}
	else{
		fSensitivityRate = fSum / iCounter;
		fSensitivityRateDtoU = 1. / fSensitivityRate;
		fprintf(fp, "\n%d	%lf	%lf\n", iCounter, fSensitivityRate, fSensitivityRateDtoU);
	}
*/



/*
	const float fLowerTh = 0.7;
	const float fUpperTh = 1.5;
*/
	const float fLowerTh = fLowerLimitOfPanelSensRatio;
	const float fUpperTh = fUpperLimitOfPanelSensRatio;

	fSumU = 0.0;
	fSumD = 0.0;
	iCounter2 = 0;
	for (int iS = 0; iS < SubPixel; iS++){
		fSumU = fSumD = 0.0;
		iCounter2 = 0;

		for (int iM = iMStart; iM <= iMEnd; iM++){//照射野境界を除外して集計
			if (1200==iM) {
				int a = 0;
			}

  #ifdef _HIST_CENTER
			//感度比を計算する際に、Top(or Bottom)のみならずCenterの画素値も考慮する(150316 ttsuji)
			if (0 != ImageRateOfUpToDown.Dt[iM][0] && fLowerTh < ImageRateOfUpToDown.Dt[iM][0] && fUpperTh > ImageRateOfUpToDown.Dt[iM][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] && iThMaxQL_Center > Dt[edgeY[iM] + iSitaOffset][iM]) {
  #else
			if (0 != ImageRateOfUpToDown.Dt[iM][0] && fLowerTh < ImageRateOfUpToDown.Dt[iM][0] && fUpperTh > ImageRateOfUpToDown.Dt[iM][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM]) {//異常値は排除
  #endif
				//主方向に平均化
  #if 0
				fSumU += SmoothImageUp.Dt[iS][iM];
				fSumD += SmoothImageDown.Dt[iS][iM];
  #else         //段差部に金属を配置した場合に移動平均すると金属エッジがぼけて広がり、シンチ段差位置におけるQL値が変わり、パネル間感度補正係数が狂うため、移動平均を削除(150331 ttsuji)
				fSumU += ImageUp.Dt[iS][iM];
				fSumD += Dt[iS][iM];
  #endif
				iCounter2++;


  #ifdef _LP_DEBUG_OUTPUT
				if (0 == iS) {
					fprintf(fp3, "%d	%d\n", iM, iCounter2);
				}
  #endif

			}
		}
		if (iCounter2) {
			fSumU /= iCounter2;
			fSumD /= iCounter2	;
		}
		SubProfileU.SetVal(iS, 0, fSumU);
		SubProfileD.SetVal(iS, 0, fSumD);
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpSubProfU2, "%d\n", SubProfileU.Dt[iS][0]);
		fprintf(fpSubProfD2, "%d\n", SubProfileD.Dt[iS][0]);
  #endif


	}

  #ifdef _LP_DEBUG_OUTPUT
//	SubProfileU.WriteTma(ExtractFilePath(FileName) + "SubProfileU(" + IntToStr(iThMaxQL) + ").tma");
//	SubProfileD.WriteTma(ExtractFilePath(FileName) + "SubProfileD(" + IntToStr(iThMaxQL) + ").tma");
	fclose(fpSubProfU2);
	fclose(fpSubProfD2);
	fclose(fp);
	fclose(fp3);
  #endif

	//最小二乗法近似で上パネルの傾きを計算
//	iLen_PanelOverlap = 137;//TopとCenterとで同じ絵が写る重複領域長さ
	iLen_PanelOverlap = 137 + round(m_EdgeShiftU.c / 10.);//TopとCenterとで同じ絵が写る重複領域長さ(150810 ttsuji 長手斜入を考慮できていない不具合対応)

	for (int iS = 0; iS < iNumOfSample; iS++) {
		dS[iS]  = iS + iLen_PanelOverlap - iNumOfSample;
		dQL[iS] = SubProfileU.Dt[SubProfileU.SubPixel - iNumOfSample + iS][0];
	}

	//冪乗fitting
	CLeastSquare LSPower(iNumOfSample, dS, dQL, _POWER);
	double dPanelSensitivityRate;//パネル間感度補正係数(Center基準)
	double dApower;
	double dBpower;
	double dQLu_GaisohAtShiroobiEnd;
	double dQLd_AtShiroobiEnd;
//	int iYMaxSintiEdge;

#if 0   //ゼロ行列のときの対応が未実装
	if (!LSPower.Calc()){
		CApplication::GetInstance()->MessageBox("補間プロファイルの冪乗fittingでエラーが発生しました。", "Error", MB_OK);
		delete[] dS;
		delete[] dQL;
		return;
	}
	v = LSPower.GetSolve();
	//冪乗fittingの係数Bx^(A)
	dApower = v[1];
	dBpower = exp(v[0]);

	//感度比を計算する際に、Top下端の副プロファイルをCenterパネルの白帯終点まで外挿することで感度比を計算
//	double dQLu_GaisohAtShiroobiEnd = dA * (edgeY[0] + iSitaOffset) + dB;//白帯終点におけるTopパネル下端副プロファイル外挿(直線fitting)
  #if 0
	//シンチ段差のｙ座標の最下点はx=0で決め打ち
	dQLu_GaisohAtShiroobiEnd = dBpower * pow(edgeY[0] + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
	dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[edgeY[0] + iSitaOffset][0];
  #else
	//シンチ段差のｙ座標の最下点を計算する。
//	iYMaxSintiEdge = round(m_SintiEdgeU.ymax / 10.);
//	dQLu_GaisohAtShiroobiEnd = dBpower * pow(edgeY[MainPixel - 1] + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
	dQLu_GaisohAtShiroobiEnd = dBpower * pow(iYMaxSintiEdge + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
//	dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[edgeY[MainPixel - 1] + iSitaOffset][0];
	dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[iYMaxSintiEdge + iSitaOffset][0];
  #endif
	dPanelSensitivityRate = dQLd_AtShiroobiEnd / dQLu_GaisohAtShiroobiEnd;

  #if 0 //手動で強制的にパネル間感度補正係数を指定
	dPanelSensitivityRate = 0.97;
  #endif

	//異常値の場合は1.0とする
	if (dPanelSensitivityRate <= fLowerTh || dPanelSensitivityRate >= fUpperTh){
		dPanelSensitivityRate = 1.0;
	}
#else
	if (LSPower.Calc()){
		v = LSPower.GetSolve();
		//冪乗fittingの係数Bx^(A)
		dApower = v[1];
		dBpower = exp(v[0]);

		//感度比を計算する際に、Top下端の副プロファイルをCenterパネルの白帯終点まで外挿することで感度比を計算
		//	double dQLu_GaisohAtShiroobiEnd = dA * (edgeY[0] + iSitaOffset) + dB;//白帯終点におけるTopパネル下端副プロファイル外挿(直線fitting)
  #if 0
		//シンチ段差のｙ座標の最下点はx=0で決め打ち
		dQLu_GaisohAtShiroobiEnd = dBpower * pow(edgeY[0] + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
		dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[edgeY[0] + iSitaOffset][0];
  #else
		//シンチ段差のｙ座標の最下点を計算する。

//	kokowojissou surukoto.
		iYMaxSintiEdge = round(m_SintiEdgeU.ymax / 10.);
		dQLu_GaisohAtShiroobiEnd = dBpower * pow(iYMaxSintiEdge + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
//		double dQLu_GaisohAtShiroobiEnd = dBpower * pow(edgeY[MainPixel - 1] + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
//		dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[edgeY[MainPixel - 1] + iSitaOffset][0];
		dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[iYMaxSintiEdge + iSitaOffset][0]; //150918 ttsuji修正
  #endif
		dPanelSensitivityRate = dQLd_AtShiroobiEnd / dQLu_GaisohAtShiroobiEnd;

  #if 0 //手動で強制的にパネル間感度補正係数を指定
//		dPanelSensitivityRate = 0.87708;
//		dPanelSensitivityRate = 0.863087;
//		dPanelSensitivityRate = 1.;
//		dPanelSensitivityRate = 1.1;
		dPanelSensitivityRate = 1.15;
//		dPanelSensitivityRate = 1.05;//決めうち　20150326-0013CB
  #endif

		//異常値の場合は1.0とする
		if (dPanelSensitivityRate <= fLowerTh || dPanelSensitivityRate >= fUpperTh){
			dPanelSensitivityRate = 1.0;
		}
	}
	else{//ゼロ行列のとき
		dPanelSensitivityRate = 1.0;

	}
#endif

	//Topパネルに感度補正係数を乗算
	for (int iS = 0; iS < ImageUp.SubPixel; iS++){
		for (int iM = 0; iM < ImageUp.MainPixel; iM++){
			ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM] * dPanelSensitivityRate);
		}
	}
  #ifdef _LP_DEBUG_OUTPUT
	FILE* fp4 = fopen((ExtractFilePath(FileName) + "PanelSensitivityRate(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + ").txt").c_str(), "wt");
	fprintf(fp4, "B * x^(A)\n");
	fprintf(fp4, "A	%lf\n", dApower);
	fprintf(fp4, "B	%lf\n", dBpower);
	fprintf(fp4, "iSitaOffset	%d\n", iSitaOffset);
	fprintf(fp4, "iYMaxSintiEdge	%d\n", iYMaxSintiEdge);
//	fprintf(fp4, "edgeY[0] + iSitaOffset	%d\n", edgeY[0] + iSitaOffset);
	fprintf(fp4, "iYMaxSintiEdge + iSitaOffset	%d\n", iYMaxSintiEdge + iSitaOffset);
	fprintf(fp4, "dQLu_GaisohAtShiroobiEnd	%lf\n", dQLu_GaisohAtShiroobiEnd);
	fprintf(fp4, "dQLd_AtShiroobiEnd	%lf\n", dQLd_AtShiroobiEnd);
	fprintf(fp4, "dPanelSensitivityRate	%lf\n", dPanelSensitivityRate);
	fclose(fp4);

	ImageUp.WriteTma(ExtractFilePath(FileName) + "U(" + FloatToStrF((float)dPanelSensitivityRate, ffFixed,7,6) + ").tma");
  #endif

	fSensitivityRateDtoU = (float)dPanelSensitivityRate;


 #else
	//感度比を計算する際に、Top下端の副プロファイルをCenterパネルの白帯終点まで外挿することで感度比を計算
//	double dQLu_GaisohAtShiroobiEnd = dA * (edgeY[0] + iSitaOffset) + dB;//白帯終点におけるTopパネル下端副プロファイル外挿(直線fitting)
//	double dQLu_GaisohAtShiroobiEnd = dBpower * pow(edgeY[0] + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
	double dQLu_GaisohAtShiroobiEnd = dBpower * pow(iYMaxSintiEdge + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
//	double dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[edgeY[0] + iSitaOffset][0];
	double dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[iYMaxSintiEdge + iSitaOffset][0];
	double dPanelSensitivityRate;//パネル間感度補正係数(Center基準)
	dPanelSensitivityRate = dQLd_AtShiroobiEnd / dQLu_GaisohAtShiroobiEnd;

	//異常値の場合は1.0とする
	const float fLowerTh = 0.7;
	const float fUpperTh = 1.5;
	if (dPanelSensitivityRate <= fLowerTh || dPanelSensitivityRate >= fUpperTh){
		dPanelSensitivityRate = 1.0;
	}

	//Topパネルに感度補正係数を乗算
	for (int iS = 0; iS < ImageUp.SubPixel; iS++){
		for (int iM = 0; iM < ImageUp.MainPixel; iM++){
			ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM] * dPanelSensitivityRate);
		}
	}
  #ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "AdjustEndPanelSensitivity(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + ").txt").c_str(), "wt");
	fprintf(fp, "B * x^(A)\n");
	fprintf(fp, "A	%lf\n", dApower);
	fprintf(fp, "B	%lf\n", dBpower);
	fprintf(fp, "iSitaOffset	%d\n", iSitaOffset);
//	fprintf(fp, "edgeY[0] + iSitaOffset	%d\n", edgeY[0] + iSitaOffset);
	fprintf(fp, "iYMaxSintiEdge + iSitaOffset	%d\n", iYMaxSintiEdge + iSitaOffset);
	fprintf(fp, "dQLu_GaisohAtShiroobiEnd	%lf\n", dQLu_GaisohAtShiroobiEnd);
	fprintf(fp, "dQLd_AtShiroobiEnd	%lf\n", dQLd_AtShiroobiEnd);
	fprintf(fp, "dPanelSensitivityRate	%lf\n", dPanelSensitivityRate);
	fclose(fp);

	ImageUp.WriteTma(ExtractFilePath(FileName) + "U(" + FloatToStrF((float)dPanelSensitivityRate, ffFixed,7,6) + ").tma");
	fSensitivityRateDtoU = (float)dPanelSensitivityRate;
  #endif
 #endif

#else
 #ifdef _HIST_CENTER
	//感度比を計算する際に、Top(or Bottom)のみならずCenterのHistgramも解析する
//	Histogram(edgeY[0] + iSitaOffset - 1, edgeY[0] + iSitaOffset + 1, iMStart, iMEnd);
	Histogram(iYMaxSintiEdge + iSitaOffset - 1, iYMaxSintiEdge + iSitaOffset + 1, iMStart, iMEnd);
	int iThMaxQL_Center = HistRuisekiDosuuPercent(fThRate, true);
 #endif

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "AdjustEndPanelSensitivity(" + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + ").txt").c_str(), "wt");
	fprintf(fp, "iM	ImageU	ImageD	RateOfUToD\n");
  #ifdef _HIST_CENTER
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + "," + IntToStr(iThMaxQL_Center) + ").txt").c_str(), "wt");
  #else
	FILE* fp3 = fopen((ExtractFilePath(FileName) + "KansinXRyoiki(" + IntToStr(iThMaxQL) + ").txt").c_str(), "wt");
  #endif
	fprintf(fp3, "iM	iCounter\n");
 #endif
	//感度の比を各ｘごとに計算し、ｘ方向に集計
	for (int iM = 0; iM < MainPixel; iM++){
		if (0 == Dt[edgeY[iM] + iSitaOffset][iM]) {
			ImageRateOfUpToDown.Dt[iM][0] = 0.; //集計時に除外するように値を設定
		}
		else{
			ImageRateOfUpToDown.Dt[iM][0] = ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] / (float)Dt[edgeY[iM] + iSitaOffset][iM];
		}
 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fp, "%d	%d	%d	%lf\n", iM, ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM], Dt[edgeY[iM] + iSitaOffset][iM], ImageRateOfUpToDown.Dt[iM][0]);
 #endif
	}
 #ifdef _LP_DEBUG_OUTPUT
	ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "RateOfUpToDown.txt", true);
 #endif
	ImageRateOfUpToDown.MainMedian(201);
 #ifdef _LP_DEBUG_OUTPUT
	ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "Med_RateOfUpToDown.txt", true);
 #endif

	float fSensitivityRate;//主方向に平均した感度比
	float fSum = 0.0;
	const int iCut = iMainShiftMax;//平均値算出から除外する端画素
/*
	const float fLowerTh = 0.7;
	const float fUpperTh = 1.5;
*/
	const float fLowerTh = fLowerLimitOfPanelSensRatio;
	const float fUpperTh = fUpperLimitOfPanelSensRatio;
	iCounter = 0;

	for (int iMCnt = iMStart; iMCnt <= iMEnd; iMCnt++){//照射野境界を除外して集計
  #ifdef _HIST_CENTER
		//感度比を計算する際に、Top(or Bottom)のみならずCenterの画素値も考慮する(150316 ttsuji)
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt] && iThMaxQL_Center > Dt[edgeY[iMCnt] + iSitaOffset][iMCnt]) {
  #else
//		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] || fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] || fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0]) {//異常値は排除
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt]) {//異常値は排除
  #endif
			fSum += ImageRateOfUpToDown.Dt[iMCnt][0];
			iCounter++;
  #ifdef _LP_DEBUG_OUTPUT
			fprintf(fp3, "%d	%d\n", iMCnt, iCounter);
  #endif
		}
	}
	if (!iCounter) {
		fSensitivityRate = fSensitivityRateDtoU = 1.0;
		fprintf(fp, "\n%d	%lf	%lf\n", iCounter, fSensitivityRate, fSensitivityRateDtoU);
	}
	else{
		fSensitivityRate = fSum / iCounter;
		fSensitivityRateDtoU = 1. / fSensitivityRate;
		fprintf(fp, "\n%d	%lf	%lf\n", iCounter, fSensitivityRate, fSensitivityRateDtoU);
	}

	for (int iS = 0; iS < SubPixel; iS++){
		for (int iM = 0; iM < MainPixel; iM++){
			if (ImageRateOfUpToDown.Dt[iM][0]) {
				ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM]/fSensitivityRate);
			}
			else{
				ImageUp.SetVal(iS, iM, 0);
			}
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	fclose(fp);
	fclose(fp3);
	ImageUp.WriteTma(ExtractFilePath(FileName) + "U(" + FloatToStrF(fSensitivityRateDtoU,ffFixed,7,6) + ").tma");
 #endif
#endif

}
//---------------------------------------------------------------------------
//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主Medianをかけて、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする

//#define _PROFILING　//副方向に平均化して主方向プロファイルを作成して相互相関する
//#define _BINARY//2値化してエッジを強調する
#define _MAINDIF//主方向に微分してエッジを強調する


void TSIMARS::LPDetectHorizontalMisalignmentAndShiftImageUp(TTMA& ImageUp, string FileNameImageUp, int& iMShiftDetect, string FileName,
										int iMainShiftMax,   //短手斜入による左右方向の画像シフトの最大値
										int iSubNumMatch,    //マッチングする短冊画像の副方向の長さ（とりあえず10画素）
										int iMainMedianMatch,//マッチングする短冊画像への主Median(とりあえず5画素)
										int iSubRangeOnlyMidPanel, int iYSintiEnd)
{

	//上・中画像の絵が重複している短冊領域のみ抽出（パターンマッチングに使用）
	TTMA FImageUp, FImageDown;
	FImageUp = ImageUp;
	FImageDown = *this;

 #if 0
	//左右端の除外なし
	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel, -(ImageUp.SubPixel - 1 - iSubNumMatch), 0);//中画像の上短冊を抽出
	FImageDown.Format(iSubNumMatch, MainPixel, -(iYSintiEnd - iSubRangeOnlyMidPanel - iSubNumMatch), 0);//中画像の上短冊を抽出
 #elif 1
	//左右端から30pixを除外（左右位置ずれの最大量）
	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel - 2*iMainShiftMax, -(ImageUp.SubPixel - 1 - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
	FImageDown.Format(iSubNumMatch, MainPixel - 2*iMainShiftMax, -(iYSintiEnd - iSubRangeOnlyMidPanel - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #elif 0
	//左右端から所定の長さを除外（特に短手斜入時に画像端にコントラストの強いもの（グリッド境界等）が写り込むことによる相互相関のずれの対策）
	const int iMainPixExclusion = 200;//左右端から除外する長さ
//	const int iMainPixExclusion = 300;//左右端から除外する長さ
//	const int iMainPixExclusion = 400;//左右端から除外する長さ
//	const int iMainPixExclusion = 600;//左右端から除外する長さ
	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel - 2*iMainPixExclusion, -(ImageUp.SubPixel - 1 - iSubNumMatch),  -iMainPixExclusion);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して両端を除外してトリミング）
	FImageDown.Format(iSubNumMatch, MainPixel - 2*iMainPixExclusion, -(iYSintiEnd - iSubRangeOnlyMidPanel - iSubNumMatch),  -iMainPixExclusion);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して両端を除外してトリミング）
 #endif

	//ログ変換してから相互相関と計算する
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	string StrPre = "";
	FImageUp.LogThreshold(dMidVal, dKeta, iLogTh);
	FImageUp.BitChange(14);
	FImageDown.LogThreshold(dMidVal, dKeta, iLogTh);
	FImageDown.BitChange(14);

 #if 0
	//temporary処理（斜め頭蓋の画像の上・中画像の感度を揃えるためにログ画像にオフセット加算する）
	FImageDown += 102;//上・中画像の平均QLを揃える
//	FImageDown -= 100;//上・中画像の平均QLをあえて変える
 #endif


 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "FImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "FImageDown.tma");
 #endif


 #ifdef _MAINDIF
	//左右方向へ微分
	FImageUp.DifMainUp();
	FImageDown.DifMainUp();
	FImageUp.Format(iSubNumMatch, FImageUp.MainPixel - 1, 0, -1);
	FImageDown.Format(iSubNumMatch, FImageDown.MainPixel - 1, 0, -1);
	FImageUp.WriteTma(ExtractFilePath(FileName) + "DifFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "DifFImageDown.tma");
 #endif


 #ifndef _MAINDIF
	//主方向Medianでノイズ落とす
	FImageUp.MainMedian(iMainMedianMatch);
	FImageDown.MainMedian(iMainMedianMatch);
 #else
	//主方向移動平均でノイズ落とす(主方向微分のときはこれが精度良い)
	FImageUp.MainMoveAve(iMainMedianMatch);
	FImageDown.MainMoveAve(iMainMedianMatch);
 #endif


 #ifdef _BINARY
	int iQLThBinary = FImageUp.Average();
	double dA = 65535;
	for (int iS = 0 ; iS < FImageUp.SubPixel ; iS++){
		for (int iM = 0 ; iM < FImageUp.MainPixel ; iM++){
			FImageUp.SetVal(iS, iM, dA * (FImageUp.Dt[iS][iM] - iQLThBinary));
			FImageDown.SetVal(iS, iM, dA * (FImageDown.Dt[iS][iM] - iQLThBinary));
		}
	}
	FImageUp.WriteTma(ExtractFilePath(FileName) + "BinaryFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "BinaryFImageDown.tma");
 #endif



 #ifdef _PROFILING
	//副方向に平均化してノイズ落とす
	TTMA ProfileUp, ProfileDown;
	ProfileUp.NewDt(1, FImageUp.MainPixel, FImageUp.Bit);
	ProfileDown.NewDt(1, FImageDown.MainPixel, FImageDown.Bit);
	double dSumUp, dSumDown;
	for (int iM = 0; iM < FImageUp.MainPixel; iM++) {
		dSumUp = dSumDown = 0.;
		for (int iS = 0; iS < FImageUp.SubPixel; iS++) {
			dSumUp += FImageUp.Dt[iS][iM];
			dSumDown += FImageDown.Dt[iS][iM];
		}
		ProfileUp.SetVal(0, iM, dSumUp / FImageUp.SubPixel);
		ProfileDown.SetVal(0, iM, dSumDown / FImageDown.SubPixel);
	}
  #ifdef _LP_DEBUG_OUTPUT
	ProfileUp.WriteTma(ExtractFilePath(FileName) + "ProfileUp.tma");
	ProfileDown.WriteTma(ExtractFilePath(FileName) + "ProfileDown.tma");
  #endif

 #endif


 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "Med_FImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "Med_FImageDown.tma");
 #endif



	//２画像の正規化相互相関（NormalizedCrossCorrelation）を計算
	double* dNCC = new double[2 * iMainShiftMax + 1];//Δx = -30～+30までスキャンする
#ifndef _PROFILING
	double dQLaveUp   = FImageUp.dAve();
	double dQLaveDown = FImageDown.dAve();

	for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
		double dNormUp = 0.;
		double dNormDown = 0.;
//		double dQLaveUp   = FImageUp.dAve(0, iMainShiftMax/2, FImageUp.SubPixel - 1, FImageUp.MainPixel - 1 - iMainShiftMax/2);
//		double dQLaveDown = FImageDown.dAve(0, iMainShiftMax/2, FImageDown.SubPixel - 1, FImageDown.MainPixel - 1 - iMainShiftMax/2);
		for(int iS = 0; iS < iSubNumMatch; iS++){
//		for(int iM = 0; iM < FImageUp.MainPixel - iMainShiftMax; iM++){
			for(int iM = CLIP(-iMShift, 0, FImageUp.MainPixel); iM < CLIP(FImageUp.MainPixel - iMShift, 0, FImageUp.MainPixel); iM++){
				double dDeltaUp   = (FImageUp.Dt[iS][iM] - dQLaveUp);
				double dDeltaDown = (FImageDown.Dt[iS][iM + iMShift] - dQLaveDown);
				dNCC[iMShift + iMainShiftMax] += dDeltaUp * dDeltaDown;
				dNormUp     += dDeltaUp * dDeltaUp;
				dNormDown   += dDeltaDown * dDeltaDown;
			}
		}
		double dNormUp2 = sqrt(dNormUp);
		double dNormDown2 = sqrt(dNormDown);
		dNCC[iMShift + iMainShiftMax] /= (dNormUp2 * dNormDown2);
	}
#else
	double dQLaveUp   = ProfileUp.dAve();
	double dQLaveDown = ProfileDown.dAve();

	for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
		double dNormUp = 0.;
		double dNormDown = 0.;
		for(int iM = CLIP(-iMShift, 0, ProfileUp.MainPixel); iM < CLIP(ProfileUp.MainPixel - iMShift, 0, ProfileUp.MainPixel); iM++){
			double dDeltaUp   = (ProfileUp.Dt[0][iM] - dQLaveUp);
			double dDeltaDown = (ProfileDown.Dt[0][iM + iMShift] - dQLaveDown);
			dNCC[iMShift + iMainShiftMax] += dDeltaUp * dDeltaDown;
			dNormUp     += dDeltaUp * dDeltaUp;
			dNormDown   += dDeltaDown * dDeltaDown;
		}
		double dNormUp2 = sqrt(dNormUp);
		double dNormDown2 = sqrt(dNormDown);
		dNCC[iMShift + iMainShiftMax] /= (dNormUp2 * dNormDown2);
	}
#endif
	//相互相関の最大値を探索
	int iMShift;
	double dNCCMax = 0.;
	for (iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
		if (dNCCMax < dNCC[iMShift + iMainShiftMax]) {
			dNCCMax = dNCC[iMShift + iMainShiftMax];
			iMShiftDetect = iMShift;
		}
	}

	//上画像を中画像と絵のパターンが合うように左右シフト
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect);//中画像の上短冊を抽出
 #ifdef _LP_DEBUG_OUTPUT
	ImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpMShift" + IntToStr(iMShiftDetect) + ".tma");
 #endif


 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpRel = fopen((ExtractFilePath(FileName) + "NormalizedCrossCorrelation.txt").c_str(), "wt");
	fprintf(fpRel, "%s\n", FileNameImageUp.c_str());
	fprintf(fpRel, "%s\n", FileName.c_str());
	fprintf(fpRel, "MShiftMax	NCCMax\n");
	fprintf(fpRel, "%d	%lf\n", iMShiftDetect, dNCCMax);
	fprintf(fpRel, "\nMShift	NCC\n");
	for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
		fprintf(fpRel, "%d	%lf\n", iMShift, dNCC[iMShift + iMainShiftMax]);
	}
	fclose(fpRel);
 #endif
	delete[] dNCC;

/*
	//左右を中央で２分割してそれぞれのROIごとに相互相関を計算
	TTMA FFImageUp, FFImageDown;
	FFImageUp = FImageUp;
	FFImageDown = FImageDown;
	FFImageUp.Format(iSubNumMatch, FImageUp.MainPixel / 2, 0, 0);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
	FFImageDown.Format(iSubNumMatch, FImageDown.MainPixel / 2, 0,  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）

	//実装途中

*/


}
//---------------------------------------------------------------------------
//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主方向微分して、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする

#define _LOWDOSE_LINEAR_TABLE
#define _SKINLINE_KUBOMI_TAISAKU

void TSIMARS::LPDetectHorizontalMisalignmentAndShiftImageUpMultiROI(TTMA& ImageUp, string FileNameImageUp, int& iMShiftDetect, string FileName,
										int iMainShiftMax,   //短手斜入による左右方向の画像シフトの最大値
										int iSubNumMatch,    //マッチングする短冊画像の副方向の長さ（とりあえず10画素）
										int iMainMoveAveMatch,//マッチングする短冊画像への主MoveAve(とりあえず5画素)
										int iSubRangeOnlyMidPanel,
										int iYSintiEnd,
										int iWidthROI, 		 //ROIの横幅
										int iWidthOverlap,   //ROI間の重複の幅
										double dKandoTop, double dKandoCenter, //パネルの感度補正係数
										bool bGPRMatch)
{
	if (iWidthROI > MainPixel) {
		CApplication::GetInstance()->MessageBox("ROIの幅の設定が不正です。", "Error", MB_OK);
		return;
	}
	if (iWidthROI < iWidthOverlap) {
		CApplication::GetInstance()->MessageBox("ROIの幅の設定が不正です。", "Error", MB_OK);
		return;
	}

	//上・中画像の絵が重複している短冊領域のみ抽出（パターンマッチングに使用）
	TTMA FImageUp, FImageDown;
	FImageUp = ImageUp;
	FImageDown = *this;

	//左右端から30pixを除外（左右位置ずれの最大量）
//	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel - 2*iMainShiftMax, -(ImageUp.SubPixel - 1 - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）//ｙが1画素ずれてトリミングしていたbug
	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel - 2*iMainShiftMax, -(ImageUp.SubPixel - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
	FImageDown.Format(iSubNumMatch, MainPixel - 2*iMainShiftMax, -(iYSintiEnd - iSubRangeOnlyMidPanel - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "FImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "FImageDown.tma");
 #endif

#if 0 //メ品証ランニング評価で出た案件対応（141117 kuwabara） →素抜けの飽和縦スジパターンが相互相関の誤検出に影響するためクリッピングする
	const int iClipQL = 43000;//飽和スジは45000QLで見えてくるので、43000QLでクリップ処理を入れる。
	FImageUp.Clip(0, iClipQL);
	FImageDown.Clip(0, iClipQL);
 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "CFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "CFImageDown.tma");
 #endif
#else //パネルの感度補正係数を反映し、パネルごとに異なるclippingQLとする (150915 ttsuji)
	const int iClipQL = 52000;
	int iClipUp   = (int)(iClipQL * dKandoTop / 4);//切捨て
	int iClipDown = (int)(iClipQL * dKandoCenter / 4);//切捨て
	FImageUp.Clip(0, iClipUp);
	FImageDown.Clip(0, iClipDown);
 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(iClipUp) + ")FImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(iClipDown) + ")FImageDown.tma");
 #endif
#endif


	//ログ変換してから相互相関を計算する
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	string StrPre = "";
 #ifndef _LOWDOSE_LINEAR_TABLE
	FImageUp.LogThreshold(dMidVal, dKeta, iLogTh);
	FImageUp.BitChange(14);
	FImageDown.LogThreshold(dMidVal, dKeta, iLogTh);
	FImageDown.BitChange(14);
 #else
	FImageUp.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
	FImageDown.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
 #endif

 #ifdef _LP_DEBUG_OUTPUT
//	FImageUp.WriteTma(ExtractFilePath(FileName) + "LFImageUp.tma");
//	FImageDown.WriteTma(ExtractFilePath(FileName) + "LFImageDown.tma");
	FImageUp.WriteTma(ExtractFilePath(FileName) + "LCFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "LCFImageDown.tma");
 #endif


 #ifdef _SKINLINE_KUBOMI_TAISAKU //スキンラインくぼみ対策（LCFImageかDLCFImageにかける）(180517ttsuji)
	//GPRをかけて直接線分のグリッドリス目を除去する
	if (bGPRMatch) {
  #if 0
		FImageUp.GPR();
		FImageDown.GPR();
  #else   //副フィルターのtap数を短くする
  /*
  GPR(const float fMainFilter[],
			const int iMainFilterNum,
			const float fSubFilter[],
			const int iSubFilterNum)
  */
		const float fMainFilter[] =
			{-0.001708984, 0.001464844, 0.002197266,-0.002441406,-0.003173828,
			  0.004150391, 0.003417969,-0.007324219,-0.002929688, 0.011474609,
			  0.000488281,-0.016357422, 0.004394531, 0.021728516,-0.012451172,
			 -0.026855469, 0.025634766, 0.031494141,-0.048583984,-0.03515625,
			  0.096435547, 0.037597656,-0.314941406, 0.462890625,-0.314941406,
			  0.037597656, 0.096435547,-0.03515625 ,-0.048583984, 0.031494141,
			  0.025634766,-0.026855469,-0.012451172, 0.021728516, 0.004394531,
			 -0.016357422, 0.000488281, 0.011474609,-0.002929688,-0.007324219,
			  0.003417969, 0.004150391,-0.003173828,-0.002441406, 0.002197266,
			  0.001464844,-0.001708984};
		const int iMainFilterNum = 47;

		const float fSubFilter[] = {0.0156071,
									0.0631302,
									0.1292630,
									0.1870300,
									0.2099400,
									0.1870300,
									0.1292630,
									0.0631302,
									0.0156071};
		const int iSubFilterNum = 9;

		FImageUp.GPR(fMainFilter, iMainFilterNum, fSubFilter, iSubFilterNum);
		FImageDown.GPR(fMainFilter, iMainFilterNum, fSubFilter, iSubFilterNum);
  #endif

  #ifdef _LP_DEBUG_OUTPUT
		FImageUp.WriteTma(ExtractFilePath(FileName) + "GLCFImageUp.tma");
		FImageDown.WriteTma(ExtractFilePath(FileName) + "GLCFImageDown.tma");
  #endif
 	}
 #endif

	//左右方向へ微分
	FImageUp.DifMainUp();
	FImageDown.DifMainUp();
	FImageUp.Format(iSubNumMatch, FImageUp.MainPixel - 1, 0, -1);
	FImageDown.Format(iSubNumMatch, FImageDown.MainPixel - 1, 0, -1);
 #ifdef _LP_DEBUG_OUTPUT
//	FImageUp.WriteTma(ExtractFilePath(FileName) + "DLFImageUp.tma");
//	FImageDown.WriteTma(ExtractFilePath(FileName) + "DLFImageDown.tma");
	FImageUp.WriteTma(ExtractFilePath(FileName) + "DLCFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "DLCFImageDown.tma");
 #endif

 #if 0
	//主方向Medianでノイズ落とす
	FImageUp.MainMedian(iMainMoveAveMatch);
	FImageDown.MainMedian(iMainMoveAveMatch);
 #else
	//主方向移動平均でノイズ落とす(主方向微分のときはこれが精度良い)
	FImageUp.MainMoveAve(iMainMoveAveMatch);
	FImageDown.MainMoveAve(iMainMoveAveMatch);
 #endif

 #ifdef _LP_DEBUG_OUTPUT
//	FImageUp.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageUp.tma");
//	FImageDown.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageDown.tma");
	FImageUp.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageDown.tma");
 #endif

//	const double dThNCC = 0.4;
	const double dThNCC = 0.3;//ROIにエッジがあり、正しく相関が出ていることを示す閾値(default)
//	const double dThNCC = 0.1;
//	const double dThNCC = 1.;
	double* dNCC = new double[2 * iMainShiftMax + 1];//Δx = -30～+30までスキャンする
	for (int i = 0; i < 2 * iMainShiftMax + 1; i++) {
   		dNCC[i] = -100.;//デフォルトを負の値とする
	}
	double dNCCMaxX0, dNCCMaxXE;
	double dQLaveUp;
	double dQLaveDown;
	double dNormUp = 0.;
	double dNormDown = 0.;
	double dNormUp2, dNormDown2;
	TTMA FImageUpROIX0, FImageDownROIX0, FImageUpROIXE, FImageDownROIXE;
	bool bFindEdgeX0 = false;//X0方向のROIから有効なエッジを見つけたらtrue
	bool bFindEdgeXE = false;//XE方向のROIから有効なエッジを見つけたらtrue
    int iXROI;
	int iROICnt = 0;//ROIを修正する試行回数
	int iMShiftDetectX0(0), iMShiftDetectXE(0);

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpRel = fopen((ExtractFilePath(FileName) + "NormalizedCrossCorrelationX0.txt").c_str(), "wt");
	fprintf(fpRel, "%s\n", FileNameImageUp.c_str());
	fprintf(fpRel, "%s\n", FileName.c_str());
 #endif


	//X=0側のROIについて上・中２画像の正規化相互相関（NormalizedCrossCorrelation）を計算
	do{
		iXROI = iROICnt * (iWidthROI - iWidthOverlap);
		FImageUpROIX0   = FImageUp;
		FImageDownROIX0 = FImageDown;
		FImageUpROIX0.Format(FImageUpROIX0.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
		FImageDownROIX0.Format(FImageDownROIX0.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #ifdef _LP_DEBUG_OUTPUT
//		FImageUpROIX0.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageUp.tma");
//		FImageDownROIX0.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageDown.tma");
		FImageUpROIX0.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageUp.tma");
		FImageDownROIX0.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageDown.tma");
 #endif

		//ROI画像に相互相関
		dQLaveUp   = FImageUpROIX0.dAve();
		dQLaveDown = FImageDownROIX0.dAve();
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			dNormUp = 0.;
			dNormDown = 0.;
//			double dQLaveUp   = FImageUp.dAve(0, iMainShiftMax/2, FImageUp.SubPixel - 1, FImageUp.MainPixel - 1 - iMainShiftMax/2);
//			double dQLaveDown = FImageDown.dAve(0, iMainShiftMax/2, FImageDown.SubPixel - 1, FImageDown.MainPixel - 1 - iMainShiftMax/2);
			for(int iS = 0; iS < iSubNumMatch; iS++){
//			for(int iM = 0; iM < FImageUp.MainPixel - iMainShiftMax; iM++){
				for(int iM = CLIP(-iMShift, 0, FImageUpROIX0.MainPixel); iM < CLIP(FImageUpROIX0.MainPixel - iMShift, 0, FImageUpROIX0.MainPixel); iM++){
					double dDeltaUp   = (FImageUpROIX0.Dt[iS][iM] - dQLaveUp);
					double dDeltaDown = (FImageDownROIX0.Dt[iS][iM + iMShift] - dQLaveDown);
					dNCC[iMShift + iMainShiftMax] += dDeltaUp * dDeltaDown;
					dNormUp     += dDeltaUp * dDeltaUp;
					dNormDown   += dDeltaDown * dDeltaDown;
				}
			}
			dNormUp2 = sqrt(dNormUp);
			dNormDown2 = sqrt(dNormDown);
//			if (0. == (dNormUp2 * dNormDown2)) {//一律同じQLで分散ゼロの場合 →相互相関が定義できず、発散するためcontinueする
			if (0 == round(dNormUp2 * dNormDown2)) {//一律同じQLで分散ゼロの場合 →相互相関が定義できず、発散するためcontinueする
				break;
			}
			dNCC[iMShift + iMainShiftMax] /= (dNormUp2 * dNormDown2);
		}
		//相互相関の最大値を探索
		int iMShift;
		dNCCMaxX0 = 0.;
		for (iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			if (dNCCMaxX0 < dNCC[iMShift + iMainShiftMax]) {
				dNCCMaxX0 = dNCC[iMShift + iMainShiftMax];
				iMShiftDetectX0 = iMShift;
			}
		}

 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpRel, "x = %d - %d\n", iXROI, iXROI + iWidthROI);
		fprintf(fpRel, "dQLaveUp	dQLaveDown\n");
		fprintf(fpRel, "%lf	%lf\n", dQLaveUp, dQLaveDown);
		fprintf(fpRel, "MShiftMax	NCCMax\n");
		fprintf(fpRel, "%d	%lf\n", iMShiftDetectX0, dNCCMaxX0);
		fprintf(fpRel, "MShift	NCC\n");
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			fprintf(fpRel, "%d	%lf\n", iMShift, dNCC[iMShift + iMainShiftMax]);
		}
		fprintf(fpRel, "\n\n");
 #endif

		//相互相関値の値によりエッジを適切に検出できているか判断
		if (dNCCMaxX0 >= dThNCC) {
			bFindEdgeX0 = true;
			break;
		}
		iROICnt++;

	} while (!bFindEdgeX0 && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2);
//	} while (!bFindEdgeX0 && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2 && 0. != (dNormUp2 * dNormDown2));
//	} while (!bFindEdgeX0 && iROICnt * iXROI < ImageUp.MainPixel / 2);

 #ifdef _LP_DEBUG_OUTPUT
	fclose(fpRel);
 #endif



	//Xendの側のROIで相互相関を計算
	iROICnt = 0;//ROIを修正する試行回数

	for (int i = 0; i < 2 * iMainShiftMax + 1; i++) {
		dNCC[i] = -100.;//デフォルトを負の値とする
	}

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpRel2 = fopen((ExtractFilePath(FileName) + "NormalizedCrossCorrelationXE.txt").c_str(), "wt");
	fprintf(fpRel2, "%s\n", FileNameImageUp.c_str());
	fprintf(fpRel2, "%s\n", FileName.c_str());
 #endif


	//X=End側のROIについて上・中２画像の正規化相互相関（NormalizedCrossCorrelation）を計算
	do{
//		iXROI = FImageUp.MainPixel - 1 - iWidthROI - iROICnt * (iWidthROI - iWidthOverlap);
		iXROI = FImageUp.MainPixel - iWidthROI - iROICnt * (iWidthROI - iWidthOverlap); //トリミングが1画素ずれていたbug修正(141216 ttsuji)
		FImageUpROIXE   = FImageUp;
		FImageDownROIXE = FImageDown;
		FImageUpROIXE.Format(FImageUpROIXE.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
		FImageDownROIXE.Format(FImageDownROIXE.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #ifdef _LP_DEBUG_OUTPUT
//		FImageUpROIXE.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageUp.tma");
//		FImageDownROIXE.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageDown.tma");
		FImageUpROIXE.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageUp.tma");
		FImageDownROIXE.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageDown.tma");
 #endif

		//ROI画像に相互相関
		dQLaveUp   = FImageUpROIXE.dAve();
		dQLaveDown = FImageDownROIXE.dAve();
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			dNormUp = 0.;
			dNormDown = 0.;
//			double dQLaveUp   = FImageUp.dAve(0, iMainShiftMax/2, FImageUp.SubPixel - 1, FImageUp.MainPixel - 1 - iMainShiftMax/2);
//			double dQLaveDown = FImageDown.dAve(0, iMainShiftMax/2, FImageDown.SubPixel - 1, FImageDown.MainPixel - 1 - iMainShiftMax/2);
			for(int iS = 0; iS < iSubNumMatch; iS++){
//			for(int iM = 0; iM < FImageUp.MainPixel - iMainShiftMax; iM++){
				for(int iM = CLIP(-iMShift, 0, FImageUpROIXE.MainPixel); iM < CLIP(FImageUpROIXE.MainPixel - iMShift, 0, FImageUpROIXE.MainPixel); iM++){
					double dDeltaUp   = (FImageUpROIXE.Dt[iS][iM] - dQLaveUp);
					double dDeltaDown = (FImageDownROIXE.Dt[iS][iM + iMShift] - dQLaveDown);
					dNCC[iMShift + iMainShiftMax] += dDeltaUp * dDeltaDown;
					dNormUp     += dDeltaUp * dDeltaUp;
					dNormDown   += dDeltaDown * dDeltaDown;
				}
			}
			dNormUp2 = sqrt(dNormUp);
			dNormDown2 = sqrt(dNormDown);
			if (0. == (dNormUp2 * dNormDown2)) {//一律同じQLで分散ゼロの場合 →相互相関が定義できず、発散するためcontinueする
				break;
			}
			dNCC[iMShift + iMainShiftMax] /= (dNormUp2 * dNormDown2);
		}
		//相互相関の最大値を探索
		int iMShift;
		dNCCMaxXE = 0.;
		for (iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			if (dNCCMaxXE < dNCC[iMShift + iMainShiftMax]) {
				dNCCMaxXE = dNCC[iMShift + iMainShiftMax];
				iMShiftDetectXE = iMShift;
			}
		}

 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpRel2, "x = %d - %d\n", iXROI, iXROI + iWidthROI);
		fprintf(fpRel2, "dQLaveUp	dQLaveDown\n");
		fprintf(fpRel2, "%lf	%lf\n", dQLaveUp, dQLaveDown);
		fprintf(fpRel2, "MShiftMax	NCCMax\n");
		fprintf(fpRel2, "%d	%lf\n", iMShiftDetectXE, dNCCMaxXE);
		fprintf(fpRel2, "MShift	NCC\n");
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			fprintf(fpRel2, "%d	%lf\n", iMShift, dNCC[iMShift + iMainShiftMax]);
		}
		fprintf(fpRel2, "\n\n");
 #endif

		//相互相関値の値によりエッジを適切に検出できているか判断
		if (dNCCMaxXE >= dThNCC) {
			bFindEdgeXE = true;
			break;
		}
		iROICnt++;

	} while (!bFindEdgeXE && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2);
//	} while (!bFindEdgeXE && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2  && 0. != (dNormUp2 * dNormDown2));

	delete[] dNCC;
 #ifdef _LP_DEBUG_OUTPUT
	fclose(fpRel2);
 #endif



	if (bFindEdgeX0 & bFindEdgeXE) {//画像の左右端それぞれから正しくエッジ検出できた場合
		iMShiftDetect = round((iMShiftDetectX0 + iMShiftDetectXE) / 2.);//X=0側のエッジの左右シフトとX＝END側のエッジの左右シフトとを平均処理する
	}
#if 1 //桑原さん指摘（照射野を狭く絞って左右のいずれかによせて短手斜入されたケースでずれる）により修正（141117 ttsuji）
	else if (bFindEdgeX0) {
		iMShiftDetect = iMShiftDetectX0;
	}
	else if (bFindEdgeXE) {
		iMShiftDetect = iMShiftDetectXE;
	}
#endif
	else{
		iMShiftDetect = 0;//適切に求まっていないため、0を出力する
	}

#if 0 //手動で左右位置ずれ量を指定(tmp)
	iMShiftDetect = 0;
#endif

	//上画像を中画像と絵のパターンが合うように左右シフト
#if 0 //白で埋める
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect);//中画像の上短冊を抽出 bug(左右シフトしてできた空間を真っ白で埋めていた)
#elif 1 //黒で埋める
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect, ImageUp.DtMax);//中画像の上短冊を抽出 (左右シフトしてできた空間を真っ黒で埋めるよう修正)
#else //Dupe拡大して補間する (141201 ttsuji)
	const int iMMedDupe = 5;//tmp
	TTMA MedImageUp;
	if (iMShiftDetect) {
		MedImageUp = ImageUp;
		MedImageUp.MainMedian(iMMedDupe);
 #ifdef _LP_DEBUG_OUTPUT
		MedImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpMMed(" + IntToStr(iMMedDupe) + ").tma");
 #endif
	}
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect, ImageUp.DtMax);//中画像の上短冊を抽出 (左右シフトしてできた空間を真っ黒で埋めるよう修正)
	//Top画像の下端をDupe拡大する(Centerパネルの重複領域の段差補正用にのみ行い、合成時はDupe拡大していない黒塗りしたものを使うべき？)
	for (int iS = 0; iS < ImageUp.SubPixel; iS++) {
		if (iMShiftDetect > 0) {//空白領域はxのStart側
			for (int iM = 0; iM < iMShiftDetect; iM++) {
				ImageUp.SetVal(iS, iM, MedImageUp.Dt[iS][iMMedDupe/2]);
			}
		}
		else if (iMShiftDetect < 0) {//空白領域はxのEnd側
			for (int iM = MedImageUp.MainPixel - 1; iM > MedImageUp.MainPixel - 1 + iMShiftDetect; iM--) {
				ImageUp.SetVal(iS, iM, MedImageUp.Dt[iS][MedImageUp.MainPixel - 1 - iMMedDupe/2]);
			}
		}
	}

#endif

 #ifdef _LP_DEBUG_OUTPUT
	ImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpMShift" + IntToStr(iMShiftDetect) + ".tma");
	WriteTma(ExtractFilePath(FileName) + "ImageDown.tma");
 #endif

}
//---------------------------------------------------------------------------

//#define _CORRDEF_U-D //定義しない(140924 )



//上画像のうち、中画像と同じ絵が存在する領域に対して中画像との差分をとって、主Median、副Medianして上画像との差分画像の低周波情報を取得

void TSIMARS::	LPCorrectOverlapAreaReferToImageUp(TTMA& ImageUp, TTMA& CorrImg, TTMA& QLRateImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, double dNormQL, int iSubRangeOnlyMidPanel, int iMainMedianDiffImg, int iSubMedianDiffImg, int iMShiftDetect)
{
#if 1
	//Dupe拡大して補間する (141201 ttsuji)
	const int iMMedDupe = 5;//tmp
	TTMA MedImageUp;
	if (iMShiftDetect) {
		MedImageUp = ImageUp;
 #ifdef _LP_DEBUG_OUTPUT
		MedImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpBeforeMMed(" + IntToStr(iMMedDupe) + ").tma");
 #endif
		MedImageUp.MainMedian(iMMedDupe);
 #ifdef _LP_DEBUG_OUTPUT
		MedImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpMMed(" + IntToStr(iMMedDupe) + ").tma");
 #endif
	}
//	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect, ImageUp.DtMax);//中画像の上短冊を抽出 (左右シフトしてできた空間を真っ黒で埋めるよう修正)
	//Top画像の下端をDupe拡大する(Centerパネルの重複領域の段差補正用にのみ行い、合成時はDupe拡大していない黒塗りしたものを使うべき？)
	for (int iS = 0; iS < ImageUp.SubPixel; iS++) {
		if (iMShiftDetect > 0) {//空白領域はxのStart側
			for (int iM = 0; iM < iMShiftDetect; iM++) {
				ImageUp.SetVal(iS, iM, MedImageUp.Dt[iS][iMMedDupe/2 + iMShiftDetect]);
			}
		}
		else if (iMShiftDetect < 0) {//空白領域はxのEnd側
			for (int iM = MedImageUp.MainPixel - 1; iM > MedImageUp.MainPixel - 1 + iMShiftDetect; iM--) {
				ImageUp.SetVal(iS, iM, MedImageUp.Dt[iS][MedImageUp.MainPixel - 1 - iMMedDupe/2 + iMShiftDetect]);
			}
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	ImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpDupe(MMed" + IntToStr(iMMedDupe) + ",MShift" + IntToStr(iMShiftDetect) + ").tma");
 #endif
#endif


	for (int iM = 0; iM < ImageUp.MainPixel; iM++){
		int iS, uiS;

		//オーバーラップ領域なので、上画像の情報を用いて補正量を算出
		for (iS = edgeY[iM] - iSubRangeOnlyMidPanel, uiS = 1; iS >= 0; iS--, uiS++){
			//通常
// #ifdef _CORRDEF_U-D
#if 1
			CorrImg.SetVal(iS, iM, ImageUp.Dt[ImageUp.SubPixel-uiS][iM] - Dt[iS][iM] + iKasa);
 #else
			CorrImg.SetVal(iS, iM, -ImageUp.Dt[ImageUp.SubPixel-uiS][iM] + Dt[iS][iM] + iKasa);
 #endif
			//tmp 上下の画像のQL比を計算（左右位置ずれ検出のため）
			if (Dt[iS][iM]) {
				QLRateImg.SetVal(iS, iM, dNormQL * ImageUp.Dt[ImageUp.SubPixel-uiS][iM] / Dt[iS][iM]);
			}
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg.tma");
	QLRateImg.WriteTma(ExtractFilePath(FileName) + "QLRateImg.tma");//tmp
 #endif

	//オーバーラップ領域の補正画像に主フィルターをかけることで、主の高周波成分（縦スジ）をなくし、。     上下画像の位置ずれに対するロバストを確保する
	CorrImg.MainMedian(iMainMedianDiffImg);
	//副フィルター処理をかけることで、副の高周波成分（横スジ）をなくし、上下画像の位置ずれに対するロバストを確保する。
	TTMA FormatCorrImg = CorrImg;
 #if 0
	FormatCorrImg.Format(iYSintiEnd - iSubRangeOnlyMidPanel, CorrImg.MainPixel, 0, 0);
 #else
//	FormatCorrImg.Format(iYSintiEnd - iSubRangeOnlyMidPanel + , CorrImg.MainPixel, 0, 0);//副Median11の可変長MedianにおけるMedianを適切にかけるため、補正する矩形領域の下端を延長する必要あり(140822 ttsuji)
 #endif
	FormatCorrImg.SubMedian(iSubMedianDiffImg);
//	FormatCorrImg.SubMedian(11);//端処理をマスクサイズ減少にすると、黒スジとれるが、その領域に横スジが発生したときに副フィルター処理がかからないリスクあり。（マスクサイズ低減により、残った横スジは、短冊横傘で対応する。）
//	FormatCorrImg.SubMedian(21);//マスクサイズを１１から２１にすると、その副作用として、低周波横スジムラが発生しNG。11くらいが良い。
//	FormatCorrImg.SubMedianAveExpand(11);//端処理を平均拡張方式にすると、黒スジがとれない。（端処理した画素値はほぼ平均値となってしまうため、精度が悪い。）
	CorrImg.CopyFormat(0, 0, FormatCorrImg, FormatCorrImg.SubPixel, FormatCorrImg.MainPixel, 0, 0);

 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg.tma");
 #endif


	for( int iM = 0; iM < MainPixel; iM++){
//		for( int iS = 0; iS < SubPixel; iS++){
		for( int iS = 0; iS < CorrImg.SubPixel; iS++){
// #ifdef _CORRDEF_U-D
#if 1
			SetVal(iS, iM, Dt[iS][iM] + CorrImg.Dt[iS][iM] - iKasa);
 #else
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
 #endif
		}
	}


 #ifdef _LP_DEBUG_OUTPUT
	WriteTma(ExtractFilePath(FileName) + "ImageDown-CorrImg.tma");
 #endif

}
//---------------------------------------------------------------------------
//上画像低周波情報で補正した中画像のうち、上画像情報が存在しない補正領域は中画像のDC成分を線形補間にて計算する。その際、DC成分線形補間の端点のQL比がある閾値以上であれば、濃度の濃い画素の濃度を下げた上で線形補間する。
//DC成分線形補間した画像を中画像から減算して補正画像を作成し、これに主Medianをかけて、補正量の主方向スムージングを行う。これを最終的な補正画像として、中画像から減算する。

void TSIMARS::LPCorrectNonOverlapAreaByItplDC(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD, float& fMagicPara)
{
	TTMA ItplImg;
//	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 2, MainPixel, Bit);//マージン込みで確保
	ItplImg.FillValue(0);

//	const float fThQLRateUD = 2.;//とりあえず
//	const float fThQLRateUD = 1.5;//とりあえず

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl = fopen((ExtractFilePath(FileName) + "QLrateDCItplMainProfile.txt").c_str(), "wt");
	fprintf(fpDCItpl, "iM	QLUp	QLDown	QLRate\n");
 #endif

	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
//		const int iSAveNum = 1;//副方向の平均画素数
		const int iSAveNum = 3;//副方向の平均画素数
//		const int iSAveNum = 7;//副方向の平均画素数

  #if 1 //DC成分を線形補間で計算する
		float fAveU = 0.;
		float fAveD = 0.;
		for (iS = 0; iS < iSAveNum; iS++){
			fAveU += Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			fAveD += Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
		}
		fAveU /= iSAveNum;
		fAveD /= iSAveNum;

//		float fQLRateUD = fAveU/fAveD;//DC成分補間する2点のQL比
//		float fQLRateDU = fAveD/fAveU;//DC成分補間する2点のQL比
		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		bool bflag = false;//斜めエッジ対応特別処理フラグ

		//iS= edgeY[iM] - iSubRangeOnlyMidPanel + 1 ～  edgeY[iM] + iItplSubOffset までを補間
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for (iS = iSstart; iS <= iSend; iS++){
 #if 0
			float fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
 #else		//QL比が閾値以上の場合は、例えば素抜けと骨の補間であり、コントラストが強く、素抜けを使わないほうが良い。
			float fQLitpl, fAveUtmp, fAveDtmp;
//			if (fQLRateUD >= fThQLRateUD) {
			if (fQLRateUD >= fThQLRateUD || fQLRateDU >= fThQLRateUD) {
				bflag = true;
 #if 0			//bug(上下と白黒の関係が一方だけだった)
				fAveUtmp = fAveD + (fAveU - fAveD) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
				fQLitpl = ((iSend - iS) * fAveUtmp + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
 #else
				if (fQLRateUD > 1) {//UがDよりも黒いとき
					fAveUtmp = fAveD + (fAveU - fAveD) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
					fQLitpl = ((iSend - iS) * fAveUtmp + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
				}
				else{//DがUよりも黒いとき
					fAveDtmp = fAveU + (fAveD - fAveU) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
					fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveDtmp) / (iSend - iSstart);//線形補間
				}
 #endif
			}
			else{
				bflag = false;
				fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
			}
 #endif
			ItplImg.SetVal(iS, iM, fQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - fQLitpl + iKasa);
		}
   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf	%d\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU, bflag);
   #endif

  #else //DC成分を二次式の最小二乗法を用いた補間で計算する
		const int iNumOfSample = 4;//サンプル点の数
		const int iDeltaSOfSample = 10;//サンプル点のY方向間隔
		double* dS = new double[iNumOfSample];
		double* dQLave = new double[iNumOfSample];
		for (int iCnt = 0; iCnt < iNumOfSample; iCnt++) {
			dQLave[iCnt] = 0.;
		}
		dS[0] =  (double)(edgeY[iM] - iSubRangeOnlyMidPanel - 1 - iDeltaSOfSample);
		dS[1] =  (double)(edgeY[iM] - iSubRangeOnlyMidPanel - 1);
		dS[2] =  (double)(edgeY[iM] + 2 + iItplSubOffset);
		dS[3] =  (double)(edgeY[iM] + 2 + iItplSubOffset + iDeltaSOfSample);
		for (iS = 0; iS < iSAveNum; iS++){
			dQLave[0] += ImageDown.Dt[round(dS[0]) + 1 - iS][iM];
			dQLave[1] += ImageDown.Dt[round(dS[1]) + 1 - iS][iM];
			dQLave[2] += ImageDown.Dt[round(dS[2]) - 1 + iS][iM];
			dQLave[3] += ImageDown.Dt[round(dS[3]) - 1 + iS][iM];
/*
			dQLave[0] += ImageDown.Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS - iDeltaSOfSample][iM];
			dQLave[1] += ImageDown.Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			dQLave[2] += ImageDown.Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
			dQLave[3] += ImageDown.Dt[edgeY[iM] + 1 + iItplSubOffset + iS + iDeltaSOfSample][iM];
*/
		}
		for (int iCnt = 0; iCnt < iNumOfSample; iCnt++) {
			dQLave[iCnt] /= iSAveNum;
		}

		CLeastSquare LSItpl(iNumOfSample, dS, dQLave, _TWO_DIM);
		if (!LSItpl.Calc()){
			CApplication::GetInstance()->MessageBox("補間プロファイルの最小二乗法計算でエラーが発生しました。", "Error", MB_OK);
			delete[] dS;
			delete[] dQLave;
			return;
		}
		Vct vItpl = LSItpl.GetSolve();
		double dAItpl = vItpl[2];
		double dBItpl = vItpl[1];
		double dCItpl = vItpl[0];

		//iS= edgeY[iM] - iSubRangeOnlyMidPanel + 1 ～  edgeY[iM] + iItplSubOffset までを補間
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for (iS = iSstart; iS <= iSend; iS++){
			double dQLitpl = dAItpl * iS * iS + dBItpl * iS + dCItpl;
			ItplImg.SetVal(iS, iM, dQLitpl);
			CorrImg.SetVal(iS, iM, ImageDown.Dt[iS][iM] - dQLitpl + iKasa);
		}
		delete[] dS;
		delete[] dQLave;
  #endif
	}//end for iM


  #ifdef _LP_DEBUG_OUTPUT
	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl);
  #endif

	/*   この行はデバッグ終えたら実施に変更する     */
//	CorrImg.MainMedian(201);//201にすると、外挿部の縦スジは消えるが、マーカーのアーチファクトが発生する
	CorrImg.MainMedian(iMainMedianDCItpl);

//	CorrImg.SubMedian(3);//tmp(これを入れると、消したい高周波の横スジ（段差の残差等）を消せない)



  #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg2.tma");
  #endif

	for (int iM = 0; iM < MainPixel; iM++){
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for( int iS = iSstart; iS <= iSend; iS++){
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
}
//---------------------------------------------------------------------------
//上画像低周波情報で補正した中画像のうち、上画像情報が存在しない補正領域は中画像のDC成分を線形補間にて計算する。その際、DC成分線形補間の端点のQL比がある閾値以上であれば、濃度の濃い画素の濃度を下げた上で線形補間する。
//DC成分線形補間した画像を中画像から減算して補正画像を作成し、これに主Medianをかけて、補正量の主方向スムージングを行う。これを最終的な補正画像として、中画像から減算する。

void TSIMARS::LPCorrectNonOverlapAreaByItplDC2(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD, float& fMagicPara)
{
	TTMA ItplImg;
	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
	ItplImg.FillValue(0);


 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl = fopen((ExtractFilePath(FileName) + "QLrateDCItplMainProfile.txt").c_str(), "wt");
	fprintf(fpDCItpl, "iM	QLUp	QLDown	QLRate\n");
 #endif

	const int iItplSubOffsetShort = 5;//斜めエッジがある箇所の値

	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
//		const int iSAveNum = 1;//副方向の平均画素数
		const int iSAveNum = 3;//副方向の平均画素数
//		const int iSAveNum = 7;//副方向の平均画素数

  #if 1 //DC成分を線形補間で計算する
		float fAveU = 0.;
		float fAveD = 0.;
		for (iS = 0; iS < iSAveNum; iS++){
			fAveU += Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			fAveD += Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
		}
		fAveU /= iSAveNum;
		fAveD /= iSAveNum;

//		float fQLRateUD = fAveU/fAveD;//DC成分補間する2点のQL比
//		float fQLRateDU = fAveD/fAveU;//DC成分補間する2点のQL比
		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		bool bflag = false;//斜めエッジ対応特別処理フラグ

		//iS= edgeY[iM] - iSubRangeOnlyMidPanel + 1 ～  edgeY[iM] + iItplSubOffset までを補間
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for (iS = iSstart; iS <= iSend; iS++){
 #if 0
			float fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
 #else		//QL比が閾値以上の場合は、例えば素抜けと骨の補間であり、コントラストが強く、素抜けを使わないほうが良い。
			float fQLitpl, fAveUtmp, fAveDtmp;
//			if (fQLRateUD >= fThQLRateUD) {
			if (fQLRateUD >= fThQLRateUD || fQLRateDU >= fThQLRateUD) {
				bflag = true;
  #if 0
   #if 0		//bug(上下と白黒の関係が一方だけだった)
				fAveUtmp = fAveD + (fAveU - fAveD) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
				fQLitpl = ((iSend - iS) * fAveUtmp + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
   #else
				if (fQLRateUD > 1) {//UがDよりも黒いとき
					fAveUtmp = fAveD + (fAveU - fAveD) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
					fQLitpl = ((iSend - iS) * fAveUtmp + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
				}
				else{//DがUよりも黒いとき
					fAveDtmp = fAveU + (fAveD - fAveU) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
					fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveDtmp) / (iSend - iSstart);//線形補間
				}
   #endif
  #else
				//斜めエッジのときはDC補間の副方向長さを短くしてみる
				float fAveD2 = 0.;
				int iSend2 = edgeY[iM] + iItplSubOffsetShort;
				for (int iS2 = 0; iS2 < iSAveNum; iS2++){
					fAveD2 += Dt[edgeY[iM] + 1 + iItplSubOffsetShort + iS2][iM];
				}
				fAveD2 /= iSAveNum;
   #if 0		//bug(上下と白黒の関係が一方だけだった)
				fAveUtmp = fAveD2 + (fAveU - fAveD2) * fMagicPara;
				fQLitpl = ((iSend2 - iS) * fAveUtmp + (iS -iSstart) * fAveD2) / (iSend2 - iSstart);//線形補間
   #else
				if (fQLRateUD > 1) {//UがDよりも黒いとき
					fAveUtmp = fAveD2 + (fAveU - fAveD2) * fMagicPara;
					fQLitpl = ((iSend2 - iS) * fAveUtmp + (iS -iSstart) * fAveD2) / (iSend2 - iSstart);//線形補間
				}
				else{//DがUよりも黒いとき
					fAveDtmp = fAveU + (fAveD2 - fAveU) * fMagicPara;//黒い画素の寄与を下げる補間を行う。
					fQLitpl = ((iSend2 - iS) * fAveU + (iS -iSstart) * fAveDtmp) / (iSend2 - iSstart);//線形補間
				}
   #endif
				if (iS > iSend2) {
					continue;//何もせず次へ
				}
  #endif
			}
			else{
				bflag = false;
				fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間
			}
 #endif
			ItplImg.SetVal(iS, iM, fQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - fQLitpl + iKasa);
		}
   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf	%d\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU, bflag);
   #endif

  #else //DC成分を二次式の最小二乗法を用いた補間で計算する
		const int iNumOfSample = 4;//サンプル点の数
		const int iDeltaSOfSample = 10;//サンプル点のY方向間隔
		double* dS = new double[iNumOfSample];
		double* dQLave = new double[iNumOfSample];
		for (int iCnt = 0; iCnt < iNumOfSample; iCnt++) {
			dQLave[iCnt] = 0.;
		}
		dS[0] =  (double)(edgeY[iM] - iSubRangeOnlyMidPanel - 1 - iDeltaSOfSample);
		dS[1] =  (double)(edgeY[iM] - iSubRangeOnlyMidPanel - 1);
		dS[2] =  (double)(edgeY[iM] + 2 + iItplSubOffset);
		dS[3] =  (double)(edgeY[iM] + 2 + iItplSubOffset + iDeltaSOfSample);
		for (iS = 0; iS < iSAveNum; iS++){
			dQLave[0] += ImageDown.Dt[round(dS[0]) + 1 - iS][iM];
			dQLave[1] += ImageDown.Dt[round(dS[1]) + 1 - iS][iM];
			dQLave[2] += ImageDown.Dt[round(dS[2]) - 1 + iS][iM];
			dQLave[3] += ImageDown.Dt[round(dS[3]) - 1 + iS][iM];
/*
			dQLave[0] += ImageDown.Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS - iDeltaSOfSample][iM];
			dQLave[1] += ImageDown.Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			dQLave[2] += ImageDown.Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
			dQLave[3] += ImageDown.Dt[edgeY[iM] + 1 + iItplSubOffset + iS + iDeltaSOfSample][iM];
*/
		}
		for (int iCnt = 0; iCnt < iNumOfSample; iCnt++) {
			dQLave[iCnt] /= iSAveNum;
		}

		CLeastSquare LSItpl(iNumOfSample, dS, dQLave, _TWO_DIM);
		if (!LSItpl.Calc()){
			CApplication::GetInstance()->MessageBox("補間プロファイルの最小二乗法計算でエラーが発生しました。", "Error", MB_OK);
			delete[] dS;
			delete[] dQLave;
			return;
		}
		Vct vItpl = LSItpl.GetSolve();
		double dAItpl = vItpl[2];
		double dBItpl = vItpl[1];
		double dCItpl = vItpl[0];

		//iS= edgeY[iM] - iSubRangeOnlyMidPanel + 1 ～  edgeY[iM] + iItplSubOffset までを補間
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for (iS = iSstart; iS <= iSend; iS++){
			double dQLitpl = dAItpl * iS * iS + dBItpl * iS + dCItpl;
			ItplImg.SetVal(iS, iM, dQLitpl);
			CorrImg.SetVal(iS, iM, ImageDown.Dt[iS][iM] - dQLitpl + iKasa);
		}
		delete[] dS;
		delete[] dQLave;
  #endif
	}//end for iM


  #ifdef _LP_DEBUG_OUTPUT
	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl);
  #endif

	/*   この行はデバッグ終えたら実施に変更する     */
//	CorrImg.MainMedian(201);//201にすると、外挿部の縦スジは消えるが、マーカーのアーチファクトが発生する
	CorrImg.MainMedian(iMainMedianDCItpl);

//	CorrImg.SubMedian(3);//tmp(これを入れると、消したい高周波の横スジ（段差の残差等）を消せない)



  #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg2.tma");
  #endif

	for (int iM = 0; iM < MainPixel; iM++){
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for( int iS = iSstart; iS <= iSend; iS++){
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
}
//---------------------------------------------------------------------------
//斜めエッジをQL比閾値判定で検出し、かつ副微分で斜めエッジのｙ座標を特定し、斜めエッジの上側と下側とでそれぞれ三角補間する

//#define _INT_S_AT_M	//あるｘにおける代表的なｙ座標をint型で持つ

void TSIMARS::LPCorrectNonOverlapAreaByItplDC3(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD)
{
	TTMA ItplImg;
//	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
//	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 2, MainPixel, Bit);//マージン込み確保
	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 3, MainPixel, Bit);//マージン込み確保
	ItplImg.FillValue(0);

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl = fopen((ExtractFilePath(FileName) + "QLrateDCItplMainProfile.txt").c_str(), "wt");
	fprintf(fpDCItpl, "iM	QLUp	QLDown	QLRate	QLRate2\n");
//	fprintf(fpDCItpl, "iM	QLUp	QLDown	QLRate	QLRate2	flag	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
 #endif
	TTMA CorrImgDebug;
	CorrImgDebug.NewDt(CorrImg.SubPixel, CorrImg.MainPixel, CorrImg.Bit);
	CorrImgDebug.FillValue(0);


	//ある点の微分情報
//	DIFFERENTIALARROUNDPIXELS DifPixels;


	//debugのための画像
	TTMA ImageUp2Down = *this;
	TTMA ImageLeft2Right = *this;
	TTMA ImageUpLeft2DownRight = *this;
	TTMA ImageUpRight2DownLeft = *this;

	//主Medianでノイズを落とす
	const int iMMed = 11;//DC補間長さ20pixの情報は保持するマスクサイズ
	TTMA MedImg = *this;
	MedImg.MainMedian(iMMed);

	//Log変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	TTMA MedLogImg = MedImg;
	MedLogImg.LogThreshold(dMidVal, dKeta, iLogTh);
 #ifdef _LP_DEBUG_OUTPUT
	MedLogImg.WriteTma( AddPrefix(FileName, "L") );
 #endif

	//DC補間サンプル点のQL比の主プロファイル
	TFIMG fImgRateUD;
	fImgRateUD.NewDt(MainPixel, 1);
	fImgRateUD.Fill(0.);

	//QL比閾値（低周波情報除去画像）
//	const float fThQLRatioHighPass = 1.05;//とりあえず
	const float fThQLRatioHighPass = 1.1;//とりあえず


	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
//		const int iSAveNum = 1;//副方向の平均画素数
		const int iSAveNum = 3;//副方向の平均画素数
//		const int iSAveNum = 7;//副方向の平均画素数

		//DC成分を線形補間で計算する
		float fAveU = 0.;
		float fAveD = 0.;
		for (iS = 0; iS < iSAveNum; iS++){
   #if 0
			fAveU += Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			fAveD += Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
   #else
			fAveU += MedImg.Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			fAveD += MedImg.Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
   #endif
		}
		fAveU /= iSAveNum;
		fAveD /= iSAveNum;

		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend   = edgeY[iM] + iItplSubOffset;
//		int iSend   = edgeY[iM] + iItplSubOffset - 1;

		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		fImgRateUD.Dt[iM][0] = fQLRateUD;
#if 0
		bool bflag = false;//斜めエッジ対応特別処理フラグ
		if (fQLRateUD >= fThQLRateUD || fQLRateDU >= fThQLRateUD) {
			bflag = true;
//			DifPixels = CalcDifferentialArroundPixel(*this, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
//			DifPixels = CalcDifferentialArroundPixel(LogImage, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
			DifPixels = CalcDifferentialArroundPixel(MedLogImg, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf	%d	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU, bflag, DifPixels.dEnergy, DifPixels.dOrientation, DifPixels.dVmNorm, DifPixels.dVmX, DifPixels.dVmY, DifPixels.Left2Right, DifPixels.Up2Down, DifPixels.UpLeft2DownRight, DifPixels.UpRight2DownLeft);
   #endif
		}
		else{
			bflag = false;

			//iS= edgeY[iM] - iSubRangeOnlyMidPanel + 1 ～  edgeY[iM] + iItplSubOffset までを補間
//			for (iS = iSstart; iS <= iSend; iS++){
			for (iS = iSstart; iS < iSend; iS++){
				//QL比が閾値以上の場合は、例えば素抜けと骨の補間であり、コントラストが強く、素抜けを使わないほうが良い。
				float fQLitpl, fAveUtmp, fAveDtmp;
				fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間

				ItplImg.SetVal(iS, iM, fQLitpl);
//				CorrImg.SetVal(iS, iM, Dt[iS][iM] - fQLitpl + iKasa);
			}
   #ifdef _LP_DEBUG_OUTPUT
	#if 0
			fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf	%d\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU, bflag);
	#else
//			DifPixels = CalcDifferentialArroundPixel(*this, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
//			DifPixels = CalcDifferentialArroundPixel(LogImage, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
			DifPixels = CalcDifferentialArroundPixel(MedLogImg, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
			fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf	%d	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU, bflag, DifPixels.dEnergy, DifPixels.dOrientation, DifPixels.dVmNorm, DifPixels.dVmX, DifPixels.dVmY, DifPixels.Left2Right, DifPixels.Up2Down, DifPixels.UpLeft2DownRight, DifPixels.UpRight2DownLeft);
	#endif
   #endif
		}
#else
		//QL比が閾値以上かに関わらず、まずDC成分の線形補間を行う
//		for (iS = iSstart; iS < iSend; iS++){
		for (iS = iSstart; iS <= iSend; iS++){//石丸さん指摘によりiSendも含むよう修正(140901 ttsuji)
			float fQLitpl, fAveUtmp, fAveDtmp;
			fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間

			if (iS >= CorrImg.SubPixel || iS >= ItplImg.SubPixel || iM >= CorrImg.MainPixel || iM >= ItplImg.MainPixel) {
				continue;
			}
			ItplImg.SetVal(iS, iM, fQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - fQLitpl + iKasa);
//  #ifdef _LP_DEBUG_OUTPUT
			CorrImgDebug.SetVal(iS, iM, CorrImg.Dt[iS][iM]);//debug
//  #endif
		}
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
  #endif
#endif

	}//end for iM

  #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl);
	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	CorrImgDebug.WriteTma(ExtractFilePath(FileName) + "CorrImg2debug.tma");

	FILE* fpDCItpl2 = fopen((ExtractFilePath(FileName) + "QLrateHighPassDCItplMainProfile.txt").c_str(), "wt");
//	fprintf(fpDCItpl2, "iM	QLUp	QLDown	QLRate	QLRate2	flag	iS	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
	fprintf(fpDCItpl2, "iM	QLRate	flag	iS	Energy	Orientation	L2R	U2D	UL2DR	UR2DL\n");
  #endif

	//QL比の主プロファイルを主Medianし、低周波のみを抽出
	const int iMainMedQLRate = 201;
	TFIMG fImgRateUDMMed = fImgRateUD;
	fImgRateUDMMed.MainMedian(iMainMedQLRate);
	fImgRateUD -= fImgRateUDMMed;//低周波情報を除去し、高周波のみを抽出
	fImgRateUD += 1.;//１を加算し、UとDが等しいときを１倍とする
  #ifdef _LP_DEBUG_OUTPUT
	fImgRateUD.bSaveAsText((ExtractFilePath(FileName) + "QLRateHighPass.txt").c_str(), true);
  #endif

	//QL比の主プロファイルのHighPass後のQL比から斜めエッジを検出（この方法だと、DC補間の20pix間の濃度変化の影響を受けず、精度の良いエッジ検出が可能）
//	float fThQLRateDU = 1 / fThQLRateUD;
	float fThQLRatioHighPass2 = 1 / fThQLRatioHighPass;
//	double dOrientCut = 5.;//tmp
//	double dOrientCut = 10.;//tmp
	double dOrientCut = 20.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 30.;//NG
//	double dOrientCut = 35.;//NG
//	double dOrientCut = 40.;//NG
	double* dOrientAtEnergyMax = new double[MainPixel];//iSをふったときにEnergy（周辺の微分値）が最大となるときの角度（ただしCut角は除く）
	double* dEnergyMax = new double[MainPixel];//iSをふったときの最大Energy
#ifdef _INT_S_AT_M
	int* iSAtEnergyMax = new int[MainPixel];
#else
	double* dSAtEnergyMax = new double[MainPixel];
#endif
	for (int iM = 0; iM < MainPixel; iM++){
		dEnergyMax[iM] = 0.;//初期化
		dOrientAtEnergyMax[iM] = 0.;//初期化
#ifdef _INT_S_AT_M
		iSAtEnergyMax[iM] = 0;//初期化
#else
		dSAtEnergyMax[iM] = 0;//初期化
#endif
	}

	//主方向にスキャンし、QL比を越える主領域について、DC補間対象の副領域すべての微分値を調査
//	for (int iM = 0; iM < MainPixel; iM++){
	for (int iM = 1; iM < MainPixel - 1; iM++){//石丸さん指摘により、端にエッジがあった場合に実行エラーとなるはずなので、端を除外(140901 ttsuji)
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend   = edgeY[iM] + iItplSubOffset;
		DIFFERENTIALARROUNDPIXELS* DifPixelsArray = new DIFFERENTIALARROUNDPIXELS[iSend - iSstart + 1];//ある点の微分情報

		bool bflag = false;//斜めエッジ対応フラグ

//		if (fImgRateUD.Dt[iM][0] >= fThQLRateUD || fImgRateUD.Dt[iM][0] <= fThQLRateDU) {
		if (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2) {
			bflag = true;
 #if 0		//DC補間する副領域の中間地点の微分値のみ調査
			DifPixels = CalcDifferentialArroundPixel(MedLogImg, round((iSstart + iSend)/2.), iM);//ある点の周囲の微分情報を計算する
   #ifdef _LP_DEBUG_OUTPUT
//			fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, round((iSstart + iSend)/2.), DifPixels.dEnergy, DifPixels.dOrientation, DifPixels.dVmNorm, DifPixels.dVmX, DifPixels.dVmY, DifPixels.Left2Right, DifPixels.Up2Down, DifPixels.UpLeft2DownRight, DifPixels.UpRight2DownLeft);
			fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, round((iSstart + iSend)/2.), DifPixels.dEnergy, DifPixels.dOrientation, DifPixels.Left2Right, DifPixels.Up2Down, DifPixels.UpLeft2DownRight, DifPixels.UpRight2DownLeft);
   #endif
 #elif 0	//DC補間する副領域すべての画素の微分値を調査　→シンチ段差の横スジの影響を受けて、斜めエッジと横スジが混在した場合に変な角度を算出してしまう不具合あり。
			for (int iS = iSstart; iS <= iSend; iS++) {
				DifPixels = CalcDifferentialArroundPixel(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixels.dEnergy, DifPixels.dOrientation, DifPixels.Left2Right, DifPixels.Up2Down, DifPixels.UpLeft2DownRight, DifPixels.UpRight2DownLeft);
   #endif
				if (dEnergyMax[iM] < DifPixels.dEnergy && dOrientCut < DifPixels.dOrientation && DifPixels.dOrientation < 180. - dOrientCut) {
					dEnergyMax[iM] = DifPixels.dEnergy;//dEnergyMaxを更新
					dOrientAtEnergyMax[iM] = DifPixels.dOrientation;
   #ifdef _INT_S_AT_M
					iSAtEnergyMax[iM] = iS;
   #else
					dSAtEnergyMax[iM] = (double)iS;
   #endif
				}
			}
   #ifdef _LP_DEBUG_OUTPUT
	#ifdef _INT_S_AT_M
			fprintf(	, "%d	%lf	%d	%d	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, iSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
	#else
			fprintf(	, "%d	%lf	%d	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
	#endif
   #endif
 #else		//DC補間する副領域すべての画素の微分値を調査するが、上下方向のエッジが強い画素は斜めエッジの角度算出から除く.
			double dAveUD = 0.;//上下方向の微分値の平均値
			double dAveSquareUD = 0.;//上下方向の微分値の二乗平均
			double dStdEvpUD = 0.;//上下方向の微分値の標準偏差
			double dParaExclude = 1.0;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか
			for (int iS = iSstart; iS <= iSend; iS++) {
				if (0 == iM || MainPixel - 1 == iM) {
					int b = 0;//4方向微分の端処理がどうなっているか確認するため。
				}
  #if 0
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
  #else 		//除算をビットシフトに変更
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel_Fast(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
  #endif
				dAveUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff;
				dAveSquareUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff * DifPixelsArray[iS - iSstart].Up2Down.dDiff;
			}
			dAveUD /= iSend - iSstart + 1;//副方向の平均値を計算
			dAveSquareUD /= iSend - iSstart + 1;//副方向の二乗平均を計算
			dStdEvpUD = sqrt(dAveSquareUD - dAveUD * dAveUD);//副方向の標準偏差を計算

			//上下方向の微分値が、平均±(σ*dParaExclude)の範囲内のときの斜めエッジを統計処理
			double dUDUpperLimit = dAveUD + dParaExclude * dStdEvpUD;//上下微分の上限値
			double dUDLowerLimit = dAveUD - dParaExclude * dStdEvpUD;//上下微分の下限値
			double dSAve = 0.;
			double dOrientAve = 0.;
        	double dSumEnergy = 0.;
			for (int iS = iSstart; iS <= iSend; iS++) {
				bool bFlag2 = false;
				if (dUDLowerLimit < DifPixelsArray[iS - iSstart].Up2Down.dDiff && DifPixelsArray[iS - iSstart].Up2Down.dDiff < dUDUpperLimit && dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
					bFlag2 = true;
					dSAve += iS * DifPixelsArray[iS - iSstart].dEnergy;
					dOrientAve += DifPixelsArray[iS - iSstart].dOrientation * DifPixelsArray[iS - iSstart].dEnergy;
					dSumEnergy += DifPixelsArray[iS - iSstart].dEnergy;

					if (dEnergyMax[iM] < DifPixelsArray[iS - iSstart].dEnergy) {
						dEnergyMax[iM] = DifPixelsArray[iS - iSstart].dEnergy;//dEnergyMaxを更新
					}
				}
   #ifdef _LP_DEBUG_OUTPUT
			  	fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixelsArray[iS - iSstart].dEnergy, DifPixelsArray[iS - iSstart].dOrientation, DifPixelsArray[iS - iSstart].Left2Right, DifPixelsArray[iS - iSstart].Up2Down, DifPixelsArray[iS - iSstart].UpLeft2DownRight, DifPixelsArray[iS - iSstart].UpRight2DownLeft, bFlag2);
   #endif
			}
			if (!dSumEnergy) {
				continue;//上下微分が平均±(σ*dParaExclude)の範囲内で、かつdOrientCut度～(180-dOrientCut)度の範囲内のものがなければエッジなしと判断
			}
			dSAve /= dSumEnergy;
			dOrientAve /= dSumEnergy;
   #ifdef _INT_S_AT_M
			iSAtEnergyMax[iM] = round(dSAve);
   #else
			dSAtEnergyMax[iM] = dSAve;
   #endif
			dOrientAtEnergyMax[iM] = dOrientAve;

   #ifdef _LP_DEBUG_OUTPUT
	#ifdef _INT_S_AT_M
			fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, iSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
	#else
			fprintf(fpDCItpl2, "%d	%lf	%d	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
	#endif
   #endif
 #endif
		}
		else{
			bflag = false;
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag);
   #endif
		}

		delete[] DifPixelsArray;
	}

  #ifdef _LP_DEBUG_OUTPUT
//	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl2);
  #endif


  #if 0
  	//強制的に区分境界を入れるtest 141014
	dSAtEnergyMax[2020] = 0.;
	dSAtEnergyMax[2040] = 0.;
	dSAtEnergyMax[2060] = 0.;
  #endif

	//QL比を越える主領域について、グルーピングしてグループごとのエッジ角度を決めて、斜め方向のDC成分補間を行う
	int iMGrpStart = 0;
	int iMGrpEnd = 0;
	double dMGrpAve = 0.;
	double dOrientGrpAve = 0.;
	double dEnergyGrpMax = 0.;
	double dEnergyGrpAve = 0.;
	double dSAtGrpAve = 0.;
	double dCutRate = 0.3;//EnergyがMaxのdCutRate倍以下は統計から除外する
  #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl3 = fopen((ExtractFilePath(FileName) + "EdgePositonInformation.txt").c_str(), "wt");
	fprintf(fpDCItpl3, "iMGrpStart	iMGrpEnd	dMGrpAve	dSAtGrpAve	dOrientGrpAve	dEnergyGrpAve	dMItpl1	dMItpl2\n");
  #endif
	for (int iM = 0; iM < MainPixel; iM++){
  #ifdef _INT_S_AT_M
		if (!iSAtEnergyMax[iM]) {
  #else
		if (0.0 == dSAtEnergyMax[iM]) {
  #endif
			continue;
		}

		iMGrpStart = iM;
		dMGrpAve = 0.;
		dOrientGrpAve = 0.;
		dEnergyGrpMax = 0.;
		dEnergyGrpAve = 0.;
		dSAtGrpAve = 0.;
		//QL比を越える主領域が続く終点を探索
  #ifdef _INT_S_AT_M
		while (iSAtEnergyMax[iM] && iM < MainPixel){
  #else
		while (0.0 != dSAtEnergyMax[iM] && iM < MainPixel){
  #endif
			iMGrpEnd = iM;
			if (dEnergyGrpMax < dEnergyMax[iM]) {
             	dEnergyGrpMax = dEnergyMax[iM];
			}
			iM++;
		}

		int iCnt = 0;
		for (int iMtmp = iMGrpStart; iMtmp <= iMGrpEnd; iMtmp++) {
			if (dEnergyMax[iMtmp] < dEnergyGrpMax * dCutRate) {
				continue;
			}
			dOrientGrpAve += dOrientAtEnergyMax[iMtmp];
			dEnergyGrpAve += dEnergyMax[iMtmp];
			dMGrpAve += iMtmp * dEnergyMax[iMtmp];
   #ifdef _INT_S_AT_M
			dSAtGrpAve += iSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
   #else
			dSAtGrpAve += dSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
   #endif

   #ifdef _LP_DEBUG_OUTPUT
    #ifdef _INT_S_AT_M
			fprintf(fpDCItpl3, "	%d	%d	%lf	%lf\n", iMtmp, iSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
	#else
			fprintf(fpDCItpl3, "	%d	%lf	%lf	%lf\n", iMtmp, dSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
	#endif
   #endif
			iCnt++;
		}
		if (!iCnt) {
			continue;
		}
		dMGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dSAtGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dOrientGrpAve /= iCnt;
		dEnergyGrpAve /= iCnt;

		//斜めエッジとDC補間の副領域の交差点のｘ座標
		double dMItpl1 = 0.;//補間の開始ｘ座標
		double dMItpl2 = 0.;//補間の終了ｘ座標
/*		int iSItpl1 = edgeY[iM] - iSubRangeOnlyMidPanel;
//		int iSItpl2 = edgeY[iM] + iItplSubOffset + 1;
		int iSItpl2 = edgeY[iM] + iItplSubOffset;
*/
		int iSItpl1 = edgeY[round(dMGrpAve)] - iSubRangeOnlyMidPanel;//どのｘでのedgeYなのかを明確化(140816 ttsuji)
//		int iSItpl2 = edgeY[round(dMGrpAve)] + iItplSubOffset; //どのｘでのedgeYなのかを明確化(140816 ttsuji)
		int iSItpl2 = edgeY[round(dMGrpAve)] + iItplSubOffset + 1; //石丸さん指摘により修正(140901 ttsuji)

		double dTan = tan(PI/180. * dOrientGrpAve);
		dMItpl1 = (iSItpl1 - dSAtGrpAve) / dTan + dMGrpAve;
		dMItpl2 = (iSItpl2 - dSAtGrpAve) / dTan + dMGrpAve;
		double dTmp;
		if (dMItpl1 > dMItpl2) {//dMItpl1 < dMItpl2 となるようにソートする
			dTmp = dMItpl1;
			dMItpl1 = dMItpl2;
			dMItpl2 = dTmp;
		}

   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl3, "%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iMGrpStart, iMGrpEnd, dMGrpAve, dSAtGrpAve, dOrientGrpAve, dEnergyGrpAve, dMItpl1, dMItpl2);
   #endif

   #if 1
		if (dMItpl1 > 2020) {
			int c = 0;
		}
   #endif


		//斜め方向のDC成分補間(3点補間)
//   #ifndef _LP_DEBUG_OUTPUT
//		LPDCItplNanameSankakuHokan(ItplImg, CorrImg, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa);
//   #else
		LPDCItplNanameSankakuHokan(ItplImg, CorrImg, CorrImgDebug, edgeY, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, iSubRangeOnlyMidPanel, iItplSubOffset);
//   #endif

	}

   #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl3);
	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg2.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg3.tma");
	CorrImgDebug.WriteTma(ExtractFilePath(FileName) + "CorrImg3debug.tma");
   #endif
	delete[] dOrientAtEnergyMax;
	delete[] dEnergyMax;
#ifdef _INT_S_AT_M
	delete[] iSAtEnergyMax;
#else
	delete[] dSAtEnergyMax;
#endif



	/*   この行はデバッグ終えたら実施に変更する     */
//	CorrImg.MainMedian(201);//201にすると、外挿部の縦スジは消えるが、マーカーのアーチファクトが発生する
	CorrImg.MainMedian(iMainMedianDCItpl);

//	CorrImg.SubMedian(3);//tmp(これを入れると、消したい高周波の横スジ（段差の残差等）を消せない)



  #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg2.tma");
  #endif

	for (int iM = 0; iM < MainPixel; iM++){
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for( int iS = iSstart; iS <= iSend; iS++){//石丸さん指摘により修正(140901 ttsuji)
//		for( int iS = iSstart; iS < iSend; iS++){
			if (iS >= CorrImg.SubPixel || iS >= SubPixel || iM >= CorrImg.MainPixel || iM >= MainPixel) {
				continue;
			}
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}

}
//---------------------------------------------------------------------------
//斜めエッジをQL比閾値判定で検出し、かつ副微分で斜めエッジのｙ座標を特定し、斜めエッジに平行に平行四辺形補間する

#define _141121KANAJAKU
#define _151031YPERCENTTH
#define _151106XRANGETH

void TSIMARS::LPCorrectNonOverlapAreaByItplDC4(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, bool bDetectKanajakuMemori, int iMedianHasiSyori, int iThQLSubProf, int iSAveNum, int iMarginXMed, bool& bExistPbScale)
{
	TTMA ItplImg;
//	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
//	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 2, MainPixel, Bit);//マージン込み確保
	ItplImg.NewDt(iYSintiEnd + iItplSubOffset + 3, MainPixel, Bit);//マージン込み確保
	ItplImg.FillValue(0);

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl = fopen((ExtractFilePath(FileName) + "QLrateDCItplMainProfile.txt").c_str(), "wt");
	fprintf(fpDCItpl, "iM	QLUp	QLDown	QLRate	QLRate2\n");
//	fprintf(fpDCItpl, "iM	QLUp	QLDown	QLRate	QLRate2	flag	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
 #endif
	TTMA CorrImgDebug;
	CorrImgDebug.NewDt(CorrImg.SubPixel, CorrImg.MainPixel, CorrImg.Bit);
	CorrImgDebug.FillValue(0);


	//ある点の微分情報
//	DIFFERENTIALARROUNDPIXELS DifPixels;


	//debugのための画像
	TTMA ImageUp2Down = *this;
	TTMA ImageLeft2Right = *this;
	TTMA ImageUpLeft2DownRight = *this;
	TTMA ImageUpRight2DownLeft = *this;

	//主Medianでノイズを落とす
	const int iMMed = 11;//DC補間長さ20pixの情報は保持するマスクサイズ
	TTMA MedImg = *this;
	MedImg.MainMedian(iMMed);
 #ifdef _LP_DEBUG_OUTPUT
	MedImg.WriteTma( AddPrefix(FileName, "MMed(" + IntToStr(iMMed) + ")") );
 #endif

/*	//Log変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	MedImg.SubMedian(iMMed); //斜めエッジ誤検出対策（141113 ttsuji)
	TTMA MedLogImg = MedImg;
	MedLogImg.LogThreshold(dMidVal, dKeta, iLogTh);
	MedLogImg.WriteTma( AddPrefix(FileName, "L") );
*/
	//DC補間サンプル点のQL比の主プロファイル
	TFIMG fImgRateUD;
	fImgRateUD.NewDt(MainPixel, 1);
	fImgRateUD.Fill(0.);

	//QL比閾値（低周波情報除去画像）
	const float fThQLRatioHighPass = 1.1;//平行四辺形補間にすることで、角度誤検出の副作用が重篤となったため、こちらに戻す(141106 ttsuji)
//	const float fThQLRatioHighPass = 1.05;//黒腰椎側面の斜めエッジ黒つぶれ対策としてQL比閾値を緩め、広いｘ範囲を斜めエッジとして計算することとした(141103 ttsuji)


	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
#if 0   //引数に格上げ （150121ttsuji）
		const int iSAveNum = 1;//副方向の平均画素数 //150120 ttsuji
//		const int iSAveNum = 3;//副方向の平均画素数
//		const int iSAveNum = 7;//副方向の平均画素数
#endif
		//DC成分を線形補間で計算する
		float fAveU = 0.;
		float fAveD = 0.;
		for (iS = 0; iS < iSAveNum; iS++){
   #if 0
			fAveU += Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			fAveD += Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
   #else
			fAveU += MedImg.Dt[edgeY[iM] - iSubRangeOnlyMidPanel - iS][iM];
			fAveD += MedImg.Dt[edgeY[iM] + 1 + iItplSubOffset + iS][iM];
   #endif
		}
		fAveU /= iSAveNum;
		fAveD /= iSAveNum;

		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend   = edgeY[iM] + iItplSubOffset;

		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		fImgRateUD.Dt[iM][0] = fQLRateUD;

		//QL比が閾値以上かに関わらず、まずDC成分の線形補間を行う
		for (iS = iSstart; iS <= iSend; iS++){//石丸さん指摘によりiSendも含むよう修正(140901 ttsuji)
			float fQLitpl, fAveUtmp, fAveDtmp;
			fQLitpl = ((iSend - iS) * fAveU + (iS -iSstart) * fAveD) / (iSend - iSstart);//線形補間

			if (iS >= CorrImg.SubPixel || iS >= ItplImg.SubPixel || iM >= CorrImg.MainPixel || iM >= ItplImg.MainPixel) {
				continue;
			}
			ItplImg.SetVal(iS, iM, fQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - fQLitpl + iKasa);
//  #ifdef _LP_DEBUG_OUTPUT
			CorrImgDebug.SetVal(iS, iM, CorrImg.Dt[iS][iM]);//debug
//  #endif
		}
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
  #endif
	}//end for iM

  #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl);
	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	CorrImgDebug.WriteTma(ExtractFilePath(FileName) + "CorrImg2debug.tma");

	FILE* fpDCItpl2 = fopen((ExtractFilePath(FileName) + "QLrateHighPassDCItplMainProfile.txt").c_str(), "wt");
//	fprintf(fpDCItpl2, "iM	QLUp	QLDown	QLRate	QLRate2	flag	iS	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
	fprintf(fpDCItpl2, "iM	QLRate	flag	iS	Energy	Orientation	L2R	U2D	UL2DR	UR2DL\n");
  #endif


#if 1//斜め補間をしない場合に0にする

	//Log変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	MedImg.SubMedian(iMMed); //斜めエッジ誤検出対策（141113 ttsuji)
	TTMA MedLogImg = MedImg;
	MedLogImg.LogThreshold(dMidVal, dKeta, iLogTh);
 #ifdef _LP_DEBUG_OUTPUT
	MedLogImg.WriteTma( AddPrefix(FileName, "L") );
 #endif

	//QL比の主プロファイルを主Medianし、低周波のみを抽出
	const int iMainMedQLRate = 201;
	TFIMG fImgRateUDMMed = fImgRateUD;
	fImgRateUDMMed.MainMedian(iMainMedQLRate);
	fImgRateUD -= fImgRateUDMMed;//低周波情報を除去し、高周波のみを抽出
	fImgRateUD += 1.;//１を加算し、UとDが等しいときを１倍とする
  #ifdef _LP_DEBUG_OUTPUT
	fImgRateUD.bSaveAsText((ExtractFilePath(FileName) + "QLRateHighPass.txt").c_str(), true);
  #endif

	//QL比の主プロファイルのHighPass後のQL比から斜めエッジを検出（この方法だと、DC補間の20pix間の濃度変化の影響を受けず、精度の良いエッジ検出が可能）
	float fThQLRatioHighPass2 = 1 / fThQLRatioHighPass;
//	double dOrientCut = 5.;//tmp
//	double dOrientCut = 10.;//tmp
	double dOrientCut = 20.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 30.;//NG
//	double dOrientCut = 35.;//NG
//	double dOrientCut = 40.;//NG
	double* dOrientAtEnergyMax = new double[MainPixel];//iSをふったときにEnergy（周辺の微分値）が最大となるときの角度（ただしCut角は除く）
	double* dEnergyMax = new double[MainPixel];//iSをふったときの最大Energy
	double* dSAtEnergyMax = new double[MainPixel];
	for (int iM = 0; iM < MainPixel; iM++){
		dEnergyMax[iM] = 0.;//初期化
		dOrientAtEnergyMax[iM] = 0.;//初期化
		dSAtEnergyMax[iM] = 0;//初期化
	}
	int iExcludeRange = 3;//シンチ段差近傍で統計から除外するレンジ

	//主方向にスキャンし、QL比を越える主領域について、DC補間対象の副領域すべての微分値を調査
	for (int iM = 1; iM < MainPixel - 1; iM++){//石丸さん指摘により、端にエッジがあった場合に実行エラーとなるはずなので、端を除外(140901 ttsuji)
  #if 1
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend   = edgeY[iM] + iItplSubOffset;
  #elif 0//聖隷佐倉病院の金属ロッド部で斜め角度誤検出の市場対応（151029 ttsuji）
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 2;
		int iSend   = edgeY[iM] - iExcludeRange;
  #endif
		DIFFERENTIALARROUNDPIXELS* DifPixelsArray = new DIFFERENTIALARROUNDPIXELS[iSend - iSstart + 1];//ある点の微分情報

		bool bflag = false;//斜めエッジ対応フラグ

  #if 1 //debug
		if (659 == iM) {
			int aa = 0;
		}
  #endif


		if (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2) {
//		if (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2 || (1070 <= iM && iM <= 1150)) {//黒腰椎側面の斜めエッジ黒つぶれ解析用
			bflag = true;

			//DC補間する副領域すべての画素の微分値を調査するが、上下方向のエッジが強い画素は斜めエッジの角度算出から除く.
			double dAveUD = 0.;//上下方向の微分値の平均値
			double dAveSquareUD = 0.;//上下方向の微分値の二乗平均
			double dStdEvpUD = 0.;//上下方向の微分値の標準偏差
  #ifndef _151031YPERCENTTH
			double dParaExclude = 1.0;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか
//			double dParaExclude = 0.5;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか (斜めエッジ誤検出対応　141107 ttsuji)
//			double dParaExclude = 0.2;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか (斜めエッジ誤検出対応　141107 ttsuji)
  #else
			double dParaExclude = 3.0;//上下方向の微分値の値によらず統計から除外しない（シンチ段差の横スジ除去するため不要）(151030 ttsuji)
  #endif

			double dPercentTh = 0.4;//ｙ方向の統計をとる際の、全母集団（DC補間長さ）に対する採用された画素数の割合の閾値(151031 ttsuji)
			int iCntY = 0;//該当ｘにおけるDC補間幅の全画素のうち、エッジ角度検出の統計に採用される画素数(151031 ttsuji)
			double dYPercent;//エッジ角度検出の統計に採用された画素数の割合

			for (int iS = iSstart; iS <= iSend; iS++) {
/*
				if (0 == iM || MainPixel - 1 == iM) {
					int b = 0;//4方向微分の端処理がどうなっているか確認するため。
				}
*/
  #if 0
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
  #else 		//除算をビットシフトに変更
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel_Fast(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
  #endif
				dAveUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff;
				dAveSquareUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff * DifPixelsArray[iS - iSstart].Up2Down.dDiff;
			}
			dAveUD /= iSend - iSstart + 1;//副方向の平均値を計算
			dAveSquareUD /= iSend - iSstart + 1;//副方向の二乗平均を計算
			dStdEvpUD = sqrt(dAveSquareUD - dAveUD * dAveUD);//副方向の標準偏差を計算

			//上下方向の微分値が、平均±(σ*dParaExclude)の範囲内のときの斜めエッジを統計処理
			double dUDUpperLimit = dAveUD + dParaExclude * dStdEvpUD;//上下微分の上限値
			double dUDLowerLimit = dAveUD - dParaExclude * dStdEvpUD;//上下微分の下限値
			double dSAve = 0.;
			double dOrientAve = 0.;
			double dSumEnergy = 0.;
			int iThEnergy = 300; //エネルギー閾値を設け、これ以下エネルギーしかない画素は統計処理から除外する(151030 ttsuji)

			iCntY = 0;//初期化
			for (int iS = iSstart; iS <= iSend; iS++) {
				bool bFlag2 = false;

   #ifdef _151031YPERCENTTH
				if (iSstart == iS || ( edgeY[iM] - iExcludeRange < iS && iS <=  edgeY[iM] + iExcludeRange)) {//統計に除外するｙ座標
	 #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixelsArray[iS - iSstart].dEnergy, DifPixelsArray[iS - iSstart].dOrientation, DifPixelsArray[iS - iSstart].Left2Right, DifPixelsArray[iS - iSstart].Up2Down, DifPixelsArray[iS - iSstart].UpLeft2DownRight, DifPixelsArray[iS - iSstart].UpRight2DownLeft, bFlag2);
	 #endif
					continue;
				}
   #endif

   #if 1
				if (dUDLowerLimit < DifPixelsArray[iS - iSstart].Up2Down.dDiff && DifPixelsArray[iS - iSstart].Up2Down.dDiff < dUDUpperLimit && dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
   #else		//エネルギー閾値より小さい画素は統計から除外する(151030 ttsuji)  →頭蓋側面(5R-4)の斜めエッジを正しく検出できないことが判明し、断念。
				if (dUDLowerLimit < DifPixelsArray[iS - iSstart].Up2Down.dDiff && DifPixelsArray[iS - iSstart].Up2Down.dDiff < dUDUpperLimit && dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut && DifPixelsArray[iS - iSstart].dEnergy > iThEnergy) {
   #endif
					bFlag2 = true;
					iCntY++;
					dSAve += iS * DifPixelsArray[iS - iSstart].dEnergy;
					dOrientAve += DifPixelsArray[iS - iSstart].dOrientation * DifPixelsArray[iS - iSstart].dEnergy;
					dSumEnergy += DifPixelsArray[iS - iSstart].dEnergy;

					if (dEnergyMax[iM] < DifPixelsArray[iS - iSstart].dEnergy) {
						dEnergyMax[iM] = DifPixelsArray[iS - iSstart].dEnergy;//dEnergyMaxを更新
					}
				}
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixelsArray[iS - iSstart].dEnergy, DifPixelsArray[iS - iSstart].dOrientation, DifPixelsArray[iS - iSstart].Left2Right, DifPixelsArray[iS - iSstart].Up2Down, DifPixelsArray[iS - iSstart].UpLeft2DownRight, DifPixelsArray[iS - iSstart].UpRight2DownLeft, bFlag2);
   #endif
			}
			if (!dSumEnergy) {
				continue;//上下微分が平均±(σ*dParaExclude)の範囲内で、かつdOrientCut度～(180-dOrientCut)度の範囲内のものがなければエッジなしと判断
			}
			dSAve /= dSumEnergy;
			dOrientAve /= dSumEnergy;
			dSAtEnergyMax[iM] = dSAve;
			dOrientAtEnergyMax[iM] = dOrientAve;
			dYPercent = (double)iCntY / (iSend - iSstart + 1);
  #ifdef _151031YPERCENTTH  //採用されたｙ座標の割合の閾値判定を実施する(151031 ttsuji)
			if (dYPercent > dPercentTh) {
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%lf	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM], dYPercent);
   #endif
			}
			else{
				bflag = false;
				dSAtEnergyMax[iM] = 0.0;
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dYPercent);
   #endif
			}
  #else
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
   #endif
  #endif
		}
		else{
			bflag = false;
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag);
   #endif
		}

		delete[] DifPixelsArray;
	}

  #ifdef _LP_DEBUG_OUTPUT
//	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl2);
  #endif


  #if 0
	//強制的に区分境界を入れるtest 141014
	dSAtEnergyMax[2020] = 0.;
	dSAtEnergyMax[2040] = 0.;
	dSAtEnergyMax[2060] = 0.;
  #endif

	//QL比を越える主領域について、グルーピングしてグループごとのエッジ角度を決めて、斜め方向のDC成分補間を行う
	int iMGrpStart = 0;
	int iMGrpEnd = 0;
	double dMGrpAve = 0.;
	double dOrientGrpAve = 0.;
	double dEnergyGrpMax = 0.;
	double dEnergyGrpAve = 0.;
	double dSAtGrpAve = 0.;
	double dCutRate = 0.3;//EnergyがMaxのdCutRate倍以下は統計から除外する
  #ifdef _151106XRANGETH
//	int iMRangeTh = 2;//tmp 151106 ttsuji
	int iMRangeTh = 5;//tmp 151106 ttsuji
  #endif

  #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl3 = fopen((ExtractFilePath(FileName) + "EdgePositonInformation.txt").c_str(), "wt");
	fprintf(fpDCItpl3, "iMGrpStart	iMGrpEnd	dMGrpAve	dSAtGrpAve	dOrientGrpAve	dEnergyGrpAve	dMItpl1	dMItpl2\n");
  #endif
	for (int iM = 0; iM < MainPixel; iM++){
  #if 1
		if (iM == 523) {
			int c = 0;
		}
  #endif
		if (0.0 == dSAtEnergyMax[iM]) {
			continue;
		}

		iMGrpStart = iM;
		dMGrpAve = 0.;
		dOrientGrpAve = 0.;
		dEnergyGrpMax = 0.;
		dEnergyGrpAve = 0.;
		dSAtGrpAve = 0.;
		//QL比を越える主領域が続く終点を探索
		while (0.0 != dSAtEnergyMax[iM] && iM < MainPixel){
			iMGrpEnd = iM;
			if (dEnergyGrpMax < dEnergyMax[iM]) {
				dEnergyGrpMax = dEnergyMax[iM];
			}
			iM++;
		}

   #ifdef _151106XRANGETH //test
		if (iMGrpEnd - iMGrpStart < iMRangeTh) {
        	continue;//ｘごとに検出した斜めエッジがある閾値以上連続して検出されなければ斜め補間しない。(151106 ttsuji)
		}
   #endif

		int iCnt = 0;
		for (int iMtmp = iMGrpStart; iMtmp <= iMGrpEnd; iMtmp++) {
			if (dEnergyMax[iMtmp] < dEnergyGrpMax * dCutRate) {
				continue;
			}
			dOrientGrpAve += dOrientAtEnergyMax[iMtmp];
			dEnergyGrpAve += dEnergyMax[iMtmp];
			dMGrpAve += iMtmp * dEnergyMax[iMtmp];
			dSAtGrpAve += dSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];

   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl3, "	%d	%lf	%lf	%lf\n", iMtmp, dSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
   #endif
			iCnt++;
		}
		if (!iCnt) {
			continue;
		}
		dMGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dSAtGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dOrientGrpAve /= iCnt;
		dEnergyGrpAve /= iCnt;

		//斜めエッジとDC補間の副領域の交差点のｘ座標
		double dMItpl1 = 0.;//補間の開始ｘ座標
		double dMItpl2 = 0.;//補間の終了ｘ座標
/*		int iSItpl1 = edgeY[iM] - iSubRangeOnlyMidPanel;
//		int iSItpl2 = edgeY[iM] + iItplSubOffset + 1;
		int iSItpl2 = edgeY[iM] + iItplSubOffset;
*/
		int iSItpl1 = edgeY[round(dMGrpAve)] - iSubRangeOnlyMidPanel;//どのｘでのedgeYなのかを明確化(140816 ttsuji)
//		int iSItpl2 = edgeY[round(dMGrpAve)] + iItplSubOffset; //どのｘでのedgeYなのかを明確化(140816 ttsuji)
		int iSItpl2 = edgeY[round(dMGrpAve)] + iItplSubOffset + 1; //石丸さん指摘により修正(140901 ttsuji)

		double dTan = tan(PI/180. * dOrientGrpAve);
		dMItpl1 = (iSItpl1 - dSAtGrpAve) / dTan + dMGrpAve;
		dMItpl2 = (iSItpl2 - dSAtGrpAve) / dTan + dMGrpAve;
		double dTmp;
		if (dMItpl1 > dMItpl2) {//dMItpl1 < dMItpl2 となるようにソートする
			dTmp = dMItpl1;
			dMItpl1 = dMItpl2;
			dMItpl2 = dTmp;
		}

   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl3, "%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iMGrpStart, iMGrpEnd, dMGrpAve, dSAtGrpAve, dOrientGrpAve, dEnergyGrpAve, dMItpl1, dMItpl2);
   #endif

   #if 0
		if (dMItpl1 > 2020) {
			int c = 0;
		}
   #endif


		//斜め方向のDC成分補間(平行四辺形補間)
		LPDCItplNanameHeikohShihenkeiHokan(ItplImg, CorrImg, CorrImgDebug, edgeY, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, iSubRangeOnlyMidPanel, iItplSubOffset);
	}

   #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl3);
	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg2.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg3.tma");
	CorrImgDebug.WriteTma(ExtractFilePath(FileName) + "CorrImg3debug.tma");
   #endif
	delete[] dOrientAtEnergyMax;
	delete[] dEnergyMax;
	delete[] dSAtEnergyMax;



	/*   この行はデバッグ終えたら実施に変更する     */
//	CorrImg.MainMedian(201);//201にすると、外挿部の縦スジは消えるが、マーカーのアーチファクトが発生する
	CorrImg.MainMedian(iMainMedianDCItpl);
//	CorrImg.SubMedian(3);//tmp(これを入れると、消したい高周波の横スジ（段差の残差等）を消せない)
  #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg2.tma");
  #endif


	//DC補間領域内の横長の金属エッジを探索する(141108 ttsuji 金尺の目盛り消失対策として)
	if (bDetectKanajakuMemori) {
		const int iDx = 5;//エッジ探索するｘの間隔
//		const int iDx = 1;//エッジ探索するｘの間隔
 #if 0  //iThQLSubProfは引数指定
//		const int iThQLSubProf = 2000;//副微分プロファイルのQL閾値
		const int iThQLSubProf = 1800;//副微分プロファイルのQL閾値(現在の設定)
//		const int iThQLSubProf = 1200;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 1100;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 800;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 700;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 600;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 300;//副微分プロファイルのQL閾値
 #endif
//		const int iThQLMainProf = 2000;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1800;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1500;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1300;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1100;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1000;//主微分プロファイルのQL閾値
		const int iThQLMainProf = 800;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 700;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 600;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 300;//主微分プロファイルのQL閾値
		int iNumOfXROI = 0;//検出したPbスケールのROI数（DC補間領域外を用いた補正用）
		int iNumOfXROIMed = 0;//検出したPbスケールのROI数(主Median用)
		ROI XROI[512];//検出したｘ領域（DC補間領域外を用いた補正用）
		ROI XROIMed[512];//検出したｘ領域(主Median用)
		for (int iROI = 0; iROI < 512; iROI++) {
			XROI[iROI].x1 = XROI[iROI].x2 = XROI[iROI].y1 = XROI[iROI].y2 = XROI[iROI].Pattern = 0;
			XROIMed[iROI].x1 = XROIMed[iROI].x2 = XROIMed[iROI].y1 = XROIMed[iROI].y2 = XROIMed[iROI].Pattern = 0;
		}
#if 0  //矩形アーチファクト対策により削除 (150722 ttsuji)
 #ifdef _141121KANAJAKU
		const int iMedLarge = 401;//金尺が消えないくらい大きなマスク
		TTMA MedLargeCorrImg, MedLargeThis;
		MedLargeCorrImg = CorrImg;
		MedLargeCorrImg.MainMedian(iMedLarge);//端処理は可変長
  #ifdef _LP_DEBUG_OUTPUT
		MedLargeCorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg2Large.tma");
  #endif

		switch (iMedianHasiSyori) {
		case 0://端から(Medianサイズ/2 +1)番目のMedian値で置換
			MedLargeCorrImg.MainPixelExpand(iMedLarge / 2);//画素延長処理(端に金尺があった場合の対応)
			break;
		case 1://可変長マスクサイズでMedian
			break;//何もせずbreak
		case 2://可変長Medianの端部プロファイルから低周波のみを抽出
			for (int iM = 0; iM < iMedLarge / 2; iM++){
				int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
				int iSend = edgeY[iM] + iItplSubOffset;
				for( int iS = iSstart; iS <= iSend; iS++){//石丸さん指摘により修正(140901 ttsuji)
					if (iS >= CorrImg.SubPixel || iS >= SubPixel || iM >= CorrImg.MainPixel || iM >= MainPixel) {
						continue;
					}
					MedLargeCorrImg.SetVal(iS, iM, (MedLargeCorrImg.Dt[iS][0] * (iMedLarge / 2 - iM) + MedLargeCorrImg.Dt[iS][iMedLarge / 2] * iM) / (iMedLarge / 2) );
				}
			}
			break;
		}
/*
  #if 0 //画素延長だと濃度の勾配があったときに追従できない不具合あり(150115 ttsuji)
	MedLargeCorrImg.MainPixelExpand(iMedLarge / 2);//画素延長処理(端に金尺があった場合の対応)
  #elif 0 //可変長Medianの端部プロファイルから低周波のみを抽出
	for (int iM = 0; iM < iMedLarge / 2; iM++){
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		for( int iS = iSstart; iS <= iSend; iS++){//石丸さん指摘により修正(140901 ttsuji)
			if (iS >= CorrImg.SubPixel || iS >= SubPixel || iM >= CorrImg.MainPixel || iM >= MainPixel) {
				continue;
			}
			MedLargeCorrImg.SetVal(iS, iM, (MedLargeCorrImg.Dt[iS][0] * (iMedLarge / 2 - iM) + MedLargeCorrImg.Dt[iS][iMedLarge / 2] * iM) / (iMedLarge / 2) );
		}
	}
  #endif
*/
  #ifdef _LP_DEBUG_OUTPUT
		MedLargeCorrImg.WriteTma(ExtractFilePath(FileName) + "MedExpand_CorrImg2Large.tma");
  #endif
		MedLargeThis = *this;
		for (int iM = 0; iM < MedLargeThis.MainPixel; iM++){
			int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
			int iSend = edgeY[iM] + iItplSubOffset;
			for( int iS = iSstart; iS <= iSend; iS++){//石丸さん指摘により修正(140901 ttsuji)
				if (iS >= CorrImg.SubPixel || iS >= SubPixel || iM >= CorrImg.MainPixel || iM >= MainPixel) {
					continue;
				}
				MedLargeThis.SetVal(iS, iM, MedLargeThis.Dt[iS][iM] - MedLargeCorrImg.Dt[iS][iM] + iKasa);
			}
		}
  #ifdef _LP_DEBUG_OUTPUT
		MedLargeThis.WriteTma(ExtractFilePath(FileName) + "SintiMainMedLarge.tma");
  #endif

		MedLargeThis.MainMedian(iMMed);
		MedLargeThis.SubMedian(iMMed); //斜めエッジ誤検出対策（141113 ttsuji)

		//Log変換
		TTMA MedLogLargeThis = MedLargeThis;
		MedLogLargeThis.LogThreshold(dMidVal, dKeta, iLogTh);
  #ifdef _LP_DEBUG_OUTPUT
		MedLogLargeThis.WriteTma( AddPrefix(FileName, "LogMedLarge") );
  #endif
 #endif
#endif //矩形アーチファクト対策により削除

 #ifndef _141121KANAJAKU
//		LPDetectPbScaleWhiteBandXROI(MedLogImg, FileName, edgeY, iDx, iThQLSubProf, iThQLMainProf, iSubRangeOnlyMidPanel, iItplSubOffset, iNumOfXROI, XROI);//DC補間領域内の横長の白帯を検出
		LPDetectPbScaleEdgeXROI(MedLogImg, FileName, edgeY, iDx, iThQLSubProf, iThQLMainProf, iSubRangeOnlyMidPanel, iItplSubOffset, iNumOfXROI, XROI, iNumOfXROIMed, XROIMed);
 #elif 0 //401Medianかけた補正画像をつかって暫定的に非重複領域補正した画像から目盛検出
		LPDetectPbScaleEdgeXROI(MedLogLargeThis, FileName, edgeY, iDx, iThQLSubProf, iThQLMainProf, iSubRangeOnlyMidPanel, iItplSubOffset, iNumOfXROI, XROI, iNumOfXROIMed, XROIMed);
 #else  //矩形アーチファクト対策テスト(目盛抽出用の副微分QL閾値は1500→300へ)（150514 ttsuji）
		int iSubRangeOnlyMidPanelTest = iSubRangeOnlyMidPanel - 1;
//		int iSubRangeOnlyMidPanelTest = iSubRangeOnlyMidPanel - 2;
//		int iSubRangeOnlyMidPanelTest = iSubRangeOnlyMidPanel - 3;
//		int iItplSubOffsetTest = 0;
//		int iItplSubOffsetTest = -1;
//		int iItplSubOffsetTest = -2;
		int iItplSubOffsetTest = -3;
		LPDetectPbScaleEdgeXROI(MedLogImg, FileName, edgeY, iDx, iThQLSubProf, iThQLMainProf, iSubRangeOnlyMidPanelTest, iItplSubOffsetTest, iNumOfXROI, XROI, iNumOfXROIMed, XROIMed, bExistPbScale);
 #endif

		//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。
//		LPMainMedianForEachROI(CorrImg, edgeY, iNumOfXROI, XROI, iMainMedianDCItpl, iSubRangeOnlyMidPanel, iItplSubOffset, iKasa);
		LPMainMedianAndCopyForEachROI(CorrImg, FileName, edgeY, iNumOfXROI, XROI, iNumOfXROIMed, XROIMed, iMainMedianDCItpl, iSubRangeOnlyMidPanel, iItplSubOffset, iKasa, iMedianHasiSyori, iMarginXMed);
  #ifdef _LP_DEBUG_OUTPUT
		CorrImg.WriteTma(ExtractFilePath(FileName) + "Med_CorrImg3.tma");
  #endif
	}
#else //斜め検出しない場合
	CorrImg.MainMedian(iMainMedianDCItpl);
#endif


	for (int iM = 0; iM < MainPixel; iM++){
		//tmp
		if (iM == 100) {
			int d= 0;
		}
		if (iM == 1000) {
			int d= 0;
		}
		if (iM == 2000) {
			int d= 0;
		}

		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
#if 1
		for( int iS = iSstart; iS <= iSend; iS++){//石丸さん指摘により修正(140901 ttsuji)
#else
		for( int iS = iSstart; iS <= iSend + 5; iS++){//非重複領域外の金尺対応(150120 ttsuji)
//		for( int iS = iSstart; iS <= iSend + 10; iS++){//非重複領域外の金尺対応(150120 ttsuji)
#endif
			if (iS >= CorrImg.SubPixel || iS >= SubPixel || iM >= CorrImg.MainPixel || iM >= MainPixel) {
				continue;
			}
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}

	int d = 0;
  #ifdef _LP_DEBUG_OUTPUT
  	WriteTma(ExtractFilePath(FileName) + "S_tmp.tma");
  #endif

}
//---------------------------------------------------------------------------
//ある点の周囲の微分情報を計算する

DIFFERENTIALARROUNDPIXELS TSIMARS::CalcDifferentialArroundPixel(TTMA& Image, int iSCnt, int iMCnt)
{
	DIFFERENTIALARROUNDPIXELS DifPixels;
	double dMax(0.), dMin(100000.);
	double dFilterSobelUp2Down[9] = {679/32768., 31411/32768., 679/32768., 0, 0, 0, -679/32768., -31411/32768., -679/32768.};
	double dFilterSobelLeft2Right[9] = {-679/32768., 0, 679/32768., -31411/32768., 0, 31411/32768., -679/32768., 0, 679/32768.};
	double dFilterSobelUpLeft2DownRight[9] = {-1357/32768., -15705/32768., 0, -15705/32768., 0, 15705/32768., 0, 15705/32768., 1357/32768.};
	double dFilterSobelUpRight2DownLeft[9] = {0, 15705/32768., 1357/32768., -15705/32768., 0, 15705/32768., -1357/32768., -15705/32768., 0};

	//上下方向の微分を計算
	DifPixels.Up2Down.dDiff =  Image.Dt[iSCnt - 1][iMCnt - 1]*dFilterSobelUp2Down[0] +
							   Image.Dt[iSCnt - 1][iMCnt    ]*dFilterSobelUp2Down[1] +
							   Image.Dt[iSCnt - 1][iMCnt + 1]*dFilterSobelUp2Down[2] +
							   Image.Dt[iSCnt    ][iMCnt - 1]*dFilterSobelUp2Down[3] +
							   Image.Dt[iSCnt    ][iMCnt    ]*dFilterSobelUp2Down[4] +
							   Image.Dt[iSCnt    ][iMCnt + 1]*dFilterSobelUp2Down[5] +
							   Image.Dt[iSCnt + 1][iMCnt - 1]*dFilterSobelUp2Down[6] +
							   Image.Dt[iSCnt + 1][iMCnt    ]*dFilterSobelUp2Down[7] +
							   Image.Dt[iSCnt + 1][iMCnt + 1]*dFilterSobelUp2Down[8];
	if (dMax < abs(DifPixels.Up2Down.dDiff)) {
		dMax = abs(DifPixels.Up2Down.dDiff);
		DifPixels.indexMax = _Vertical;
	}
	if (dMin > abs(DifPixels.Up2Down.dDiff)) {
		dMin = abs(DifPixels.Up2Down.dDiff);
		DifPixels.indexMin = _Vertical;
	}

	//左右方向の微分を計算
	DifPixels.Left2Right.dDiff = Image.Dt[iSCnt - 1][iMCnt - 1]*dFilterSobelLeft2Right[0] +
								 Image.Dt[iSCnt - 1][iMCnt    ]*dFilterSobelLeft2Right[1] +
								 Image.Dt[iSCnt - 1][iMCnt + 1]*dFilterSobelLeft2Right[2] +
								 Image.Dt[iSCnt    ][iMCnt - 1]*dFilterSobelLeft2Right[3] +
								 Image.Dt[iSCnt    ][iMCnt    ]*dFilterSobelLeft2Right[4] +
								 Image.Dt[iSCnt    ][iMCnt + 1]*dFilterSobelLeft2Right[5] +
								 Image.Dt[iSCnt + 1][iMCnt - 1]*dFilterSobelLeft2Right[6] +
								 Image.Dt[iSCnt + 1][iMCnt    ]*dFilterSobelLeft2Right[7] +
								 Image.Dt[iSCnt + 1][iMCnt + 1]*dFilterSobelLeft2Right[8];
	if (dMax < abs(DifPixels.Left2Right.dDiff)) {
		dMax = abs(DifPixels.Left2Right.dDiff);
		DifPixels.indexMax = _Horizontal;
	}
	if (dMin > abs(DifPixels.Left2Right.dDiff)) {
		dMin = abs(DifPixels.Left2Right.dDiff);
		DifPixels.indexMin = _Horizontal;
	}

	//左上→右下方向の微分を計算
	DifPixels.UpLeft2DownRight.dDiff = Image.Dt[iSCnt - 1][iMCnt - 1]*dFilterSobelUpLeft2DownRight[0] +
									   Image.Dt[iSCnt - 1][iMCnt    ]*dFilterSobelUpLeft2DownRight[1] +
									   Image.Dt[iSCnt - 1][iMCnt + 1]*dFilterSobelUpLeft2DownRight[2] +
									   Image.Dt[iSCnt    ][iMCnt - 1]*dFilterSobelUpLeft2DownRight[3] +
									   Image.Dt[iSCnt    ][iMCnt    ]*dFilterSobelUpLeft2DownRight[4] +
									   Image.Dt[iSCnt    ][iMCnt + 1]*dFilterSobelUpLeft2DownRight[5] +
									   Image.Dt[iSCnt + 1][iMCnt - 1]*dFilterSobelUpLeft2DownRight[6] +
									   Image.Dt[iSCnt + 1][iMCnt    ]*dFilterSobelUpLeft2DownRight[7] +
									   Image.Dt[iSCnt + 1][iMCnt + 1]*dFilterSobelUpLeft2DownRight[8];
	if (dMax < abs(DifPixels.UpLeft2DownRight.dDiff)) {
		dMax = abs(DifPixels.UpLeft2DownRight.dDiff);
		DifPixels.indexMax = _RightDown;
	}
	if (dMin > abs(DifPixels.UpLeft2DownRight.dDiff)) {
		dMin = abs(DifPixels.UpLeft2DownRight.dDiff);
		DifPixels.indexMin = _RightDown;
	}

	//右上→左下方向の微分を計算
	DifPixels.UpRight2DownLeft.dDiff = Image.Dt[iSCnt - 1][iMCnt - 1]*dFilterSobelUpRight2DownLeft[0] +
									   Image.Dt[iSCnt - 1][iMCnt    ]*dFilterSobelUpRight2DownLeft[1] +
									   Image.Dt[iSCnt - 1][iMCnt + 1]*dFilterSobelUpRight2DownLeft[2] +
									   Image.Dt[iSCnt    ][iMCnt - 1]*dFilterSobelUpRight2DownLeft[3] +
									   Image.Dt[iSCnt    ][iMCnt    ]*dFilterSobelUpRight2DownLeft[4] +
									   Image.Dt[iSCnt    ][iMCnt + 1]*dFilterSobelUpRight2DownLeft[5] +
									   Image.Dt[iSCnt + 1][iMCnt - 1]*dFilterSobelUpRight2DownLeft[6] +
									   Image.Dt[iSCnt + 1][iMCnt    ]*dFilterSobelUpRight2DownLeft[7] +
									   Image.Dt[iSCnt + 1][iMCnt + 1]*dFilterSobelUpRight2DownLeft[8];
	if (dMax < abs(DifPixels.UpRight2DownLeft.dDiff)) {
		dMax = abs(DifPixels.UpRight2DownLeft.dDiff);
		DifPixels.indexMax = _RightUp;
	}
	if (dMin > abs(DifPixels.UpRight2DownLeft.dDiff)) {
		dMin = abs(DifPixels.UpRight2DownLeft.dDiff);
		DifPixels.indexMin = _RightUp;
	}
/*
	//debug
	ImageUp2Down.SetVal(iSCnt, iMCnt, DifPixels.Up2Down.dDiff + Image.DtMax/2);
	ImageLeft2Right.SetVal(iSCnt, iMCnt, DifPixels.Left2Right.dDiff + Image.DtMax/2);
	ImageUpLeft2DownRight.SetVal(iSCnt, iMCnt, DifPixels.UpLeft2DownRight.dDiff + Image.DtMax/2);
	ImageUpRight2DownLeft.SetVal(iSCnt, iMCnt, DifPixels.UpRight2DownLeft.dDiff + Image.DtMax/2);
*/
	//エネルギーE
	DifPixels.dEnergy = (abs(DifPixels.Up2Down.dDiff) + abs(DifPixels.Left2Right.dDiff) + abs(DifPixels.UpLeft2DownRight.dDiff) + abs(DifPixels.UpRight2DownLeft.dDiff)) / 4;
	//仮想ベクトル
	DifPixels.dVmX = abs(DifPixels.Up2Down.dDiff) - abs(DifPixels.Left2Right.dDiff);
	DifPixels.dVmY = abs(DifPixels.UpLeft2DownRight.dDiff) - abs(DifPixels.UpRight2DownLeft.dDiff);
	DifPixels.dVmNorm = sqrt(DifPixels.dVmX * DifPixels.dVmX + DifPixels.dVmY * DifPixels.dVmY);
	//信頼性（線らしさ）
	if (DifPixels.dEnergy) {
		DifPixels.dConfidence = DifPixels.dVmNorm / DifPixels.dEnergy;
	}
	CalcEdgeOrientation(DifPixels);//仮想ベクトルVmから実画像上のエッジ方向を計算


	return DifPixels;
}
//---------------------------------------------------------------------------
//ある点の周囲の微分情報を計算する（bitシフトで高速化する代わりに小数でなく、整数として扱う）

DIFFERENTIALARROUNDPIXELS TSIMARS::CalcDifferentialArroundPixel_Fast(TTMA& Image,  int iSCnt, int iMCnt)
{
#if 1
	if (2669 ==iMCnt && 144 == iSCnt) {
		int xx = 0;
	}

#endif

	DIFFERENTIALARROUNDPIXELS DifPixels;
	double dMax(0.), dMin(100000.);
	int iFilterSobelUp2Down[9] = {679, 31411, 679, 0, 0, 0, -679, -31411, -679};
	int iFilterSobelLeft2Right[9] = {-679, 0, 679, -31411, 0, 31411, -679, 0, 679};
	int iFilterSobelUpLeft2DownRight[9] = {-1357, -15705, 0, -15705, 0, 15705, 0, 15705, 1357};
	int iFilterSobelUpRight2DownLeft[9] = {0, 15705, 1357, -15705, 0, 15705, -1357, -15705, 0};
	long lU2D(0), lL2R(0), lUL2DR(0), lUR2DL(0), lTmp;
//	double d;

	//上下方向の微分を計算
	lU2D = 					   Image.Dt[iSCnt - 1][iMCnt - 1]*iFilterSobelUp2Down[0] +
							   Image.Dt[iSCnt - 1][iMCnt    ]*iFilterSobelUp2Down[1] +
							   Image.Dt[iSCnt - 1][iMCnt + 1]*iFilterSobelUp2Down[2] +
							   Image.Dt[iSCnt    ][iMCnt - 1]*iFilterSobelUp2Down[3] +
							   Image.Dt[iSCnt    ][iMCnt    ]*iFilterSobelUp2Down[4] +
							   Image.Dt[iSCnt    ][iMCnt + 1]*iFilterSobelUp2Down[5] +
							   Image.Dt[iSCnt + 1][iMCnt - 1]*iFilterSobelUp2Down[6] +
							   Image.Dt[iSCnt + 1][iMCnt    ]*iFilterSobelUp2Down[7] +
							   Image.Dt[iSCnt + 1][iMCnt + 1]*iFilterSobelUp2Down[8];
#if 0
	DifPixels.Up2Down.dDiff = (double)(lU2D >> 15);//15bitシフト
#else//石丸さん指摘により修正(ttsuji 141121)
	if (lU2D < 0) {
		lTmp = -lU2D;
		DifPixels.Up2Down.dDiff = -(double)(lTmp >> 15);//15bitシフト
//		d = (double)(lU2D >> 15);//tmp
	}
	else{
		DifPixels.Up2Down.dDiff = (double)(lU2D >> 15);//15bitシフト
	}
#endif
	if (dMax < abs(DifPixels.Up2Down.dDiff)) {
		dMax = abs(DifPixels.Up2Down.dDiff);
		DifPixels.indexMax = _Vertical;
	}
	if (dMin > abs(DifPixels.Up2Down.dDiff)) {
		dMin = abs(DifPixels.Up2Down.dDiff);
		DifPixels.indexMin = _Vertical;
	}

	//左右方向の微分を計算
	lL2R = 					     Image.Dt[iSCnt - 1][iMCnt - 1]*iFilterSobelLeft2Right[0] +
								 Image.Dt[iSCnt - 1][iMCnt    ]*iFilterSobelLeft2Right[1] +
								 Image.Dt[iSCnt - 1][iMCnt + 1]*iFilterSobelLeft2Right[2] +
								 Image.Dt[iSCnt    ][iMCnt - 1]*iFilterSobelLeft2Right[3] +
								 Image.Dt[iSCnt    ][iMCnt    ]*iFilterSobelLeft2Right[4] +
								 Image.Dt[iSCnt    ][iMCnt + 1]*iFilterSobelLeft2Right[5] +
								 Image.Dt[iSCnt + 1][iMCnt - 1]*iFilterSobelLeft2Right[6] +
								 Image.Dt[iSCnt + 1][iMCnt    ]*iFilterSobelLeft2Right[7] +
								 Image.Dt[iSCnt + 1][iMCnt + 1]*iFilterSobelLeft2Right[8];
#if 0
	DifPixels.Left2Right.dDiff = (double)(lL2R >> 15);//15bitシフト
#else//石丸さん指摘により修正(ttsuji 141121)
	if (lL2R < 0) {
		lTmp = -lL2R;
		DifPixels.Left2Right.dDiff = -(double)(lTmp >> 15);//15bitシフト
	}
	else{
		DifPixels.Left2Right.dDiff = (double)(lL2R >> 15);//15bitシフト
	}
#endif
	if (dMax < abs(DifPixels.Left2Right.dDiff)) {
		dMax = abs(DifPixels.Left2Right.dDiff);
		DifPixels.indexMax = _Horizontal;
	}
	if (dMin > abs(DifPixels.Left2Right.dDiff)) {
		dMin = abs(DifPixels.Left2Right.dDiff);
		DifPixels.indexMin = _Horizontal;
	}

	//左上→右下方向の微分を計算
	lUL2DR =				   		   Image.Dt[iSCnt - 1][iMCnt - 1]*iFilterSobelUpLeft2DownRight[0] +
									   Image.Dt[iSCnt - 1][iMCnt    ]*iFilterSobelUpLeft2DownRight[1] +
									   Image.Dt[iSCnt - 1][iMCnt + 1]*iFilterSobelUpLeft2DownRight[2] +
									   Image.Dt[iSCnt    ][iMCnt - 1]*iFilterSobelUpLeft2DownRight[3] +
									   Image.Dt[iSCnt    ][iMCnt    ]*iFilterSobelUpLeft2DownRight[4] +
									   Image.Dt[iSCnt    ][iMCnt + 1]*iFilterSobelUpLeft2DownRight[5] +
									   Image.Dt[iSCnt + 1][iMCnt - 1]*iFilterSobelUpLeft2DownRight[6] +
									   Image.Dt[iSCnt + 1][iMCnt    ]*iFilterSobelUpLeft2DownRight[7] +
									   Image.Dt[iSCnt + 1][iMCnt + 1]*iFilterSobelUpLeft2DownRight[8];
#if 0
	DifPixels.UpLeft2DownRight.dDiff = (double)(lUL2DR >> 15);//15bitシフト
#else//石丸さん指摘により修正(ttsuji 141121)
	if (lUL2DR < 0) {
		lTmp = -lUL2DR;
		DifPixels.UpLeft2DownRight.dDiff = -(double)(lTmp >> 15);//15bitシフト
	}
	else{
		DifPixels.UpLeft2DownRight.dDiff = (double)(lUL2DR >> 15);//15bitシフト
	}
#endif
	if (dMax < abs(DifPixels.UpLeft2DownRight.dDiff)) {
		dMax = abs(DifPixels.UpLeft2DownRight.dDiff);
		DifPixels.indexMax = _RightDown;
	}
	if (dMin > abs(DifPixels.UpLeft2DownRight.dDiff)) {
		dMin = abs(DifPixels.UpLeft2DownRight.dDiff);
		DifPixels.indexMin = _RightDown;
	}

	//右上→左下方向の微分を計算
	lUR2DL =				           Image.Dt[iSCnt - 1][iMCnt - 1]*iFilterSobelUpRight2DownLeft[0] +
									   Image.Dt[iSCnt - 1][iMCnt    ]*iFilterSobelUpRight2DownLeft[1] +
									   Image.Dt[iSCnt - 1][iMCnt + 1]*iFilterSobelUpRight2DownLeft[2] +
									   Image.Dt[iSCnt    ][iMCnt - 1]*iFilterSobelUpRight2DownLeft[3] +
									   Image.Dt[iSCnt    ][iMCnt    ]*iFilterSobelUpRight2DownLeft[4] +
									   Image.Dt[iSCnt    ][iMCnt + 1]*iFilterSobelUpRight2DownLeft[5] +
									   Image.Dt[iSCnt + 1][iMCnt - 1]*iFilterSobelUpRight2DownLeft[6] +
									   Image.Dt[iSCnt + 1][iMCnt    ]*iFilterSobelUpRight2DownLeft[7] +
									   Image.Dt[iSCnt + 1][iMCnt + 1]*iFilterSobelUpRight2DownLeft[8];
#if 0
	DifPixels.UpRight2DownLeft.dDiff = (double)(lUR2DL >> 15);//15bitシフト
#else//石丸さん指摘により修正(ttsuji 141121)
	if (lUR2DL < 0) {
		lTmp = -lUR2DL;
		DifPixels.UpRight2DownLeft.dDiff = -(double)(lTmp >> 15);//15bitシフト
	}
	else{
		DifPixels.UpRight2DownLeft.dDiff = (double)(lUR2DL >> 15);//15bitシフト
	}
#endif
	if (dMax < abs(DifPixels.UpRight2DownLeft.dDiff)) {
		dMax = abs(DifPixels.UpRight2DownLeft.dDiff);
		DifPixels.indexMax = _RightUp;
	}
	if (dMin > abs(DifPixels.UpRight2DownLeft.dDiff)) {
		dMin = abs(DifPixels.UpRight2DownLeft.dDiff);
		DifPixels.indexMin = _RightUp;
	}

	//エネルギーE
	DifPixels.dEnergy = (abs(DifPixels.Up2Down.dDiff) + abs(DifPixels.Left2Right.dDiff) + abs(DifPixels.UpLeft2DownRight.dDiff) + abs(DifPixels.UpRight2DownLeft.dDiff)) / 4;
	//仮想ベクトル
	DifPixels.dVmX = abs(DifPixels.Up2Down.dDiff) - abs(DifPixels.Left2Right.dDiff);
	DifPixels.dVmY = abs(DifPixels.UpLeft2DownRight.dDiff) - abs(DifPixels.UpRight2DownLeft.dDiff);
	DifPixels.dVmNorm = sqrt(DifPixels.dVmX * DifPixels.dVmX + DifPixels.dVmY * DifPixels.dVmY);
	//信頼性（線らしさ）
	if (DifPixels.dEnergy) {
		DifPixels.dConfidence = DifPixels.dVmNorm / DifPixels.dEnergy;
	}
	CalcEdgeOrientation(DifPixels);//仮想ベクトルVmから実画像上のエッジ方向を計算


	return DifPixels;
}
//---------------------------------------------------------------------------

void TSIMARS::CalcEdgeOrientation(DIFFERENTIALARROUNDPIXELS& DifPixels)
{
	if (0 == DifPixels.dVmX) {
		if (DifPixels.dVmY > 0) {
			DifPixels.dOrientation = 135;
		}
		else if (DifPixels.dVmY < 0) {
			DifPixels.dOrientation = 45;
		}
        else{
			DifPixels.dOrientation = 0;
		}
	}
	else{
		double dTheta;
		//仮想ベクトル空間の第1象限（実画像空間で0度と135度の範囲）
		if (DifPixels.dVmX >= 0 && DifPixels.dVmY >= 0) {
			dTheta = atan(fabs(DifPixels.dVmY / DifPixels.dVmX)) * 180 / PI;
#if 0
			DifPixels.dOrientation = 180 - dTheta / 2;
#else
			DifPixels.dOrientation = round(180 - dTheta / 2);//MCにあわせる 140902 ttsuji
#endif
		}
		//仮想ベクトル空間の第2象限（実画像空間で90度と135度の範囲）
		else if (DifPixels.dVmX < 0 && DifPixels.dVmY >= 0) {
			dTheta = atan(fabs(DifPixels.dVmY / DifPixels.dVmX)) * 180 / PI;
#if 0
			DifPixels.dOrientation = 90 + dTheta / 2;
#else
			DifPixels.dOrientation = round(90 + dTheta / 2);//MCにあわせる 140902 ttsuji
#endif
		}
		//仮想ベクトル空間の第3象限（実画像空間で90度と45度の範囲）
		else if (DifPixels.dVmX < 0 && DifPixels.dVmY < 0) {
			dTheta = atan(fabs(DifPixels.dVmY / DifPixels.dVmX)) * 180 / PI;
#if 0
			DifPixels.dOrientation = 90 - dTheta / 2;
#else
			DifPixels.dOrientation = round(90 - dTheta / 2);//MCにあわせる 140902 ttsuji
#endif
		}
		//仮想ベクトル空間の第4象限（実画像空間で0度と45度の範囲）
		else if (DifPixels.dVmX >= 0 && DifPixels.dVmY < 0) {
			dTheta = atan(fabs(DifPixels.dVmY / DifPixels.dVmX)) * 180 / PI;
#if 0
			DifPixels.dOrientation = dTheta / 2;
#else
			DifPixels.dOrientation = round(dTheta / 2);//MCにあわせる 140902 ttsuji
#endif
		}
	}
}
//---------------------------------------------------------------------------
//斜め方向のDC成分補間(3点補間)

#define _SINTIDANSA_KAIDAN_TAIOH //シンチ段差が水平でなく、階段状になっていることへの対応(141002 ttsuji)

//void TSIMARS::LPDCItplNanameSankakuHokan(TTMA& ItplImg, TTMA& CorrImg, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa)
void TSIMARS::LPDCItplNanameSankakuHokan(TTMA& ItplImg, TTMA& CorrImg, TTMA& CorrImgDebug, int* edgeY, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, int iSubRangeOnlyMidPanel, int iItplSubOffset)
{
	if (iMsample1 == iMsample2) {
		return;//斜めエッジでないため処理しない
	}

	int iTmp;
	if (iSsample1 > iSsample2) {//iSsample1 < iSsample2 となるようにソートする
		iTmp = iSsample1;
		iSsample1 = iSsample2;
		iSsample2 = iTmp;
	}
	if (iMsample1 > iMsample2) {//iSsample1 < iSsample2 となるようにソートする
		iTmp = iMsample1;
		iMsample1 = iMsample2;
		iMsample2 = iTmp;
	}

	if (iMsample1 == 0) {
	 int a = 0;
	}

	double* dYBorder = new double[iMsample2 - iMsample1 + 1];
	for (int iM = iMsample1; iM <= iMsample2; iM++){
		dYBorder[iM - iMsample1] = dTan * (iM - dMGrpAve) + dSAtGrpAve;
	}


	double dTanCurrent = 0.;
	double dQLitpl, dMVirtualPixel, dQLVirtualPixel, dDistanceVirtualNeighbor;
	int iMNeighbor;
#ifndef _SINTIDANSA_KAIDAN_TAIOH
	for (int iS = iSsample1 + 1; iS < iSsample2; iS++){//iS=iSsample1とiSsample2とを用いて間を補間
 #if 0
		for (int iM = iMsample1; iM <= iMsample2; iM++){
 #else 	//石丸さん指摘により端の斜め補間はしないように修正（ttsuji 140924）
		for (int iM = iMsample1 + 1; iM < iMsample2; iM++){
 #endif
#else	//試しにM,Sのループ順を変えてみた。目的はSの値が三角補間の最中に変わる不具合対策のため。→順の影響なし
	int iS1AtiMsample1 = edgeY[iMsample1] - iSubRangeOnlyMidPanel;
	int iS2AtiMsample1 = edgeY[iMsample1] + iItplSubOffset + 1;
	int iS1AtiMsample2 = edgeY[iMsample2] - iSubRangeOnlyMidPanel;
	int iS2AtiMsample2 = edgeY[iMsample2] + iItplSubOffset + 1;
	for (int iM = iMsample1 + 1; iM < iMsample2; iM++){

		iSsample1 = edgeY[iM] - iSubRangeOnlyMidPanel;//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
		iSsample2 = edgeY[iM] + iItplSubOffset + 1;
		for (int iS = iSsample1 + 1; iS < iSsample2; iS++){//iS=iSsample1とiSsample2とを用いて間を補間
#endif

			//temp
			if (iS == 117) {
				int c = 0;
			}

			if (iMsample1 == iM || iMsample2 == iM) {//通常の斜めでないDC補間
				dQLitpl = ((iSsample2 - iS) * Dt[iSsample1][iM] + (iS -iSsample1) * Dt[iSsample2][iM]) / (iSsample2 - iSsample1);//線形補間
			}
			else{
				if (dTan > 0.) {//θ=0～90°の場合
					//斜めエッジを越えているか否かで場合分け
					if (iS > dYBorder[iM - iMsample1]) {//斜めエッジよりもiSsample2側の画素のときはiSsample2側の3点目の画素で補間
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dTanCurrent = (double)(iS - iSsample1) / (iM - iMsample1);//+
						dMVirtualPixel = iMsample1 + (iSsample2 - iSsample1) / dTanCurrent;
#else
						dTanCurrent = (double)(iS - iS1AtiMsample1) / (iM - iMsample1);//+
						dMVirtualPixel = iMsample1 + (iS2AtiMsample1 - iS1AtiMsample1) / dTanCurrent;
#endif
//						iMNeighbor = (int)dMVirtualPixel;//切捨て
						iMNeighbor = CLIP((int)dMVirtualPixel, 0, MainPixel - 2);//切捨て後、clipping
						dDistanceVirtualNeighbor = dMVirtualPixel - iMNeighbor;
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[iSsample2][iMNeighbor] + dDistanceVirtualNeighbor * Dt[iSsample2][iMNeighbor + 1];//線形補間
#else
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[edgeY[iMNeighbor] + iItplSubOffset + 1][iMNeighbor] + dDistanceVirtualNeighbor * Dt[edgeY[iMNeighbor + 1] + iItplSubOffset + 1][iMNeighbor + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						if (dMVirtualPixel == iMsample1) {//通常の斜めでないDC補間
							dQLitpl = ((iSsample2 - iS) * Dt[iSsample1][iM] + (iS -iSsample1) * Dt[iSsample2][iM]) / (iSsample2 - iSsample1);//線形補間
						}
						else{
#ifndef _SINTIDANSA_KAIDAN_TAIOH
							dQLitpl = ((dMVirtualPixel - iM) * Dt[iSsample1][iMsample1] + (iM - iMsample1) * dQLVirtualPixel) / (dMVirtualPixel - iMsample1);//線形補間
#else
							dQLitpl = ((dMVirtualPixel - iM) * Dt[edgeY[iMsample1] - iSubRangeOnlyMidPanel][iMsample1] + (iM - iMsample1) * dQLVirtualPixel) / (dMVirtualPixel - iMsample1);//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						}
					}
					else{//斜めエッジよりもiSsample1側の画素のときはiSsample1側の3点目の画素で補間
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dTanCurrent = (double)(iSsample2 - iS) / (iMsample2 - iM);//+
						dMVirtualPixel = iMsample2 - (iSsample2 - iSsample1) / dTanCurrent;
#else
						dTanCurrent = (double)(iS2AtiMsample2 - iS) / (iMsample2 - iM);//+
						dMVirtualPixel = iMsample2 - (iS2AtiMsample2 - iS1AtiMsample2) / dTanCurrent;
#endif
//						iMNeighbor = (int)dMVirtualPixel;//切捨て
						iMNeighbor = CLIP((int)dMVirtualPixel, 0, MainPixel - 2);//切捨て後、clipping
						dDistanceVirtualNeighbor = dMVirtualPixel - iMNeighbor;
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[iSsample1][iMNeighbor] + dDistanceVirtualNeighbor * Dt[iSsample1][iMNeighbor + 1];
#else
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[edgeY[iMNeighbor] - iSubRangeOnlyMidPanel][iMNeighbor] + dDistanceVirtualNeighbor * Dt[edgeY[iMNeighbor + 1] - iSubRangeOnlyMidPanel][iMNeighbor + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						if (dMVirtualPixel == iMsample2) {//通常の斜めでないDC補間
							dQLitpl = ((iSsample2 - iS) * Dt[iSsample1][iM] + (iS -iSsample1) * Dt[iSsample2][iM]) / (iSsample2 - iSsample1);//線形補間
						}
						else{
#ifndef _SINTIDANSA_KAIDAN_TAIOH
							dQLitpl = ((iM - dMVirtualPixel) * Dt[iSsample2][iMsample2] + (iMsample2 - iM) * dQLVirtualPixel) / (iMsample2 - dMVirtualPixel);//線形補間
#else
							dQLitpl = ((iM - dMVirtualPixel) * Dt[edgeY[iMsample2] + iItplSubOffset + 1][iMsample2] + (iMsample2 - iM) * dQLVirtualPixel) / (iMsample2 - dMVirtualPixel);//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						}
					}
				}
				else{//θ=90～180°の場合
					//斜めエッジを越えているか否かで場合分け
					if (iS > dYBorder[iM - iMsample1]) {//斜めエッジよりもiSsample2側の画素のときはiSsample2側の3点目の画素で補間
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dTanCurrent = (double)(iS - iSsample1) / (iMsample2 - iM);//+
						dMVirtualPixel = iMsample2 - (iSsample2 - iSsample1) / dTanCurrent;
#else
						dTanCurrent = (double)(iS - iS1AtiMsample2) / (iMsample2 - iM);//+
						dMVirtualPixel = iMsample2 - (iS2AtiMsample2 - iS1AtiMsample2) / dTanCurrent;
#endif
//						iMNeighbor = (int)dMVirtualPixel;//切捨て
						iMNeighbor = CLIP((int)dMVirtualPixel, 0, MainPixel - 2);//切捨て後、clipping
						dDistanceVirtualNeighbor = dMVirtualPixel - iMNeighbor;
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[iSsample2][iMNeighbor] + dDistanceVirtualNeighbor * Dt[iSsample2][iMNeighbor + 1];//線形補間
#else
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[edgeY[iMNeighbor] + iItplSubOffset + 1][iMNeighbor] + dDistanceVirtualNeighbor * Dt[edgeY[iMNeighbor + 1] + iItplSubOffset + 1][iMNeighbor + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						if (dMVirtualPixel == iMsample2) {//通常の斜めでないDC補間
							dQLitpl = ((iSsample2 - iS) * Dt[iSsample1][iM] + (iS -iSsample1) * Dt[iSsample2][iM]) / (iSsample2 - iSsample1);//線形補間
						}
						else{
#ifndef _SINTIDANSA_KAIDAN_TAIOH
							dQLitpl = ((iM - dMVirtualPixel) * Dt[iSsample1][iMsample2] + (iMsample2 - iM) * dQLVirtualPixel) / (iMsample2 - dMVirtualPixel);//線形補間
#else
							dQLitpl = ((iM - dMVirtualPixel) * Dt[edgeY[iMsample2] - iSubRangeOnlyMidPanel][iMsample2] + (iMsample2 - iM) * dQLVirtualPixel) / (iMsample2 - dMVirtualPixel);//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						}
					}
					else{//斜めエッジよりもiSsample1側の画素のときはiSsample1側の3点目の画素で補間
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dTanCurrent = (double)(iSsample2 - iS) / (iM - iMsample1);//+
						dMVirtualPixel = iMsample1 + (iSsample2 - iSsample1) / dTanCurrent;
#else
						dTanCurrent = (double)(iS2AtiMsample1 - iS) / (iM - iMsample1);//+
						dMVirtualPixel = iMsample1 + (iS2AtiMsample1 - iS1AtiMsample1) / dTanCurrent;
#endif
//						iMNeighbor = (int)dMVirtualPixel;//切捨て
						iMNeighbor = CLIP((int)dMVirtualPixel, 0, MainPixel - 2);//切捨て後、clipping
						dDistanceVirtualNeighbor = dMVirtualPixel - iMNeighbor;
#ifndef _SINTIDANSA_KAIDAN_TAIOH
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[iSsample1][iMNeighbor] + dDistanceVirtualNeighbor * Dt[iSsample1][iMNeighbor + 1];
#else
						dQLVirtualPixel = (1 - dDistanceVirtualNeighbor) * Dt[edgeY[iMNeighbor] - iSubRangeOnlyMidPanel][iMNeighbor] + dDistanceVirtualNeighbor * Dt[edgeY[iMNeighbor + 1] - iSubRangeOnlyMidPanel][iMNeighbor + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						if (dMVirtualPixel == iMsample1) {//通常の斜めでないDC補間
							dQLitpl = ((iSsample2 - iS) * Dt[iSsample1][iM] + (iS -iSsample1) * Dt[iSsample2][iM]) / (iSsample2 - iSsample1);//線形補間
						}
						else{
#ifndef _SINTIDANSA_KAIDAN_TAIOH
							dQLitpl = ((dMVirtualPixel - iM) * Dt[iSsample2][iMsample1] + (iM - iMsample1) * dQLVirtualPixel) / (dMVirtualPixel - iMsample1);//線形補間
#else
							dQLitpl = ((dMVirtualPixel - iM) * Dt[edgeY[iMsample1] + iItplSubOffset + 1][iMsample1] + (iM - iMsample1) * dQLVirtualPixel) / (dMVirtualPixel - iMsample1);//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
#endif
						}
					}
				}

			}

			ItplImg.SetVal(iS, iM, dQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - dQLitpl + iKasa);
			CorrImgDebug.SetVal(iS, iM, CorrImg.Dt[iS][iM]);
		}
//		fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
	}

	delete[] dYBorder;

}
//---------------------------------------------------------------------------
//斜め方向のDC成分補間（平行四辺形補間）

void TSIMARS::LPDCItplNanameHeikohShihenkeiHokan(TTMA& ItplImg, TTMA& CorrImg, TTMA& CorrImgDebug, int* edgeY, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, int iSubRangeOnlyMidPanel, int iItplSubOffset)
{
	if (iMsample1 == iMsample2) {
		return;//斜めエッジでないため処理しない
	}

	int iTmp;
	if (iSsample1 > iSsample2) {//iSsample1 < iSsample2 となるようにソートする
		iTmp = iSsample1;
		iSsample1 = iSsample2;
		iSsample2 = iTmp;
	}
	if (iMsample1 > iMsample2) {//iMsample1 < iMsample2 となるようにソートする
		iTmp = iMsample1;
		iMsample1 = iMsample2;
		iMsample2 = iTmp;
	}

	//tanθの正負で平行四辺形補間するｘ領域が異なる（平行四辺形の頂点のｘ座標）
	int iMStartAtiS1, iMEndAtiS1, iMStartAtiS2, iMEndAtiS2;
	int iMNeighborAtiS2, iMNeighborAtiS1;//隣接画素のiM座標
	double dMAtiS1, dMAtiS2, dMShiftAtiS1, dMShiftAtiS2, dDistance1VirtualNeighbor, dDistance2VirtualNeighbor, dQLVirtualPixelAtiS1, dQLVirtualPixelAtiS2, dQLitpl;
	int iM1M2 = iMsample2 - iMsample1;
	if (dTan > 0) {
		iMStartAtiS1 = CLIP(iMsample1 - iM1M2, 0, MainPixel - 1);//頂点のｘ座標が画像の左右端を越えた場合は、クリップして画像の左右端を平行四辺形の頂点とする。
		iMEndAtiS1 = iMsample2;
		iMStartAtiS2 = iMsample1;
		iMEndAtiS2 = CLIP(iMsample2 + iM1M2, 0, MainPixel - 1);
	}
	else{
		iMStartAtiS1 = iMsample1;
		iMEndAtiS1 = CLIP(iMsample2 + iM1M2, 0, MainPixel - 1);
		iMStartAtiS2 = CLIP(iMsample1 - iM1M2, 0, MainPixel - 1);
		iMEndAtiS2 = iMsample2;
	}

	//平行四辺形の頂点のｙ座標
	int iS1AtiMStart = edgeY[iMStartAtiS1] - iSubRangeOnlyMidPanel;
	int iS2AtiMStart = edgeY[iMStartAtiS2] + iItplSubOffset + 1;
	int iS1AtiMEnd   = edgeY[iMEndAtiS1] - iSubRangeOnlyMidPanel;
	int iS2AtiMEnd   = edgeY[iMEndAtiS2] + iItplSubOffset + 1;
    int iS1AtiM, iS2AtiM;


//	for (int iM = iMsample1 + 1; iM < iMsample2; iM++){
	for (int iM = iMsample1 - iM1M2 + 1; iM < iMsample2 + iM1M2; iM++){
		iS1AtiM = edgeY[iM] - iSubRangeOnlyMidPanel;//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
		iS2AtiM = edgeY[iM] + iItplSubOffset + 1;
//		for (int iS = iSsample1 + 1; iS < iSsample2; iS++){//iS=iSsample1とiSsample2とを用いて間を補間
		for (int iS = iS1AtiM + 1; iS < iS2AtiM; iS++){//iS=iS1AtiMとiS2AtiMとを用いて間を補間

			//斜辺のｙ座標を計算し、平行四辺形領域の内外判定
			double dYBorderEnd   = dTan * (iM - iMEndAtiS1) + iS1AtiMEnd;//iMEnd側の斜辺のｙ座標
			double dYBorderStart = dTan * (iM - iMStartAtiS1) + iS1AtiMStart;//iMEnd側の斜辺のｙ座標
			if (dTan > 0.) {
//				if (iS < dYBorderEnd || iS > dYBorderStart) {
				if (iS <= dYBorderEnd || iS >= dYBorderStart) {//141118　石丸さん指摘により修正（平行四辺形の頂点iS1AtiMStart、iS1AtiMEnd、。。は補間しない）
					continue;//平行四辺形の外側は計算しない
				}
			}
			else{
//				if (iS > dYBorderEnd || iS < dYBorderStart) {
				if (iS >= dYBorderEnd || iS <= dYBorderStart) {//141118　石丸さん指摘により修正（平行四辺形の頂点iS1AtiMStart、iS1AtiMEnd、。。は補間しない）
					continue;//平行四辺形の外側は計算しない
				}
			}

			dMShiftAtiS1 = (iS - iS1AtiM) / dTan;
			dMShiftAtiS2 = (iS2AtiM - iS) / dTan;
			dMAtiS1 = CLIP(iM - dMShiftAtiS1, 0, MainPixel - 1);
			dMAtiS2 = CLIP(iM + dMShiftAtiS2, 0, MainPixel - 1);

			iMNeighborAtiS1 = CLIP((int)dMAtiS1, 0, MainPixel - 2);//切捨て後、clipping
			iMNeighborAtiS2 = CLIP((int)dMAtiS2, 0, MainPixel - 2);//切捨て後、clipping
			dDistance1VirtualNeighbor = dMAtiS1 - iMNeighborAtiS1;
			dDistance2VirtualNeighbor = dMAtiS2 - iMNeighborAtiS2;
			dQLVirtualPixelAtiS1 = (1 - dDistance1VirtualNeighbor) * Dt[edgeY[iMNeighborAtiS1] - iSubRangeOnlyMidPanel][iMNeighborAtiS1] + dDistance1VirtualNeighbor * Dt[edgeY[iMNeighborAtiS1 + 1] - iSubRangeOnlyMidPanel][iMNeighborAtiS1 + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
			dQLVirtualPixelAtiS2 = (1 - dDistance2VirtualNeighbor) * Dt[edgeY[iMNeighborAtiS2] + iItplSubOffset + 1][iMNeighborAtiS2] + dDistance2VirtualNeighbor * Dt[edgeY[iMNeighborAtiS2 + 1] + iItplSubOffset + 1][iMNeighborAtiS2 + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
			dQLitpl = ((iS2AtiM - iS) * dQLVirtualPixelAtiS1 + (iS - iS1AtiM) * dQLVirtualPixelAtiS2) / (iS2AtiM - iS1AtiM);

			ItplImg.SetVal(iS, iM, dQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - dQLitpl + iKasa);
			CorrImgDebug.SetVal(iS, iM, CorrImg.Dt[iS][iM]);

		}
	}
}
//---------------------------------------------------------------------------

void TSIMARS::LPCorrectDefectArroundSintiEdge2(string FileName, int iDefect3SubRange, int iDefect3MedianMask1, float fDefect3ThQL1, int iDefect3MedianMask2, float fDefect3ThQL2, int iDefect3MedianMask3, float fDefect3ThQL3, bool bLimitBaseQL, int iThBaseQL, bool bUpperEdge)
{
	string StrPre = "";//接頭辞
	double* delta = new double[MainPixel];
	int* edgeY = new int[MainPixel];
	TFIMG ImageRateOfUpToDown;
	LPCalcSintiEdge(delta, edgeY, FileName, bUpperEdge);//シンチ段差の段差量を計算する

	int iYSintiEnd = round((bUpperEdge ? m_SintiEdgeU.ymax : m_SintiEdgeD.ymax) / 10.);//シンチ段差の下端

#if 0
	LPCorrectDefectArroundSintiEdge(FileName, edgeY, iYSintiEnd, iDefect3SubRange, iDefect3MedianMask, fDefect3ThQL);
//	string StrPre2 = "m(" + IntToStr(iDefect3SubRange) + "," + IntToStr(iDefect3MedianMask) + "," + FloatToStrF(fDefect3ThQL, ffFixed, 3, 0) + ",2D)";//2Dメディアン差分の閾値判定のとき
//	string StrPre2 = "m(" + IntToStr(iDefect3SubRange) + "," + IntToStr(iDefect3MedianMask) + "," + FloatToStrF(fDefect3ThQL, ffFixed, 2, 1) + ")";//メディアン比の閾値判定のとき
//	string StrPre2 = "m(" + IntToStr(iDefect3SubRange) + "," + IntToStr(iDefect3MedianMask) + "," + FloatToStrF(fDefect3ThQL, ffFixed, 3, 0) + ",1D)";//1Dメディアン差分の閾値判定のとき
	string StrPre2 = "m(" + IntToStr(iDefect3SubRange) + "," + IntToStr(iDefect3MedianMask) + "," + FloatToStrF(fDefect3ThQL, ffFixed, 2, 1) + ",1D)";//1Dメディアン比の閾値判定のとき
	WriteTma(AddPrefix(FileName, StrPre2 + StrPre));
#else
	//繰り返し処理に変更
	//1Dメディアン比の閾値判定
	LPCorrectDefectArroundSintiEdge(FileName, edgeY, iYSintiEnd, iDefect3SubRange, iDefect3MedianMask1, fDefect3ThQL1, bLimitBaseQL, iThBaseQL);
	string StrPre2 = "m(" + IntToStr(iDefect3MedianMask1) + "," + FloatToStrF(fDefect3ThQL1, ffFixed, 2, 1) + ")";//1Dメディアン比の閾値判定のとき
//	WriteTma(AddPrefix(FileName, StrPre2 + StrPre));

	LPCorrectDefectArroundSintiEdge(FileName, edgeY, iYSintiEnd, iDefect3SubRange, iDefect3MedianMask2, fDefect3ThQL2, bLimitBaseQL, iThBaseQL);
	StrPre2 		   = "m(" + IntToStr(iDefect3MedianMask2) + "," + FloatToStrF(fDefect3ThQL2, ffFixed, 2, 1) + ")" + StrPre2;//1Dメディアン比の閾値判定のとき
//	WriteTma(AddPrefix(FileName, StrPre2 + StrPre));

	LPCorrectDefectArroundSintiEdge(FileName, edgeY, iYSintiEnd, iDefect3SubRange, iDefect3MedianMask3, fDefect3ThQL3, bLimitBaseQL, iThBaseQL);
	StrPre2 		   = "m(" + IntToStr(iDefect3MedianMask3) + "," + FloatToStrF(fDefect3ThQL3, ffFixed, 3, 2) + ")" + StrPre2;//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
//	WriteTma(AddPrefix(FileName, StrPre2 + StrPre));
#endif

	delete[] edgeY;
	delete[] delta;
}
//---------------------------------------------------------------------------
//シンチ段差付近の点欠陥補正残差を補正するため、シンチ段差付近の短冊に対して欠陥補正３を行う。

void TSIMARS::LPCorrectDefectArroundSintiEdge(string FileName, int* edgeY, int iYSintiEnd, int iDefect3SubRange, int iDefect3MedianMask, float fDefect3ThQL, bool bLimitBaseQL, int iThBaseQL)
{
	TSIMARS tempImage(SubPixel, MainPixel, Bit);
	tempImage = *this;
	tempImage.Format(iYSintiEnd + iDefect3SubRange + iDefect3MedianMask, MainPixel, 0, 0);

#if 0
 #if 0  //sub median
	tempImage.SubMedian(iDefect3MedianMask);
 #elif 0
	//欠陥補正3 （2Dメディアン前後の差分の閾値判定）
	tempImage.bDefectCorrection3(iDefect3MedianMask, iDefect3MedianMask, fDefect3ThQL);
 #elif 0
	//欠陥補正3改（メディアン前後の比の閾値判定）
	tempImage.bDefectCorrection3byMedianRatio(iDefect3MedianMask, iDefect3MedianMask, fDefect3ThQL);
 #elif 0
	//欠陥補正3改（副1Dメディアン前後の差分の閾値判定）
	tempImage.bDefectCorrection3by1DMedianDiff(iDefect3MedianMask, fDefect3ThQL);
 #elif 0
	//欠陥補正3改（副1Dメディアン前後の比の閾値判定）
	tempImage.bDefectCorrection3by1DMedianRatio(iDefect3MedianMask, fDefect3ThQL);
 #else
	const int iThBaseQL = 30;//とりあえず
	tempImage.bDefectCorrection3by1DMedianRatioAndBaseQL(iDefect3MedianMask, fDefect3ThQL, iThBaseQL);
 #endif

#else
	//欠陥補正3改（副1Dメディアン前後の比の閾値判定）
	switch (bLimitBaseQL) {
	case false:
		tempImage.bDefectCorrection3by1DMedianRatio(iDefect3MedianMask, fDefect3ThQL);
		break;
	case true:
		tempImage.bDefectCorrection3by1DMedianRatioAndBaseQL(iDefect3MedianMask, fDefect3ThQL, iThBaseQL);//ベースQLに応じてメディアンへの置換を制御することで、低線量部の粒状を維持する
		break;
	}
#endif

	for (int iM = 0; iM < MainPixel; iM++){
		for (int iS = edgeY[iM] - iDefect3SubRange/2; iS <= edgeY[iM] + iDefect3SubRange/2; iS++){
/*
			if (iS >= iYSintiEnd + iDefect3SubRange + iDefect3MedianMask) {
				int a = 0;
			}
*/
			SetVal(iS, iM, tempImage.Dt[iS][iM]);
		}
	}
	int a = 0;

}
//---------------------------------------------------------------------------

void TSIMARS::LPCorrectTogeArroundSintiEdge(TSIMARS& NoMedImage, string FileName, double* pdSintiEdgeU10, double* pdSintiEdgeD10, int iDefect3SubRange, int iDefect3MainMedianMask, float fDefect3ThQL)
{
	TSIMARS MedImage(SubPixel, MainPixel, Bit);
//	MedImage = *this;
	MedImage = NoMedImage;
	MedImage.bDefectCorrection3byMain1DMedianRatio(iDefect3MainMedianMask, fDefect3ThQL);


	//シンチ段差近傍のみ欠陥補正3もどきの画素値に置換する。
	int iM, iS;
	int iHalfSubRange = iDefect3SubRange / 2;
	for (iM = 0; iM < MainPixel; iM++) {
		int iYSintiEdgeU = (int)(pdSintiEdgeU10[iM]/10. + 0.5);
		for (iS = iYSintiEdgeU - iHalfSubRange; iS <= iYSintiEdgeU + iHalfSubRange; iS++) {
			SetVal(iS, iM, MedImage.Dt[iS][iM]);
		}

		int iYSintiEdgeD = SubPixel - 1 - (int)(pdSintiEdgeD10[iM]/10. + 0.5);
		for (iS = iYSintiEdgeD - iHalfSubRange; iS <= iYSintiEdgeD + iHalfSubRange; iS++) {
			SetVal(iS, iM, MedImage.Dt[iS][iM]);
		}
//		fprintf(fp, "%d	%d	%d	%lf	%lf\n",iM, iYSintiEdgeU, iYSintiEdgeD, DetectEdgeImage.m_CalibSintiEdgeU.y10[iM], DetectEdgeImage.m_CalibSintiEdgeD.y10[iM]);

	}
	//debug
	FileName = AddPrefix(FileName, "TogeCorr(m" + IntToStr(iDefect3MainMedianMask) + "," + IntToStr(iDefect3SubRange) + ")");
	WriteTma(FileName);
}
//---------------------------------------------------------------------------
//シンチ段差近傍の短冊画像にログ横傘を行う

#define _LOWDOSE_LINEAR_TABLE
#define _LOG_ONLY_TANZAKUROI


bool TSIMARS::LPTanzakuYokokasa(string FileName, int iYstartSinti, int iYendSinti, int iDeltaYUp, int iDeltaYDown, int iSubMedianTanzaku, int iSubMoveAveTanzaku, int iClipVal, int iComboMainDiv, string YokokasaMainFilter, string YokokasaSubFilter, string YokokasaHighCutFilter, bool bCalcSubLowFrequency, string YokokasaMainFilter2, string YokokasaSubFilter2, string YokokasaHighCutFilter2, bool bBlend, int iYRangeNotBlend, int iShiroobiLen, bool bLogYokokasa, bool bUpperEdge, bool bDebugYokokasa, bool bWriteYokokasaClipVal)
{
	TTMA CopyImg;
	CopyImg = *this;

	//ログ変換パラメータ
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;

	if (!bUpperEdge) {
		CopyImg.UpDown();//上下反転
	}

	int iYUp, iYDown;
	if (iYstartSinti > iYendSinti) {
		iYUp = iYendSinti;
		iYDown = iYstartSinti;
	}
	else{
		iYUp = iYstartSinti;
		iYDown = iYendSinti;
	}

	//閾値マップを規定する矩形のyレンジ指定
//	int iYborder1 = iYUp - iDeltaYUp;
	int iYborder1 = (iYUp - iDeltaYUp >= 0) ? iYUp - iDeltaYUp : 0;
//	int iYborder2   = iYDown + iDeltaYDown;
	int iYborder2   = iYDown + iDeltaYDown + 1;


#ifndef _LOG_ONLY_TANZAKUROI
	if (bLogYokokasa) {
 #ifndef _LOWDOSE_LINEAR_TABLE
		CopyImg.LogThreshold(dMidVal, dKeta, iLogTh);
		CopyImg.BitChange(14);
//		StrPre = "BL";
 #else
		CopyImg.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
 #endif

 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		CopyImg.WriteTma(AddPrefix(FileName, StrPre));
  #endif
 #endif
	}
#endif


	//ROIを上下3に分ける
	string StrPre = "";
	//CopyFormat(const int iOrgSub, const int iOrgMain,CTDAT& CopyImg, const int iCopySubPixel, const int iCopyMainPixel,const int iCopyOrgSub, const int iCopyOrgMain)
	TSIMARS Up0Img(iYborder1, CopyImg.MainPixel, CopyImg.Bit);
	TSIMARS Up1Img(iYborder2 - iYborder1, CopyImg.MainPixel, CopyImg.Bit);
	TSIMARS Up2Img(CopyImg.SubPixel - iYborder2, CopyImg.MainPixel, CopyImg.Bit);
	Up0Img.CopyFormat(0, 0, CopyImg, Up0Img.SubPixel, Up0Img.MainPixel, 0, 0);
	Up1Img.CopyFormat(0, 0, CopyImg, Up1Img.SubPixel, Up1Img.MainPixel, iYborder1, 0);
	Up2Img.CopyFormat(0, 0, CopyImg, Up2Img.SubPixel, Up2Img.MainPixel, iYborder2, 0);

#ifdef _LOG_TANZAKUYOKOKASA
 #ifdef _LP_DEBUG_OUTPUT
	//debug
	Up0Img.WriteTma( AddPrefix( FileName, "Up0" + StrPre ));
	Up1Img.WriteTma( AddPrefix( FileName, "Up1" + StrPre ));
	Up2Img.WriteTma( AddPrefix( FileName, "Up2" + StrPre ));
 #endif
#endif


#ifdef _LOG_ONLY_TANZAKUROI
	if (bLogYokokasa) {
 #ifndef _LOWDOSE_LINEAR_TABLE
		Up1Img.LogThreshold(dMidVal, dKeta, iLogTh);
		Up1Img.BitChange(14);
 #else
		Up1Img.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
 #endif

 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		Up1Img.WriteTma(AddPrefix(FileName, "LUp1" + StrPre));
  #endif
 #endif
	}
#endif




	//シンチ段差付近の短冊画像だけ横スジ補正
	TSIMARS YUp1Img;
	YUp1Img = Up1Img;
	TTMA ioUmbImg;
	ioUmbImg = Up1Img;
#if 0
	if (!YUp1Img.bYokoKasa(YokokasaHighCutFilter, YokokasaMainFilter, YokokasaSubFilter, (float)iClipVal, iComboMainDiv, bDebugYokokasa, FileName, false, NULL)){
#elif 1
	if (!bCalcSubLowFrequency) {
		if (!YUp1Img.bYokoKasaIntType(YokokasaHighCutFilter, YokokasaMainFilter, YokokasaSubFilter, (float)iClipVal, iComboMainDiv, true, FileName, false, NULL)){
			CApplication::GetInstance()->MessageBox("横傘でエラーが発生しました", "Error", MB_OK);
			return false;
		}
	}
	else{
		//2種類のフィルター処理を混合(150408 ttsuji)
		if (!YUp1Img.bYokoKasaIntTypeMix2Filter(YokokasaHighCutFilter, YokokasaMainFilter, YokokasaSubFilter, YokokasaHighCutFilter2, YokokasaMainFilter2, YokokasaSubFilter2, (float)iClipVal, iComboMainDiv, true, FileName, false, NULL, ioUmbImg)){
			CApplication::GetInstance()->MessageBox("横傘でエラーが発生しました", "Error", MB_OK);
			return false;
		}
	}
#endif


#ifdef _LOG_TANZAKUYOKOKASA
 #ifdef _LP_DEBUG_OUTPUT
	YUp1Img.WriteTma( AddPrefix( FileName, "yUp1" + StrPre ));
 #endif
#endif

	//3つのROIを合体
	CopyImg.CopyFormat(0, 0, 		 	Up0Img, Up0Img.SubPixel, Up0Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder1, 0, 	YUp1Img, YUp1Img.SubPixel, YUp1Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder2, 0, 	Up2Img, Up2Img.SubPixel, Up2Img.MainPixel, 0, 0);
#ifdef _LOG_TANZAKUYOKOKASA
 #ifdef _LP_DEBUG_OUTPUT
	CopyImg.WriteTma( AddPrefix( FileName, "y" + StrPre ));
 #endif
#endif


	//短冊画像の上下端の不連続性を補正
	if (iSubMedianTanzaku > 1 && iSubMoveAveTanzaku > 1) {//副Median、副移動平均マスクサイズが１より大きい場合
		TFIMG fImgRate;
		fImgRate.NewDt(Up1Img.MainPixel, Up1Img.SubPixel);
		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
 #if 0 //雨画像を再び計算(横傘後画像時にclipingすると正しく雨が求まらずNG)
//				fImgRate.Dt[iM][iS] = (float)Up1Img.Dt[iS][iM] / YUp1Img.Dt[iS][iM];//画像data（Dt[iM][iS]の順）
				fImgRate.Dt[iM][iS] = (float)Up1Img.Dt[iS][iM] - YUp1Img.Dt[iS][iM];//画像data（Dt[iM][iS]の順）
 #else  //MC指摘により修正(ttsuji151015)
				int iMidColor = 1 << (ioUmbImg.Bit - 1);
				fImgRate.Dt[iM][iS] = (float)ioUmbImg.Dt[iS][iM] - iMidColor;
 #endif
			}
		}
 #if 1  //Medianの端処理は可変長
/*
  #if 0
		//副Median前に副移動平均をかけることで、白帯ピークの影響を緩和する　→効果なし　（151004 ttsuji）
//		fImgRate.SubMoveAve(101);//不足
		fImgRate.SubMoveAve(201);//

		TTMA ImgUnSigned;
		ImgUnSigned.NewDt(fImgRate.SubPixel, fImgRate.MainPixel, YUp1Img.Bit);
		ImgUnSigned = (fImgRate + YUp1Img.DtMax/2);
   #ifdef _LOG_TANZAKUYOKOKASA
	#ifdef _LP_DEBUG_OUTPUT
		ImgUnSigned.WriteTma( AddPrefix( FileName, "SubMoveAve101" + StrPre ));
	#endif
   #endif
  #endif
*/

		fImgRate.SubMedian(iSubMedianTanzaku);//端処理は可変長

		TTMA SubMed;
		SubMed.NewDt(fImgRate.SubPixel, fImgRate.MainPixel, YUp1Img.Bit);
		SubMed = (fImgRate + YUp1Img.DtMax/2);
  #ifdef _LOG_TANZAKUYOKOKASA
   #ifdef _LP_DEBUG_OUTPUT
		SubMed.WriteTma( AddPrefix( FileName, "SubMedian" + IntToStr(iSubMedianTanzaku) + StrPre ));
   #endif
  #endif
		//副Median処理後の低周波画像にさらに副方向に移動平均をかけてMedian処理後に残存する局所的な高周波情報をスムージングする(141023 ttsuji)
		fImgRate.SubMoveAve(iSubMoveAveTanzaku);
		SubMed = (fImgRate + YUp1Img.DtMax/2);
  #ifdef _LOG_TANZAKUYOKOKASA
   #ifdef _LP_DEBUG_OUTPUT
		SubMed.WriteTma( AddPrefix( FileName, "SubMoveAve" + IntToStr(iSubMoveAveTanzaku) + "SubMedian" + IntToStr(iSubMedianTanzaku) + StrPre ));
   #endif
  #endif

		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
  #if 0 		//クリッピング処理が入っているためコメントアウト
				YUp1Img.SetVal(iS, iM, fImgRate.Dt[iM][iS] + YUp1Img.Dt[iS][iM]);//画像data（Dt[iM][iS]の順）
  #else			//MC指摘により修正(151021ttsuji)
				int iMidColor = 1 << (ioUmbImg.Bit - 1);
				YUp1Img.SetVal(iS, iM, Up1Img.Dt[iS][iM] - ioUmbImg.Dt[iS][iM] + iMidColor + fImgRate.Dt[iM][iS]);//画像data（Dt[iM][iS]の順）
  #endif
			}
		}

 #else  //Medianの端処理は端画素の画素値の拡張 →効果なし（151004 ttsuji）
		TTMA SubMed;
		SubMed.NewDt(fImgRate.SubPixel, fImgRate.MainPixel, YUp1Img.Bit);
		SubMed = (fImgRate + YUp1Img.DtMax/2);
		SubMed.SubMedianBorderPixelExpand(iSubMedianTanzaku);//端処理は端画素の画素値の拡張
  #ifdef _LOG_TANZAKUYOKOKASA
   #ifdef _LP_DEBUG_OUTPUT
		SubMed.WriteTma( AddPrefix( FileName, "SubMedian" + IntToStr(iSubMedianTanzaku) + StrPre ));
   #endif
  #endif
		SubMed.SubMoveAve(iSubMoveAveTanzaku);

  #ifdef _LOG_TANZAKUYOKOKASA
   #ifdef _LP_DEBUG_OUTPUT
		SubMed.WriteTma( AddPrefix( FileName, "SubMoveAve" + IntToStr(iSubMoveAveTanzaku) + "SubMedian" + IntToStr(iSubMedianTanzaku) + StrPre ));
   #endif
  #endif

		for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
			for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
				YUp1Img.SetVal(iS, iM, SubMed.Dt[iS][iM] + YUp1Img.Dt[iS][iM] - YUp1Img.DtMax/2);
			}
		}

 #endif

 #ifdef _LOG_TANZAKUYOKOKASA
  #ifdef _LP_DEBUG_OUTPUT
		StrPre = "mod_yUp1" + StrPre;
		YUp1Img.WriteTma( AddPrefix( FileName, StrPre ));
  #endif
 #endif
	}


	//短冊横傘前後の画像を副方向にブレンドする
//	if (bBlend) {
	if (bBlend && iShiroobiLen >= 100) {
		LPBlendTwoImagesForTanzakuYokokasa(AddPrefix(FileName, "mod_yUp1" + StrPre), YUp1Img, Up1Img, iYRangeNotBlend);
		StrPre = "Blend_" + StrPre;
	}



#ifdef _LOG_ONLY_TANZAKUROI
	//逆ログ変換
	if (bLogYokokasa) {
 #ifndef _LOWDOSE_LINEAR_TABLE
		YUp1Img.BitChange(16);
		YUp1Img.LogToLinear2(YUp1Img.Bit, dMidVal, dKeta);
 #else
		YUp1Img.Log14ToLin16_LowDoseLinear(YUp1Img.Bit, dMidVal, round(dKeta));
 #endif

  #ifdef _LP_DEBUG_OUTPUT
		YUp1Img.WriteTma(AddPrefix(FileName, "al" + StrPre));
  #endif
#endif

	//3つのROIを合体
	CopyImg.CopyFormat(0, 0, 		 	Up0Img, Up0Img.SubPixel, Up0Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder1, 0, 	YUp1Img, YUp1Img.SubPixel, YUp1Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder2, 0, 	Up2Img, Up2Img.SubPixel, Up2Img.MainPixel, 0, 0);

/*
	//短冊の上下端境界でスムージング ->　オーバーラップする領域がないため不可能と判明 ->　短冊差分画像に副メディアンする方法で対応
	if ((iYborder1 - iSubRangeSmoothing / 2) < 0 || (iYborder2 + iSubRangeSmoothing / 2) > Image.SubPixel - 1) {
		CApplication::GetInstance()->MessageBox("副方向のスムージングの範囲設定でエラーが発生しました", "Error", MB_OK);
		return false;
	}
	for (int iS = iYborder1 - iSubRangeSmoothing / 2; iS <= iYborder1 + iSubRangeSmoothing / 2; iS++) {
		for (int iM = 0; iM < Image.MainPixel; iM++) {
			double dQL = (Up0Img.Dt[Up0Img.SubPixel - 1 - iS][iM] * () + Up1Img.Dt[iS][iM] * ()) / ();
			Image.SetVal(iS, iM, dQL);
		}
	}
	for (int iS = iYborder2 - iSubRangeSmoothing / 2; iS <= iYborder2 + iSubRangeSmoothing / 2; iS++) {
		for (int iM = 0; iM < Image.MainPixel; iM++) {

		}
	}
*/


#ifndef _LOG_ONLY_TANZAKUROI
	//逆ログ変換
	if (bLogYokokasa) {
 #ifndef _LOWDOSE_LINEAR_TABLE
		CopyImg.BitChange(16);
		CopyImg.LogToLinear2(CopyImg.Bit, dMidVal, dKeta);
 #else
		CopyImg.Log14ToLin16_LowDoseLinear(14, dMidVal, round(dKeta));
 #endif
#endif

/*
#if 1
		CopyImg.WriteTma(AddPrefix(FileName, "al" + StrPre));
#else
		if (EdgeDetect->Checked) {
			CopyImg.WriteTma(AddPrefix(FileName, "1al" + StrPre));
		}
		else{
			CopyImg.WriteTma(AddPrefix(FileName, "al" + StrPre));
		}
#endif
*/
	}

	*this = (TSIMARS)CopyImg;

	return true;
}
//---------------------------------------------------------------------------
//短冊横傘前後の画像を副方向にブレンドする (151022 ttsuji)

void TSIMARS::LPBlendTwoImagesForTanzakuYokokasa(string FileName, TTMA& YUp1Img, TTMA& Up1Img, int iYRangeNotBlend)
{
	//長尺合成画像
	const int iNorishiroYLength = YUp1Img.SubPixel;
	double* dBlendTable = new double[iNorishiroYLength];

	//ブレンドテーブルの設定
	string StrPre = "Blend";
	for (int iCnt = 0; iCnt < iYRangeNotBlend; iCnt++) {
		dBlendTable[iCnt] = 1.;
	}
	for (int iCnt = iYRangeNotBlend; iCnt < iNorishiroYLength; iCnt++) {
		dBlendTable[iCnt] = 1 - (double)(iCnt - iYRangeNotBlend) / (iNorishiroYLength - 1 - iYRangeNotBlend);
	}
	StrPre += "(" + IntToStr(iYRangeNotBlend) + "of" + IntToStr(iNorishiroYLength) + ")";


	//長尺合成
	TTMA BlendImage;
	BlendImage.NewDt(Up1Img.SubPixel, Up1Img.MainPixel, Up1Img.Bit);
	//上画像と下画像のブレンド領域
	for (int iSCnt = 0; iSCnt < iNorishiroYLength; iSCnt++){
		for (int iMCnt = 0; iMCnt < Up1Img.MainPixel; iMCnt++){
			double dUp = YUp1Img.Dt[iSCnt][iMCnt] * dBlendTable[iSCnt];
			double dDown = Up1Img.Dt[iSCnt][iMCnt] * (1. - dBlendTable[iSCnt]);
			BlendImage.SetVal(iSCnt, iMCnt, dUp + dDown);
		}
	}
	delete[] dBlendTable;

#ifdef _LP_DEBUG_OUTPUT
	BlendImage.WriteTma(AddPrefix(FileName, StrPre));
#endif

	YUp1Img = BlendImage;
}
//---------------------------------------------------------------------------
//シンチ段差近傍の短冊画像に高周波横傘を行う     20141222 Kuwabara ここから
bool TSIMARS::bLPHighkasa(string FileName, int iYstartSinti, int iYendSinti, int iDeltaYUp, int iDeltaYDown, int iSubMoveAveTanzaku, int iClipVal, int iComboMainDiv, string YokokasaMainFilter, string YokokasaSubFilter, string YokokasaHighCutFilter, bool bLogYokokasa, bool bUpperEdge, bool bDebugYokokasa, bool bWriteYokokasaClipVal)
{
	TTMA CopyImg;
	CopyImg = *this;

	int iYUp, iYDown;
	if (iYstartSinti > iYendSinti) {
		iYUp = iYendSinti;
		iYDown = iYstartSinti;
	}
	else{
		iYUp = iYstartSinti;
		iYDown = iYendSinti;
	}

	//閾値マップを規定する矩形のyレンジ指定
	int iYborder1 = (iYUp - iDeltaYUp >= 0) ? iYUp - iDeltaYUp : 0;
	int iYborder2   = iYDown + iDeltaYDown + 1;

	//ROIを上下3に分ける   Up1Imgが横傘対象
	string StrPre = "";
	TSIMARS Up0Img(iYborder1, CopyImg.MainPixel, CopyImg.Bit);
	TSIMARS Up1Img(iYborder2 - iYborder1, CopyImg.MainPixel, CopyImg.Bit);
	TSIMARS Up2Img(CopyImg.SubPixel - iYborder2, CopyImg.MainPixel, CopyImg.Bit);
	Up0Img.CopyFormat(0, 0, CopyImg, Up0Img.SubPixel, Up0Img.MainPixel, 0, 0);
	Up1Img.CopyFormat(0, 0, CopyImg, Up1Img.SubPixel, Up1Img.MainPixel, iYborder1, 0);
	Up2Img.CopyFormat(0, 0, CopyImg, Up2Img.SubPixel, Up2Img.MainPixel, iYborder2, 0);

	//シンチ段差付近の短冊画像だけ横スジ補正
	TSIMARS YUp1Img;
	YUp1Img = Up1Img;
	if (!YUp1Img.bYokoKasaIntType(YokokasaHighCutFilter, YokokasaMainFilter, YokokasaSubFilter, (float)iClipVal, iComboMainDiv, bDebugYokokasa, FileName, false, NULL)){
		CApplication::GetInstance()->MessageBox("高周波横傘でエラーが発生しました", "Error", MB_OK);
		return false;
	}


	//3つのROIを合体
	CopyImg.CopyFormat(0, 0, 		 	Up0Img, Up0Img.SubPixel, Up0Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder1, 0, 	YUp1Img, YUp1Img.SubPixel, YUp1Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder2, 0, 	Up2Img, Up2Img.SubPixel, Up2Img.MainPixel, 0, 0);


	//短冊画像の上下端の不連続性を補正
	TFIMG fImgRate;
	fImgRate.NewDt(Up1Img.MainPixel, Up1Img.SubPixel);
	for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
		for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
			fImgRate.Dt[iM][iS] = (float)Up1Img.Dt[iS][iM] - YUp1Img.Dt[iS][iM];//画像data（Dt[iM][iS]の順）
		}
	}
//	fImgRate.SubMedian(iSubMedianTanzaku);
	TTMA SubMed;
	SubMed.NewDt(fImgRate.SubPixel, fImgRate.MainPixel, YUp1Img.Bit);
//	SubMed = (fImgRate + YUp1Img.DtMax/2);
//	SubMed.WriteTma( AddPrefix( FileName, "SubMedian" + IntToStr(iSubMedianTanzaku) + StrPre ));

 #if 1 //副Median処理後の低周波画像にさらに副方向に移動平均をかけてMedian処理後に残存する局所的な高周波情報をスムージングする(141023 ttsuji)
//	fImgRate.SubMoveAve(31);
	fImgRate.SubMoveAve(iSubMoveAveTanzaku);
	SubMed = (fImgRate + YUp1Img.DtMax/2);
	SubMed.WriteTma( AddPrefix( FileName, "SubMoveAve" + IntToStr(iSubMoveAveTanzaku) + StrPre ));
 #endif


	for (int iS = 0; iS < fImgRate.SubPixel; iS++) {
		for (int iM = 0; iM < fImgRate.MainPixel; iM++) {
			YUp1Img.SetVal(iS, iM, fImgRate.Dt[iM][iS] + YUp1Img.Dt[iS][iM]);//画像data（Dt[iM][iS]の順）
		}
	}

	//3つのROIを合体
	CopyImg.CopyFormat(0, 0, 		 	Up0Img, Up0Img.SubPixel, Up0Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder1, 0, 	YUp1Img, YUp1Img.SubPixel, YUp1Img.MainPixel, 0, 0);
	CopyImg.CopyFormat(iYborder2, 0, 	Up2Img, Up2Img.SubPixel, Up2Img.MainPixel, 0, 0);


	*this = (TSIMARS)CopyImg;

	return true;
}
// 20141222 Kuwabara ここまで
//---------------------------------------------------------------------------
//ガラス段差補正

void TSIMARS::LPGlassEdgeCor(string FileName, int iMainMedianGlassProfile, bool bUpperEdge)
{
	int* edgeY;
	edgeY = new int[MainPixel];

	switch (bUpperEdge) {
	case true:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeU.y10[iM] / 10.);
		}
		break;
	case false:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeD.y10[iM] / 10.);
		}
		break;
	}


	double* delta;
	delta = new double[MainPixel];
	int iKasa = 4000;
	TTMA CorrImg;
	CorrImg = *this;
//	CorrImg.FillValue(0);
	CorrImg.FillValue(iKasa);

 //	 edgeY[0] = Ystart;
 //	 edgeY[MainPixel-1] = Yend;
	for (int iM = 0; iM < MainPixel; iM++){
		delta[iM] = (Dt[edgeY[iM]-2][iM]+Dt[edgeY[iM]-3][iM]
				  -Dt[edgeY[iM]+2][iM]-Dt[edgeY[iM]+3][iM])/2.;
	}

#if 0
	int isize=17;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//17個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#else
	int isize=14;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//14個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#endif

	for (int iM = 0; iM < MainPixel; iM++){
		for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--){
//			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS]);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS] + iKasa);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCor.tma");
 #endif

	CorrImg.MainMedian(iMainMedianGlassProfile);
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif

 #if 0
	io_img -= CorrImg;
 #else
	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
 #endif


 #ifdef _LP_DEBUG_OUTPUT
//	io_img.WriteTma(ExtractFilePath(FileName) + "GlassCor.tma");
 #endif
//	string StrPre = "g(" + IntToStr(Ystart) + "," + IntToStr(Yend) + "," + IntToStr(iMainMedianGlassProfile) + ")";
//	Image.WriteTma( AddPrefix( FileName, StrPre ));

	delete[] edgeY;
	delete[] delta;

}
//---------------------------------------------------------------------------
//ガラス段差補正

void TSIMARS::LPGlassEdgeCorNanameEdge(string FileName, int iMainMedianGlassProfile, bool bUpperEdge)
{
	//主Medianでノイズを落とす
	const int iMMed = 11;//DC補間長さ20pixの情報は保持するマスクサイズ
	TTMA MedImg = *this;
	MedImg.MainMedian(iMMed);

	//Log変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	TTMA MedLogImg = MedImg;
	MedLogImg.LogThreshold(dMidVal, dKeta, iLogTh);
	MedLogImg.WriteTma( AddPrefix(FileName, "LogMed" + IntToStr(iMMed) + "_") );



	int* edgeY;
	edgeY = new int[MainPixel];
	int edgeYAve;

	switch (bUpperEdge) {
	case true:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeU.y10[iM] / 10.);
		}
		edgeYAve = round((m_GlassEdgeU.ymax + m_GlassEdgeU.ymin) / 2.0 / 10.);
		break;
	case false:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeD.y10[iM] / 10.);
		}
		edgeYAve = round((m_GlassEdgeD.ymax + m_GlassEdgeD.ymin) / 2.0 / 10.);
		break;
	}


	double* delta;
	delta = new double[MainPixel];
	int iKasa = 4000;
	TTMA CorrImg;
	CorrImg = *this;
//	CorrImg.FillValue(0);
	CorrImg.FillValue(iKasa);

 //	 edgeY[0] = Ystart;
 //	 edgeY[MainPixel-1] = Yend;
	for (int iM = 0; iM < MainPixel; iM++){
		delta[iM] = (Dt[edgeY[iM]-2][iM]+Dt[edgeY[iM]-3][iM]
				  -Dt[edgeY[iM]+2][iM]-Dt[edgeY[iM]+3][iM])/2.;
	}

#if 0
	int isize=17;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//17個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#else
	int isize=14;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//14個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#endif

	for (int iM = 0; iM < MainPixel; iM++){
		for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--){
//			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS]);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS] + iKasa);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCor.tma");
 #endif

	CorrImg.MainMedian(iMainMedianGlassProfile);
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif
/*
	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
*/


 #ifdef _LP_DEBUG_OUTPUT
//	io_img.WriteTma(ExtractFilePath(FileName) + "GlassCor.tma");
 #endif
//	string StrPre = "g(" + IntToStr(Ystart) + "," + IntToStr(Yend) + "," + IntToStr(iMainMedianGlassProfile) + ")";
//	Image.WriteTma( AddPrefix( FileName, StrPre ));


 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpUDrate = fopen((ExtractFilePath(FileName) + "GlassCorUDQLRateMainProfile.txt").c_str(), "wt");
	fprintf(fpUDrate, "iM	QLUp	QLDown	QLRate	QLRate2\n");
 #endif

	//DC補間サンプル点のQL比の主プロファイル
	TFIMG fImgRateUD;
	fImgRateUD.NewDt(MainPixel, 1);
	fImgRateUD.Fill(0.);

	//QL比閾値（低周波情報除去画像）
	const float fThQLRatioHighPass = 1.1;//とりあえず
//	const float fThQLRatioHighPass = 1.2;//default
//	const float fThQLRatioHighPass = 1.3;
//	const float fThQLRatioHighPass = 1.4;//やりすぎか

	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
//		int iSstart = edgeY[iM] - isize + 1;
//		int iSend   = edgeY[iM];
		int iSstart = edgeY[iM] - isize + 4;
		int iSend   = edgeY[iM] - 3;

		float fAveU = (float)Dt[iSstart][iM];
		float fAveD = (float)Dt[iSend][iM];
		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		fImgRateUD.Dt[iM][0] = fQLRateUD;
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpUDrate, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
  #endif
	}//end for iM

  #ifdef _LP_DEBUG_OUTPUT
	fclose(fpUDrate);
///	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorrImg2.tma");

	FILE* fpDCItpl2 = fopen((ExtractFilePath(FileName) + "GlassCorQLrateHighPassDCItplMainProfile.txt").c_str(), "wt");
//	fprintf(fpDCItpl2, "iM	QLUp	QLDown	QLRate	QLRate2	flag	iS	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
	fprintf(fpDCItpl2, "iM	QLRate	flag	iS	Energy	Orientation	L2R	U2D	UL2DR	UR2DL\n");
  #endif


	//QL比の主プロファイルを主Medianし、低周波のみを抽出
	const int iMainMedQLRate = 201;
	TFIMG fImgRateUDMMed = fImgRateUD;
	fImgRateUDMMed.MainMedian(iMainMedQLRate);
	fImgRateUD -= fImgRateUDMMed;//低周波情報を除去し、高周波のみを抽出
	fImgRateUD += 1.;//１を加算し、UとDが等しいときを１倍とする
  #ifdef _LP_DEBUG_OUTPUT
	fImgRateUD.bSaveAsText((ExtractFilePath(FileName) + "GlassCorQLRateHighPass.txt").c_str(), true);
  #endif


	//QL比の主プロファイルのHighPass後のQL比から斜めエッジを検出（この方法だと、DC補間の20pix間の濃度変化の影響を受けず、精度の良いエッジ検出が可能）
//	float fThQLRateDU = 1 / fThQLRateUD;
	float fThQLRatioHighPass2 = 1 / fThQLRatioHighPass;
	double dOrientCut = 20.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 30.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 35.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 40.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
	double* dOrientAtEnergyMax = new double[MainPixel];//iSをふったときにEnergy（周辺の微分値）が最大となるときの角度（ただしCut角は除く）
	double* dEnergyMax = new double[MainPixel];//iSをふったときの最大Energy
	int* iSAtEnergyMax = new int[MainPixel];
	for (int iM = 0; iM < MainPixel; iM++){
		dEnergyMax[iM] = 0.;//初期化
		dOrientAtEnergyMax[iM] = 0.;//初期化
		iSAtEnergyMax[iM] = 0;//初期化
	}

	//ガラス段差補正領域のヒストグラムをとり、低線量領域は除外するようにする
    int iMainShiftMax = 30;
	Histogram(edgeYAve - 3, edgeYAve - 3, iMainShiftMax, MainPixel - 1 - iMainShiftMax);
#if 0
	float fThRate = 0.7;//ヒストの全面積の70％（感度比を計算する画素の判定に使用） ←多すぎて素抜けを含んでしまいNG
//	float fThRate = 0.5;//ヒストの全面積の50％（感度比を計算する画素の判定に使用）
//	float fThRate = 0.3;//ヒストの全面積の30％（感度比を計算する画素の判定に使用）
	int iThMinQL = HistRuisekiDosuuPercent(fThRate, true);
#else
	float fThRate;
	int iThMinQL = 0;//とりあえず問題画像用に
//	int iThMinQL = 5000;//とりあえず問題画像用に
#endif


	//主方向にスキャンし、QL比を越える主領域について、DC補間対象の副領域すべての微分値を調査
//	for (int iM = 0; iM < MainPixel; iM++){
	for (int iM = 1; iM < MainPixel - 1; iM++){//石丸さん指摘により、端にエッジがあった場合に実行エラーとなるはずなので、端を除外(140901 ttsuji)
//		int iSstart = edgeY[iM] - isize + 1;
//		int iSend   = edgeY[iM];
		int iSstart = edgeY[iM] - isize + 4;
		int iSend   = edgeY[iM] - 3;
		DIFFERENTIALARROUNDPIXELS* DifPixelsArray = new DIFFERENTIALARROUNDPIXELS[iSend - iSstart + 1];//ある点の微分情報

		bool bflag = false;//斜めエッジ対応フラグ

//		if (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2) {
		if (iThMinQL < Dt[iSend][iM] && (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2)) {//低線量領域は斜め対策から除外するようにする
				bflag = true;
			//DC補間する副領域すべての画素の微分値を調査するが、上下方向のエッジが強い画素は斜めエッジの角度算出から除く.
			double dAveUD = 0.;//上下方向の微分値の平均値
			double dAveSquareUD = 0.;//上下方向の微分値の二乗平均
			double dStdEvpUD = 0.;//上下方向の微分値の標準偏差
			double dParaExclude = 1.0;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか
			for (int iS = iSstart; iS <= iSend; iS++) {
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
				dAveUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff;
				dAveSquareUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff * DifPixelsArray[iS - iSstart].Up2Down.dDiff;
			}
			dAveUD /= iSend - iSstart + 1;//副方向の平均値を計算
			dAveSquareUD /= iSend - iSstart + 1;//副方向の二乗平均を計算
			dStdEvpUD = sqrt(dAveSquareUD - dAveUD * dAveUD);//副方向の標準偏差を計算

			//上下方向の微分値が、平均±(σ*dParaExclude)の範囲内のときの斜めエッジを統計処理
			double dUDUpperLimit = dAveUD + dParaExclude * dStdEvpUD;//上下微分の上限値
			double dUDLowerLimit = dAveUD - dParaExclude * dStdEvpUD;//上下微分の下限値
			double dSAve = 0.;
			double dOrientAve = 0.;
        	double dSumEnergy = 0.;
			for (int iS = iSstart; iS <= iSend; iS++) {
				bool bFlag2 = false;
//				if (dUDLowerLimit < DifPixelsArray[iS - iSstart].Up2Down.dDiff && DifPixelsArray[iS - iSstart].Up2Down.dDiff < dUDUpperLimit && dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
				if (dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
					bFlag2 = true;
					dSAve += iS * DifPixelsArray[iS - iSstart].dEnergy;
					dOrientAve += DifPixelsArray[iS - iSstart].dOrientation * DifPixelsArray[iS - iSstart].dEnergy;
					dSumEnergy += DifPixelsArray[iS - iSstart].dEnergy;

					if (dEnergyMax[iM] < DifPixelsArray[iS - iSstart].dEnergy) {
						dEnergyMax[iM] = DifPixelsArray[iS - iSstart].dEnergy;//dEnergyMaxを更新
					}
				}
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixelsArray[iS - iSstart].dEnergy, DifPixelsArray[iS - iSstart].dOrientation, DifPixelsArray[iS - iSstart].Left2Right, DifPixelsArray[iS - iSstart].Up2Down, DifPixelsArray[iS - iSstart].UpLeft2DownRight, DifPixelsArray[iS - iSstart].UpRight2DownLeft, bFlag2);
   #endif
			}
			if (!dSumEnergy) {
				continue;//上下微分が平均±(σ*dParaExclude)の範囲内で、かつdOrientCut度～(180-dOrientCut)度の範囲内のものがなければエッジなしと判断
			}
			dSAve /= dSumEnergy;
			dOrientAve /= dSumEnergy;
			iSAtEnergyMax[iM] = round(dSAve);
			dOrientAtEnergyMax[iM] = dOrientAve;

   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, iSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
   #endif
		}
		else{
			bflag = false;
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag);
   #endif
		}

		delete[] DifPixelsArray;
	}

  #ifdef _LP_DEBUG_OUTPUT
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl2);
  #endif

	//QL比を越える主領域について、グルーピングしてグループごとのエッジ角度を決めて、斜め方向のDC成分補間を行う
	int iMGrpStart = 0;
	int iMGrpEnd = 0;
	double dMGrpAve = 0.;
	double dOrientGrpAve = 0.;
	double dEnergyGrpMax = 0.;
	double dEnergyGrpAve = 0.;
	double dSAtGrpAve = 0.;
	double dCutRate = 0.3;//EnergyがMaxのdCutRate倍以下は統計から除外する
  #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl3 = fopen((ExtractFilePath(FileName) + "GlassCorEdgePositonInformation.txt").c_str(), "wt");
	fprintf(fpDCItpl3, "iThMinQL	fThRate\n");
	fprintf(fpDCItpl3, "%d	%f\n", iThMinQL, fThRate);
	fprintf(fpDCItpl3, "iMGrpStart	iMGrpEnd	dMGrpAve	dOrientGrpAve	dEnergyGrpAve	dMItpl1	dMItpl2\n");
  #endif
	for (int iM = 0; iM < MainPixel; iM++){
		if (!iSAtEnergyMax[iM]) {
			continue;
		}

		iMGrpStart = iM;
		dMGrpAve = 0.;
		dOrientGrpAve = 0.;
		dEnergyGrpMax = 0.;
		dEnergyGrpAve = 0.;
		dSAtGrpAve = 0.;
		//QL比を越える主領域が続く終点を探索
		while (iSAtEnergyMax[iM] && iM < MainPixel){
			iMGrpEnd = iM;
			if (dEnergyGrpMax < dEnergyMax[iM]) {
             	dEnergyGrpMax = dEnergyMax[iM];
			}
			iM++;
		}

		int iCnt = 0;
		for (int iMtmp = iMGrpStart; iMtmp <= iMGrpEnd; iMtmp++) {
			if (dEnergyMax[iMtmp] < dEnergyGrpMax * dCutRate) {
				continue;
			}
			dOrientGrpAve += dOrientAtEnergyMax[iMtmp];
			dEnergyGrpAve += dEnergyMax[iMtmp];
			dMGrpAve += iMtmp * dEnergyMax[iMtmp];
			dSAtGrpAve += iSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl3, "	%d	%d	%lf	%lf\n", iMtmp, iSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
   #endif
			iCnt++;
		}
		if (!iCnt) {
			continue;
		}
		dMGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dSAtGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dOrientGrpAve /= iCnt;
		dEnergyGrpAve /= iCnt;

		//斜めエッジとDC補間の副領域の交差点のｘ座標
		double dMItpl1 = 0.;//補間の開始ｘ座標
		double dMItpl2 = 0.;//補間の終了ｘ座標
//		int iSItpl1 = edgeY[iM] - isize + 1;
//		int iSItpl2 = edgeY[iM];
		int iSItpl1 = edgeY[round(dMGrpAve)] - isize + 1;//どのｘでのedgeYなのかを明確化(140816 ttsuji)
		int iSItpl2 = edgeY[round(dMGrpAve)]; //どのｘでのedgeYなのかを明確化(140816 ttsuji)

		double dTan = tan(PI/180. * dOrientGrpAve);
		dMItpl1 = (iSItpl1 - dSAtGrpAve) / dTan + dMGrpAve;
		dMItpl2 = (iSItpl2 - dSAtGrpAve) / dTan + dMGrpAve;
		double dTmp;
		if (dMItpl1 > dMItpl2) {//dMItpl1 < dMItpl2 となるようにソートする
			dTmp = dMItpl1;
			dMItpl1 = dMItpl2;
			dMItpl2 = dTmp;
		}

   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl3, "%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iMGrpStart, iMGrpEnd, dMGrpAve, dSAtGrpAve, dOrientGrpAve, dEnergyGrpAve, dMItpl1, dMItpl2);
   #endif

		//斜め方向のガラス段差補正
		LPGlassCorNanameROI(CorrImg, delta, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, isize, GlassProf);
	}

   #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl3);
//	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg2.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorrImg3.tma");
   #endif
	delete[] dOrientAtEnergyMax;
	delete[] dEnergyMax;
	delete[] iSAtEnergyMax;


	CorrImg.MainMedian(iMainMedianGlassProfile);
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif

	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}

	delete[] edgeY;
	delete[] delta;
}
//---------------------------------------------------------------------------
//ガラス段差補正の斜めROIパート

void TSIMARS::LPGlassCorNanameROI(TTMA& CorrImg, double* delta, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, const int iSize, float* GlassProf)
{
	if (iMsample1 == iMsample2) {
		return;//斜めエッジでないため処理しない
	}

	int iTmp;
	if (iSsample1 > iSsample2) {//iSsample1 < iSsample2 となるようにソートする
		iTmp = iSsample1;
		iSsample1 = iSsample2;
		iSsample2 = iTmp;
	}
	if (iMsample1 > iMsample2) {//iSsample1 < iSsample2 となるようにソートする
		iTmp = iMsample1;
		iMsample1 = iMsample2;
		iMsample2 = iTmp;
	}

	double* dYBorder = new double[iMsample2 - iMsample1 + 1];
	for (int iM = iMsample1; iM <= iMsample2; iM++){
		dYBorder[iM - iMsample1] = dTan * (iM - dMGrpAve) + dSAtGrpAve;
	}


	double dTanCurrent = 0.;
	double dSecCurrent;//Cosineの逆数
	int iSizeMod;//斜め方向の台形プロファイルの長さ
	double dQLitpl, dMVirtualPixel, dQLVirtualPixel, dDistanceVirtualNeighbor, dDistance;
	int iMNeighbor;
	for (int iS = iSsample1 + 1; iS < iSsample2; iS++){//iS=iSsample1とiSsample2とを用いて間を補間
		for (int iM = iMsample1; iM <= iMsample2; iM++){
			if (1986 == iM && 154 == iS) {
				int a = 0;
			}

			if (iMsample1 == iM || iMsample2 == iM) {//通常の斜めでないDC補間
//				dQLitpl = ((iSsample2 - iS) * Dt[iSsample1][iM] + (iS -iSsample1) * Dt[iSsample2][iM]) / (iSsample2 - iSsample1);//線形補間
			}
			else{
				if (dTan > 0.) {//θ=0～90°の場合
					//斜めエッジを越えているか否かで場合分け
					if (iS > dYBorder[iM - iMsample1]) {//斜めエッジよりもiSsample2側の画素のときはiSsample2側の3点目の画素で補間
						dTanCurrent = (double)(iS - iSsample1) / (iM - iMsample1);//+
						dMVirtualPixel = iMsample1 + (iSsample2 - iSsample1) / dTanCurrent;
						iMNeighbor = round(dMVirtualPixel);//四捨五入
						CorrImg.SetVal(iS, iM, delta[iMNeighbor] * GlassProf[iSsample2 - iS] + iKasa);
					}
					else{//斜めエッジよりもiSsample1側の画素のときはiSsample1側の3点目の画素で補間
						CorrImg.SetVal(iS, iM, delta[iMsample2] * GlassProf[iSsample2 - iS] + iKasa);
					}
				}
				else{//θ=90～180°の場合
					//斜めエッジを越えているか否かで場合分け
					if (iS > dYBorder[iM - iMsample1]) {//斜めエッジよりもiSsample2側の画素のときはiSsample2側の3点目の画素で補間
						dTanCurrent = (double)(iS - iSsample1) / (iMsample2 - iM);//+
						dMVirtualPixel = iMsample2 - (iSsample2 - iSsample1) / dTanCurrent;
						iMNeighbor = round(dMVirtualPixel);//四捨五入
//						CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[iSsample2 - iS] + iKasa);
						CorrImg.SetVal(iS, iM, delta[iMNeighbor] * GlassProf[iSsample2 - iS] + iKasa);
					}
					else{//斜めエッジよりもiSsample1側の画素のときはiSsample1側の3点目の画素で補間
						CorrImg.SetVal(iS, iM, delta[iMsample1] * GlassProf[iSsample2 - iS] + iKasa);
					}
				}

			}
		}
//		fprintf(fpDCItpl, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
	}

	delete[] dYBorder;

}
//---------------------------------------------------------------------------
//ガラス段差補正の斜めROIパート（平行四辺形で補間）

void TSIMARS::LPGlassCorNanameROIHeikohShihenkei(TTMA& CorrImg, double* delta, int* edgeY, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, const int iSize, float* GlassProf)
{
	//tmp
	if (iMsample1 == 1886) {
		int aaa = 0;
	}

	if (iMsample1 == iMsample2) {
		return;//斜めエッジでないため処理しない
	}

	int iTmp;
	if (iSsample1 > iSsample2) {//iSsample1 < iSsample2 となるようにソートする
		iTmp = iSsample1;
		iSsample1 = iSsample2;
		iSsample2 = iTmp;
	}
	if (iMsample1 > iMsample2) {//iMsample1 < iMsample2 となるようにソートする
		iTmp = iMsample1;
		iMsample1 = iMsample2;
		iMsample2 = iTmp;
	}

	//tanθの正負で平行四辺形補間するｘ領域が異なる（平行四辺形の頂点のｘ座標）
	int iMStartAtiS1, iMEndAtiS1, iMStartAtiS2, iMEndAtiS2;
	int iMNeighborAtiS2, iMNeighborAtiS1;//隣接画素のiM座標
	double dMAtiS1, dMAtiS2, dMShiftAtiS1, dMShiftAtiS2, dDistance1VirtualNeighbor, dDistance2VirtualNeighbor, dQLVirtualPixelAtiS1, dQLVirtualPixelAtiS2, dQLitpl;
	int iM1M2 = iMsample2 - iMsample1;
	if (dTan > 0) {
#if 1
		iMStartAtiS1 = CLIP(iMsample1 - iM1M2, 0, MainPixel - 1);
		iMEndAtiS1 = iMsample2;
		iMStartAtiS2 = iMsample1;
		iMEndAtiS2 = CLIP(iMsample2 + iM1M2, 0, MainPixel - 1);
#else
		iMStartAtiS1 = CLIP(iMsample1 - 3*iM1M2, 0, MainPixel - 1);
		iMEndAtiS1 = CLIP(iMsample2 + 2*iM1M2, 0, MainPixel - 1);
		iMStartAtiS2 = CLIP(iMsample1 - 2*iM1M2, 0, MainPixel - 1);
		iMEndAtiS2 = CLIP(iMsample2 + 3*iM1M2, 0, MainPixel - 1);
#endif
	}
	else{
#if 1
		iMStartAtiS1 = iMsample1;
		iMEndAtiS1 = CLIP(iMsample2 + iM1M2, 0, MainPixel - 1);
		iMStartAtiS2 = CLIP(iMsample1 - iM1M2, 0, MainPixel - 1);
		iMEndAtiS2 = iMsample2;
#else
		iMStartAtiS1 = CLIP(iMsample1 - 2*iM1M2, 0, MainPixel - 1);
		iMEndAtiS1 = CLIP(iMsample2 + 3*iM1M2, 0, MainPixel - 1);
		iMStartAtiS2 = CLIP(iMsample1 - 3*iM1M2, 0, MainPixel - 1);
		iMEndAtiS2 = CLIP(iMsample2 + 2*iM1M2, 0, MainPixel - 1);
#endif
	}

	//平行四辺形の頂点のｙ座標
//	int iS1AtiMStart = edgeY[iMStartAtiS1];
	int iS1AtiMStart = edgeY[iMStartAtiS1] - iSize + 1;
	int iS2AtiMStart = edgeY[iMStartAtiS2];
//	int iS1AtiMEnd   = edgeY[iMEndAtiS1];
	int iS1AtiMEnd   = edgeY[iMEndAtiS1] - iSize + 1;
	int iS2AtiMEnd   = edgeY[iMEndAtiS2];
	int iS1AtiM, iS2AtiM;


#if 1
//	for (int iM = iMsample1 + 1; iM < iMsample2; iM++){
	for (int iM = CLIP(iMsample1 - iM1M2 + 1, 0, MainPixel - 1); iM < CLIP(iMsample2 + iM1M2, 0, MainPixel - 1); iM++){
#else
	for (int iM = CLIP(iMsample1 - 3*iM1M2 + 1, 0, MainPixel - 1); iM < CLIP(iMsample2 + 3*iM1M2, 0, MainPixel - 1); iM++){
#endif
//		iS1AtiM = edgeY[iM];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
		iS1AtiM = edgeY[iM] - iSize + 1;//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
		iS2AtiM = edgeY[iM];
//		for (int iS = iSsample1 + 1; iS < iSsample2; iS++){//iS=iSsample1とiSsample2とを用いて間を補間
		for (int iS = iS1AtiM + 1; iS < iS2AtiM; iS++){//iS=iS1AtiMとiS2AtiMとを用いて間を補間

			//斜辺のｙ座標を計算し、平行四辺形領域の内外判定
			double dYBorderEnd   = dTan * (iM - iMEndAtiS1) + iS1AtiMEnd;//iMEnd側の斜辺のｙ座標
			double dYBorderStart = dTan * (iM - iMStartAtiS1) + iS1AtiMStart;//iMEnd側の斜辺のｙ座標
			if (dTan > 0.) {
//				if (iS < dYBorderEnd || iS > dYBorderStart) {
				if (iS <= dYBorderEnd || iS >= dYBorderStart) {//141118　石丸さん指摘により修正（平行四辺形の頂点iS1AtiMStart、iS1AtiMEnd、。。は補間しない）
					continue;//平行四辺形の外側は計算しない
				}
			}
			else{
//				if (iS > dYBorderEnd || iS < dYBorderStart) {
				if (iS >= dYBorderEnd || iS <= dYBorderStart) {//141118　石丸さん指摘により修正（平行四辺形の頂点iS1AtiMStart、iS1AtiMEnd、。。は補間しない）
					continue;//平行四辺形の外側は計算しない
				}
			}

//			dMShiftAtiS1 = (iS - iS1AtiM) / dTan;
			dMShiftAtiS2 = (iS2AtiM - iS) / dTan;
//			dMAtiS1 = iM - dMShiftAtiS1;
			dMAtiS2 = CLIP(iM + dMShiftAtiS2, 0, MainPixel - 1);

//			iMNeighborAtiS1 = CLIP((int)dMAtiS1, 0, MainPixel - 2);//切捨て後、clipping
			iMNeighborAtiS2 = CLIP((int)dMAtiS2, 0, MainPixel - 2);//切捨て後、clipping
//			dDistance1VirtualNeighbor = dMAtiS1 - iMNeighborAtiS1;
			dDistance2VirtualNeighbor = dMAtiS2 - iMNeighborAtiS2;
//			dQLVirtualPixelAtiS1 = (1 - dDistance1VirtualNeighbor) * Dt[edgeY[iMNeighborAtiS1] - iSubRangeOnlyMidPanel][iMNeighborAtiS1] + dDistance1VirtualNeighbor * Dt[edgeY[iMNeighborAtiS1 + 1] - iSubRangeOnlyMidPanel][iMNeighborAtiS1 + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
			dQLVirtualPixelAtiS2 = (1 - dDistance2VirtualNeighbor) * delta[iMNeighborAtiS2] + dDistance2VirtualNeighbor * delta[iMNeighborAtiS2 + 1];//シンチ段差が水平でなく、階段状になっている場合を想定して、iMに応じて補間の上下点を規定(141002 ttsuji)
//			dQLitpl = ((iS2AtiM - iS) * dQLVirtualPixelAtiS1 + (iS - iS1AtiM) * dQLVirtualPixelAtiS2) / (iS2AtiM - iS1AtiM);

//			CorrImg.SetVal(iS, iM, dQLVirtualPixelAtiS2 * GlassProf[iSsample2 - iS] + iKasa);
			CorrImg.SetVal(iS, iM, dQLVirtualPixelAtiS2 * GlassProf[iS2AtiM - iS] + iKasa);
/*
			ItplImg.SetVal(iS, iM, dQLitpl);
			CorrImg.SetVal(iS, iM, Dt[iS][iM] - dQLitpl + iKasa);
			CorrImgDebug.SetVal(iS, iM, CorrImg.Dt[iS][iM]);
*/
		}
	}

}
//---------------------------------------------------------------------------
//ガラス段差補正（斜めエッジ補正に平行四辺形補間）

void TSIMARS::LPGlassEdgeCorNanameEdgeHS(string FileName, int iMainMedianGlassProfile, bool bUpperEdge, bool bDetectKanajakuForGlassCor)
{
	//主Medianでノイズを落とす
	const int iMMed = 11;//DC補間長さ20pixの情報は保持するマスクサイズ
	TTMA MedImg = *this;
	MedImg.MainMedian(iMMed);
	MedImg.SubMedian(iMMed);//斜めエッジ誤検出対策として追加(141113 ttsuji)

	//Log変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	TTMA MedLogImg = MedImg;
	MedLogImg.LogThreshold(dMidVal, dKeta, iLogTh);
	MedLogImg.WriteTma( AddPrefix(FileName, "LogMed" + IntToStr(iMMed) + "_") );



	int* edgeY;
	edgeY = new int[MainPixel];
	int edgeYAve;

	switch (bUpperEdge) {
	case true:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeU.y10[iM] / 10.);
		}
		edgeYAve = round((m_GlassEdgeU.ymax + m_GlassEdgeU.ymin) / 2.0 / 10.);
		break;
	case false:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeD.y10[iM] / 10.);
		}
		edgeYAve = round((m_GlassEdgeD.ymax + m_GlassEdgeD.ymin) / 2.0 / 10.);
		break;
	}


	double* delta;
	delta = new double[MainPixel];
	int iKasa = 4000;
	TTMA CorrImg;
	CorrImg = *this;
	CorrImg.FillValue(iKasa);

	for (int iM = 0; iM < MainPixel; iM++){
		delta[iM] = (Dt[edgeY[iM]-2][iM]+Dt[edgeY[iM]-3][iM]
				  -Dt[edgeY[iM]+2][iM]-Dt[edgeY[iM]+3][iM])/2.;
	}

#if 0
	int isize=17;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//17個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#else
	int isize=14;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//14個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#endif

	for (int iM = 0; iM < MainPixel; iM++){
		for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--){
//			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS]);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS] + iKasa);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCor.tma");
 #endif

#if 1 //石丸さん指摘（主Median二度がけ）により削除すべき？（141125 ttsuji）
	CorrImg.MainMedian(iMainMedianGlassProfile);
//	CorrImg.SubMedian(iMainMedianGlassProfile);//141118 tsuji（ノイズ対策）
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif
#endif
/*
	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
*/



 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpUDrate = fopen((ExtractFilePath(FileName) + "GlassCorUDQLRateMainProfile.txt").c_str(), "wt");
	fprintf(fpUDrate, "iM	QLUp	QLDown	QLRate	QLRate2\n");
 #endif

	//DC補間サンプル点のQL比の主プロファイル
	TFIMG fImgRateUD;
	fImgRateUD.NewDt(MainPixel, 1);
	fImgRateUD.Fill(0.);

	//QL比閾値（低周波情報除去画像）
//	const float fThQLRatioHighPass = 1.05;//平行四辺形補間する領域をできるだけ広げるため（141104 ttsuji)
	const float fThQLRatioHighPass = 1.1;//新default
//	const float fThQLRatioHighPass = 1.2;//default
//	const float fThQLRatioHighPass = 1.3;
//	const float fThQLRatioHighPass = 1.4;//やりすぎか

	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
//		int iSstart = edgeY[iM] - isize + 1;
//		int iSend   = edgeY[iM];
		int iSstart = edgeY[iM] - isize + 4;
		int iSend   = edgeY[iM] - 3;

 #if 0
		float fAveU = (float)Dt[iSstart][iM];
		float fAveD = (float)Dt[iSend][iM];
 #else  //シンチ段差補正で精度アップした主・副Median後の画像での斜めエッジ検出の横展開（141120 ttsuji）
		float fAveU = (float)MedImg.Dt[iSstart][iM];
		float fAveD = (float)MedImg.Dt[iSend][iM];
 #endif

		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		fImgRateUD.Dt[iM][0] = fQLRateUD;
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpUDrate, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
  #endif
	}//end for iM

  #ifdef _LP_DEBUG_OUTPUT
	fclose(fpUDrate);
///	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorrImg2.tma");

	FILE* fpDCItpl2 = fopen((ExtractFilePath(FileName) + "GlassCorQLrateHighPassDCItplMainProfile.txt").c_str(), "wt");
//	fprintf(fpDCItpl2, "iM	QLUp	QLDown	QLRate	QLRate2	flag	iS	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
	fprintf(fpDCItpl2, "iM	QLRate	flag	iS	Energy	Orientation	L2R	U2D	UL2DR	UR2DL\n");
  #endif


	//QL比の主プロファイルを主Medianし、低周波のみを抽出
	const int iMainMedQLRate = 201;
	TFIMG fImgRateUDMMed = fImgRateUD;
	fImgRateUDMMed.MainMedian(iMainMedQLRate);
	fImgRateUD -= fImgRateUDMMed;//低周波情報を除去し、高周波のみを抽出
	fImgRateUD += 1.;//１を加算し、UとDが等しいときを１倍とする
  #ifdef _LP_DEBUG_OUTPUT
	fImgRateUD.bSaveAsText((ExtractFilePath(FileName) + "GlassCorQLRateHighPass.txt").c_str(), true);
  #endif


	//QL比の主プロファイルのHighPass後のQL比から斜めエッジを検出（この方法だと、DC補間の20pix間の濃度変化の影響を受けず、精度の良いエッジ検出が可能）
	float fThQLRatioHighPass2 = 1 / fThQLRatioHighPass;
	double dOrientCut = 20.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 30.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 35.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 40.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
	double* dOrientAtEnergyMax = new double[MainPixel];//iSをふったときにEnergy（周辺の微分値）が最大となるときの角度（ただしCut角は除く）
	double* dEnergyMax = new double[MainPixel];//iSをふったときの最大Energy
//	int* iSAtEnergyMax = new int[MainPixel];
	double* dSAtEnergyMax = new double[MainPixel];
	for (int iM = 0; iM < MainPixel; iM++){
		dEnergyMax[iM] = 0.;//初期化
		dOrientAtEnergyMax[iM] = 0.;//初期化
//		iSAtEnergyMax[iM] = 0;//初期化
		dSAtEnergyMax[iM] = 0.;//初期化
	}

	//ガラス段差補正領域のヒストグラムをとり、低線量領域は除外するようにする
    int iMainShiftMax = 30;
	Histogram(edgeYAve - 3, edgeYAve - 3, iMainShiftMax, MainPixel - 1 - iMainShiftMax);
#if 0
	float fThRate = 0.7;//ヒストの全面積の70％（感度比を計算する画素の判定に使用） ←多すぎて素抜けを含んでしまいNG
//	float fThRate = 0.5;//ヒストの全面積の50％（感度比を計算する画素の判定に使用）
//	float fThRate = 0.3;//ヒストの全面積の30％（感度比を計算する画素の判定に使用）
	int iThMinQL = HistRuisekiDosuuPercent(fThRate, true);
#else
	float fThRate;
	int iThMinQL = 0;//とりあえず問題画像用に
//	int iThMinQL = 5000;//とりあえず問題画像用に
#endif


	//主方向にスキャンし、QL比を越える主領域について、DC補間対象の副領域すべての微分値を調査
//	for (int iM = 0; iM < MainPixel; iM++){
	for (int iM = 1; iM < MainPixel - 1; iM++){//石丸さん指摘により、端にエッジがあった場合に実行エラーとなるはずなので、端を除外(140901 ttsuji)
//		int iSstart = edgeY[iM] - isize + 1;
//		int iSend   = edgeY[iM];
		int iSstart = edgeY[iM] - isize + 4;
		int iSend   = edgeY[iM] - 3;
		DIFFERENTIALARROUNDPIXELS* DifPixelsArray = new DIFFERENTIALARROUNDPIXELS[iSend - iSstart + 1];//ある点の微分情報

		bool bflag = false;//斜めエッジ対応フラグ

//		if (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2) {
		if (iThMinQL < Dt[iSend][iM] && (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2)) {//低線量領域は斜め対策から除外するようにする
				bflag = true;
			//DC補間する副領域すべての画素の微分値を調査するが、上下方向のエッジが強い画素は斜めエッジの角度算出から除く.
			double dAveUD = 0.;//上下方向の微分値の平均値
			double dAveSquareUD = 0.;//上下方向の微分値の二乗平均
			double dStdEvpUD = 0.;//上下方向の微分値の標準偏差
			double dParaExclude = 1.0;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか
			for (int iS = iSstart; iS <= iSend; iS++) {
#if 0
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
#else
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel_Fast(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
#endif
				dAveUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff;
				dAveSquareUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff * DifPixelsArray[iS - iSstart].Up2Down.dDiff;
			}
			dAveUD /= iSend - iSstart + 1;//副方向の平均値を計算
			dAveSquareUD /= iSend - iSstart + 1;//副方向の二乗平均を計算
			dStdEvpUD = sqrt(dAveSquareUD - dAveUD * dAveUD);//副方向の標準偏差を計算

			//上下方向の微分値が、平均±(σ*dParaExclude)の範囲内のときの斜めエッジを統計処理
			double dUDUpperLimit = dAveUD + dParaExclude * dStdEvpUD;//上下微分の上限値
			double dUDLowerLimit = dAveUD - dParaExclude * dStdEvpUD;//上下微分の下限値
			double dSAve = 0.;
			double dOrientAve = 0.;
        	double dSumEnergy = 0.;
			for (int iS = iSstart; iS <= iSend; iS++) {
				bool bFlag2 = false;
//				if (dUDLowerLimit < DifPixelsArray[iS - iSstart].Up2Down.dDiff && DifPixelsArray[iS - iSstart].Up2Down.dDiff < dUDUpperLimit && dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
				if (dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
					bFlag2 = true;
					dSAve += iS * DifPixelsArray[iS - iSstart].dEnergy;
					dOrientAve += DifPixelsArray[iS - iSstart].dOrientation * DifPixelsArray[iS - iSstart].dEnergy;
					dSumEnergy += DifPixelsArray[iS - iSstart].dEnergy;

					if (dEnergyMax[iM] < DifPixelsArray[iS - iSstart].dEnergy) {
						dEnergyMax[iM] = DifPixelsArray[iS - iSstart].dEnergy;//dEnergyMaxを更新
					}
				}
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixelsArray[iS - iSstart].dEnergy, DifPixelsArray[iS - iSstart].dOrientation, DifPixelsArray[iS - iSstart].Left2Right, DifPixelsArray[iS - iSstart].Up2Down, DifPixelsArray[iS - iSstart].UpLeft2DownRight, DifPixelsArray[iS - iSstart].UpRight2DownLeft, bFlag2);
   #endif
			}
			if (!dSumEnergy) {
				continue;//上下微分が平均±(σ*dParaExclude)の範囲内で、かつdOrientCut度～(180-dOrientCut)度の範囲内のものがなければエッジなしと判断
			}
			dSAve /= dSumEnergy;
			dOrientAve /= dSumEnergy;
//			iSAtEnergyMax[iM] = round(dSAve);
			dSAtEnergyMax[iM] = dSAve;
			dOrientAtEnergyMax[iM] = dOrientAve;

   #ifdef _LP_DEBUG_OUTPUT
//			fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, iSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
			fprintf(fpDCItpl2, "%d	%lf	%d	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
   #endif
		}
		else{
			bflag = false;
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag);
   #endif
		}

		delete[] DifPixelsArray;
	}

  #ifdef _LP_DEBUG_OUTPUT
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl2);
  #endif

	//QL比を越える主領域について、グルーピングしてグループごとのエッジ角度を決めて、斜め方向のDC成分補間を行う
	int iMGrpStart = 0;
	int iMGrpEnd = 0;
	double dMGrpAve = 0.;
	double dOrientGrpAve = 0.;
	double dEnergyGrpMax = 0.;
	double dEnergyGrpAve = 0.;
	double dSAtGrpAve = 0.;
	double dCutRate = 0.3;//EnergyがMaxのdCutRate倍以下は統計から除外する
  #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl3 = fopen((ExtractFilePath(FileName) + "GlassCorEdgePositonInformation.txt").c_str(), "wt");
	fprintf(fpDCItpl3, "iThMinQL	fThRate\n");
	fprintf(fpDCItpl3, "%d	%f\n", iThMinQL, fThRate);
	fprintf(fpDCItpl3, "iMGrpStart	iMGrpEnd	dMGrpAve	dOrientGrpAve	dEnergyGrpAve	dMItpl1	dMItpl2\n");
  #endif
	for (int iM = 0; iM < MainPixel; iM++){
//		if (!iSAtEnergyMax[iM]) {
		if (0.0 == dSAtEnergyMax[iM]) {
			continue;
		}

		iMGrpStart = iM;
		dMGrpAve = 0.;
		dOrientGrpAve = 0.;
		dEnergyGrpMax = 0.;
		dEnergyGrpAve = 0.;
		dSAtGrpAve = 0.;
		//QL比を越える主領域が続く終点を探索
//		while (iSAtEnergyMax[iM] && iM < MainPixel){
		while (0.0 != dSAtEnergyMax[iM] && iM < MainPixel){
			iMGrpEnd = iM;
			if (dEnergyGrpMax < dEnergyMax[iM]) {
             	dEnergyGrpMax = dEnergyMax[iM];
			}
			iM++;
		}

		int iCnt = 0;
		for (int iMtmp = iMGrpStart; iMtmp <= iMGrpEnd; iMtmp++) {
			if (dEnergyMax[iMtmp] < dEnergyGrpMax * dCutRate) {
				continue;
			}
			dOrientGrpAve += dOrientAtEnergyMax[iMtmp];
			dEnergyGrpAve += dEnergyMax[iMtmp];
			dMGrpAve += iMtmp * dEnergyMax[iMtmp];
//			dSAtGrpAve += iSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
			dSAtGrpAve += dSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
   #ifdef _LP_DEBUG_OUTPUT
//			fprintf(fpDCItpl3, "	%d	%d	%lf	%lf\n", iMtmp, iSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
			fprintf(fpDCItpl3, "	%d	%lf	%lf	%lf\n", iMtmp, dSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
   #endif
			iCnt++;
		}
		if (!iCnt) {
			continue;
		}
		dMGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dSAtGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dOrientGrpAve /= iCnt;
		dEnergyGrpAve /= iCnt;

		//斜めエッジとDC補間の副領域の交差点のｘ座標
		double dMItpl1 = 0.;//補間の開始ｘ座標
		double dMItpl2 = 0.;//補間の終了ｘ座標
//		int iSItpl1 = edgeY[iM] - isize + 1;
//		int iSItpl2 = edgeY[iM];
		int iSItpl1 = edgeY[round(dMGrpAve)] - isize + 1;//どのｘでのedgeYなのかを明確化(140816 ttsuji)
		int iSItpl2 = edgeY[round(dMGrpAve)]; //どのｘでのedgeYなのかを明確化(140816 ttsuji)

		double dTan = tan(PI/180. * dOrientGrpAve);
		dMItpl1 = (iSItpl1 - dSAtGrpAve) / dTan + dMGrpAve;
		dMItpl2 = (iSItpl2 - dSAtGrpAve) / dTan + dMGrpAve;
		double dTmp;
		if (dMItpl1 > dMItpl2) {//dMItpl1 < dMItpl2 となるようにソートする
			dTmp = dMItpl1;
			dMItpl1 = dMItpl2;
			dMItpl2 = dTmp;
		}

   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl3, "%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iMGrpStart, iMGrpEnd, dMGrpAve, dSAtGrpAve, dOrientGrpAve, dEnergyGrpAve, dMItpl1, dMItpl2);
   #endif

		//斜め方向のガラス段差補正
//		LPGlassCorNanameROI(CorrImg, delta, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, isize, GlassProf);
		LPGlassCorNanameROIHeikohShihenkei(CorrImg, delta, edgeY, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, isize, GlassProf);
	}

   #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl3);
//	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg2.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorrImg3.tma");
   #endif
	delete[] dOrientAtEnergyMax;
	delete[] dEnergyMax;
//	delete[] iSAtEnergyMax;
	delete[] dSAtEnergyMax;


	CorrImg.MainMedian(iMainMedianGlassProfile);
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif



#if 0
	// 20141218 kuwabara ガラスと鉛      !! コードはT-Cにしか対応していません。
	// 主方向にサーチし、QL比が一定値を超える場合は、構造があるとみなし、一定値を下回るまでメディアン長を増やす
	if (bDetectKanajakuForGlassCor) {//金尺目盛検出しガラス段差補正に写った金尺を変える
		const double dRatioMax=1.2;
		const double dRatioMin=1./dRatioMax;
		const int iQLlimit=10000;
		double* dRatioG = new double[MainPixel];

		// まずはガラス段差比
		for (int iM = 0; iM < MainPixel; iM++){
			dRatioG[iM] = (Dt[edgeY[iM]-3][iM]+Dt[edgeY[iM]-2][iM])*1.0 / (Dt[edgeY[iM]+3][iM]+Dt[edgeY[iM]+2][iM]);
//			if ((dRatioG[iM]<dRatioMin || dRatioG[iM]>dRatioMax) && Dt[edgeY[iM]+3][iM]<iQLlimit ) {
			if ((dRatioG[iM]<dRatioMin || dRatioG[iM]>dRatioMax) ) {
				dRatioG[iM]=-1.0;  // 比率がはみ出したらマイナス値を入れる
			}
		}

		// 比がマイナスの領域のCorrImgを両端の線形補間で埋める。画像端は片側のdupe
		{
			int iMs=0;                         // マイナスでない始点サーチ
			for(  ; iMs<MainPixel; iMs++){
				if(dRatioG[iMs]>=0.) break;
			}
			int iMe=MainPixel-1;               // マイナスでない終点サーチ
			for(  ; iMe>=iMs; iMe--){
				 if(dRatioG[iMe]>=0.) break;
			}

			if( iMs>iMe ){                     // 全部マイナスのときは何もしない
			}
			else {
				for(int iM=iMs-1; iM>=0; iM--){  // 始点側をdupeで埋める
					for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
						CorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMs]);
					}
				}
				for(int iM=iMe+1; iM<MainPixel; iM++){  // 終点側をdupeで埋める
					for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
					   CorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMe]);
					}
				}
				int iMt=iMe;
				for( int iM=iMs+1; iM<=iMe-1; iM++){
					if(dRatioG[iM] >=0.) {              // プラスのときはCorrImgは変えない
				}
				else{
					int iMts=iM;                      // マイナスのときはプラスになる座標を探す
					int iMte=iM;
					for( ; iMte<iMe-1; iMte++){
						if(dRatioG[iMte]>=0.) break;
					}
 #define __hokan
 #ifdef hokan
					for(int iM=iMts+1; iM<=iMte-1; iM++){  // プラスとなる両側の線形補間で埋める。
						for (int iS = 0; iS < 200; iS++) {
							CorrImg.SetVal(iS, iM, (CorrImg.Dt[iS][iMte]*1.0*(iM-iMts)+CorrImg.Dt[iS][iMts]*1.0*(iMte-iM))/(iMte-iMts));
						}
					}
 #else  // hokan
/*			  for(int iM=iMts+1; iM<=iMte-1; iM++){  // プラスとなる両側のうち近いほうの値で埋める。
				for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
				   int iend   = abs(CorrImg.Dt[iS][iMte]-CorrImg.Dt[iS][iM]*1.0);
				   int istart = abs(CorrImg.Dt[iS][iMts]-CorrImg.Dt[iS][iM]*1.0);
				   CorrImg.SetVal(iS, iM, istart>iend?CorrImg.Dt[iS][iMts]:CorrImg.Dt[iS][iMte]);
				}
			  }
*/
					const int ith=180;
					for(int iM=iMts+1; iM<=iMte-1; iM++){  // プラスとなる両側のうち近いほうの値で埋める。
						for (int iS = 0; iS < 200; iS++) {    // 補正量が両側とも大きいか小さい場合は線形
							if( ( abs(CorrImg.Dt[iS][iMts] -iKasa) >ith && abs(CorrImg.Dt[iS][iMte] -iKasa) >ith )
								|| (abs(CorrImg.Dt[iS][iMts] -iKasa) <ith && abs(CorrImg.Dt[iS][iMte] -iKasa) <ith )){
								CorrImg.SetVal(iS, iM, (CorrImg.Dt[iS][iMte]*1.0*(iM-iMts)+CorrImg.Dt[iS][iMts]*1.0*(iMte-iM))/(iMte-iMts));
							} else if (abs(CorrImg.Dt[iS][iMts] -iKasa) >ith) {
								CorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMte]);
							} else {
								CorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMts]);
							}
						}
					}
 #endif // hokan
				}
			}
		}
		CorrImg.MainMedian(3);
		CorrImg.SubMedian(3);
 #ifdef _LP_DEBUG_OUTPUT
		CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeMainMedianNamri.tma");
 #endif
	}
	}




	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}

	delete[] edgeY;
	delete[] delta;
#endif




#if 1

#if 0  // 20141218 kuwabara ガラスと鉛      !! コードはT-Cにしか対応していません。
	// 主方向にサーチし、QL比が一定値を超える場合は、構造があるとみなし、一定値を下回る領域からコピーまたは線形補間する

	const double dRatioMax=1.2;
	const double dRatioMin=1./dRatioMax;
	const int iQLlimit=10000;
	double* dRatioG = new double[MainPixel];

	// まずはガラス段差比
	for (int iM = 0; iM < MainPixel; iM++){
		dRatioG[iM] = (Dt[edgeY[iM]-3][iM]+Dt[edgeY[iM]-2][iM])*1.0 / (Dt[edgeY[iM]+3][iM]+Dt[edgeY[iM]+2][iM]);
		if ((dRatioG[iM]<dRatioMin || dRatioG[iM]>dRatioMax) && Dt[edgeY[iM]+3][iM]<iQLlimit ) {
//		if ((dRatioG[iM]<dRatioMin || dRatioG[iM]>dRatioMax) ) {
			dRatioG[iM]=-1.0;  // 比率がはみ出したらマイナス値を入れる
		}
	}
#if 1 // 20150116 kuwabara 下記ガラスと鉛のデバッグコード
   {
   TTMA IImage;
   IImage.NewDt(6,CorrImg.MainPixel,16);
   for( int iM=0; iM<IImage.MainPixel; iM++){
	 IImage.Dt[0][iM] = Dt[edgeY[iM]-3][iM];
	 IImage.Dt[1][iM] = Dt[edgeY[iM]-2][iM];
	 IImage.Dt[2][iM] = Dt[edgeY[iM]+2][iM];
	 IImage.Dt[3][iM] = Dt[edgeY[iM]+3][iM];
	 IImage.Dt[4][iM] = 10000*(Dt[edgeY[iM]-3][iM]+Dt[edgeY[iM]-2][iM])*1.0 / (Dt[edgeY[iM]+3][iM]+Dt[edgeY[iM]+2][iM]);
	 IImage.Dt[5][iM] = dRatioG[iM]<0?65535:dRatioG[iM]*10000;
   }
   IImage.WriteTma( ExtractFilePath(FileName) + "GlassDebug.tma");
   }
#endif


	// 比がマイナスの領域のCorrImgを両端の線形補間で埋める。画像端は片側のdupe
	{ TTMA tempCorrImg;
	  tempCorrImg = CorrImg;
	  int iMs=0;                         // マイナスでない始点サーチ
	  for(  ; iMs<MainPixel; iMs++){
		  if(dRatioG[iMs]>=0.) break;
	  }
	  int iMe=MainPixel-1;               // マイナスでない終点サーチ
	  for(  ; iMe>=iMs; iMe--){
			 if(dRatioG[iMe]>=0.) break;
	  }
	  if( iMs>iMe ){                     // 全部マイナスのときは何もしない
	  } else {
		  for(int iM=iMs-1; iM>=0; iM--){  // 始点側をdupeで埋める
			for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
			   tempCorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMs]);
//		  tempCorrImg.SetVal(iS, iM, CorrImg.DtMax);
			}
		  }
		  for(int iM=iMe+1; iM<MainPixel; iM++){  // 終点側をdupeで埋める
			for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
			   tempCorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMe]);
//	   tempCorrImg.SetVal(iS, iM, CorrImg.DtMax);
			}
		  }
//		  int iMt=iMe;
		  for( int iM=iMs+1; iM<=iMe-1; iM++){
			if(dRatioG[iM] >=0.) {              // プラスのときはCorrImgは変えない
			} else{
			  int iMts=iM-1;                      // マイナスのときはプラスになる座標を探す  20150119 誤記修正 -1　を追加
			  int iMte=iM;
			  for( ; iMte<iMe-1; iMte++){
				if(dRatioG[iMte]>=0.) break;
			  }
#define __hokan
#ifdef hokan
			  for( iM=iMts+1; iM<=iMte-1; iM++){  // プラスとなる両側の線形補間で埋める。
				for (int iS = 0; iS < 200; iS++) {
				   tempCorrImg.SetVal(iS, iM, (CorrImg.Dt[iS][iMte]*1.0*(iM-iMts)+CorrImg.Dt[iS][iMts]*1.0*(iMte-iM))/(iMte-iMts));
//			   tempCorrImg.SetVal(iS, iM, CorrImg.DtMax);
				}
			  }
#else  // hokan
/*			  for(int iM=iMts+1; iM<=iMte-1; iM++){  // プラスとなる両側のうち近いほうの値で埋める。
				for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
				   int iend   = abs(CorrImg.Dt[iS][iMte]-CorrImg.Dt[iS][iM]*1.0);
				   int istart = abs(CorrImg.Dt[iS][iMts]-CorrImg.Dt[iS][iM]*1.0);
				   tempCorrImg.SetVal(iS, iM, istart>iend?CorrImg.Dt[iS][iMts]:CorrImg.Dt[iS][iMte]);
				}
			  }
*/            const int ith=180;
//			  iMts=2751;iMte=2757; // debug
			  for( iM=iMts+1; iM<=iMte-1; iM++){  // プラスとなる両側のうち近いほうの値で埋める。
				for (int iS = 0; iS < 200; iS++) {    // 補正量が両側とも大きいか小さい場合は線形
//				for (int iS = 161; iS < 162; iS++) {    // 補正量が両側とも大きいか小さい場合は線形
				   if( ( abs(CorrImg.Dt[iS][iMts] -iKasa) >ith && abs(CorrImg.Dt[iS][iMte] -iKasa) >ith )
					 || (abs(CorrImg.Dt[iS][iMts] -iKasa) <ith && abs(CorrImg.Dt[iS][iMte] -iKasa) <ith )){
					 tempCorrImg.SetVal(iS, iM, (CorrImg.Dt[iS][iMte]*1.0*(iM-iMts)+CorrImg.Dt[iS][iMts]*1.0*(iMte-iM))/(iMte-iMts));
//					 iM=iM;
//			   tempCorrImg.SetVal(iS, iM, CorrImg.DtMax);
				   } else if (abs(CorrImg.Dt[iS][iMts] -iKasa) >ith) {
					 tempCorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMte]);
//			   tempCorrImg.SetVal(iS, iM, CorrImg.DtMax);
				   } else {
					 tempCorrImg.SetVal(iS, iM, CorrImg.Dt[iS][iMts]);
//			   tempCorrImg.SetVal(iS, iM, CorrImg.DtMax);
				   }




				}
			  }
#endif // hokan



			}


		  }
		  CorrImg = tempCorrImg;
	  }
 #ifdef _LP_DEBUG_OUTPUT
	  CorrImg.WriteTma(ExtractFilePath(FileName) + "3x3maeGlassCorNanameEdgeMainMedianNamri.tma");
 #endif
	  CorrImg.MainMedian(3);
	  CorrImg.SubMedian(3);
 #ifdef _LP_DEBUG_OUTPUT
	  CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeMainMedianNamri.tma");
 #endif
	}


 #endif   // ガラスと鉛



#if 1 // ガラスと鉛の対応２  20150122 kuwabara
	{

	TTMA tempCorrImg;
	tempCorrImg=CorrImg;
	const int iQLth=200;
	const int iQLthb=130;
	const int iHashi=300;

	// まずは閾値判定
	for (int iS=0; iS < 200; iS++) {
	  for (int iM = 1; iM < MainPixel; iM++){
#if 0
		if( abs(CorrImg.Dt[iS][iM]-iKasa) > iQLth ){
		  CorrImg.SetVal( iS, iM, DtMax);
		}
#endif
#if 1
		// Zマイナス dZマイナスで差分閾値超え
		if( (0+CorrImg.Dt[iS][iM]-CorrImg.Dt[iS][iM-1] < -iQLthb)
			&& (0+CorrImg.Dt[iS][iM]-iKasa < -iQLth)){
			for( int iiM=iM; iiM<MainPixel && iiM < iM+iHashi; iiM++){
			  if( (CorrImg.Dt[iS][iiM]-CorrImg.Dt[iS][iiM-1] > iQLthb)
				&&(CorrImg.Dt[iS][iiM]-iKasa > -iQLth)  ){
				   iM=iiM;
				   break;
			  }
			  tempCorrImg.SetVal( iS, iiM, DtMax);
			}
		} else if( (0+CorrImg.Dt[iS][iM]-CorrImg.Dt[iS][iM-1] > iQLthb)
				&& (0+CorrImg.Dt[iS][iM]-iKasa > iQLth)){
			for( int iiM=iM; iiM<MainPixel && iiM < iM+iHashi; iiM++){
			  if( (0+CorrImg.Dt[iS][iiM]-CorrImg.Dt[iS][iiM-1] < -iQLthb)
				&&(0+CorrImg.Dt[iS][iiM]-iKasa < iQLth)  ){
				   iM=iiM;
				   break;
			  }
			  tempCorrImg.SetVal( iS, iiM, DtMax);
			}
		}


	  }
	  //鉛が始点に接している場合の再探索
	  int iMst=0;
	  for (int iM = 1; iM < iHashi; iM++){
		if( tempCorrImg.Dt[iS][iM] == DtMax ) break;
		if( (0+CorrImg.Dt[iS][iM]-CorrImg.Dt[iS][iM-1] > iQLthb)
		  &&(0+CorrImg.Dt[iS][iM]-iKasa > -iQLth)
		  || (0+CorrImg.Dt[iS][iM]-CorrImg.Dt[iS][iM-1] < -iQLthb)
		  &&(0+CorrImg.Dt[iS][iM]-iKasa < iQLth) ){
			iMst=iM;
			break;
		}
	  }
	  for (int iM = 0; iM < iMst; iM++){
		tempCorrImg.SetVal( iS, iM, DtMax);
	  }
	}
#endif

	CorrImg=tempCorrImg;


	CorrImg.WriteTma( ExtractFilePath(FileName) +"GlassDebug2.tma");

	int iMs=MainPixel-1;
	int iMe=0;
	for (int iS=0; iS < 200; iS++) {
	  for (int iM = 0; iM < MainPixel; iM++){
		if( CorrImg.Dt[iS][iM] == DtMax ){
#if 1
		  iMs=iM;
		  while( iMs>=0 && CorrImg.Dt[iS][iMs] == DtMax ){
			iMs--;
		  }
		  if (iMs<0){
			iMs=-1;
		  }
#endif
#if 1
		  iMe=iM;
		  while( iMe<=MainPixel-1 && CorrImg.Dt[iS][iMe] == DtMax ){
			iMe++;
		  }
		  if (iMe>MainPixel-1) iMe=MainPixel;
#endif
		  if( iMs<0 && iMe>MainPixel-1 ){
			break;
		  } else if( iMs<0 ){
			for( int iiM=iMs+1; iiM<iMe; iiM++){
			  CorrImg.SetVal( iS, iiM, CorrImg.Dt[iS][iMe]);
			}
			iM=iMe;
		  } else if( iMe>MainPixel-1 ){
			for( int iiM=iMs+1; iiM<iMe; iiM++){
			  CorrImg.SetVal( iS, iiM, CorrImg.Dt[iS][iMs]);
			}
			iM=iMe;
		  } else {
			for( int iiM=iMs+1; iiM<iMe; iiM++){
			  CorrImg.SetVal( iS, iiM, ((iMe-iiM)*1.0*CorrImg.Dt[iS][iMs]+(iiM-iMs)*1.0*CorrImg.Dt[iS][iMe])/(iMe-iMs));
			}
			iM=iMe;
		  }
		}
	  }
	}

	}
#endif // ガラスと鉛２

	CorrImg.SubMedian(5);

	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	  CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeMainMedianNamri.tma");
 #endif

	delete[] edgeY;
	delete[] delta;
#endif
}
//---------------------------------------------------------------------------
//ガラス段差補正（斜めエッジ補正に平行四辺形補間 & 金尺検出に副方向微分閾値判定導入）

#define _ADAPTIVE_ITPL_BY_CORR_QL   //金尺目盛領域の線形補間の際に、ガラス段差補正画像Corrの両側の画素値の大小関係に応じて補間方法を変える(桑原さんアルゴ)


void TSIMARS::LPGlassEdgeCorNanameEdgeHS2(string FileName, int iMainMedianGlassProfile, bool bUpperEdge, bool bDetectKanajakuForGlassCor, int iThQLSubProf, int iSMargin)
{
	//主Medianでノイズを落とす
	const int iMMed = 11;//DC補間長さ20pixの情報は保持するマスクサイズ
	TTMA MedImg = *this;
	MedImg.MainMedian(iMMed);
	MedImg.SubMedian(iMMed);//斜めエッジ誤検出対策として追加(141113 ttsuji)

	//Log変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	TTMA MedLogImg = MedImg;
	MedLogImg.LogThreshold(dMidVal, dKeta, iLogTh);
	MedLogImg.WriteTma( AddPrefix(FileName, "LogMed" + IntToStr(iMMed) + "_") );



	int* edgeY;
	edgeY = new int[MainPixel];
	int edgeYAve;

	switch (bUpperEdge) {
	case true:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeU.y10[iM] / 10.);
		}
		edgeYAve = round((m_GlassEdgeU.ymax + m_GlassEdgeU.ymin) / 2.0 / 10.);
		break;
	case false:
		for (int iM = 0; iM < MainPixel; iM++) {
			edgeY[iM] = round(m_GlassEdgeD.y10[iM] / 10.);
		}
		edgeYAve = round((m_GlassEdgeD.ymax + m_GlassEdgeD.ymin) / 2.0 / 10.);
		break;
	}


	double* delta;
	delta = new double[MainPixel];
	int iKasa = 4000;
	TTMA CorrImg;
	CorrImg = *this;
	CorrImg.FillValue(iKasa);

	for (int iM = 0; iM < MainPixel; iM++){
		delta[iM] = (Dt[edgeY[iM]-2][iM]+Dt[edgeY[iM]-3][iM]
				  -Dt[edgeY[iM]+2][iM]-Dt[edgeY[iM]+3][iM])/2.;
	}

#if 0
	int isize=17;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//17個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#else
	int isize=14;
	float GlassProf[] = { 0.4, 0.9, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 1.2, 0.9, 0.4};//14個の台形プロファイル配列
	for (int i = 0; i < isize; i++){
		GlassProf[i] /= 1.2;
	}
#endif

	for (int iM = 0; iM < MainPixel; iM++){
		for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--){
//			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS]);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
			CorrImg.SetVal(iS, iM, delta[iM] * GlassProf[edgeY[iM] - iS] + iKasa);//U.F.発生し、ガラス段差補正量が0クリップした。(手指)
		}
	}
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCor.tma");
 #endif

#if 1 //石丸さん指摘（主Median二度がけ）により削除すべき？（141125 ttsuji）
	CorrImg.MainMedian(iMainMedianGlassProfile);
//	CorrImg.SubMedian(iMainMedianGlassProfile);//141118 tsuji（ノイズ対策）
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif
#endif
/*
	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
*/



 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpUDrate = fopen((ExtractFilePath(FileName) + "GlassCorUDQLRateMainProfile.txt").c_str(), "wt");
	fprintf(fpUDrate, "iM	QLUp	QLDown	QLRate	QLRate2\n");
 #endif

	//DC補間サンプル点のQL比の主プロファイル
	TFIMG fImgRateUD;
	fImgRateUD.NewDt(MainPixel, 1);
	fImgRateUD.Fill(0.);

	//QL比閾値（低周波情報除去画像）
//	const float fThQLRatioHighPass = 1.05;//平行四辺形補間する領域をできるだけ広げるため（141104 ttsuji)
	const float fThQLRatioHighPass = 1.1;//新default
//	const float fThQLRatioHighPass = 1.2;//default
//	const float fThQLRatioHighPass = 1.3;
//	const float fThQLRatioHighPass = 1.4;//やりすぎか

	for (int iM = 0; iM < MainPixel; iM++){
		int iS;
//		int iSstart = edgeY[iM] - isize + 1;
//		int iSend   = edgeY[iM];
		int iSstart = edgeY[iM] - isize + 4;
		int iSend   = edgeY[iM] - 3;

 #if 0
		float fAveU = (float)Dt[iSstart][iM];
		float fAveD = (float)Dt[iSend][iM];
 #else  //シンチ段差補正で精度アップした主・副Median後の画像での斜めエッジ検出の横展開（141120 ttsuji）
		float fAveU = (float)MedImg.Dt[iSstart][iM];
		float fAveD = (float)MedImg.Dt[iSend][iM];
 #endif

		float fQLRateUD = (fAveD) ? fAveU/fAveD : 1.;//DC成分補間する2点のQL比
		float fQLRateDU = (fAveU) ? fAveD/fAveU : 1.;//DC成分補間する2点のQL比
		fImgRateUD.Dt[iM][0] = fQLRateUD;
  #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpUDrate, "%d	%lf	%lf	%lf	%lf\n", iM, fAveU, fAveD, fQLRateUD, fQLRateDU);
  #endif
	}//end for iM

  #ifdef _LP_DEBUG_OUTPUT
	fclose(fpUDrate);
///	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg.tma");
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorrImg2.tma");

	FILE* fpDCItpl2 = fopen((ExtractFilePath(FileName) + "GlassCorQLrateHighPassDCItplMainProfile.txt").c_str(), "wt");
//	fprintf(fpDCItpl2, "iM	QLUp	QLDown	QLRate	QLRate2	flag	iS	Energy	Orientation	VmNorm	VmX	VmY	L2R	U2D	UL2DR	UR2DL\n");
	fprintf(fpDCItpl2, "iM	QLRate	flag	iS	Energy	Orientation	L2R	U2D	UL2DR	UR2DL\n");
  #endif


	//QL比の主プロファイルを主Medianし、低周波のみを抽出
	const int iMainMedQLRate = 201;
	TFIMG fImgRateUDMMed = fImgRateUD;
	fImgRateUDMMed.MainMedian(iMainMedQLRate);
	fImgRateUD -= fImgRateUDMMed;//低周波情報を除去し、高周波のみを抽出
	fImgRateUD += 1.;//１を加算し、UとDが等しいときを１倍とする
  #ifdef _LP_DEBUG_OUTPUT
	fImgRateUD.bSaveAsText((ExtractFilePath(FileName) + "GlassCorQLRateHighPass.txt").c_str(), true);
  #endif


	//QL比の主プロファイルのHighPass後のQL比から斜めエッジを検出（この方法だと、DC補間の20pix間の濃度変化の影響を受けず、精度の良いエッジ検出が可能）
	float fThQLRatioHighPass2 = 1 / fThQLRatioHighPass;
	double dOrientCut = 20.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 30.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 35.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
//	double dOrientCut = 40.;//0～dOrientCut度、および(180-dOrientCut)～180度は統計から除外する（横スジ状の補正残差の影響を除外するため）
	double* dOrientAtEnergyMax = new double[MainPixel];//iSをふったときにEnergy（周辺の微分値）が最大となるときの角度（ただしCut角は除く）
	double* dEnergyMax = new double[MainPixel];//iSをふったときの最大Energy
//	int* iSAtEnergyMax = new int[MainPixel];
	double* dSAtEnergyMax = new double[MainPixel];
	for (int iM = 0; iM < MainPixel; iM++){
		dEnergyMax[iM] = 0.;//初期化
		dOrientAtEnergyMax[iM] = 0.;//初期化
//		iSAtEnergyMax[iM] = 0;//初期化
		dSAtEnergyMax[iM] = 0.;//初期化
	}

	//ガラス段差補正領域のヒストグラムをとり、低線量領域は除外するようにする
    int iMainShiftMax = 30;
	Histogram(edgeYAve - 3, edgeYAve - 3, iMainShiftMax, MainPixel - 1 - iMainShiftMax);
#if 0
	float fThRate = 0.7;//ヒストの全面積の70％（感度比を計算する画素の判定に使用） ←多すぎて素抜けを含んでしまいNG
//	float fThRate = 0.5;//ヒストの全面積の50％（感度比を計算する画素の判定に使用）
//	float fThRate = 0.3;//ヒストの全面積の30％（感度比を計算する画素の判定に使用）
	int iThMinQL = HistRuisekiDosuuPercent(fThRate, true);
#else
	float fThRate;
	int iThMinQL = 0;//とりあえず問題画像用に
//	int iThMinQL = 5000;//とりあえず問題画像用に
#endif


	//主方向にスキャンし、QL比を越える主領域について、DC補間対象の副領域すべての微分値を調査
//	for (int iM = 0; iM < MainPixel; iM++){
	for (int iM = 1; iM < MainPixel - 1; iM++){//石丸さん指摘により、端にエッジがあった場合に実行エラーとなるはずなので、端を除外(140901 ttsuji)
//		int iSstart = edgeY[iM] - isize + 1;
//		int iSend   = edgeY[iM];
		int iSstart = edgeY[iM] - isize + 4;
		int iSend   = edgeY[iM] - 3;
		DIFFERENTIALARROUNDPIXELS* DifPixelsArray = new DIFFERENTIALARROUNDPIXELS[iSend - iSstart + 1];//ある点の微分情報

		bool bflag = false;//斜めエッジ対応フラグ

//		if (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2) {
		if (iThMinQL < Dt[iSend][iM] && (fImgRateUD.Dt[iM][0] >= fThQLRatioHighPass || fImgRateUD.Dt[iM][0] <= fThQLRatioHighPass2)) {//低線量領域は斜め対策から除外するようにする
				bflag = true;
			//DC補間する副領域すべての画素の微分値を調査するが、上下方向のエッジが強い画素は斜めエッジの角度算出から除く.
			double dAveUD = 0.;//上下方向の微分値の平均値
			double dAveSquareUD = 0.;//上下方向の微分値の二乗平均
			double dStdEvpUD = 0.;//上下方向の微分値の標準偏差
			double dParaExclude = 1.0;//上下方向の微分値が、平均±何σの範囲外であれば統計処理から除外するか
			for (int iS = iSstart; iS <= iSend; iS++) {
#if 0
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
#else
				DifPixelsArray[iS - iSstart] = CalcDifferentialArroundPixel_Fast(MedLogImg, iS, iM);//ある点の周囲の微分情報を計算する
#endif
				dAveUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff;
				dAveSquareUD += DifPixelsArray[iS - iSstart].Up2Down.dDiff * DifPixelsArray[iS - iSstart].Up2Down.dDiff;
			}
			dAveUD /= iSend - iSstart + 1;//副方向の平均値を計算
			dAveSquareUD /= iSend - iSstart + 1;//副方向の二乗平均を計算
			dStdEvpUD = sqrt(dAveSquareUD - dAveUD * dAveUD);//副方向の標準偏差を計算

			//上下方向の微分値が、平均±(σ*dParaExclude)の範囲内のときの斜めエッジを統計処理
			double dUDUpperLimit = dAveUD + dParaExclude * dStdEvpUD;//上下微分の上限値
			double dUDLowerLimit = dAveUD - dParaExclude * dStdEvpUD;//上下微分の下限値
			double dSAve = 0.;
			double dOrientAve = 0.;
        	double dSumEnergy = 0.;
			for (int iS = iSstart; iS <= iSend; iS++) {
				bool bFlag2 = false;
//				if (dUDLowerLimit < DifPixelsArray[iS - iSstart].Up2Down.dDiff && DifPixelsArray[iS - iSstart].Up2Down.dDiff < dUDUpperLimit && dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
				if (dOrientCut < DifPixelsArray[iS - iSstart].dOrientation && DifPixelsArray[iS - iSstart].dOrientation < 180. - dOrientCut) {
					bFlag2 = true;
					dSAve += iS * DifPixelsArray[iS - iSstart].dEnergy;
					dOrientAve += DifPixelsArray[iS - iSstart].dOrientation * DifPixelsArray[iS - iSstart].dEnergy;
					dSumEnergy += DifPixelsArray[iS - iSstart].dEnergy;

					if (dEnergyMax[iM] < DifPixelsArray[iS - iSstart].dEnergy) {
						dEnergyMax[iM] = DifPixelsArray[iS - iSstart].dEnergy;//dEnergyMaxを更新
					}
				}
   #ifdef _LP_DEBUG_OUTPUT
				fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf	%lf	%lf	%lf	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag, iS, DifPixelsArray[iS - iSstart].dEnergy, DifPixelsArray[iS - iSstart].dOrientation, DifPixelsArray[iS - iSstart].Left2Right, DifPixelsArray[iS - iSstart].Up2Down, DifPixelsArray[iS - iSstart].UpLeft2DownRight, DifPixelsArray[iS - iSstart].UpRight2DownLeft, bFlag2);
   #endif
			}
			if (!dSumEnergy) {
				continue;//上下微分が平均±(σ*dParaExclude)の範囲内で、かつdOrientCut度～(180-dOrientCut)度の範囲内のものがなければエッジなしと判断
			}
			dSAve /= dSumEnergy;
			dOrientAve /= dSumEnergy;
//			iSAtEnergyMax[iM] = round(dSAve);
			dSAtEnergyMax[iM] = dSAve;
			dOrientAtEnergyMax[iM] = dOrientAve;

   #ifdef _LP_DEBUG_OUTPUT
//			fprintf(fpDCItpl2, "%d	%lf	%d	%d	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, iSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
			fprintf(fpDCItpl2, "%d	%lf	%d	%lf	%lf	%lf\n", iM, fImgRateUD.Dt[iM][0], bflag, dSAtEnergyMax[iM], dEnergyMax[iM], dOrientAtEnergyMax[iM]);
   #endif
		}
		else{
			bflag = false;
   #ifdef _LP_DEBUG_OUTPUT
			fprintf(fpDCItpl2, "%d	%lf	%d\n", iM, fImgRateUD.Dt[iM][0], bflag);
   #endif
		}

		delete[] DifPixelsArray;
	}

  #ifdef _LP_DEBUG_OUTPUT
//	CorrImg.WriteTma(ExtractFilePath(FileName) + "CorrImg2.tma");
	fclose(fpDCItpl2);
  #endif

	//QL比を越える主領域について、グルーピングしてグループごとのエッジ角度を決めて、斜め方向のDC成分補間を行う
	int iMGrpStart = 0;
	int iMGrpEnd = 0;
	double dMGrpAve = 0.;
	double dOrientGrpAve = 0.;
	double dEnergyGrpMax = 0.;
	double dEnergyGrpAve = 0.;
	double dSAtGrpAve = 0.;
	double dCutRate = 0.3;//EnergyがMaxのdCutRate倍以下は統計から除外する
  #ifdef _LP_DEBUG_OUTPUT
	FILE* fpDCItpl3 = fopen((ExtractFilePath(FileName) + "GlassCorEdgePositonInformation.txt").c_str(), "wt");
	fprintf(fpDCItpl3, "iThMinQL	fThRate\n");
	fprintf(fpDCItpl3, "%d	%f\n", iThMinQL, fThRate);
	fprintf(fpDCItpl3, "iMGrpStart	iMGrpEnd	dMGrpAve	dOrientGrpAve	dEnergyGrpAve	dMItpl1	dMItpl2\n");
  #endif
	for (int iM = 0; iM < MainPixel; iM++){
//		if (!iSAtEnergyMax[iM]) {
		if (0.0 == dSAtEnergyMax[iM]) {
			continue;
		}

		iMGrpStart = iM;
		dMGrpAve = 0.;
		dOrientGrpAve = 0.;
		dEnergyGrpMax = 0.;
		dEnergyGrpAve = 0.;
		dSAtGrpAve = 0.;
		//QL比を越える主領域が続く終点を探索
//		while (iSAtEnergyMax[iM] && iM < MainPixel){
		while (0.0 != dSAtEnergyMax[iM] && iM < MainPixel){
			iMGrpEnd = iM;
			if (dEnergyGrpMax < dEnergyMax[iM]) {
             	dEnergyGrpMax = dEnergyMax[iM];
			}
			iM++;
		}

		int iCnt = 0;
		for (int iMtmp = iMGrpStart; iMtmp <= iMGrpEnd; iMtmp++) {
			if (dEnergyMax[iMtmp] < dEnergyGrpMax * dCutRate) {
				continue;
			}
			dOrientGrpAve += dOrientAtEnergyMax[iMtmp];
			dEnergyGrpAve += dEnergyMax[iMtmp];
			dMGrpAve += iMtmp * dEnergyMax[iMtmp];
//			dSAtGrpAve += iSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
			dSAtGrpAve += dSAtEnergyMax[iMtmp] * dEnergyMax[iMtmp];
   #ifdef _LP_DEBUG_OUTPUT
//			fprintf(fpDCItpl3, "	%d	%d	%lf	%lf\n", iMtmp, iSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
			fprintf(fpDCItpl3, "	%d	%lf	%lf	%lf\n", iMtmp, dSAtEnergyMax[iMtmp], dOrientAtEnergyMax[iMtmp], dEnergyMax[iMtmp]);
   #endif
			iCnt++;
		}
		if (!iCnt) {
			continue;
		}
		dMGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dSAtGrpAve /= dEnergyGrpAve;//エネルギー総和で割って規格化
		dOrientGrpAve /= iCnt;
		dEnergyGrpAve /= iCnt;

		//斜めエッジとDC補間の副領域の交差点のｘ座標
		double dMItpl1 = 0.;//補間の開始ｘ座標
		double dMItpl2 = 0.;//補間の終了ｘ座標
//		int iSItpl1 = edgeY[iM] - isize + 1;
//		int iSItpl2 = edgeY[iM];
		int iSItpl1 = edgeY[round(dMGrpAve)] - isize + 1;//どのｘでのedgeYなのかを明確化(140816 ttsuji)
		int iSItpl2 = edgeY[round(dMGrpAve)]; //どのｘでのedgeYなのかを明確化(140816 ttsuji)

		double dTan = tan(PI/180. * dOrientGrpAve);
		dMItpl1 = (iSItpl1 - dSAtGrpAve) / dTan + dMGrpAve;
		dMItpl2 = (iSItpl2 - dSAtGrpAve) / dTan + dMGrpAve;
		double dTmp;
		if (dMItpl1 > dMItpl2) {//dMItpl1 < dMItpl2 となるようにソートする
			dTmp = dMItpl1;
			dMItpl1 = dMItpl2;
			dMItpl2 = dTmp;
		}

   #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpDCItpl3, "%d	%d	%lf	%lf	%lf	%lf	%lf	%lf\n", iMGrpStart, iMGrpEnd, dMGrpAve, dSAtGrpAve, dOrientGrpAve, dEnergyGrpAve, dMItpl1, dMItpl2);
   #endif

		//斜め方向のガラス段差補正
//		LPGlassCorNanameROI(CorrImg, delta, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, isize, GlassProf);
		LPGlassCorNanameROIHeikohShihenkei(CorrImg, delta, edgeY, iSItpl1, iSItpl2, CLIP((int)(dMItpl1 + 0.5), 0, MainPixel - 1), CLIP((int)(dMItpl2 + 0.5), 0, MainPixel - 1), dTan, dSAtGrpAve, dMGrpAve, iKasa, isize, GlassProf);
	}

   #ifdef _LP_DEBUG_OUTPUT
	fclose(fpDCItpl3);
//	ItplImg.WriteTma(ExtractFilePath(FileName) + "ItplImg2.tma");
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorrImg3.tma");
   #endif
	delete[] dOrientAtEnergyMax;
	delete[] dEnergyMax;
//	delete[] iSAtEnergyMax;
	delete[] dSAtEnergyMax;


	CorrImg.MainMedian(iMainMedianGlassProfile);
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeMainMedian" + IntToStr(iMainMedianGlassProfile) + ".tma");
 #endif


	// 副方向にサーチし、QL比が一定値を超える場合は、金尺目盛があるとみなし、補正画像の該当ｘ領域外の画素値でｘ方向に線形補間する
	if (bDetectKanajakuForGlassCor) {//金尺目盛検出しガラス段差補正に写った金尺を変える
//		const int iDx = 5;//エッジ探索するｘの間隔
		const int iDx = 1;//エッジ探索するｘの間隔
 #if 0  //iThQLSubProfは引数指定
//		const int iThQLSubProf = 2000;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 1800;//副微分プロファイルのQL閾値(現在の設定)
		const int iThQLSubProf = 1200;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 1100;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 800;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 700;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 600;//副微分プロファイルのQL閾値
//		const int iThQLSubProf = 300;//副微分プロファイルのQL閾値
 #endif
//		const int iThQLMainProf = 2000;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1800;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1500;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1300;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1100;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 1000;//主微分プロファイルのQL閾値
		const int iThQLMainProf = 800;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 700;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 600;//主微分プロファイルのQL閾値
//		const int iThQLMainProf = 300;//主微分プロファイルのQL閾値
		const int iSubOffset = 20;//ガラス段差よりも下方にもエッジ探索する範囲
		int iNumOfXROI = 0;//検出したPbスケールのROI数（DC補間領域外を用いた補正用）
		int iNumOfXROIMed = 0;//検出したPbスケールのROI数(主Median用)
		ROI XROI[512];//検出したｘ領域（DC補間領域外を用いた補正用）
		ROI XROIMed[512];//検出したｘ領域(主Median用)
		for (int iROI = 0; iROI < 512; iROI++) {
			XROI[iROI].x1 = XROI[iROI].x2 = XROI[iROI].y1 = XROI[iROI].y2 = XROI[iROI].Pattern = 0;
			XROIMed[iROI].x1 = XROIMed[iROI].x2 = XROIMed[iROI].y1 = XROIMed[iROI].y2 = XROIMed[iROI].Pattern = 0;
		}

		int* iEdgeYSinti;
		iEdgeYSinti = new int[MainPixel];

		switch (bUpperEdge) {
		case true:
			for (int iM = 0; iM < MainPixel; iM++) {
				iEdgeYSinti[iM] = round(m_SintiEdgeU.y10[iM] / 10.);
			}
			break;
		case false:
			for (int iM = 0; iM < MainPixel; iM++) {
				iEdgeYSinti[iM] = round(m_SintiEdgeD.y10[iM] / 10.);
			}
			break;
		}

		TTMA MedLogThis;
		MedLogThis = *this;
#if 1   //test
		MedLogThis.MainMedian(iMMed);
		MedLogThis.SubMedian(iMMed); //斜めエッジ誤検出対策（141113 ttsuji)
#endif
		//Log変換
		MedLogThis.LogThreshold(dMidVal, dKeta, iLogTh);
		MedLogThis.WriteTma( AddPrefix(FileName, "LogMedThis") );

		//シンチ段差からガラス段差＋iSubOffsetのｙレンジの中で微分閾値判定（シンチ段差を上限としたのはシンチ段差を誤検出しないため）
		LPDetectPbScaleEdgeXROI_Glass(MedLogThis, FileName, edgeY, iEdgeYSinti, iDx, iThQLSubProf, iThQLMainProf, iSubOffset, iNumOfXROI, XROI, iSMargin);

		//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。
//		LPMainMedianForEachROI(CorrImg, edgeY, iNumOfXROI, XROI, iMainMedianDCItpl, iSubRangeOnlyMidPanel, iItplSubOffset, iKasa);
//		LPMainMedianAndCopyForEachROI(CorrImg, FileName, edgeY, iNumOfXROI, XROI, iNumOfXROIMed, XROIMed, iMainMedianDCItpl, iSubRangeOnlyMidPanel, iItplSubOffset, iKasa, iMedianHasiSyori, iMarginXMed);

		//ROIの左右端の画素にて線形補間する
		TTMA CorrImgTmp;
		CorrImgTmp = CorrImg;
//		const int iMMargin = 5;
//		const int iMMargin = 10;
//		const int iMMargin = 1;
//		const int iMMargin = 2;
		const int iMMargin = 3;
		const int ith = 180;
		int iMs, iMe, iSs, iSe;
		for (int iROI = 0; iROI < iNumOfXROI; iROI++) {
//			int iMs = CLIP(XROI[iROI].x1 - iMMargin, 0, MedLogThis.MainPixel - 1);
//			int iMe = CLIP(XROI[iROI].x2 + iMMargin, 0, MedLogThis.MainPixel - 1);
			//x始点の端処理
//			if (0 == iMs) {
			if (XROI[iROI].x1 - iMMargin < 0) {
				iMs = CLIP(XROI[iROI].x1 - iMMargin, 0, MedLogThis.MainPixel - 1);
				iMe = CLIP(XROI[iROI].x2 + iMMargin, 0, MedLogThis.MainPixel - 1);
				for (int iM = 0; iM < iMe; iM++) {
					iSs = edgeY[iMs];
					iSe = edgeY[iMe];
					for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--, iSe--){
//						CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iS][iMe]);//x=iMeでdupe
						CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iSe][iMe]);//x=iMeでdupe
					}
				}
			}
			//x終点の端処理
//			else if (MedLogThis.MainPixel - 1 == iMe) {//x1側でdupe
			else if (XROI[iROI].x2 + iMMargin > MedLogThis.MainPixel - 1) {//x1側でdupe
				iMs = CLIP(XROI[iROI].x1 - iMMargin, 0, MedLogThis.MainPixel - 1);
				iMe = CLIP(XROI[iROI].x2 + iMMargin, 0, MedLogThis.MainPixel - 1);
				for (int iM = iMs + 1; iM <= iMe; iM++) {
					iSs = edgeY[iMs];
					iSe = edgeY[iMe];
					for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--, iSs--){
//						CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iS][iMs]);//x=iMsでdupe
						CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iSs][iMs]);//x=iMsでdupe
					}
				}
			}
			//金尺が画像内部に存在する場合の処理
			else{
				iMs = CLIP(XROI[iROI].x1 - iMMargin, 0, MedLogThis.MainPixel - 1);
				iMe = CLIP(XROI[iROI].x2 + iMMargin, 0, MedLogThis.MainPixel - 1);
				for (int iM = iMs + 1; iM < iMe; iM++) {
					iSs = edgeY[iMs];
					iSe = edgeY[iMe];
//					for (int iS = iEdgeYSinti[iM]; iS < edgeY[iM]; iS++) {
					for (int iS = edgeY[iM]; iS > edgeY[iM] - isize; iS--, iSs--, iSe--){
 #ifndef _ADAPTIVE_ITPL_BY_CORR_QL
						CorrImgTmp.SetVal(iS, iM, (float)(CorrImg.Dt[iS][iMs] * (iMe - iM) + CorrImg.Dt[iS][iMe] * (iM - iMs)) / (iMe - iMs));
 #else	//桑原さんアルゴ（補正画像の画素値によって、線形補間するか片方をdupeするか切替）
  #if 0		//包含できていないケースあり、bug
						if ((abs(CorrImg.Dt[iS][iMs] - iKasa) > ith && abs(CorrImg.Dt[iS][iMe] -iKasa) > ith )
						 || (abs(CorrImg.Dt[iS][iMs] -iKasa) < ith && abs(CorrImg.Dt[iS][iMe] -iKasa) < ith)){
							CorrImgTmp.SetVal(iS, iM, (float)(CorrImg.Dt[iS][iMs] * (iMe - iM) + CorrImg.Dt[iS][iMe] * (iM - iMs)) / (iMe - iMs));
						}
						else if (abs(CorrImg.Dt[iS][iMs] -iKasa) > ith) {
							CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iS][iMe]);
						}
						else {
							CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iS][iMs]);
						}
  #else                 //MC実装にあわせこみ
//						if (abs(CorrImg.Dt[iS][iMs] - iKasa) > ith && abs(CorrImg.Dt[iS][iMe] -iKasa) <= ith ){//補正量の小さいiMeを使う
						if (abs(CorrImg.Dt[iSs][iMs] - iKasa) > ith && abs(CorrImg.Dt[iSe][iMe] -iKasa) <= ith ){//補正量の小さいiMeを使う
//							CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iS][iMe]);
							CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iSe][iMe]);
						}
//						else if (abs(CorrImg.Dt[iS][iMs] - iKasa) <= ith && abs(CorrImg.Dt[iS][iMe] -iKasa) > ith ){//補正量の小さいiMsを使う
						else if (abs(CorrImg.Dt[iSs][iMs] - iKasa) <= ith && abs(CorrImg.Dt[iSe][iMe] -iKasa) > ith ){//補正量の小さいiMsを使う
//							CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iS][iMs]);
							CorrImgTmp.SetVal(iS, iM, CorrImg.Dt[iSs][iMs]);
						}
						else{//iMs, iMeの両方で線形補間する
//							CorrImgTmp.SetVal(iS, iM, (float)(CorrImg.Dt[iS][iMs] * (iMe - iM) + CorrImg.Dt[iS][iMe] * (iM - iMs)) / (iMe - iMs));
							CorrImgTmp.SetVal(iS, iM, (float)(CorrImg.Dt[iSs][iMs] * (iMe - iM) + CorrImg.Dt[iSe][iMe] * (iM - iMs)) / (iMe - iMs));
						}
  #endif
 #endif
					}
				}
			}
		}
		CorrImg = CorrImgTmp;
		delete[] iEdgeYSinti;
  #ifdef _LP_DEBUG_OUTPUT
		CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeAndPbROI.tma");
  #endif

  #ifdef _ADAPTIVE_ITPL_BY_CORR_QL
		const int iMedAdptItpl = 3;
		CorrImg.MainMedian(iMedAdptItpl);
		CorrImg.SubMedian(iMedAdptItpl);
   #ifdef _LP_DEBUG_OUTPUT
		CorrImg.WriteTma(ExtractFilePath(FileName) + "GlassCorNanameEdgeAndPbROI_MMed" + IntToStr(iMedAdptItpl) + ".tma");
   #endif
  #endif
	}
	delete[] edgeY;
	delete[] delta;

	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iM, Dt[iS][iM] - CorrImg.Dt[iS][iM] + iKasa);
		}
	}
}
//---------------------------------------------------------------------------
//左右の照射野境界を検出

#define _LIMIT_SEARCH_RANGE

void TSIMARS::LPDetectBorderRadiationField(string FileName, TTMA& SmoothImageUp, int iThQLMDif, bool& bExist0, bool& bExistEnd, int& iMBorder0, int& iMBorderEnd, int iSSearchRadiationField0, int iSSearchRadiationFieldEnd, int iLimitSearchRange)
{
	//例外処理
	if (iSSearchRadiationField0 > iSSearchRadiationFieldEnd) {
		iSSearchRadiationField0 = iSSearchRadiationFieldEnd;
	}

	//ログ変換してから主微分をとる
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	string StrPre = "";
	TTMA LogImage = SmoothImageUp;
#ifndef _LOWDOSE_LINEAR_TABLE
	LogImage.LogThreshold(dMidVal, dKeta, iLogTh);
	LogImage.BitChange(14);
#else
	LogImage.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
#endif


#ifdef _LP_DEBUG_OUTPUT
	LogImage.WriteTma(ExtractFilePath(FileName) + "BLSmoothImageUp.tma");
	FILE* fp1 = fopen(string(ExtractFilePath(FileName) + "MainProfLog(" + IntToStr(iSSearchRadiationField0) + "," + IntToStr(iSSearchRadiationFieldEnd) + ").txt").c_str(), "wt");
	FILE* fp2 = fopen((ExtractFilePath(FileName) + "MainDiffProfLog(" + IntToStr(iSSearchRadiationField0) + "," + IntToStr(iSSearchRadiationFieldEnd) + ").txt").c_str(), "wt");
	fprintf(fp1, "iM	MainQL\n");
	fprintf(fp2, "iM	MainDiffQL\n");
#endif

	TTMA MainProfLog, MainDiffProfLog;
	MainProfLog.NewDt(1, LogImage.MainPixel, LogImage.Bit);
	MainDiffProfLog.NewDt(1, LogImage.MainPixel, LogImage.Bit);

	int iM, iS;
	for (iM = 0; iM < LogImage.MainPixel; iM++) {
		float fSum = 0.;
		for (iS = iSSearchRadiationField0; iS <= iSSearchRadiationFieldEnd; iS++) {
			fSum += LogImage.Dt[iS][iM];
		}
		float fAve = fSum / (iSSearchRadiationFieldEnd - iSSearchRadiationField0 + 1);
		MainProfLog.SetVal(0, iM, fAve);
#ifdef _LP_DEBUG_OUTPUT
		fprintf(fp1, "%d	%f\n", iM, fAve);
#endif
	}
	MainDiffProfLog = MainProfLog;
	MainDiffProfLog.DifMainUp();//主方向に微分
#ifdef _LP_DEBUG_OUTPUT
	for (int iM = 0; iM < LogImage.MainPixel; iM++) {
		fprintf(fp2, "%d	%d\n", iM, MainDiffProfLog.Dt[0][iM]);
    }
#endif


	int iKasa = MainDiffProfLog.DtMax / 2 + 1;//U.F.防止のためのかさあげQL
	int iMainCut = 30;//左右位置ずれによる空白領域との境界を避けるため

	//x=0側から＋方向に照射野境界を探索
	iM = iMainCut;
#ifndef _LIMIT_SEARCH_RANGE
	while (!bExist0 && iM < MainDiffProfLog.MainPixel - iMainCut){
#else
//	const int iLimitSearchRange = 1600;
//	while (!bExist0 && iM < MainDiffProfLog.MainPixel / 2){
	while (!bExist0 && iM < iLimitSearchRange){
#endif
		if (MainDiffProfLog.Dt[0][iM] > iKasa + iThQLMDif) {
			bExist0 = true;
			iMBorder0 = iM;
		}
		iM++;
	}
	//x=MainPixel - 1側から-方向に照射野境界を探索
	iM = MainDiffProfLog.MainPixel - 1 - iMainCut;
#ifndef _LIMIT_SEARCH_RANGE
	while (!bExistEnd && iM > iMainCut){
#else
//	while (!bExistEnd && iM > MainDiffProfLog.MainPixel / 2){
	while (!bExistEnd && iM > MainDiffProfLog.MainPixel - 1 - iLimitSearchRange){
#endif
		if (MainDiffProfLog.Dt[0][iM] < iKasa - iThQLMDif) {
			bExistEnd = true;
			iMBorderEnd = iM;
		}
		iM--;
	}
  #ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\niMBorder0Tmp	iMBorderEndTmp\n");
	fprintf(fp2, "\%d	%d\n", iMBorder0, iMBorderEnd);
  #endif
	//エラーチェック①
	//左右ともに照射野境界を見つけた場合は、左右照射野境界の位置の異常をチェック
	if (bExist0 && bExistEnd) {
		if (iMBorder0 > iMBorderEnd) {
			bExist0 = bExistEnd = false;
			iMBorder0 = 0;
			iMBorderEnd = SmoothImageUp.MainPixel - 1;
		}
	}
  #ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\niMBorder0Tmp	iMBorderEndTmp\n");
	fprintf(fp2, "\%d	%d\n", iMBorder0, iMBorderEnd);
  #endif

	//エラーチェック②
	//境界の内外のQL値の大小関係チェック
    const int iOffsetMainShift = 10;//平均QLを計算するために、検出した照射野境界からの主方向のオフセットをもたせる
	double dAve0(0.), dAveEnd(0.), dAveROI(0.);
	if (bExist0 && bExistEnd) {
		dAve0 = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMainCut, iMBorder0 - iOffsetMainShift);
		dAveROI  = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMBorder0 + iOffsetMainShift, iMBorderEnd - iOffsetMainShift);
		dAveEnd = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMBorderEnd + iOffsetMainShift, SmoothImageUp.MainPixel - 1 - iMainCut);
		if (dAve0 > dAveROI || dAveROI < dAveEnd) {
			bExist0 = false;
			iMBorder0 = 0;
		}
	}
	else if (bExist0) {//ｘ＝０側の照射野境界より外の平均QLとｘ全体の平均QLの大小をチェック
		dAve0 = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMainCut, iMBorder0 - iOffsetMainShift);
		dAveROI  = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMBorder0 + iOffsetMainShift, SmoothImageUp.MainPixel - 1 - iMainCut);
		if (dAve0 > dAveROI) {
			bExist0 = false;
			iMBorder0 = 0;
		}
	}
	else if (bExistEnd) {//ｘ＝End側の照射野境界より外の平均QLとｘ全体の平均QLの大小をチェック
		dAveROI = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMainCut, iMBorderEnd - iOffsetMainShift);
		dAveEnd = SmoothImageUp.AverageD(iSSearchRadiationField0, iSSearchRadiationFieldEnd, iMBorderEnd + iOffsetMainShift, SmoothImageUp.MainPixel - 1 - iMainCut);
		if (dAveEnd > dAveROI) {
			bExistEnd = false;
			iMBorderEnd = SmoothImageUp.MainPixel - 1;
		}
	}


#ifdef _LP_DEBUG_OUTPUT
	fprintf(fp2, "\niMBorder0	iMBorderEnd\n");
	fprintf(fp2, "\%d	%d\n", iMBorder0, iMBorderEnd);
	fprintf(fp2, "\ndAveROI	dAve0	dAveEnd\n");
	fprintf(fp2, "%lf	%lf	%lf\n", dAveROI, dAve0, dAveEnd);
	fclose(fp1);
	fclose(fp2);
#endif

}
//---------------------------------------------------------------------------
//DC補間領域内の横長の白い金属エッジを探索する(141108 ttsuji 金尺の目盛り消失対策として)

void TSIMARS::LPDetectPbScaleWhiteBandXROI(TTMA MedLogImg, string FileName, int* edgeY, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubRangeOnlyMidPanel, int iItplSubOffset, int& iNumOfXROI, ROI* XROI)
{
	int iM, iS, iM2, iS2, iCnt(0);
	int iMainCut = 30;//左右位置ずれによる空白領域との境界を避けるため

	TTMA SubDifMedLogImg;
	SubDifMedLogImg = MedLogImg;
	SubDifMedLogImg.DifSubUp();//副微分（Dt[0]はそのまま、Dt[1]は Dt[1] - Dt[0]）
	int iKasa = SubDifMedLogImg.DtMax / 2 + 1;
	bool* bflag = new bool[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
//	int* iSAtMinQL = new int[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];

#ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "PbScaleXROI.txt").c_str(), "wt");
	fprintf(fp, "iD	iM	iSUpperEdge	iSLowerEdge	iSAtMinQL	iMinQL\n");
#endif

	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
		bflag[iCnt] = false;
//		iSAtMinQL[iCnt] = -100;
		bool bflagExistUpperEdge = false;
		int iSUpperEdge, iSLowerEdge;
		int iSAtMinQL;

		for (iS = iSstart + 1; iS <= iSend; iS++) {
			if (!bflagExistUpperEdge) {
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf) {//副微分値が閾値を下回る場合
					bflagExistUpperEdge = true;
					iSUpperEdge = iS;
                	continue;
				}
			}
			else{
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) {//副微分値が閾値を上回る場合
					bflag[iCnt] = true;
					iSLowerEdge = iS;

					//最小QLとなるy座標を計算
					int iMinQL = MedLogImg.DtMax;
					int iSAtMinQLTmp = -100;
					for (iS2 = iSUpperEdge + 1; iS2 < iSLowerEdge; iS2++) {
						if (iMinQL > MedLogImg.Dt[iS2][iM]) {
							iMinQL = MedLogImg.Dt[iS2][iM];
							iSAtMinQLTmp = iS2;
						}
					}
//					iSAtMinQL[iCnt] = iSAtMinQLTmp;
#ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d	%d\n", iCnt, iM, iSUpperEdge, iSLowerEdge, iSAtMinQLTmp, iMinQL);
#endif
//					iNumOfXROI++;
					break;
				}
			}
		}

	}

#ifdef _LP_DEBUG_OUTPUT
	fclose(fp);
	FILE* fp2 = fopen((ExtractFilePath(FileName) + "PbScaleXROI2.txt").c_str(), "wt");
	fprintf(fp2, "ID	iMStart	iMEnd	MLength\n");
#endif

	//連続しているｘで区分化し、区分の個数をカウント
	iCnt = 0;
	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
		if (!bflag[iCnt]) {
			continue;
		}

		XROI[iNumOfXROI].x1 = iM;
		while (bflag[iCnt] && iM < MedLogImg.MainPixel - iMainCut){
			iM += iDx;
			iCnt++;

		}
 /*		if (iM >= MedLogImg.MainPixel - iMainCut) {
			break;
		}
 */
		XROI[iNumOfXROI].x2 = iM - iDx;
		iNumOfXROI++;
	}




#ifdef _LP_DEBUG_OUTPUT
	for (int iROI = 0; iROI < iNumOfXROI; iROI++) {
		fprintf(fp2, "%d	%d	%d	%d\n", iROI, XROI[iROI].x1, XROI[iROI].x2, (XROI[iROI].x2 - XROI[iROI].x1));
	}
	fclose(fp2);
#endif
	delete[] bflag;
//    delete[] iSAtMinQL;
}
//---------------------------------------------------------------------------
//DC補間領域内の横長の金属エッジ(白帯、黒帯、エッジのみも含む)を探索する(141110 ttsuji 金尺の目盛り消失対策として)

#define _ABNORMAL_CASE //141201 ttsuji

void TSIMARS::LPDetectPbScaleEdgeXROI(TTMA MedLogImg, string FileName, int* edgeY, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubRangeOnlyMidPanel, int iItplSubOffset, int& iNumOfXROI, ROI* XROI, int& iNumOfXROIMed, ROI* XROIMed, bool& bExistPbScale)
{
	int iM, iS, iM2, iS2, iCnt(0);
//	int iMainCut = 30;//左右位置ずれによる空白領域との境界を避けるため
	int iMainCut = 0;//左右位置ずれによる空白領域との境界を避けるため

	TTMA SubDifMedLogImg, MainDifMedLogImg;
	SubDifMedLogImg = MedLogImg;
	SubDifMedLogImg.DifSubUp();//副微分（Dt[0]はそのまま、Dt[1]は Dt[1] - Dt[0]）
 #ifdef _LP_DEBUG_OUTPUT
	SubDifMedLogImg.WriteTma(ExtractFilePath(FileName) + "SubDifMedLogImg.tma");
 #endif
	MainDifMedLogImg = MedLogImg;
	MainDifMedLogImg.DifMainUp();//主微分 （Dt[0]はそのまま、Dt[1]は Dt[1] - Dt[0]）
 #ifdef _LP_DEBUG_OUTPUT
	MainDifMedLogImg.WriteTma(ExtractFilePath(FileName) + "MainDifMedLogImg.tma");
 #endif
	int iKasa = SubDifMedLogImg.DtMax / 2 + 1;
	bool* bflagExistB2WEdge = new bool[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
	bool* bflagExistW2BEdge = new bool[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
	int* iSEdgeW2B = new int[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
	int* iSEdgeB2W = new int[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
//	int* iSAtMinQL = new int[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
//	const int iSMargin = 1;//シンチ段差位置からのマージン
	const int iSMargin = 2;//シンチ段差位置からのマージン
//	const int iSMargin = 3;//シンチ段差位置からのマージン
	const int iMSearchLimit = 200;//金尺の長い方の目盛り長さが160pixのためそれを包含するサイズをエイッと決めた.
//	const int iMSearchLimit = 400;//金尺の長い方の目盛り長さが160pixのためそれを包含するサイズをエイッと決めた.
	int iMEdge0, iMEdgeE;

#ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "PbScaleXROI.txt").c_str(), "wt");
//	fprintf(fp, "iD	iM	iSEdge	DeltaQLEdge\n");
//	fprintf(fp, "iD	iM	flag	iSEdgeW2B	iSEdgeB2W	SubDeltaQLEdge	iMEdge0	iMEdgeE\n");
	fprintf(fp, "iD	iM	iSEdgeW2B	iSEdgeB2W	SubDeltaQLEdge	iMEdge0	iMEdgeE\n");
#endif


	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
//	for (iM = 0; iM < MedLogImg.MainPixel; iM += iDx, iCnt++) {
		int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
		int iSend = edgeY[iM] + iItplSubOffset;
//		bflag[iCnt] = false;
//		iSAtMinQL[iCnt] = -100;
		bflagExistB2WEdge[iCnt] = false;
		bflagExistW2BEdge[iCnt] = false;
//		int iSUpperEdge(0), iSLowerEdge(0);
//		int iSEdgeB2W(0), iSEdgeW2B(0);
		iSEdgeB2W[iCnt] = iSEdgeW2B[iCnt] = 0;
//		int iSAtMinQL;


#ifndef _ABNORMAL_CASE
		for (iS = iSstart + 1; iS <= iSend; iS++) {
#elif 0	//DC補間の境界で濃度が急激に変化した場合でも検出可能にするように範囲を広げた(141201 ttsuji)
		for (iS = iSstart; iS <= iSend + 1; iS++) {
#elif 1
		for (iS = iSstart; iS <= iSend; iS++) {//矩形アーチファクト対策で探索範囲を狭めた（151008 ttsuji）
#endif

#if 0
			if (!bflagExistUpperEdge) {
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf) {//副微分値が閾値を下回る場合
					bflagExistUpperEdge = true;
					iSUpperEdge = iS;
					continue;
				}
			}
			else{
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) {//副微分値が閾値を上回る場合
					bflag[iCnt] = true;
					iSLowerEdge = iS;

					//最小QLとなるy座標を計算
					int iMinQL = MedLogImg.DtMax;
					int iSAtMinQLTmp = -100;
					for (iS2 = iSUpperEdge + 1; iS2 < iSLowerEdge; iS2++) {
						if (iMinQL > MedLogImg.Dt[iS2][iM]) {
							iMinQL = MedLogImg.Dt[iS2][iM];
							iSAtMinQLTmp = iS2;
						}
					}
//					iSAtMinQL[iCnt] = iSAtMinQLTmp;
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d	%d\n", iCnt, iM, iSUpperEdge, iSLowerEdge, iSAtMinQLTmp, iMinQL);
 #endif
//					iNumOfXROI++;
					break;
				}
			}
#elif 0
			if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf || //副微分値が閾値を下回る場合
				SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) { //副微分値が閾値を上回る場合
				bflag[iCnt] = true;
				iSUpperEdge = iS;
 #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp, "%d	%d	%d	%d\n", iCnt, iM, iSEdgeW2B, iSEdgeB2W, (SubDifMedLogImg.Dt[iS][iM] - iKasa));
 #endif
				break;
			}
#elif 1     //PbスケールがDC補間領域に重複する4通りのパターンを網羅(141111 tsuji)
			if (!bflagExistB2WEdge[iCnt] & !bflagExistW2BEdge[iCnt]) {
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf) {//副微分値が閾値を下回る場合（黒→白の方向）
					bflagExistB2WEdge[iCnt] = true;
					bExistPbScale = true;
					iSEdgeB2W[iCnt] = iS;
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d\n", iCnt, iM, iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa));
 #endif
					continue;
				}
				else if (SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) {//副微分値が閾値を上回る場合（白→黒の方向）
					bflagExistW2BEdge[iCnt] = true;
					bExistPbScale = true;
					iSEdgeW2B[iCnt] = iS;
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d\n", iCnt, iM, iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa));
 #endif
					continue;
				}
			}
			else if (bflagExistB2WEdge[iCnt]) {//既に黒→白の方向のエッジを検出している場合
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) {//副微分値が閾値を上回る場合（白→黒の方向）
					bflagExistW2BEdge[iCnt] = true;
					bExistPbScale = true;
					iSEdgeW2B[iCnt] = iS;
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d\n", iCnt, iM, iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa));
 #endif
					break;
				}
			}
			else if (bflagExistW2BEdge[iCnt]) {//既に白→黒の方向のエッジを検出している場合
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf) {//副微分値が閾値を下回る場合（黒→白の方向）
					bflagExistB2WEdge[iCnt] = true;
					bExistPbScale = true;
					iSEdgeB2W[iCnt] = iS;
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d\n", iCnt, iM, iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa));
 #endif
					break;
				}
			}
			else{
				break;
			}
#else       //Pbスケールの横エッジとシンチ段差の横エッジが重複したときの区別を追加(141116 tsuji)
            iMEdgeE = iMEdge0 = -100;
			if (!bflagExistB2WEdge[iCnt] & !bflagExistW2BEdge[iCnt]) {//エッジを未検出の場合
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf) {//副微分値が閾値を下回る場合（黒→白の方向）
//					bflagExistB2WEdge[iCnt] = true;
					iSEdgeB2W[iCnt] = iS;
//					if (edgeY[iM] - iSMargin <= iS && iS <= edgeY[iM] + iSMargin) {//シンチ段差の近傍である場合
						//iSの前後のｙにおいて、ｘ方向に微分を調査し、QL閾値を越えるｘが存在するか調査する
						int iMPlus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
								iMPlus < iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMPlus++;
						}
						if (iMPlus != iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistB2WEdge[iCnt] = true;
							iMEdgeE = iM;
						}
						int iMMinus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
								iMMinus > -iMSearchLimit && iM + iMMinus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMMinus--;
						}
						if (iMMinus != -iMSearchLimit && iM + iMMinus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistB2WEdge[iCnt] = true;
							iMEdge0 = iM;
						}

/*					}
					else{//シンチ段差の近傍でない場合
						bflagExistB2WEdge[iCnt] = true;
					}
*/
 #ifdef _LP_DEBUG_OUTPUT
//					fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%d\n", iCnt, iM, bflagExistB2WEdge[iCnt], iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa), iMEdge0, iMEdgeE);
					fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\n", iCnt, iM, bflagExistB2WEdge[iCnt], iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa), iMEdge0, iMEdgeE, MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa, MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa, MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa, MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa);
 #endif
					continue;
				}
				else if (SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) {//副微分値が閾値を上回る場合（白→黒の方向）
//					bflagExistW2BEdge[iCnt] = true;
					iSEdgeW2B[iCnt] = iS;
//					if (edgeY[iM] - iSMargin <= iS && iS <= edgeY[iM] + iSMargin) {//シンチ段差の近傍である場合
						//iSの前後のｙにおいて、ｘ方向に微分を調査し、QL閾値を越えるｘが存在するか調査する
						int iMPlus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
								iMPlus < iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMPlus++;
						}
						if (iMPlus != iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistW2BEdge[iCnt] = true;
							iMEdgeE = iM;
						}
						int iMMinus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
								iMMinus > -iMSearchLimit && iM + iMMinus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMMinus--;
						}
						if (iMMinus != -iMSearchLimit && iM + iMMinus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistW2BEdge[iCnt] = true;
							iMEdge0 = iM;
						}

/*					}
					else{//シンチ段差の近傍でない場合
						bflagExistW2BEdge[iCnt] = true;
					}
*/
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%d\n", iCnt, iM, bflagExistW2BEdge[iCnt], iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa), iMEdge0, iMEdgeE);
 #endif
					continue;
				}
			}
			else if (bflagExistB2WEdge[iCnt]) {//既に黒→白の方向のエッジを検出している場合
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) {//副微分値が閾値を上回る場合（白→黒の方向）
//					bflagExistW2BEdge[iCnt] = true;
					iSEdgeW2B[iCnt] = iS;
//					if (edgeY[iM] - iSMargin <= iS && iS <= edgeY[iM] + iSMargin) {//シンチ段差の近傍である場合
						//iSの前後のｙにおいて、ｘ方向に微分を調査し、QL閾値を越えるｘが存在するか調査する
						int iMPlus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
								iMPlus < iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMPlus++;
						}
						if (iMPlus != iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistW2BEdge[iCnt] = true;
							iMEdgeE = iM;
						}
						int iMMinus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
								iMMinus > -iMSearchLimit && iM + iMMinus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMMinus--;
						}
						if (iMMinus != -iMSearchLimit && iM + iMMinus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistW2BEdge[iCnt] = true;
							iMEdge0 = iM;
						}
/*					}
					else{//シンチ段差の近傍でない場合
						bflagExistW2BEdge[iCnt] = true;
					}
*/
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%d\n", iCnt, iM, bflagExistW2BEdge[iCnt], iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa), iMEdge0, iMEdgeE);
 #endif
					break;
				}
			}
			else if (bflagExistW2BEdge[iCnt]) {//既に白→黒の方向のエッジを検出している場合
				if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf) {//副微分値が閾値を下回る場合（黒→白の方向）
//					bflagExistB2WEdge[iCnt] = true;
					iSEdgeB2W[iCnt] = iS;
//					if (edgeY[iM] - iSMargin <= iS && iS <= edgeY[iM] + iSMargin) {//シンチ段差の近傍である場合
						//iSの前後のｙにおいて、ｘ方向に微分を調査し、QL閾値を越えるｘが存在するか調査する
						int iMPlus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] - iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMPlus] + iSMargin][iM + iMPlus] - iKasa <=  iThQLMainProf)) &&
								iMPlus < iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMPlus++;
						}
						if (iMPlus != iMSearchLimit && iM + iMPlus < MainPixel - 1 && iM + iMPlus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistB2WEdge[iCnt] = true;
							iMEdgeE = iM;
						}
						int iMMinus = 0;
						while (((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] - iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
							   ((MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa >= -iThQLMainProf) &&
								(MainDifMedLogImg.Dt[edgeY[iM + iMMinus] + iSMargin][iM + iMMinus] - iKasa <=  iThQLMainProf)) &&
								iMMinus > -iMSearchLimit && iM + iMMinus > 0){//主微分が±閾値を越えない場合はループをまわす
							iMMinus--;
						}
						if (iMMinus != -iMSearchLimit && iM + iMMinus > 0) {//閾値を越えたｘが見つかった場合
							bflagExistB2WEdge[iCnt] = true;
							iMEdge0 = iM;
						}

/*					}
					else{//シンチ段差の近傍でない場合
						bflagExistB2WEdge[iCnt] = true;
					}
*/
 #ifdef _LP_DEBUG_OUTPUT
					fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%d\n", iCnt, iM, bflagExistB2WEdge[iCnt], iSEdgeW2B[iCnt], iSEdgeB2W[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa), iMEdge0, iMEdgeE);
 #endif
					break;
				}
			}
			else{
				break;
			}
#endif
		}
	}

 #ifdef _LP_DEBUG_OUTPUT
	fclose(fp);
	FILE* fp2 = fopen((ExtractFilePath(FileName) + "PbScaleXROI2.txt").c_str(), "wt");
	fprintf(fp2, "ID	Status	iMStart	iMEnd	iSStart	iSEnd	MLength\n");
 #endif

	//連続しているｘで区分化し、区分の個数をカウント
	iCnt = 0;
	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
//	for (iM = 0; iM < MedLogImg.MainPixel; iM += iDx, iCnt++) {
//		if (!bflag[iCnt]) {

		if (850 == iM) {
			int c = 123;
		}


		if (!bflagExistW2BEdge[iCnt] & !bflagExistB2WEdge[iCnt]) {
			continue;
		}

		if (bflagExistW2BEdge[iCnt] & !bflagExistB2WEdge[iCnt]) {//W2Bエッジのみ存在する場合
			XROI[iNumOfXROI].x1 = iM;
			XROI[iNumOfXROI].y1 = iSEdgeW2B[iCnt];
			while ((bflagExistW2BEdge[iCnt] & !bflagExistB2WEdge[iCnt]) && iM < MedLogImg.MainPixel - iMainCut){
				iM += iDx;
				iCnt++;
			}
 #if 0
			XROI[iNumOfXROI].x2 = iM - iDx;
 #else      //iMが一つとばされるバグ修正(141222 ttsuji)
			iM -= iDx;
			iCnt--;
			XROI[iNumOfXROI].x2 = iM;
 #endif
			XROI[iNumOfXROI].Pattern = _W2B;
		}
		else if (!bflagExistW2BEdge[iCnt] & bflagExistB2WEdge[iCnt]) {//B2Wエッジのみ存在する場合
			XROI[iNumOfXROI].x1 = iM;
			XROI[iNumOfXROI].y1 = iSEdgeB2W[iCnt];
			while ((!bflagExistW2BEdge[iCnt] & bflagExistB2WEdge[iCnt]) && iM < MedLogImg.MainPixel - iMainCut){
				iM += iDx;
				iCnt++;
			}
 #if 0
			XROI[iNumOfXROI].x2 = iM - iDx;
 #else      //iMが一つとばされるバグ修正(141222 ttsuji)
			iM -= iDx;
			iCnt--;
			XROI[iNumOfXROI].x2 = iM;
 #endif
			XROI[iNumOfXROI].Pattern = _B2W;
		}
		else if ((bflagExistW2BEdge[iCnt] & bflagExistB2WEdge[iCnt]) && iSEdgeW2B[iCnt] < iSEdgeB2W[iCnt]) {//W2B2Wエッジが存在する場合
			XROI[iNumOfXROI].x1 = iM;
			XROI[iNumOfXROI].y1 = iSEdgeW2B[iCnt];
			XROI[iNumOfXROI].y2 = iSEdgeB2W[iCnt];
			while ((bflagExistW2BEdge[iCnt] & bflagExistB2WEdge[iCnt]) && iSEdgeW2B[iCnt] < iSEdgeB2W[iCnt] && iM < MedLogImg.MainPixel - iMainCut){
				iM += iDx;
				iCnt++;
			}
 #if 0
			XROI[iNumOfXROI].x2 = iM - iDx;
 #else      //iMが一つとばされるバグ修正(141222 ttsuji)
			iM -= iDx;
			iCnt--;
			XROI[iNumOfXROI].x2 = iM;
 #endif
			XROI[iNumOfXROI].Pattern = _W2B2W;
		}
		else if ((bflagExistW2BEdge[iCnt] & bflagExistB2WEdge[iCnt]) && iSEdgeW2B[iCnt] > iSEdgeB2W[iCnt]) {//B2W2Bエッジが存在する場合
			XROI[iNumOfXROI].x1 = iM;
			XROI[iNumOfXROI].y1 = iSEdgeB2W[iCnt];
			XROI[iNumOfXROI].y2 = iSEdgeW2B[iCnt];
			while ((bflagExistW2BEdge[iCnt] & bflagExistB2WEdge[iCnt]) && iSEdgeW2B[iCnt] > iSEdgeB2W[iCnt] && iM < MedLogImg.MainPixel - iMainCut){
				iM += iDx;
				iCnt++;
			}
 #if 0
			XROI[iNumOfXROI].x2 = iM - iDx;
 #else      //iMが一つとばされるバグ修正(141222 ttsuji)
			iM -= iDx;
			iCnt--;
			XROI[iNumOfXROI].x2 = iM;
 #endif
			XROI[iNumOfXROI].Pattern = _B2W2B;
		}
		iNumOfXROI++;
	}


	//連続しているｘで区分化し、区分の個数をカウント
	iCnt = 0;
	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
//	for (iM = 0; iM < MedLogImg.MainPixel; iM += iDx, iCnt++) {
		if (!bflagExistW2BEdge[iCnt] & !bflagExistB2WEdge[iCnt]) {
			continue;
		}

		XROIMed[iNumOfXROIMed].x1 = iM;
		while ((bflagExistW2BEdge[iCnt] | bflagExistB2WEdge[iCnt]) && iM < MedLogImg.MainPixel - iMainCut){
			iM += iDx;
			iCnt++;

		}

 /*		if (iM >= MedLogImg.MainPixel - iMainCut) {
			break;
		}
 */
 #if 1
		XROIMed[iNumOfXROIMed].x2 = iM - iDx;
 #else      //iMが一つとばされるバグ修正(141222 ttsuji)
		iM -= iDx;
		iCnt--;
		XROIMed[iNumOfXROIMed].x2 = iM;
 #endif
		iNumOfXROIMed++;

	}




#ifdef _LP_DEBUG_OUTPUT
	for (int iROI = 0; iROI < iNumOfXROI; iROI++) {
		fprintf(fp2, "%d	%d	%d	%d	%d	%d\	%d\n", iROI, XROI[iROI].Pattern, XROI[iROI].x1, XROI[iROI].x2, XROI[iROI].y1, XROI[iROI].y2, (XROI[iROI].x2 - XROI[iROI].x1));
	}
	fprintf(fp2, "\nID	iMStart	iMEnd	MLength	MedianMask\n");

	const int iMarginX = 20;//検出した金属エッジの長さに加算するマージン
	int iMMedianMask;
	for (int iROI = 0; iROI < iNumOfXROIMed; iROI++) {
		iMMedianMask = (XROIMed[iROI].x2 - XROIMed[iROI].x1 + iMarginX) * 2 + 1;
 #if 1 //401 でクリップ（MCコンペア）
//		CLIP(iMMedianMask, 0, 401);//tmp
		if (iMMedianMask > 401) {
			iMMedianMask = 401;
		}
 #endif
		fprintf(fp2, "%d	%d	%d	%d	%d\n", iROI, XROIMed[iROI].x1, XROIMed[iROI].x2, (XROIMed[iROI].x2 - XROIMed[iROI].x1), iMMedianMask);
	}
	fclose(fp2);
#endif
	delete[] bflagExistB2WEdge;
	delete[] bflagExistW2BEdge;
	delete[] iSEdgeW2B;
	delete[] iSEdgeB2W;
//    delete[] iSAtMinQL;
}
//---------------------------------------------------------------------------
//シンチ段差、ガラス段差領域内の横長の金属エッジを探索する(150122 ttsuji 金尺の目盛りボケ対策として)

#define _ABNORMAL_CASE //141201 ttsuji

void TSIMARS::LPDetectPbScaleEdgeXROI_Glass	(TTMA MedLogImg, string FileName, int* edgeY, int* edgeYSinti, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubOffset, int& iNumOfXROI, ROI* XROI, int iSMargin)
{
	int iM, iS, iM2, iS2, iCnt(0);
//	int iMainCut = 30;//左右位置ずれによる空白領域との境界を避けるため
	int iMainCut = 0;//左右位置ずれによる空白領域との境界を避けるため

	TTMA SubDifMedLogImg, MainDifMedLogImg;
	SubDifMedLogImg = MedLogImg;
	SubDifMedLogImg.DifSubUp();//副微分（Dt[0]はそのまま、Dt[1]は Dt[1] - Dt[0]）
 #ifdef _LP_DEBUG_OUTPUT
	SubDifMedLogImg.WriteTma(ExtractFilePath(FileName) + "GlassSubDifMedLogImg.tma");
 #endif
	MainDifMedLogImg = MedLogImg;
	MainDifMedLogImg.DifMainUp();//主微分 （Dt[0]はそのまま、Dt[1]は Dt[1] - Dt[0]）
 #ifdef _LP_DEBUG_OUTPUT
	MainDifMedLogImg.WriteTma(ExtractFilePath(FileName) + "GlassMainDifMedLogImg.tma");
 #endif

	int iKasa = SubDifMedLogImg.DtMax / 2 + 1;
	bool* bflag = new bool[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
	int* iSAtEdge = new int[(MedLogImg.MainPixel - 2*iMainCut) / iDx + 1];
 /*
//	const int iSMargin = 1;//シンチ段差位置からのマージン
//	const int iSMargin = 2;//シンチ段差位置からのマージン(default)
//	const int iSMargin = 3;//シンチ段差位置からのマージン
//	const int iSMargin = 4;//シンチ段差位置からのマージン
	const int iSMargin = 5;//シンチ段差位置からのマージン
*/
	const int iMSearchLimit = 200;//金尺の長い方の目盛り長さが160pixのためそれを包含するサイズをエイッと決めた.
//	const int iMSearchLimit = 400;//金尺の長い方の目盛り長さが160pixのためそれを包含するサイズをエイッと決めた.
	int iMEdge0, iMEdgeE;

#ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "PbScaleXROI_Glass.txt").c_str(), "wt");
	fprintf(fp, "iD	iM	iSEdge	DeltaQLEdge\n");
#endif


	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
		int iSstart = edgeYSinti[iM] + iSMargin;//(edgeYSinti[iM] + 1)だと、シンチ段差の濃度差を拾ってしまうため。
		int iSend = edgeY[iM] + iSubOffset;
		bflag[iCnt] = false;
		iSAtEdge[iCnt] = -100;

		//DC補間の境界で濃度が急激に変化した場合でも検出可能にするように範囲を広げた(141201 ttsuji)
//		for (iS = iSstart; iS <= iSend + 1; iS++) {
		for (iS = iSstart; iS <= iSend; iS++) {//本命
//		for (iS = edgeY[iM] - 14; iS <= iSend; iS++) {//test
			if (SubDifMedLogImg.Dt[iS][iM] - iKasa < -iThQLSubProf || //副微分値が閾値を下回る場合
				SubDifMedLogImg.Dt[iS][iM] - iKasa > iThQLSubProf) { //副微分値が閾値を上回る場合
				bflag[iCnt] = true;
				iSAtEdge[iCnt] = iS;
 #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp, "%d	%d	%d	%d\n", iCnt, iM, iSAtEdge[iCnt], (SubDifMedLogImg.Dt[iS][iM] - iKasa));
 #endif
				break;
			}
		}
	}

 #ifdef _LP_DEBUG_OUTPUT
	fclose(fp);
	FILE* fp2 = fopen((ExtractFilePath(FileName) + "PbScaleXROI2_Glass.txt").c_str(), "wt");
	fprintf(fp2, "ID	Status	iMStart	iMEnd	iSStart	iSEnd	MLength\n");
 #endif

	//連続しているｘで区分化し、区分の個数をカウント
	iCnt = 0;
	for (iM = iMainCut; iM < MedLogImg.MainPixel - iMainCut; iM += iDx, iCnt++) {
		if (!bflag[iCnt]) {
			continue;
		}

		XROI[iNumOfXROI].x1 = iM;
		while (bflag[iCnt] && iM < MedLogImg.MainPixel - iMainCut){
			iM += iDx;
			iCnt++;

		}

 /*		if (iM >= MedLogImg.MainPixel - iMainCut) {
			break;
		}
 */
 #if 1
		XROI[iNumOfXROI].x2 = iM - iDx;
 #else      //iMが一つとばされるバグ修正(141222 ttsuji)
		iM -= iDx;
		iCnt--;
		XROI[iNumOfXROI].x2 = iM;
 #endif
		iNumOfXROI++;

	}




#ifdef _LP_DEBUG_OUTPUT
	for (int iROI = 0; iROI < iNumOfXROI; iROI++) {
		fprintf(fp2, "%d	%d	%d	%d	%d	%d\	%d\n", iROI, XROI[iROI].Pattern, XROI[iROI].x1, XROI[iROI].x2, XROI[iROI].y1, XROI[iROI].y2, (XROI[iROI].x2 - XROI[iROI].x1));
	}
	fprintf(fp2, "\nID	iMStart	iMEnd	MLength	MedianMask\n");

	const int iMarginX = 20;//検出した金属エッジの長さに加算するマージン
	int iMMedianMask;
	for (int iROI = 0; iROI < iNumOfXROI; iROI++) {
		iMMedianMask = (XROI[iROI].x2 - XROI[iROI].x1 + iMarginX) * 2 + 1;
		fprintf(fp2, "%d	%d	%d	%d	%d\n", iROI, XROI[iROI].x1, XROI[iROI].x2, (XROI[iROI].x2 - XROI[iROI].x1), iMMedianMask);
	}
	fclose(fp2);
#endif
	delete[] bflag;
	delete[] iSAtEdge;
}
//---------------------------------------------------------------------------

//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。

//#define _FIXED_MEDIANMASK

void TSIMARS::LPMainMedianForEachROI(TTMA& CorrImg, int* edgeY, int iNumOfXROI, ROI* XROI, int iMainMedianDCItpl, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iKasa)
{
	int iROI;
//	const int iMarginX = 40;//検出した金属エッジの長さに加算するマージン
	const int iMarginX = 20;//検出した金属エッジの長さに加算するマージン
//	const int iMarginX = 10;//検出した金属エッジの長さに加算するマージン
	const int iMarginXPbROI = 5;//検出した金属エッジの長さに加算するマージンであり、大サイズMedian値で置換する領域。

	int iMMedianMask;




	for (iROI = 0; iROI < iNumOfXROI; iROI++) {
		TTMA CorrImgCopy;
		CorrImgCopy = CorrImg;
#ifndef _FIXED_MEDIANMASK
		iMMedianMask = (XROI[iROI].x2 - XROI[iROI].x1 + iMarginX) * 2 + 1;
#else
		iMMedianMask = 401;//金尺の長い方の目盛り長さが160pixのためそれを消せるサイズをエイッと決めた.
#endif
		if (iMMedianMask <= iMainMedianDCItpl) {
			continue;//既に実施済みのMedianマスクサイズ以下であれば処理しない
		}
		CorrImgCopy.MainMedian(iMMedianMask);

//		for (int iM = CLIP(XROI[iROI].x1 - iMarginX, 0, MainPixel - 1); iM < CLIP(XROI[iROI].x2 + iMarginX, 0, MainPixel - 1); iM++) {
		for (int iM = CLIP(XROI[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1); iM < CLIP(XROI[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1); iM++) {
			for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
				CorrImg.SetVal(iS, iM, CorrImgCopy.Dt[iS][iM]);
			}
		}


		//ｔｍｐ追加
		double dDeltaQL = 0.;
		int iCnt = 0;
		for (int iM = CLIP(XROI[iROI].x1, 0, MainPixel - 1); iM < CLIP(XROI[iROI].x2, 0, MainPixel - 1); iM++) {
			int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
			int iSend   = edgeY[iM] + iItplSubOffset;
			dDeltaQL += (Dt[iSstart][iM] - Dt[iSstart - 1][iM]);
        	iCnt++;
		}
		if (!iCnt) {
        	continue;
		}
		dDeltaQL /= iCnt;

		for (int iM = CLIP(XROI[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1); iM < CLIP(XROI[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1); iM++) {
			for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
				CorrImg.SetVal(iS, iM, dDeltaQL + iKasa);
			}
		}
	}

}
//---------------------------------------------------------------------------
//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。StatusによってはDC補間領域外の画素値を目標としてコピーする

void TSIMARS::LPMainMedianAndCopyForEachROI(TTMA& CorrImg, string FileName, int* edgeY, int iNumOfXROI, ROI* XROI, int iNumOfXROIMed, ROI* XROIMed, int iMainMedianDCItpl, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iKasa, int iMedianHasiSyori, int iMarginXMMed)
{
	int iROI;
/*
	const int iMarginXMMed = 70;//検出した金属エッジの長さに加算する主Medianマスクのマージン
//	const int iMarginXMMed = 60;//検出した金属エッジの長さに加算する主Medianマスクのマージン
//	const int iMarginXMMed = 50;//検出した金属エッジの長さに加算する主Medianマスクのマージン
//	const int iMarginXMMed = 40;//検出した金属エッジの長さに加算する主Medianマスクのマージン
//	const int iMarginXMMed = 20;//検出した金属エッジの長さに加算する主Medianマスクのマージン(default)
//	const int iMarginXMMed = 10;//検出した金属エッジの長さに加算する主Medianマスクのマージン
//	const int iMarginXMMed = 5;//検出した金属エッジの長さに加算する主Medianマスクのマージン
*/
	const int iMarginXPbROI = 5;//検出した金属エッジの長さに加算するマージンであり、大サイズMedian値で置換する領域。
//	const int iMarginXPbROI = 3;//検出した金属エッジの長さに加算するマージンであり、大サイズMedian値で置換する領域。


	//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。
	TTMA CorrImgBuffer;
	CorrImgBuffer = CorrImg;
//	for (iROI = 0; iROI < iNumOfXROI; iROI++) {
	for (iROI = 0; iROI < iNumOfXROIMed; iROI++) {
		int iMMedianMask;
		TTMA CorrImgCopy;
		CorrImgCopy = CorrImg;
		iMMedianMask = (XROIMed[iROI].x2 - XROIMed[iROI].x1 + iMarginXMMed) * 2 + 1;

		if (iMMedianMask <= iMainMedianDCItpl) {
			continue;//既に実施済みのMedianマスクサイズ以下であれば処理しない
		}
//#if 0
#if 1	//端処理を、端から(Medianサイズ/2 +1)番目のMedian値で置換をやめて、可変長Medにしたことで端にある目盛は消えるようになったため、可変長に戻す (151001 ttsuji)
		CorrImgCopy.MainMedian(iMMedianMask);
#else	//端処理追加(141126 ttsuji)
		const int iXHasi = 401;
		//画像の左右端の場合
		if (CLIP(XROIMed[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1) <= iXHasi / 2 || CLIP(XROIMed[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1) >= MainPixel - 1 - iXHasi / 2) {
			CorrImgCopy.MainMedian(iXHasi);
			switch (iMedianHasiSyori) {
			case 0://端から(Medianサイズ/2 +1)番目のMedian値で置換
				CorrImgCopy.MainPixelExpand(iXHasi / 2);//不具合対応としてコメントアウト(150119 ttsuji)
				break;
			}
		}
		else{//画像の中央寄りの場合
			CorrImgCopy.MainMedian(iMMedianMask);
		}
#endif
//		for (int iM = CLIP(XROI[iROI].x1 - iMarginXMMed, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2 + iMarginXMMed, 0, MainPixel - 1); iM++) {
		for (int iM = CLIP(XROIMed[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1); iM <= CLIP(XROIMed[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1); iM++) {
			for (int iS = 0; iS < CorrImg.SubPixel; iS++) {
//				CorrImg.SetVal(iS, iM, CorrImgCopy.Dt[iS][iM]);
				CorrImgBuffer.SetVal(iS, iM, CorrImgCopy.Dt[iS][iM]);//CorrImgにコピーすると、次のiROIでMedian後のものに再度Medianすることになり、NG。
			}
		}
	}
	CorrImg = CorrImgBuffer;
 #ifdef _LP_DEBUG_OUTPUT
	CorrImg.WriteTma(ExtractFilePath(FileName) + "AdaptiveMedian_CorrImg.tma");
 #endif


#if 0 //Step2の処理をOFFしたいときはコメントアウト
 #ifdef _LP_DEBUG_OUTPUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "DeltaQL.txt").c_str(), "wt");
	fprintf(fp, "iROI	Status	iCnt	Dt[iSstart - 1][iM]	Dt[iSstart][iM]	DeltaQL\n");
	fprintf(fp, "iROI	Status	iCnt	Dt[iSend - 1][iM]	Dt[iSend][iM]	Dt[iSend + 1][iM]	Dt[iSend + 2][iM]	DeltaQL\n");
 #endif

	//DC補間領域の境界が白いときは領域外の白を目標に補正画像を修正
	const int iXLengthTh = 5;//何も処理しない区分の長さ閾値
	for (iROI = 0; iROI < iNumOfXROI; iROI++) {
//		if (XROI[iROI].x2 - XROI[iROI].x1 == 0){
		if (XROI[iROI].x2 - XROI[iROI].x1 <= iXLengthTh){
			continue;
		}

		double dDeltaQL = 0.;
		double dDeltaQL2 = 0.;
		int iCnt = 0;
		if (XROI[iROI].Pattern == _W2B) {//DC補間の上端を参照
			for (int iM = CLIP(XROI[iROI].x1, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2, 0, MainPixel - 1); iM++) {
				int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
#ifdef _ABNORMAL_CASE
				if (2035 == iM) {
					int a = 1;
				}
				//DC補間領域の上側境界にエッジがある場合は差分画素値での補正しない(141201 ttsuji)
				if (XROI[iROI].y1 == iSstart) {
					break;
				}
#endif
				dDeltaQL += (Dt[iSstart][iM] - Dt[iSstart - 1][iM]);
				iCnt++;
 #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp, "%d	%d	%d	%d	%d	%lf\n", iROI, XROI[iROI].Pattern, iCnt, Dt[iSstart - 1][iM], Dt[iSstart][iM], dDeltaQL);
 #endif
			}
			if (!iCnt) {
				continue;
			}
			dDeltaQL /= iCnt;

			for (int iM = CLIP(XROI[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1); iM++) {
				int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
//				for (int iS = iSstart; iS <= XROI[iROI].y1; iS++) {
//				for (int iS = iSstart; iS < XROI[iROI].y1; iS++) {//y=y1は、y≧y1から黒に変わるため補正対象に含めない（石丸さん指摘によりy1を除外(141222 ttsuji)）
				for (int iS = iSstart; iS < CLIP(XROI[iROI].y1, iSstart + 1,  edgeY[iM] + 1); iS++) {//y=y1がシンチ段差位置edgeY[iM]よりも画像中央寄りにある場合はedgeY[iM]までを白塗りする(石丸さん指摘により修正 150105 ttsuji)  //y=y1は、y≧y1から黒に変わるため補正対象に含めない（石丸さん指摘によりy1を除外(141222 ttsuji)）
					CorrImg.SetVal(iS, iM, dDeltaQL + iKasa);
				}
			}
		}
		else if (XROI[iROI].Pattern == _B2W) {//DC補間の下端を参照
			for (int iM = CLIP(XROI[iROI].x1, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2, 0, MainPixel - 1); iM++) {
//				int iSend = edgeY[iM] + iItplSubOffset;
				int iSend = edgeY[iM];
#ifdef _ABNORMAL_CASE
				//DC補間領域の上側境界にエッジがある場合は差分画素値での補正しない(141201 ttsuji)
				if (XROI[iROI].y1 == iSend + 1) {
					break;
				}
#endif

 #if 0
				dDeltaQL2 += (Dt[iSend][iM] - Dt[iSend + 1][iM]);
 #else
				dDeltaQL2 += (Dt[iSend][iM] + Dt[iSend - 1][iM] - Dt[iSend + 1][iM] - Dt[iSend + 2][iM]) / 2.;//エッジの最近接画素と2番目に近接する画素の平均値同士の差分（補正精度向上のため(Z:\_takeshi\LP\20141105_金尺撮影(８R)\20141105-0045x)）
 #endif
				iCnt++;
 #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%lf\n", iROI, XROI[iROI].Pattern, iCnt, Dt[iSend - 1][iM], Dt[iSend][iM], Dt[iSend + 1][iM], Dt[iSend + 2][iM], dDeltaQL2);
 #endif
			}
			if (!iCnt) {
				continue;
			}
			dDeltaQL2 /= iCnt;

			for (int iM = CLIP(XROI[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1); iM++) {
//				int iSend   = edgeY[iM] + iItplSubOffset;
				int iSend   = edgeY[iM];
				for (int iS = XROI[iROI].y1; iS <= iSend; iS++) {//y=y1は、y≧y1から白に変わるため補正対象に含める
					CorrImg.SetVal(iS, iM, dDeltaQL2 + iKasa);
				}
			}
		}
		else if (XROI[iROI].Pattern == _W2B2W) {//DC補間の上端と下端をそれぞれ参照
			for (int iM = CLIP(XROI[iROI].x1, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2, 0, MainPixel - 1); iM++) {
				int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
//				int iSend = edgeY[iM] + iItplSubOffset;
				int iSend = edgeY[iM];

#ifdef _ABNORMAL_CASE
				//DC補間領域の上側境界にエッジがある場合は差分画素値での補正しない(141201 ttsuji)
				if (XROI[iROI].y1 == iSstart) {
					break;
				}
#endif

#ifdef _ABNORMAL_CASE
				//DC補間領域の上側境界にエッジがある場合は差分画素値での補正しない(141201 ttsuji)
				if (XROI[iROI].y2 == iSend + 1) {
					break;
				}
#endif

				dDeltaQL  += (Dt[iSstart][iM] - Dt[iSstart - 1][iM]);
 #if 0
				dDeltaQL2 += (Dt[iSend][iM] - Dt[iSend + 1][iM]);
 #else
				dDeltaQL2 += (Dt[iSend][iM] + Dt[iSend - 1][iM] - Dt[iSend + 1][iM] - Dt[iSend + 2][iM]) / 2.;//エッジの最近接画素と2番目に近接する画素の平均値同士の差分（補正精度向上のため(Z:\_takeshi\LP\20141105_金尺撮影(８R)\20141105-0045x)）
 #endif
				iCnt++;

 #ifdef _LP_DEBUG_OUTPUT
				fprintf(fp, "%d	%d	%d	%d	%d	%lf\n", iROI, XROI[iROI].Pattern, iCnt, Dt[iSstart - 1][iM], Dt[iSstart][iM], dDeltaQL);
				fprintf(fp, "%d	%d	%d	%d	%d	%d	%d	%lf\n", iROI, XROI[iROI].Pattern, iCnt, Dt[iSend - 1][iM], Dt[iSend][iM], Dt[iSend + 1][iM], Dt[iSend + 2][iM], dDeltaQL2);
 #endif
			}
			if (!iCnt) {
				continue;
			}
			dDeltaQL  /= iCnt;
			dDeltaQL2 /= iCnt;

			for (int iM = CLIP(XROI[iROI].x1 - iMarginXPbROI, 0, MainPixel - 1); iM <= CLIP(XROI[iROI].x2 + iMarginXPbROI, 0, MainPixel - 1); iM++) {
				int iSstart = edgeY[iM] - iSubRangeOnlyMidPanel + 1;
//				int iSend   = edgeY[iM] + iItplSubOffset;
				int iSend   = edgeY[iM];
//				for (int iS = iSstart; iS <= XROI[iROI].y1; iS++) {
//				for (int iS = iSstart; iS < XROI[iROI].y1; iS++) {//y=y1は、y≧y1から黒に変わるため補正対象に含めない（石丸さん指摘によりy1を除外(141222 ttsuji)）
				for (int iS = iSstart; iS < CLIP(XROI[iROI].y1, iSstart + 1,  edgeY[iM] + 1); iS++) {//y=y1がシンチ段差位置edgeY[iM]よりも画像中央寄りにある場合はedgeY[iM]までを白塗りする(石丸さん指摘により修正 150105 ttsuji)  //y=y1は、y≧y1から黒に変わるため補正対象に含めない（石丸さん指摘によりy1を除外(141222 ttsuji)）
					CorrImg.SetVal(iS, iM, dDeltaQL + iKasa);
				}
				for (int iS = XROI[iROI].y2; iS <= iSend; iS++) {//y=y2は、y≧y2から白に変わるため補正対象に含める
					CorrImg.SetVal(iS, iM, dDeltaQL2 + iKasa);
				}
			}
		}

	}
 #ifdef _LP_DEBUG_OUTPUT
	fclose(fp);
 #endif

#endif

}
//---------------------------------------------------------------------------
#define _DUMMYIMMAGE_EXPLR //ノイズ画像を計算する際に、平均拡張を左右も行うか

//粒状抑制処理(150706 ttsuji)
void TSIMARS::LPGranularityReduction(string& FileName, int iMaskSize1, double dThEnergy1, double dSelfWeight1, int iMaskSize2, double dThEnergy2, double dSelfWeight2, int iMaskSize3, double dThEnergy3, double dSelfWeight3, int iYShiftMinus)
{
	//ログ変換
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	string StrPre = "BL";
	string FileName05 =  AddPrefix(FileName, "Mf0.5");
	string FileName025 =  AddPrefix(FileName05, "Mf0.5");

#if 0
	LogThreshold(dMidVal, dKeta, iLogTh);
	BitChange(14);
#else
	Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
#endif

#ifdef _LP_DEBUG_OUTPUT
	FileName = AddPrefix(FileName, StrPre);
	WriteTma(FileName);
#endif

	//0.5倍、0.25倍画像をガウシアンピラミッド演算にて生成
	TSIMARS Img05, Img025;
	Img05 = *this;
//	Img05.LPResize05(FileName05);
	Img05.LPResizeExpand05(FileName05);
	Img025 = Img05;
//	Img025.LPResize05(FileName025);
	Img025.LPResizeExpand05(FileName025);

	TSIMARS ImgNoise, ImgNoise05, ImgNoise025;

#ifdef _DUMMYIMMAGE_EXPLR
	//１倍画像に適応フィルター
	LPGranularityReductionForEachSizeImage2(FileName, ImgNoise, iMaskSize1, dThEnergy1, dSelfWeight1);
	//0.5倍画像に適応フィルター
	Img05.LPGranularityReductionForEachSizeImage2(FileName05, ImgNoise05, iMaskSize2, dThEnergy2, dSelfWeight2);
	//0.25倍画像に適応フィルター
	Img025.LPGranularityReductionForEachSizeImage2(FileName025, ImgNoise025, iMaskSize3, dThEnergy3, dSelfWeight3);
#else
	//１倍画像に適応フィルター
	LPGranularityReductionForEachSizeImage(FileName, ImgNoise, iMaskSize1, dThEnergy1, dSelfWeight1);
	//0.5倍画像に適応フィルター
	Img05.LPGranularityReductionForEachSizeImage(FileName05, ImgNoise05, iMaskSize2, dThEnergy2, dSelfWeight2);
	//0.25倍画像に適応フィルター
	Img025.LPGranularityReductionForEachSizeImage(FileName025, ImgNoise025, iMaskSize3, dThEnergy3, dSelfWeight3);
#endif

	//ノイズ画像のラプラシアンピラミッド演算にて生成
//	ImgNoise025.LPResize2(FileName);
	string StrResize2 = ExtractFilePath(FileName) + "Noise025.tma";
	ImgNoise025.LPResizeExpand2(StrResize2);
#ifdef _LP_DEBUG_OUTPUT
	ImgNoise025.WriteTma(ExtractFilePath(FileName) + "fENoise025.tma");
#endif
//	ImgNoise05 += ImgNoise025 - 30000;
	for (int iMCnt = 0; iMCnt < ImgNoise05.MainPixel; iMCnt++) {
		for (int iSCnt = 0; iSCnt < ImgNoise05.SubPixel; iSCnt++) {
			ImgNoise05.SetVal(iSCnt, iMCnt, ImgNoise05.Dt[iSCnt][iMCnt] + ImgNoise025.Dt[iSCnt][iMCnt] - 30000);
		}
	}
#ifdef _LP_DEBUG_OUTPUT
	ImgNoise05.WriteTma(ExtractFilePath(FileName) + "C(0.5+0.25)Noise.tma");
#endif
//	ImgNoise05.LPResize2(FileName);
//	ImgNoise05.LPResizeExpand2(ExtractFilePath(FileName) + "C(0.5+0.25)Noise.tma");
	StrResize2 = ExtractFilePath(FileName) + "C(0.5+0.25)Noise.tma";
	ImgNoise05.LPResizeExpand2(StrResize2);
#ifdef _LP_DEBUG_OUTPUT
	ImgNoise05.WriteTma(ExtractFilePath(FileName) + "fEC(0.5+0.25)Noise.tma");
#endif
//	ImgNoise += ImgNoise05 - 30000;
	for (int iMCnt = 0; iMCnt < ImgNoise.MainPixel; iMCnt++) {
		for (int iSCnt = 0; iSCnt < ImgNoise.SubPixel; iSCnt++) {
			ImgNoise.SetVal(iSCnt, iMCnt, ImgNoise.Dt[iSCnt][iMCnt] + ImgNoise05.Dt[iSCnt][iMCnt] - 30000);
		}
	}
#ifdef _LP_DEBUG_OUTPUT
	ImgNoise.WriteTma((ExtractFilePath(FileName) + "C(1+0.5+0.25)Noise.tma"));
#endif

	//画像サイズチェック
	if (MainPixel != ImgNoise.MainPixel || SubPixel != ImgNoise.SubPixel) {
		CApplication::GetInstance()->MessageBox("ノイズ画像のサイズが不正です", "Error", MB_OK);
		return;
	}

//	*this -= ImgNoise + 30000;
	for (int iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		for (int iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
			SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] - ImgNoise.Dt[iSCnt][iMCnt] + 30000);
		}
	}
#ifdef _LP_DEBUG_OUTPUT
	FileName = AddPrefix(FileName, "NR_");
	WriteTma(FileName);
#endif

	//フォーマット処理で画像の下端12pixをカット
	Format(SubPixel - iYShiftMinus, MainPixel, 0, 0, DtMax);
#ifdef _LP_DEBUG_OUTPUT
	FileName = AddPrefix(FileName, "F");
	WriteTma(FileName);
#endif

	//逆LOG変換
#if 0
	BitChange(16);
	LogToLinear2(Bit, dMidVal, dKeta);
#else
	Log14ToLin16_LowDoseLinear(Bit, dMidVal, dKeta);
#endif

#ifdef _LP_DEBUG_OUTPUT
	FileName = AddPrefix(FileName, "al");
	WriteTma(FileName);
#endif
}
//---------------------------------------------------------------------------
#define _EDGE_WEIGHT_OUT
#define _CALCDIFF_FAST  //微分をビットシフトで実施

//各解像度画像に対する粒状抑制処理（ただし、左右方向の平均拡張が未実装）
void TSIMARS::LPGranularityReductionForEachSizeImage(string FileName, TSIMARS& ImgNoise, int iMaskSize, double dThEnergy, double dSelfWeight)
{
	DIFFERENTIALARROUNDPIXELS DifPixels;

	double dFilterSobelUp2Down[9] = {679/32768., 31411/32768., 679/32768., 0, 0, 0, -679/32768., -31411/32768., -679/32768.};
	double dFilterSobelLeft2Right[9] = {-679/32768., 0, 679/32768., -31411/32768., 0, 31411/32768., -679/32768., 0, 679/32768.};
	double dFilterSobelUpLeft2DownRight[9] = {-1357/32768., -15705/32768., 0, -15705/32768., 0, 15705/32768., 0, 15705/32768., 1357/32768.};
	double dFilterSobelUpRight2DownLeft[9] = {0, 15705/32768., 1357/32768., -15705/32768., 0, 15705/32768., -1357/32768., -15705/32768., 0};

	//debugのための画像
	TTMA ImageUp2Down = *this;
	TTMA ImageLeft2Right = *this;
	TTMA ImageUpLeft2DownRight = *this;
	TTMA ImageUpRight2DownLeft = *this;
	TTMA OutImage = *this;

	//フィルター処理（まずは主と副の２パターンで場合わけ）
	int iHalfMask = iMaskSize / 2;

	//端処理用の平均拡張の画素値計算
	TTMA DummyImageUp, DummyImageDown, DummyImageLeft, DummyImageRight;
	DummyImageUp.NewDt(1, MainPixel, Bit);
	DummyImageDown.NewDt(1, MainPixel, Bit);
	DummyImageLeft.NewDt(SubPixel, 1, Bit);
	DummyImageRight.NewDt(SubPixel, 1, Bit);
	double dSumEdge = 0.;
	int iMCnt, iSCnt;
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		dSumEdge = 0.;
		for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) {
			dSumEdge += Dt[iSCnt][iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageUp.SetVal(0, iMCnt, dSumEdge);
	}
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		dSumEdge = 0.;
		for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) {
			dSumEdge += Dt[SubPixel - 1 - iSCnt][iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageDown.SetVal(0, iMCnt, dSumEdge);
	}
	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
		dSumEdge = 0.;
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			dSumEdge += Dt[iSCnt][iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageLeft.SetVal(iSCnt, 0, dSumEdge);
	}
	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
		dSumEdge = 0.;
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			dSumEdge += Dt[iSCnt][MainPixel - 1 - iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageRight.SetVal(iSCnt, 0, dSumEdge);
	}
#if 0
	//debug
	DummyImageUp.WriteTma( AddPrefix(FileName, "ExpUp"));
	DummyImageDown.WriteTma( AddPrefix(FileName, "ExpDown"));
	DummyImageLeft.WriteTma( AddPrefix(FileName, "ExpLeft"));
	DummyImageRight.WriteTma( AddPrefix(FileName, "ExpRight"));
#endif

	TTMA DummyImage;
	DummyImage.NewDt(SubPixel + iHalfMask*2, MainPixel, Bit);//とりあえず左右端の端処理は後回しで上下端のみとする
	for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) {
		for (iMCnt = 0; iMCnt < DummyImage.MainPixel; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = DummyImageUp.Dt[0][iMCnt];
		}
	}
	for (iSCnt = iHalfMask; iSCnt < DummyImage.SubPixel - iHalfMask; iSCnt++) {
		for (iMCnt = 0; iMCnt < DummyImage.MainPixel; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = Dt[iSCnt - iHalfMask][iMCnt];
		}
	}
	for (iSCnt = DummyImage.SubPixel - iHalfMask; iSCnt < DummyImage.SubPixel; iSCnt++) {
		for (iMCnt = 0; iMCnt < DummyImage.MainPixel; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = DummyImageDown.Dt[0][iMCnt];
		}
	}

#ifdef _LP_DEBUG_OUTPUT
	DummyImage.WriteTma( AddPrefix(FileName, "DummyImage"));
 #ifdef _EDGE_WEIGHT_OUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "EdgeAndWeightInf_" + ExtractFileName(FileName) + ".txt").c_str(), "wt");
 #endif
#endif


//	for (iSCnt = 0; iSCnt < Image.SubPixel; iSCnt++) {
//		for (iMCnt = 0; iMCnt < Image.MainPixel; iMCnt++) {
	for (iSCnt = 1; iSCnt < SubPixel - 1; iSCnt++) {
		for (iMCnt = 1; iMCnt < MainPixel - 1; iMCnt++) {
//	for (iSCnt = iSubMask; iSCnt < Image.SubPixel - iSubMask; iSCnt++) {//とりあえず端処理はしない
//		for (iMCnt = iMainMask; iMCnt < Image.MainPixel - iMainMask; iMCnt++) {
			double dMax(0.), dMin(100000.);

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//			if (119 == iSCnt && 703 == iMCnt) {
//			if (1 == iSCnt && 46 == iMCnt) {
			if (9 == iSCnt && 504 == iMCnt) {
				int bbb = 0;
				fprintf(fp, "iM	%d\n", iMCnt);
				fprintf(fp, "iS	%d\n", iSCnt);
			}
 #endif
#endif

#ifdef _CALCDIFF_FAST
			DifPixels = CalcDifferentialArroundPixel_Fast(DummyImage, iSCnt + iHalfMask, iMCnt);//ある点の周囲の微分情報を計算する

			//debug
			ImageUp2Down.SetVal(iSCnt, iMCnt, DifPixels.Up2Down.dDiff + DtMax/2);
			ImageLeft2Right.SetVal(iSCnt, iMCnt, DifPixels.Left2Right.dDiff + DtMax/2);
			ImageUpLeft2DownRight.SetVal(iSCnt, iMCnt, DifPixels.UpLeft2DownRight.dDiff + DtMax/2);
			ImageUpRight2DownLeft.SetVal(iSCnt, iMCnt, DifPixels.UpRight2DownLeft.dDiff + DtMax/2);
#else
			//端画素でも微分できるよう仮想画像を定義する
			//上下方向の微分を計算
			DifPixels.Up2Down.dDiff = DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt - 1]*dFilterSobelUp2Down[0] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt    ]*dFilterSobelUp2Down[1] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt + 1]*dFilterSobelUp2Down[2] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt - 1]*dFilterSobelUp2Down[3] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt    ]*dFilterSobelUp2Down[4] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt + 1]*dFilterSobelUp2Down[5] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt - 1]*dFilterSobelUp2Down[6] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt    ]*dFilterSobelUp2Down[7] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt + 1]*dFilterSobelUp2Down[8];
			if (dMax < fabs(DifPixels.Up2Down.dDiff)) {
				dMax = fabs(DifPixels.Up2Down.dDiff);
				DifPixels.indexMax = _Vertical;
			}
			if (dMin > fabs(DifPixels.Up2Down.dDiff)) {
				dMin = fabs(DifPixels.Up2Down.dDiff);
				DifPixels.indexMin = _Vertical;
			}

			//左右方向の微分を計算
			DifPixels.Left2Right.dDiff = DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt - 1]*dFilterSobelLeft2Right[0] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt    ]*dFilterSobelLeft2Right[1] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt + 1]*dFilterSobelLeft2Right[2] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt - 1]*dFilterSobelLeft2Right[3] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt    ]*dFilterSobelLeft2Right[4] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt + 1]*dFilterSobelLeft2Right[5] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt - 1]*dFilterSobelLeft2Right[6] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt    ]*dFilterSobelLeft2Right[7] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt + 1]*dFilterSobelLeft2Right[8];
			if (dMax < fabs(DifPixels.Left2Right.dDiff)) {
				dMax = fabs(DifPixels.Left2Right.dDiff);
				DifPixels.indexMax = _Horizontal;
			}
			if (dMin > fabs(DifPixels.Left2Right.dDiff)) {
				dMin = fabs(DifPixels.Left2Right.dDiff);
				DifPixels.indexMin = _Horizontal;
			}

			//左上→右下方向の微分を計算
			DifPixels.UpLeft2DownRight.dDiff = DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt - 1]*dFilterSobelUpLeft2DownRight[0] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt    ]*dFilterSobelUpLeft2DownRight[1] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt + 1]*dFilterSobelUpLeft2DownRight[2] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt - 1]*dFilterSobelUpLeft2DownRight[3] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt    ]*dFilterSobelUpLeft2DownRight[4] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt + 1]*dFilterSobelUpLeft2DownRight[5] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt - 1]*dFilterSobelUpLeft2DownRight[6] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt    ]*dFilterSobelUpLeft2DownRight[7] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt + 1]*dFilterSobelUpLeft2DownRight[8];
			if (dMax < fabs(DifPixels.UpLeft2DownRight.dDiff)) {
				dMax = fabs(DifPixels.UpLeft2DownRight.dDiff);
				DifPixels.indexMax = _RightDown;
			}
			if (dMin > fabs(DifPixels.UpLeft2DownRight.dDiff)) {
				dMin = fabs(DifPixels.UpLeft2DownRight.dDiff);
				DifPixels.indexMin = _RightDown;
			}

			//右上→左下方向の微分を計算
			DifPixels.UpRight2DownLeft.dDiff = DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt - 1]*dFilterSobelUpRight2DownLeft[0] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt    ]*dFilterSobelUpRight2DownLeft[1] +
					   DummyImage.Dt[iSCnt - 1 + iHalfMask][iMCnt + 1]*dFilterSobelUpRight2DownLeft[2] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt - 1]*dFilterSobelUpRight2DownLeft[3] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt    ]*dFilterSobelUpRight2DownLeft[4] +
					   DummyImage.Dt[iSCnt     + iHalfMask][iMCnt + 1]*dFilterSobelUpRight2DownLeft[5] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt - 1]*dFilterSobelUpRight2DownLeft[6] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt    ]*dFilterSobelUpRight2DownLeft[7] +
					   DummyImage.Dt[iSCnt + 1 + iHalfMask][iMCnt + 1]*dFilterSobelUpRight2DownLeft[8];
			if (dMax < fabs(DifPixels.UpRight2DownLeft.dDiff)) {
				dMax = fabs(DifPixels.UpRight2DownLeft.dDiff);
				DifPixels.indexMax = _RightUp;
			}
			if (dMin > fabs(DifPixels.UpRight2DownLeft.dDiff)) {
				dMin = fabs(DifPixels.UpRight2DownLeft.dDiff);
				DifPixels.indexMin = _RightUp;
			}

			//debug
			ImageUp2Down.SetVal(iSCnt, iMCnt, DifPixels.Up2Down.dDiff + DtMax/2);
			ImageLeft2Right.SetVal(iSCnt, iMCnt, DifPixels.Left2Right.dDiff + DtMax/2);
			ImageUpLeft2DownRight.SetVal(iSCnt, iMCnt, DifPixels.UpLeft2DownRight.dDiff + DtMax/2);
			ImageUpRight2DownLeft.SetVal(iSCnt, iMCnt, DifPixels.UpRight2DownLeft.dDiff + DtMax/2);

			//エネルギーE
			DifPixels.dEnergy = (fabs(DifPixels.Up2Down.dDiff) + fabs(DifPixels.Left2Right.dDiff) + fabs(DifPixels.UpLeft2DownRight.dDiff) + fabs(DifPixels.UpRight2DownLeft.dDiff)) / 4.0;
			//仮想ベクトル
			DifPixels.dVmX = fabs(DifPixels.Up2Down.dDiff) - fabs(DifPixels.Left2Right.dDiff);
			DifPixels.dVmY = fabs(DifPixels.UpLeft2DownRight.dDiff) - fabs(DifPixels.UpRight2DownLeft.dDiff);
			DifPixels.dVmNorm = sqrt(DifPixels.dVmX * DifPixels.dVmX + DifPixels.dVmY * DifPixels.dVmY);
			CalcEdgeOrientation(DifPixels);//仮想ベクトルVmから実画像上のエッジ方向を計算
			//信頼性（線らしさ）
//			DifPixels.dConfidence = DifPixels.dVmNorm / DifPixels.dEnergy;//Energy=0のときの例外処理どうする？
#endif  	//_CALCDIFF_FAST

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//			if (119 == iSCnt && 703 == iMCnt) {
//			if (1 == iSCnt && 46 == iMCnt) {
			if (9 == iSCnt && 504 == iMCnt) {
				fprintf(fp, "DifPixels.Up2Down.dDiff	%lf\n", DifPixels.Up2Down.dDiff);
				fprintf(fp, "DifPixels.Left2Right.dDiff	%lf\n", DifPixels.Left2Right.dDiff);
				fprintf(fp, "DifPixels.UpLeft2DownRight.dDiff	%lf\n", DifPixels.UpLeft2DownRight.dDiff);
				fprintf(fp, "DifPixels.UpRight2DownLeft.dDiff	%lf\n", DifPixels.UpRight2DownLeft.dDiff);
				fprintf(fp, "DifPixels.dEnergy	%lf\n", DifPixels.dEnergy);
				fprintf(fp, "DifPixels.dVmX	%lf\n", DifPixels.dVmX);
				fprintf(fp, "DifPixels.dVmY	%lf\n", DifPixels.dVmY);
				fprintf(fp, "DifPixels.dVmNorm	%d\n", DifPixels.dVmNorm);
				fprintf(fp, "DifPixels.dOrientation	%lf\n\n", DifPixels.dOrientation);
				fprintf(fp, "iMSmth	iSSmth	dCurOrient	dCos	dDistance	dDistance2	dWeight	QL	dSum\n");
			}
 #endif
#endif

			//エネルギーの大きさによって、フィルターを切替
		   //端処理は平均拡張
			double dSum = 0.0;
			int iCounter = 0;
			if (DifPixels.dEnergy > dThEnergy) {//異方性フィルター
				//DifPixels.Orientation の方向に近い画素ほどウェイトを重くした2次元フィルター（任意方向に対応）
				double dSumWeight =0.0;
				double dSum = 0.0;
				double dWeight, dCurOrient, dCos;
				double dDistance;//補正対象画素と参照画素との距離
				double dDistance2;//補正対象画素と参照画素との距離2乗分の１
				for (int iSSmth = -iHalfMask; iSSmth <= iHalfMask; iSSmth++) {
					for (int iMSmth = -iHalfMask; iMSmth <= iHalfMask; iMSmth++) {
						if (0 == iSSmth && 0 == iMSmth) {//フィルターの中心点
							dCos = 1.0;
							dDistance2 = dSelfWeight;
							dWeight = dCos * dCos * dDistance2;//距離による重みを加えて、ボケを少なくする
							dSum += DummyImage.Dt[iSCnt + iHalfMask][iMCnt] * dWeight;
							dSumWeight += dWeight;
						}
						else if (0 == iMSmth) {
							dCurOrient = (iSSmth > 0) ? 270. : 90.;
							dCos = cos((dCurOrient - DifPixels.dOrientation) * PI / 180);
							dDistance = sqrt((float)iSSmth * iSSmth + iMSmth * iMSmth);
							dDistance2 = 1 / dDistance / dDistance;
							dWeight = dCos * dCos * dDistance2;//距離による重みを加えて、ボケを少なくする
							dSum += DummyImage.Dt[iSCnt + iHalfMask + iSSmth][iMCnt + iMSmth] * dWeight;
							dSumWeight += dWeight;
						}
						else{
							dCurOrient = atan(-(double)iSSmth / iMSmth) * 180 / PI;
							dCos = cos((dCurOrient - DifPixels.dOrientation) * PI / 180);
							dDistance = sqrt((float)iSSmth * iSSmth + iMSmth * iMSmth);
							dDistance2 = 1 / dDistance / dDistance;
							dWeight = dCos * dCos * dDistance2;//距離による重みを加えて、ボケを少なくする
							dSum += DummyImage.Dt[iSCnt + iHalfMask + iSSmth][iMCnt + iMSmth] * dWeight;
							dSumWeight += dWeight;
						}

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//						if (119 == iSCnt && 703 == iMCnt) {
//						if (1 == iSCnt && 46 == iMCnt) {
						if (9 == iSCnt && 504 == iMCnt) {
							fprintf(fp, "%d	%d	%lf	%lf	%lf	%lf	%lf	%d	%lf\n", iMSmth, iSSmth, dCurOrient, dCos, dDistance, dDistance2, dWeight, DummyImage.Dt[iSCnt + iHalfMask + iSSmth][iMCnt + iMSmth], dSum);
						}
 #endif
#endif
					}
				}
				OutImage.Dt[iSCnt][iMCnt] = (unsigned short int)(dSum / dSumWeight);
			}
			else{//等方性フィルター
				//副フィルター
				double dSum = 0.0;
					int iCounter = 0;
				for(int i = -iHalfMask; i <= iHalfMask; ++i){
					dSum += DummyImage.Dt[iSCnt + i + iHalfMask][iMCnt];
					iCounter++;
#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//					if (1 == iSCnt && 46 == iMCnt) {
					if (9 == iSCnt && 504 == iMCnt) {
						fprintf(fp, "%d	%d	%lf\n", i, DummyImage.Dt[iSCnt + i + iHalfMask][iMCnt], dSum);
					}
 #endif
#endif
				}
				OutImage.Dt[iSCnt][iMCnt] = (unsigned short int)(dSum / iCounter);
#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//				if (1 == iSCnt && 46 == iMCnt) {
				if (9 == iSCnt && 504 == iMCnt) {
					fprintf(fp, "%d\n\n", OutImage.Dt[iSCnt][iMCnt]);
				}
 #endif
#endif

				//主フィルター
				dSum = 0.0;
				iCounter = 0;
				for(int i = -iHalfMask; i <= iHalfMask; ++i){
					if (0 > iMCnt + i ||  MainPixel - 1 < iMCnt + i) {
						continue;
					}
#if 0
					dSum += OutImage.Dt[iSCnt][iMCnt + i];
#else
					int iQLSmth;
					if (i) {
						iQLSmth = DummyImage.Dt[iSCnt + iHalfMask][iMCnt + i];
					}
					else{
						iQLSmth = OutImage.Dt[iSCnt][iMCnt + i];
					}
					dSum += iQLSmth;
#endif
					iCounter++;

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//					if (1 == iSCnt && 46 == iMCnt) {
					if (9 == iSCnt && 504 == iMCnt) {
						fprintf(fp, "%d	%d	%lf\n", i, iQLSmth, dSum);
					}
 #endif
#endif
				}
				OutImage.Dt[iSCnt][iMCnt] = (unsigned short int)(dSum / iCounter);
#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//				if (1 == iSCnt && 46 == iMCnt) {
				if (9 == iSCnt && 504 == iMCnt) {
					fprintf(fp, "%d\n\n", OutImage.Dt[iSCnt][iMCnt]);
				}
 #endif
#endif


			}//endif 等方性フィルター

		}
	}


#ifdef _LP_DEBUG_OUTPUT
	//debug
 #ifdef _EDGE_WEIGHT_OUT
	fclose(fp);
 #endif
	ImageUp2Down.WriteTma( AddPrefix(FileName, "SobelUp2Down"));
	ImageLeft2Right.WriteTma( AddPrefix(FileName, "SobelLeft2Right"));
	ImageUpLeft2DownRight.WriteTma( AddPrefix(FileName, "SobelUpLeft2DownRight"));
	ImageUpRight2DownLeft.WriteTma( AddPrefix(FileName, "SobelUpRight2DownLeft"));

	//outimage
	OutImage.WriteTma( AddPrefix(FileName, "f"));
#endif

	//適応フィルター前後の差分画像出力（ノイズ画像）
#if 0
	TTMA ImgOrg = *this;
	ImgNoise = ImgOrg - OutImage + 30000;
#else
//	ImgNoise.NewDt(SubPixel, MainPixel, Bit);
	ImgNoise.NewDt(SubPixel, MainPixel, 16);
	for (int iS = 0; iS < SubPixel; iS++) {
		for (int iM = 0; iM < MainPixel; iM++) {
			ImgNoise.SetVal(iS, iM, Dt[iS][iM] - OutImage.Dt[iS][iM] + 30000);
		}
	}
#endif

#ifdef _LP_DEBUG_OUTPUT
	ImgNoise.WriteTma( AddPrefix(FileName, "Noise(" + IntToStr(iMaskSize) + "," + FloatToStrF(dThEnergy,ffFixed,2,0) + ",w" + FloatToStrF(dSelfWeight,ffFixed,2,0) + ")_"));//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
#endif
}
//---------------------------------------------------------------------------
//DummyImageを左右方向にも平均拡張するようにアップデート　(150928 ttsuji)


//各解像度画像に対する粒状抑制処理
void TSIMARS::LPGranularityReductionForEachSizeImage2(string FileName, TSIMARS& ImgNoise, int iMaskSize, double dThEnergy, double dSelfWeight)
{
	DIFFERENTIALARROUNDPIXELS DifPixels;

	//debugのための画像
	TTMA ImageUp2Down = *this;
	TTMA ImageLeft2Right = *this;
	TTMA ImageUpLeft2DownRight = *this;
	TTMA ImageUpRight2DownLeft = *this;
	TTMA OutImage = *this;

	//フィルター処理（まずは主と副の２パターンで場合わけ）
	int iHalfMask = iMaskSize / 2;

	//端処理用の平均拡張の画素値計算
	TTMA DummyImageUp, DummyImageDown, DummyImageLeft, DummyImageRight;
	DummyImageUp.NewDt(1, MainPixel, Bit);
	DummyImageDown.NewDt(1, MainPixel, Bit);
	DummyImageLeft.NewDt(SubPixel, 1, Bit);
	DummyImageRight.NewDt(SubPixel, 1, Bit);
	double dSumEdge = 0.;
	int iMCnt, iSCnt;
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		dSumEdge = 0.;
		for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) {
			dSumEdge += Dt[iSCnt][iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageUp.SetVal(0, iMCnt, dSumEdge);
	}
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		dSumEdge = 0.;
		for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) {
			dSumEdge += Dt[SubPixel - 1 - iSCnt][iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageDown.SetVal(0, iMCnt, dSumEdge);
	}
	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
		dSumEdge = 0.;
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			dSumEdge += Dt[iSCnt][iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageLeft.SetVal(iSCnt, 0, dSumEdge);
	}
	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
		dSumEdge = 0.;
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			dSumEdge += Dt[iSCnt][MainPixel - 1 - iMCnt];
		}
		dSumEdge /= iHalfMask;
		DummyImageRight.SetVal(iSCnt, 0, dSumEdge);
	}
#if 0
	//debug
	DummyImageUp.WriteTma( AddPrefix(FileName, "ExpUp"));
	DummyImageDown.WriteTma( AddPrefix(FileName, "ExpDown"));
	DummyImageLeft.WriteTma( AddPrefix(FileName, "ExpLeft"));
	DummyImageRight.WriteTma( AddPrefix(FileName, "ExpRight"));
#endif

	TTMA DummyImage;
	DummyImage.NewDt(SubPixel + iHalfMask*2, MainPixel + iHalfMask*2, Bit);

    //ここだけ未実装（DummyImageの定義変更のみ）
	for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) { //上端
		for (iMCnt = iHalfMask; iMCnt < DummyImage.MainPixel - iHalfMask; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = DummyImageUp.Dt[0][iMCnt - iHalfMask];
		}
	}
	for (iSCnt = iHalfMask; iSCnt < DummyImage.SubPixel - iHalfMask; iSCnt++) {//中央
		for (iMCnt = iHalfMask; iMCnt < DummyImage.MainPixel - iHalfMask; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = Dt[iSCnt - iHalfMask][iMCnt - iHalfMask];
		}
	}
	for (iSCnt = DummyImage.SubPixel - iHalfMask; iSCnt < DummyImage.SubPixel; iSCnt++) {//下端
		for (iMCnt = iHalfMask; iMCnt < DummyImage.MainPixel - iHalfMask; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = DummyImageDown.Dt[0][iMCnt - iHalfMask];
		}
	}
	for (iSCnt = iHalfMask; iSCnt < DummyImage.SubPixel - iHalfMask; iSCnt++) { //左端
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = DummyImageLeft.Dt[iSCnt - iHalfMask][0];
		}
	}
	for (iSCnt = iHalfMask; iSCnt < DummyImage.SubPixel - iHalfMask; iSCnt++) { //右端
		for (iMCnt = DummyImage.MainPixel - iHalfMask; iMCnt < DummyImage.MainPixel; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = DummyImageRight.Dt[iSCnt - iHalfMask][0];
		}
	}
	//DummyImageの４コーナーをさらに実装する
	for (iSCnt = 0; iSCnt < iHalfMask; iSCnt++) {
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = Dt[0][0];//左上
		}
		for (iMCnt = DummyImage.MainPixel - iHalfMask; iMCnt < DummyImage.MainPixel; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = Dt[0][MainPixel - 1];//右上
		}
	}
	for (iSCnt = DummyImage.SubPixel - iHalfMask; iSCnt < DummyImage.SubPixel; iSCnt++) {
		for (iMCnt = 0; iMCnt < iHalfMask; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = Dt[SubPixel - 1][0];//左下
		}
		for (iMCnt = DummyImage.MainPixel - iHalfMask; iMCnt < DummyImage.MainPixel; iMCnt++) {
			DummyImage.Dt[iSCnt][iMCnt] = Dt[SubPixel - 1][MainPixel - 1];//右下
		}
	}



#ifdef _LP_DEBUG_OUTPUT
	DummyImage.WriteTma( AddPrefix(FileName, "DummyImage"));
 #ifdef _EDGE_WEIGHT_OUT
	FILE* fp = fopen((ExtractFilePath(FileName) + "EdgeAndWeightInf_" + ExtractFileName(FileName) + ".txt").c_str(), "wt");
 #endif
#endif


	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
		for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
			double dMax(0.), dMin(100000.);

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//			if (119 == iSCnt && 703 == iMCnt) {
//			if (1 == iSCnt && 46 == iMCnt) {
//			if (9 == iSCnt && 504 == iMCnt) {
//			if (5 == iSCnt && 0 == iMCnt) {
			if (167 == iSCnt && 2163 == iMCnt) {
				int bbb = 0;
				fprintf(fp, "iM	%d\n", iMCnt);
				fprintf(fp, "iS	%d\n", iSCnt);
			}
 #endif
#endif

			DifPixels = CalcDifferentialArroundPixel_Fast(DummyImage, iSCnt + iHalfMask, iMCnt + iHalfMask);//ある点の周囲の微分情報を計算する

			//debug
			ImageUp2Down.SetVal(iSCnt, iMCnt, DifPixels.Up2Down.dDiff + DtMax/2);
			ImageLeft2Right.SetVal(iSCnt, iMCnt, DifPixels.Left2Right.dDiff + DtMax/2);
			ImageUpLeft2DownRight.SetVal(iSCnt, iMCnt, DifPixels.UpLeft2DownRight.dDiff + DtMax/2);
			ImageUpRight2DownLeft.SetVal(iSCnt, iMCnt, DifPixels.UpRight2DownLeft.dDiff + DtMax/2);

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//			if (119 == iSCnt && 703 == iMCnt) {
//			if (1 == iSCnt && 46 == iMCnt) {
//			if (9 == iSCnt && 504 == iMCnt) {
//			if (5 == iSCnt && 0 == iMCnt) {
			if (167 == iSCnt && 2163 == iMCnt) {
				fprintf(fp, "DifPixels.Up2Down.dDiff	%lf\n", DifPixels.Up2Down.dDiff);
				fprintf(fp, "DifPixels.Left2Right.dDiff	%lf\n", DifPixels.Left2Right.dDiff);
				fprintf(fp, "DifPixels.UpLeft2DownRight.dDiff	%lf\n", DifPixels.UpLeft2DownRight.dDiff);
				fprintf(fp, "DifPixels.UpRight2DownLeft.dDiff	%lf\n", DifPixels.UpRight2DownLeft.dDiff);
				fprintf(fp, "DifPixels.dEnergy	%lf\n", DifPixels.dEnergy);
				fprintf(fp, "DifPixels.dVmX	%lf\n", DifPixels.dVmX);
				fprintf(fp, "DifPixels.dVmY	%lf\n", DifPixels.dVmY);
				fprintf(fp, "DifPixels.dVmNorm	%d\n", DifPixels.dVmNorm);
				fprintf(fp, "DifPixels.dOrientation	%lf\n\n", DifPixels.dOrientation);
				fprintf(fp, "iMSmth	iSSmth	dCurOrient	dCos	dDistance	dDistance2	dWeight	QL	dSum\n");
			}
 #endif
#endif

			//エネルギーの大きさによって、フィルターを切替
		   //端処理は平均拡張
			double dSum = 0.0;
			int iCounter = 0;
			if (DifPixels.dEnergy > dThEnergy) {//異方性フィルター
				//DifPixels.Orientation の方向に近い画素ほどウェイトを重くした2次元フィルター（任意方向に対応）
				double dSumWeight =0.0;
				double dSum = 0.0;
				double dWeight, dCurOrient, dCos;
				double dDistance;//補正対象画素と参照画素との距離
				double dDistance2;//補正対象画素と参照画素との距離2乗分の１
				for (int iSSmth = -iHalfMask; iSSmth <= iHalfMask; iSSmth++) {
					for (int iMSmth = -iHalfMask; iMSmth <= iHalfMask; iMSmth++) {
						if (0 == iSSmth && 0 == iMSmth) {//フィルターの中心点
							dCos = 1.0;
							dDistance2 = dSelfWeight;
							dWeight = dCos * dCos * dDistance2;//距離による重みを加えて、ボケを少なくする
							dSum += DummyImage.Dt[iSCnt + iHalfMask][iMCnt + iHalfMask] * dWeight;
							dSumWeight += dWeight;
						}
						else if (0 == iMSmth) {
							dCurOrient = (iSSmth > 0) ? 270. : 90.;
							dCos = cos((dCurOrient - DifPixels.dOrientation) * PI / 180);
							dDistance = sqrt((float)iSSmth * iSSmth + iMSmth * iMSmth);
							dDistance2 = 1 / dDistance / dDistance;
							dWeight = dCos * dCos * dDistance2;//距離による重みを加えて、ボケを少なくする
							dSum += DummyImage.Dt[iSCnt + iHalfMask + iSSmth][iMCnt + iHalfMask + iMSmth] * dWeight;
							dSumWeight += dWeight;
						}
						else{
							dCurOrient = atan(-(double)iSSmth / iMSmth) * 180 / PI;
							dCos = cos((dCurOrient - DifPixels.dOrientation) * PI / 180);
							dDistance = sqrt((float)iSSmth * iSSmth + iMSmth * iMSmth);
							dDistance2 = 1 / dDistance / dDistance;
							dWeight = dCos * dCos * dDistance2;//距離による重みを加えて、ボケを少なくする
							dSum += DummyImage.Dt[iSCnt + iHalfMask + iSSmth][iMCnt + iHalfMask + iMSmth] * dWeight;
							dSumWeight += dWeight;
						}

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//						if (119 == iSCnt && 703 == iMCnt) {
//						if (1 == iSCnt && 46 == iMCnt) {
//						if (9 == iSCnt && 504 == iMCnt) {
//						if (5 == iSCnt && 0 == iMCnt) {
						if (167 == iSCnt && 2163 == iMCnt) {
							fprintf(fp, "%d	%d	%lf	%lf	%lf	%lf	%lf	%d	%lf\n", iMSmth, iSSmth, dCurOrient, dCos, dDistance, dDistance2, dWeight, DummyImage.Dt[iSCnt + iHalfMask + iSSmth][iMCnt + iHalfMask + iMSmth], dSum);
						}
 #endif
#endif
					}
				}
				OutImage.Dt[iSCnt][iMCnt] = (unsigned short int)(dSum / dSumWeight);
			}
			else{//等方性フィルター
				//副フィルター
				double dSum = 0.0;
					int iCounter = 0;
				for(int i = -iHalfMask; i <= iHalfMask; ++i){
					dSum += DummyImage.Dt[iSCnt + i + iHalfMask][iMCnt + iHalfMask];
					iCounter++;
#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//					if (1 == iSCnt && 46 == iMCnt) {
//					if (9 == iSCnt && 504 == iMCnt) {
//					if (5 == iSCnt && 0 == iMCnt) {
					if (167 == iSCnt && 2163 == iMCnt) {
						fprintf(fp, "%d	%d	%lf\n", i, DummyImage.Dt[iSCnt + i + iHalfMask][iMCnt + iHalfMask], dSum);
					}
 #endif
#endif
				}
				OutImage.Dt[iSCnt][iMCnt] = (unsigned short int)(dSum / iCounter);
#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//				if (1 == iSCnt && 46 == iMCnt) {
//				if (9 == iSCnt && 504 == iMCnt) {
//				if (5 == iSCnt && 0 == iMCnt) {
				if (167 == iSCnt && 2163 == iMCnt) {
					fprintf(fp, "%d\n\n", OutImage.Dt[iSCnt][iMCnt]);
				}
 #endif
#endif

				//主フィルター
				dSum = 0.0;
				iCounter = 0;
				for(int i = -iHalfMask; i <= iHalfMask; ++i){
#if 0
					dSum += OutImage.Dt[iSCnt][iMCnt + i];
#else
					int iQLSmth;
					if (i) {
						iQLSmth = DummyImage.Dt[iSCnt + iHalfMask][iMCnt + iHalfMask + i];
					}
					else{
						iQLSmth = OutImage.Dt[iSCnt][iMCnt + i];
					}
					dSum += iQLSmth;
#endif
					iCounter++;

#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//					if (1 == iSCnt && 46 == iMCnt) {
//					if (9 == iSCnt && 504 == iMCnt) {
//					if (5 == iSCnt && 0 == iMCnt) {
					if (167 == iSCnt && 2163 == iMCnt) {
						fprintf(fp, "%d	%d	%lf\n", i, iQLSmth, dSum);
					}
 #endif
#endif
				}
				OutImage.Dt[iSCnt][iMCnt] = (unsigned short int)(dSum / iCounter);
#ifdef _LP_DEBUG_OUTPUT
 #ifdef _EDGE_WEIGHT_OUT
//				if (1 == iSCnt && 46 == iMCnt) {
//				if (9 == iSCnt && 504 == iMCnt) {
//				if (5 == iSCnt && 0 == iMCnt) {
				if (167 == iSCnt && 2163 == iMCnt) {
					fprintf(fp, "%d\n\n", OutImage.Dt[iSCnt][iMCnt]);
				}
 #endif
#endif


			}//endif 等方性フィルター

		}
	}


#ifdef _LP_DEBUG_OUTPUT
	//debug
 #ifdef _EDGE_WEIGHT_OUT
	fclose(fp);
 #endif
	ImageUp2Down.WriteTma( AddPrefix(FileName, "SobelUp2Down"));
	ImageLeft2Right.WriteTma( AddPrefix(FileName, "SobelLeft2Right"));
	ImageUpLeft2DownRight.WriteTma( AddPrefix(FileName, "SobelUpLeft2DownRight"));
	ImageUpRight2DownLeft.WriteTma( AddPrefix(FileName, "SobelUpRight2DownLeft"));

	//outimage
	OutImage.WriteTma( AddPrefix(FileName, "f"));
#endif

	//適応フィルター前後の差分画像出力（ノイズ画像）
#if 0
	TTMA ImgOrg = *this;
	ImgNoise = ImgOrg - OutImage + 30000;
#else
//	ImgNoise.NewDt(SubPixel, MainPixel, Bit);
	ImgNoise.NewDt(SubPixel, MainPixel, 16);
	for (int iS = 0; iS < SubPixel; iS++) {
		for (int iM = 0; iM < MainPixel; iM++) {
			ImgNoise.SetVal(iS, iM, Dt[iS][iM] - OutImage.Dt[iS][iM] + 30000);
		}
	}
#endif

#ifdef _LP_DEBUG_OUTPUT
	ImgNoise.WriteTma( AddPrefix(FileName, "Noise(" + IntToStr(iMaskSize) + "," + FloatToStrF(dThEnergy,ffFixed,2,0) + ",w" + FloatToStrF(dSelfWeight,ffFixed,2,0) + ")_"));//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
#endif
}
//---------------------------------------------------------------------------
//#define _GAUSSIAN_HASHI_SYORI
#define _EXTRACT_SEQUENTIAL

//0.5倍に縮小（ガウシアンピラミッド生成）
void TSIMARS::LPResize05(string& FileName)
{
#ifdef _GAUSSIAN_HASHI_SYORI
	TTMA ImgOrg = *this;
#endif
	float filter[5] = {0.046875, 0.25, 0.40625, 0.25, 0.046875};//1/2倍縮小時に間引き前にかけるフィルター
	Filter(filter, 5, true);
#ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MainGausian"));
#endif

#ifdef _EXTRACT_SEQUENTIAL
	Extract(1, 0, 2, 0);    //主副それぞれ1画素飛ばしの間引き（1/2倍に間引き）
 #ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MMainGausian"));
 #endif
#endif

	Filter(filter, 5, false);
#ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "SubGausian"));
#endif

#ifdef _EXTRACT_SEQUENTIAL
	Extract(2, 0, 1, 0);    //主副それぞれ1画素飛ばしの間引き（1/2倍に間引き）
 #ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MSubGausian"));
 #endif
#endif

#ifdef _GAUSSIAN_HASHI_SYORI
	int iS, iM;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			if (iS < 3 || iS >= SubPixel - 3 || iM < 3 || iM >= MainPixel - 3 ) {
				SetVal(iS, iM, ImgOrg.Dt[iS][iM]);
			}
		}
	}
#endif

#ifndef _EXTRACT_SEQUENTIAL
	Extract(2, 0, 2, 0);    //主副それぞれ1画素飛ばしの間引き（1/2倍に間引き）
#endif

#ifdef _LP_DEBUG_OUTPUT
	WriteTma(FileName);
#endif
}
//---------------------------------------------------------------------------

//0.5倍に縮小（ガウシアンピラミッド生成、端処理は平均拡張）
void TSIMARS::LPResizeExpand05(string& FileName)
{
	TTMA img;
	TIIMG iimg;
	string strGauss = "C:\\GranuraritySuppression_Gauss.txt";

	iimg = *this;

	//主方向にフィルター処理（端処理は平均拡張）
	iimg.MainSub();
	if(iimg.SubExpandMultiAve(strGauss) == false){
		CApplication::GetInstance()->MessageBox((strGauss + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return;
	}
	iimg.MainSub();

	*this = iimg;
	SetBit(Bit);
#ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MainGausian"));
#endif

	Extract(1, 0, 2, 0);    //主副それぞれ1画素飛ばしの間引き（1/2倍に間引き）
 #ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MMainGausian"));
 #endif


	//副方向にフィルター処理（端処理は平均拡張）
	iimg = *this;
	if(iimg.SubExpandMultiAve(strGauss) == false){
		CApplication::GetInstance()->MessageBox((strGauss + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return;
	}
	*this = iimg;
	SetBit(Bit);
#ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "SubGausian"));
#endif

	Extract(2, 0, 1, 0);    //主副それぞれ1画素飛ばしの間引き（1/2倍に間引き）
 #ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MSubGausian"));
 #endif


#ifdef _LP_DEBUG_OUTPUT
	WriteTma(FileName);
#endif
}
//---------------------------------------------------------------------------
//2倍に拡大（ラプラシアンピラミッド生成）

#define _LAPLACIAN_HASHI_SYORI

void TSIMARS::LPResize2(string& FileName)
{
	float filter[7] = {0.020843506, 0.166656494, 0.479156494, 0.666687012, 0.479156494, 0.166656494, 0.020843506};//2倍拡大時に0挿入補間後にかけるフィルター
	Mul0Insert(2, 2);//0挿入補間により主副にそれぞれ2倍拡大

#ifdef _LAPLACIAN_HASHI_SYORI
	TTMA ImgOrg = *this;
#endif
	Filter(filter, 7, true);
	Filter(filter, 7, false);
#ifdef _LAPLACIAN_HASHI_SYORI
	int iS, iM;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			if (iS < 3 || iS >= SubPixel - 3 || iM < 3 || iM >= MainPixel - 3 ) {
//				SetVal(iS, iM, ImgOrg.Dt[iS][iM]);
				SetVal(iS, iM, 30000);
			}
		}
	}
#endif


#ifdef _LP_DEBUG_OUTPUT
//	WriteTma(FileName);
#endif
}
//---------------------------------------------------------------------------
//2倍に拡大（ラプラシアンピラミッド生成、端処理は平均拡張）


void TSIMARS::LPResizeExpand2(string& FileName)
{
	Mul0Insert(2, 2);//0挿入補間により主副にそれぞれ2倍拡大
 #ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "0Insert"));
 #endif

	TTMA img;
	TIIMG iimg;
	string strGauss = "C:\\GranuraritySuppression_Laplacian.txt";

	iimg = *this;

	//主方向にフィルター処理（端処理は平均拡張）
	iimg.MainSub();
//	if(iimg.SubExpandMultiAve(strGauss) == false){
	if(iimg.SubExpandMultiAve(strGauss, 15) == false){
		CApplication::GetInstance()->MessageBox((strGauss + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return;
	}
	iimg.MainSub();

	*this = iimg;
	SetBit(Bit);
#ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "MainLaplacian"));
#endif

	//副方向にフィルター処理（端処理は平均拡張）
//	if(iimg.SubExpandMultiAve(strGauss) == false){
	if(iimg.SubExpandMultiAve(strGauss, 15) == false){
		CApplication::GetInstance()->MessageBox((strGauss + " がopenできません\nまたはフィルタータップ数に対して画像幅が小さすぎます。").c_str(), "Error", MB_OK);
		return;
	}
	*this = iimg;
	SetBit(Bit);
#ifdef _LP_DEBUG_OUTPUT
	WriteTma(AddPrefix(FileName, "SubLaplacian"));
#endif

#ifdef _LAPLACIAN_HASHI_SYORI
	int iS, iM;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			if (iS < 3 || iS >= SubPixel - 3 || iM < 3 || iM >= MainPixel - 3 ) {
//				SetVal(iS, iM, ImgOrg.Dt[iS][iM]);
				SetVal(iS, iM, 30000);
			}
		}
	}
#endif
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeDxaCalib(DXA_CALIB_INFO& DxaCalib)
{
	DxaCalib.iY1 = DxaCalib.iY2 = DxaCalib.iBoneX1 = DxaCalib.iBoneX2 = DxaCalib.iSoftLX1 = DxaCalib.iSoftLX2 = DxaCalib.iSoftRX1 = DxaCalib.iSoftRX2 = DxaCalib.iNumOfUretanThickness = 0;

	for (int i = 0; i < 5; i++) {//各ウレタン厚について初期化
		DxaCalib.dMAS[i] = 0.0;
		DxaCalib.dESQLSoftL[i] = 0.0;
		DxaCalib.dESQLSoftR[i] = 0.0;
		DxaCalib.dESQLSoft[i] = 0.0;
		DxaCalib.dESQLBone[i] = 0.0;
		DxaCalib.dESQLBoneContrast[i] = 0.0;
		DxaCalib.dCsIQLSoftL[i] = 0.0;
		DxaCalib.dCsIQLSoftR[i] = 0.0;
		DxaCalib.dCsIQLSoft[i] = 0.0;
		DxaCalib.dCsIQLBone[i] = 0.0;
		DxaCalib.dCsIQLBoneContrast[i] = 0.0;
		DxaCalib.dGOSQLSoftL[i] = 0.0;
		DxaCalib.dGOSQLSoftR[i] = 0.0;
		DxaCalib.dGOSQLSoft[i] = 0.0;
		DxaCalib.dGOSQLBone[i] = 0.0;
		DxaCalib.dGOSQLBoneContrast[i] = 0.0;
	}
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeDXAIntegratedSoftRoiObliqueBoneROI(DXA_INTEGRATED_SOFTROI_OBLIQUE_BONEROI& Dxa)
{
	Dxa.iTopX = Dxa.iTopY = Dxa.iBottomX = Dxa.iBottomY = Dxa.iLeftX = Dxa.iLeftY = Dxa.iRightX = Dxa.iRightY = 0;
	Dxa.dESQLSoftL = Dxa.dESQLSoftR = Dxa.dESQLSoft = Dxa.dESQLBone = Dxa.dESQLBoneContrast = 0.0;
	Dxa.dCsIQLSoftL = Dxa.dCsIQLSoftR = Dxa.dCsIQLSoft = Dxa.dCsIQLBone = Dxa.dCsIQLBoneContrast = 0.0;
	Dxa.dGOSQLSoftL = Dxa.dGOSQLSoftR = Dxa.dGOSQLSoft = Dxa.dGOSQLBone = Dxa.dGOSQLBoneContrast = 0.0;
	Dxa.dQLSoft = Dxa.dBMD = Dxa.dBMDCalib = 0.0;
	Dxa.RectSoftL;
	Dxa.RectSoftR;
	Dxa.QuadBone;
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeDXAIntegratedSoftRoiBoneMap(DXA_INTEGRATED_SOFTROI_BONEMAP& Dxa)
{
	Dxa.iTopX = Dxa.iTopY = Dxa.iBottomX = Dxa.iBottomY = Dxa.iLeftX = Dxa.iLeftY = Dxa.iRightX = Dxa.iRightY = 0;
	Dxa.dESQLSoftL = Dxa.dESQLSoftR = Dxa.dESQLSoft = Dxa.dESQLBone = Dxa.dESQLBoneContrast = 0.0;
	Dxa.dCsIQLSoftL = Dxa.dCsIQLSoftR = Dxa.dCsIQLSoft = Dxa.dCsIQLBone = Dxa.dCsIQLBoneContrast = 0.0;
	Dxa.dGOSQLSoftL = Dxa.dGOSQLSoftR = Dxa.dGOSQLSoft = Dxa.dGOSQLBone = Dxa.dGOSQLBoneContrast = 0.0;
	Dxa.dQLSoft = Dxa.dBMD = Dxa.dBMDCalib = Dxa.dAreaBone = Dxa.dBMC = Dxa.dAreaSoftL = Dxa.dAreaSoftR = 0.0;
	Dxa.fMAS = 0.0;
	Dxa.QTopBottomLine;
	Dxa.RectSoftL;
	Dxa.RectSoftR;
	Dxa.BoneMap;
	Dxa.vUnitTop;
	Dxa.vUnitBottom;
	Dxa.iAreaBone = Dxa.iAreaSoftL = Dxa.iAreaSoftR = 0;
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeDXAFemurTotal(DXA_FEMUR_TOTAL& Dxa)
{
	Dxa.dESQLSoft = Dxa.dESQLBone = Dxa.dESQLBoneContrast = 0.0;
	Dxa.dCsIQLSoft = Dxa.dCsIQLBone = Dxa.dCsIQLBoneContrast = 0.0;
	Dxa.dGOSQLSoft = Dxa.dGOSQLBone = Dxa.dGOSQLBoneContrast = 0.0;
	Dxa.dQLSoft = Dxa.dBMD = Dxa.dBMDCalib = Dxa.dAreaBone = Dxa.dAreaSoft = Dxa.dBMC = Dxa.fMAS = 0.0;
	Dxa.iXBodyCenter = Dxa.iYBodyCenter = Dxa.iXBodySide = Dxa.iYBodySide = Dxa.iXBoneTop = Dxa.iYBoneTop = 0;
	Dxa.PTotal;
	Dxa.QTotal;
	Dxa.bPentagon = true;
	Dxa.QNeck;
	Dxa.RectGlobal;
	Dxa.BoneMap;
	Dxa.iAreaBone = Dxa.iAreaSoft = 0;
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeDXAObliqueRoi(DXA_OBLIQUE_ROI& Dxa)
{
	Dxa.QuadBone;
	Dxa.QuadSoftL;
	Dxa.QuadSoftR;
	Dxa.dCsIQLSoft = 0.0;
	Dxa.dCsIQLSoftL = 0.0;
	Dxa.dCsIQLSoftR = 0.0;
	Dxa.dESQLBone = 0.0;
	Dxa.dESQLBoneContrast = 0.0;
	Dxa.dESQLSoft = 0.0;
	Dxa.dESQLSoftL = 0.0;
	Dxa.dESQLSoftR = 0.0;
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeDxaJIS(DXA_JIS& Dxa)
{
	Dxa.RectBone;
	Dxa.RectSoftL;
	Dxa.RectSoftR;
	Dxa.dBMD = Dxa.dBMDCalib = Dxa.dQLSoft = Dxa.dArea = Dxa.dBMC = 0.0;
	Dxa.dCsIQLBone = Dxa.dCsIQLBoneContrast = Dxa.dCsIQLSoft = Dxa.dCsIQLSoftL = Dxa.dCsIQLSoftR = 0.0;
	Dxa.dESQLBone = Dxa.dESQLBoneContrast = Dxa.dESQLSoft = Dxa.dESQLSoftL = Dxa.dESQLSoftR = 0.0;
	Dxa.dGOSQLBone = Dxa.dGOSQLBoneContrast = Dxa.dGOSQLSoft = Dxa.dGOSQLSoftL = Dxa.dGOSQLSoftR = 0.0;
	Dxa.fMAS = 0.0;
	Dxa.iArea = 0;
}
//---------------------------------------------------------------------------

void TSIMARS::InitializeNetsumuraInfo(NETSUMURA_INFO& Netsumura)
{
	Netsumura.iAccumTime = 0;
	Netsumura.dReadT = 0.0;
	Netsumura.dResetT = 0.0;//1ラインあたりのリセット画像読み取り時間[ms]
	Netsumura.dTotalReadT = 0.0;//画像読み取り時間[ms]
	Netsumura.iSubPixelOfReset = 0;
	Netsumura.dAccCoefTable = NULL;
	Netsumura.iResetYOfADAS = 0;
	Netsumura.iResetYOfFPGA = 0;
	Netsumura.iResetYOfRef = 0;
	Netsumura.dAccCoefAtADAS = 0.0;
	Netsumura.dAccCoefAtFPGA = 0.0;
	Netsumura.dAccCoefAtRef = 0.0;
	Netsumura.dTauADAS = 0.0;
	Netsumura.dTauFPGA = 0.0;
	Netsumura.dAlpha = 0.0;
	Netsumura.dBeta = 0.0;
	for (int iROI = 0; iROI < 3; iROI++) {
		Netsumura.dPercentileQLofRectAdas[iROI] = 0.0;
		Netsumura.dPercentileQLofRectFpga[iROI] = 0.0;
		Netsumura.dPercentileQLofRectRef[iROI] = 0.0;
		Netsumura.dMaxQLinAdasFpgaRef[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfAdas[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfFpga[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfRef[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfAdas[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfFpga[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfRef[iROI] = 0.0;

		Netsumura.iTimerLagCorOn[iROI]  = 0;
		Netsumura.iTimerLagCorOff[iROI] = 0;
		Netsumura.iTimerLagCorOnADAS[iROI]  = 0;
		Netsumura.iTimerLagCorOffADAS[iROI] = 0;
		Netsumura.iTimerLagCorOnFpga[iROI]  = 0;
		Netsumura.iTimerLagCorOffFpga[iROI] = 0;
		Netsumura.iTimerLagCorOnRef[iROI]  = 0;
		Netsumura.iTimerLagCorOffRef[iROI] = 0;
	}
	Netsumura.iROINumofAdas = 0;
	Netsumura.iROINumofFpga = 0;
	Netsumura.iROINumofRef  = 0;
	Netsumura.dGLag = 0.0;
	Netsumura.dMOGImgAveQLOfAdas = 0.0;
	Netsumura.dMOGImgAveQLOfFpga = 0.0;
	Netsumura.dMOGImgAveQLOfRef = 0.0;
	Netsumura.dShimiImgAveQLOfAdas = 0.0;
	Netsumura.dShimiImgAveQLOfFpga = 0.0;
	Netsumura.dShimiImgAveQLOfRef = 0.0;
	Netsumura.dNetsumuraADAS = 0.0;
	Netsumura.dNetsumuraFPGA = 0.0;
	Netsumura.dNetsumura = 0.0;
	Netsumura.dAmap_Adas = 0.0;
	Netsumura.dAmap_Fpga = 0.0;
	Netsumura.dFmap_Adas = 0.0;
	Netsumura.dFmap_Fpga = 0.0;
	Netsumura.iGeta = 0;

}
//---------------------------------------------------------------------------

void TSIMARS::InitializeNetsumuraInfo(NETSUMURA_INFO& Netsumura, int iSubPixelOfReset)
{
	Netsumura.iAccumTime = 0;
	Netsumura.dReadT = 0.0;
	Netsumura.dResetT = 0.0;//1ラインあたりのリセット画像読み取り時間[ms]
	Netsumura.dTotalReadT = 0.0;//画像読み取り時間[ms]
	Netsumura.iSubPixelOfReset = iSubPixelOfReset;

	if (!Netsumura.dAccCoefTable) {
		Netsumura.dAccCoefTable = new double[iSubPixelOfReset];
		for (int i = 0; i < iSubPixelOfReset; i++) {
			Netsumura.dAccCoefTable[i] = 0.0;
		}
	}

	Netsumura.iResetYOfADAS = 0;
	Netsumura.iResetYOfFPGA = 0;
	Netsumura.iResetYOfRef = 0;
	Netsumura.dAccCoefAtADAS =  0.0;
	Netsumura.dAccCoefAtFPGA =  0.0;
	Netsumura.dAccCoefAtRef =  0.0;
	Netsumura.dTauADAS = 0.0;
	Netsumura.dTauFPGA = 0.0;
	Netsumura.dAlpha = 0.0;
	Netsumura.dBeta = 0.0;
	for (int iROI = 0; iROI < 3; iROI++) {
		Netsumura.dPercentileQLofRectAdas[iROI] = 0.0;
		Netsumura.dPercentileQLofRectFpga[iROI] = 0.0;
		Netsumura.dPercentileQLofRectRef[iROI] = 0.0;
		Netsumura.dMaxQLinAdasFpgaRef[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfAdas[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfFpga[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfRef[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfAdas[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfFpga[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfRef[iROI] = 0.0;

		Netsumura.iTimerLagCorOn[iROI]  = 0;
		Netsumura.iTimerLagCorOff[iROI] = 0;
		Netsumura.iTimerLagCorOnADAS[iROI]  = 0;
		Netsumura.iTimerLagCorOffADAS[iROI] = 0;
		Netsumura.iTimerLagCorOnFpga[iROI]  = 0;
		Netsumura.iTimerLagCorOffFpga[iROI] = 0;
		Netsumura.iTimerLagCorOnRef[iROI]  = 0;
		Netsumura.iTimerLagCorOffRef[iROI] = 0;
	}
	Netsumura.iROINumofAdas = 0;
	Netsumura.iROINumofFpga = 0;
	Netsumura.iROINumofRef  = 0;
	Netsumura.dGLag = 0.0;
	Netsumura.dMOGImgAveQLOfAdas = 0.0;
	Netsumura.dMOGImgAveQLOfFpga = 0.0;
	Netsumura.dMOGImgAveQLOfRef = 0.0;
	Netsumura.dShimiImgAveQLOfAdas = 0.0;
	Netsumura.dShimiImgAveQLOfFpga = 0.0;
	Netsumura.dShimiImgAveQLOfRef = 0.0;
	Netsumura.dNetsumuraADAS = 0.0;
	Netsumura.dNetsumuraFPGA = 0.0;
	Netsumura.dNetsumura = 0.0;
	Netsumura.dAmap_Adas = 0.0;
	Netsumura.dAmap_Fpga = 0.0;
	Netsumura.dFmap_Adas = 0.0;
	Netsumura.dFmap_Fpga = 0.0;
	Netsumura.iGeta = 0;

}
//---------------------------------------------------------------------------

void TSIMARS::DeleteNetsumuraInfo(NETSUMURA_INFO& Netsumura)
{
	if (Netsumura.dAccCoefTable && Netsumura.iSubPixelOfReset) {
		delete[] Netsumura.dAccCoefTable;
		Netsumura.dAccCoefTable = NULL;
		Netsumura.iSubPixelOfReset = 0;
	}
	Netsumura.iAccumTime = 0;
	Netsumura.dReadT = 0.0;
	Netsumura.dResetT = 0.0;//1ラインあたりのリセット画像読み取り時間[ms]
	Netsumura.dTotalReadT = 0.0;//画像読み取り時間[ms]
	Netsumura.iResetYOfADAS = 0;
	Netsumura.iResetYOfFPGA = 0;
	Netsumura.iResetYOfRef = 0;
	Netsumura.dAccCoefAtADAS =  0.0;
	Netsumura.dAccCoefAtFPGA =  0.0;
	Netsumura.dAccCoefAtRef =  0.0;
	Netsumura.dTauADAS = 0.0;
	Netsumura.dTauFPGA = 0.0;
	Netsumura.dAlpha = 0.0;
	Netsumura.dBeta = 0.0;
	for (int iROI = 0; iROI < 3; iROI++) {
		Netsumura.dPercentileQLofRectAdas[iROI] = 0.0;
		Netsumura.dPercentileQLofRectFpga[iROI] = 0.0;
		Netsumura.dPercentileQLofRectRef[iROI] = 0.0;
		Netsumura.dMaxQLinAdasFpgaRef[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfAdas[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfFpga[iROI] = 0.0;
		Netsumura.dMOGLag1ImgAveQLOfRef[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfAdas[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfFpga[iROI] = 0.0;
		Netsumura.dMOGLag2ImgAveQLOfRef[iROI] = 0.0;

		Netsumura.iTimerLagCorOn[iROI]  = 0;
		Netsumura.iTimerLagCorOff[iROI] = 0;
		Netsumura.iTimerLagCorOnADAS[iROI]  = 0;
		Netsumura.iTimerLagCorOffADAS[iROI] = 0;
		Netsumura.iTimerLagCorOnFpga[iROI]  = 0;
		Netsumura.iTimerLagCorOffFpga[iROI] = 0;
		Netsumura.iTimerLagCorOnRef[iROI]  = 0;
		Netsumura.iTimerLagCorOffRef[iROI] = 0;
	}
	Netsumura.iROINumofAdas = 0;
	Netsumura.iROINumofFpga = 0;
	Netsumura.iROINumofRef  = 0;
	Netsumura.dGLag = 0.0;
	Netsumura.dMOGImgAveQLOfAdas = 0.0;
	Netsumura.dMOGImgAveQLOfFpga = 0.0;
	Netsumura.dMOGImgAveQLOfRef = 0.0;
	Netsumura.dShimiImgAveQLOfAdas = 0.0;
	Netsumura.dShimiImgAveQLOfFpga = 0.0;
	Netsumura.dShimiImgAveQLOfRef = 0.0;
	Netsumura.dNetsumuraADAS = 0.0;
	Netsumura.dNetsumuraFPGA = 0.0;
	Netsumura.dNetsumura = 0.0;
	Netsumura.dAmap_Adas = 0.0;
	Netsumura.dAmap_Fpga = 0.0;
	Netsumura.dFmap_Adas = 0.0;
	Netsumura.dFmap_Fpga = 0.0;
	Netsumura.iGeta = 0;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//画像から属性情報を読み取って構造体に格納する

//bool TSIMARS::ReadImageAttributeInf(IMG_ATTRIBUTE_INF& ImgAttInf, string StrFileName)
bool TSIMARS::ReadImageAttributeInf(TIMG_ATTRIBUTE_INF& ImgAttInf, string StrFileName)
{
#if 0
	if (!ReadTma(StrFileName)) {
		CApplication::GetInstance()->MessageBox((StrFileName + " readできませんでした").c_str(), "Error", MB_OK);
		return false;
	}
#else //必要最小限のメモリのみ確保しReadして処理時間短縮
	if(ExtractFileExt(StrFileName).AnsiCompareIC(".tma") != 0){
		CApplication::GetInstance()->MessageBox(("File が tma ではありません\n" + StrFileName).c_str(), "Error", MB_OK);
		return false;
	}

	int iCompress = iReadHeadder(StrFileName);//不要かも
    if (iCompress < 0){
		CApplication::GetInstance()->MessageBox((StrFileName + " readできませんでした").c_str(), "Error", MB_OK);
		return false;
	}

	if(iCompress == '0'){//圧縮なし
		string StrTdt = ChangeFileExt(StrFileName, ".tdt");
		int handle;		//file handler
		int i;
		int iSub(1), iMain(7); //1,7で決め打ち

		NewDt(iSub, iMain, Bit);//主7画素、副1画素のみバッファ確保
		if ((handle = open(StrTdt.c_str(), O_RDONLY|O_BINARY)) == -1){
			CApplication::GetInstance()->MessageBox((StrTdt + " openできませんでした").c_str(), "Error", MB_OK);
			return false;
		}
		if ((read(handle, Dt[0], iMain * 2)) == -1){
			CApplication::GetInstance()->MessageBox((StrTdt + "\n readできませんでした").c_str(), "Error", MB_OK);
			close(handle);
			return false;
		}
		close(handle);
	}
	else{
		CApplication::GetInstance()->MessageBox("圧縮無ししか扱えません", "Error", MB_OK);
		return false;
	}
#endif
	ImgAttInf.iShotGoID	= (int)Dt[0][0];//(iS, iM)の順
//	ImgAttInf.iMode		= (int)Dt[0][1];
	ImgAttInf.iMode		= (int)Dt[0][1] - 3;//3ずれている
	ImgAttInf.iStatus	= (int)Dt[0][2];
	ImgAttInf.iElapsedTimeSinceSWOFF = ((int)Dt[0][3] << 16) | (int)Dt[0][4]; //16bit左シフトした画素とシフトしていない画素のビット加算
	ImgAttInf.iXrayOnTime			 = ((int)Dt[0][5] << 16) | (int)Dt[0][6];
	ImgAttInf.StrFileName = StrFileName;
//	ImgAttInf.pImg = (CTDAT*)this; //画素数の配列をメンバとして持つと膨大のメモリ必要のため削除
//	ImgAttInf.bCor = false;//デフォルトはFALSE
	ImgAttInf.bCor = true;//デフォルトはTRUE
    ImgAttInf.bFirstFrOfMode = false;

	return true;
}
//---------------------------------------------------------------------------
//線欠陥補正対象画素の左右ROIにおけるそれぞれのEnergyの重みに応じてエッジの方向を計算

double TSIMARS::GetOrientWeightedByEnergy(DIFFERENTIALARROUNDPIXELS* pDifPixels)
{
	double dOrientWeightedByEnergy;
	if (fabs(pDifPixels->dOrientation - (pDifPixels + 1)->dOrientation) > 90.){//差分が90度を越える場合は角度の小さい方を90度加算した上で、Energyの重みで加算
		//角度の小さい方を180°加算
		if (pDifPixels->dOrientation < (pDifPixels + 1)->dOrientation) {
			pDifPixels->dOrientation += 180.;
		}
		else{
			(pDifPixels + 1)->dOrientation += 180.;
		}
	}
	dOrientWeightedByEnergy = (pDifPixels->dOrientation * pDifPixels->dEnergy + (pDifPixels + 1)->dOrientation * (pDifPixels + 1)->dEnergy) / (pDifPixels->dEnergy + (pDifPixels + 1)->dEnergy);

	return dOrientWeightedByEnergy;
}
//---------------------------------------------------------------------------
//線欠陥補正対象画素の左右片側ROIにおける上下±1pixシフトさせたROIを含めたEnergyとエッジの方向を計算

DIFFERENTIALARROUNDPIXELS TSIMARS::GetOrientWeightedByEnergy(DIFFERENTIALARROUNDPIXELS& DifPixels, DIFFERENTIALARROUNDPIXELS& DifPixelsU, DIFFERENTIALARROUNDPIXELS& DifPixelsD)
{
	DIFFERENTIALARROUNDPIXELS DiffAroundPix;
	const double dThEnergyRate = 2.0;//縦方向の中央のROIのEnergyに対する上下ROIのEnergyの比率の閾値

	//縦方向の中央ROIのEnergyが上下のいずれかと同等レベルであれば中央ROIのEnergy、Orientを返す
	if (DifPixels.dEnergy * dThEnergyRate > DifPixelsU.dEnergy || DifPixels.dEnergy * dThEnergyRate > DifPixelsD.dEnergy) {
		DiffAroundPix = DifPixels;
	}
	else{//縦方向の中央ROIのEnergyが上下のいずれと比較しても閾値以下で小さい場合は上中下のROIの統合のEnergy, Orientを返す
		//Energyの補正対象画素との距離の重み平均
		const double dDist2 = 4.0;//左右ROIと補正対象画素との距離2乗
		const double dDist2UD = 5.0;//拡張ROIと補正対象画素との距離2乗
		double dNorm = 1 / dDist2 + 2 / dDist2UD;
		double dEnergyWeightedByDist = DifPixels.dEnergy / dDist2 + (DifPixelsU.dEnergy + DifPixelsD.dEnergy) / dDist2UD;
		dEnergyWeightedByDist /= dNorm;


		double dOrientWeightedByEnergy = 0.;

		//上中下の3つのROIのうち2つのOrientの比較して差分が90°以下の場合は小さい方を180度加算
		//上と中
		if (fabs(DifPixels.dOrientation - DifPixelsU.dOrientation) > 90.){//差分が90度を越える場合は角度の小さい方を90度加算した上で、Energyの重みで加算
			//角度の小さい方を180°加算
			if (DifPixels.dOrientation < DifPixelsU.dOrientation) {
				DifPixels.dOrientation += 180.;
			}
			else{
				DifPixelsU.dOrientation += 180.;
			}
		}
		//中と下
		if (fabs(DifPixels.dOrientation - DifPixelsD.dOrientation) > 90.){//差分が90度を越える場合は角度の小さい方を90度加算した上で、Energyの重みで加算
			//角度の小さい方を180°加算
			if (DifPixels.dOrientation < DifPixelsD.dOrientation) {
				DifPixels.dOrientation += 180.;
			}
			else{
				DifPixelsD.dOrientation += 180.;
			}
		}
		//上と下
		if (fabs(DifPixelsU.dOrientation - DifPixelsD.dOrientation) > 90.){//差分が90度を越える場合は角度の小さい方を90度加算した上で、Energyの重みで加算
			//角度の小さい方を180°加算
			if (DifPixelsU.dOrientation < DifPixelsD.dOrientation) {
				DifPixelsU.dOrientation += 180.;
			}
			else{
				DifPixelsD.dOrientation += 180.;
			}
		}

		dOrientWeightedByEnergy = (DifPixels.dOrientation * DifPixels.dEnergy / dDist2 + DifPixelsU.dOrientation * DifPixelsU.dEnergy / dDist2UD + DifPixelsD.dOrientation * DifPixelsD.dEnergy/ dDist2UD) / (DifPixels.dEnergy/ dDist2 + (DifPixelsU.dEnergy + DifPixelsD.dEnergy) / dDist2UD);

		DiffAroundPix.dEnergy = dEnergyWeightedByDist;
		DiffAroundPix.dOrientation = dOrientWeightedByEnergy;
	}

	return DiffAroundPix;
}
//---------------------------------------------------------------------------

void TSIMARS::CalcLaplacianFilter(int iQLOffset)
{
	double dFilterLaplacian[9] = {1., 1., 1., 1., -8., 1., 1., 1., 1.};
    CTDAT Image = *this;

	//Filter演算
	for (int iSCnt = 1; iSCnt < SubPixel - 1; iSCnt++) {
		for (int iMCnt = 1; iMCnt < MainPixel - 1; iMCnt++) {
			SetVal(iSCnt, iMCnt,
				  Image.Dt[iSCnt - 1][iMCnt - 1]*dFilterLaplacian[0] +
				  Image.Dt[iSCnt - 1][iMCnt    ]*dFilterLaplacian[1] +
				  Image.Dt[iSCnt - 1][iMCnt + 1]*dFilterLaplacian[2] +
				  Image.Dt[iSCnt    ][iMCnt - 1]*dFilterLaplacian[3] +
				  Image.Dt[iSCnt    ][iMCnt    ]*dFilterLaplacian[4] +
				  Image.Dt[iSCnt    ][iMCnt + 1]*dFilterLaplacian[5] +
				  Image.Dt[iSCnt + 1][iMCnt - 1]*dFilterLaplacian[6] +
				  Image.Dt[iSCnt + 1][iMCnt    ]*dFilterLaplacian[7] +
				  Image.Dt[iSCnt + 1][iMCnt + 1]*dFilterLaplacian[8] + iQLOffset);
		}
	}

	return;
}
//---------------------------------------------------------------------------
//DLLを動的に呼び出してCTFチャートのパターン領域を検出する

bool TSIMARS::DetectionCTFChartPattern(string StrFile, double& dCtfStripesAngle)
{
	//DLLのロード
	string FileNameDLL = "DetectCTFChartPatternLib.dll";
	HMODULE hMod = LoadLibrary(FileNameDLL.c_str());///ファイル名
	if (hMod == NULL) {
		string strEvent = "DLLの読み込みに失敗しました。\nErrorCode: " + IntToStr(GetLastError()) + "\n" + FileNameDLL;
		CApplication::GetInstance()->MessageBox(strEvent.c_str(), "Error", MB_OK);
		return false;
	}

	//DLLのインターフェース関数のアドレスの取得
	//関数の名前はstdcallの場合 "_関数名@引数のバイト数"
	string StrFunctionAddress = "_DetectCtfChartPattern@16";
	FARPROC proc = GetProcAddress(hMod, StrFunctionAddress.c_str());
	if (proc == NULL) {
		string strEvent = "関数のアドレスの取得に失敗しました。\n" + StrFunctionAddress;
		CApplication::GetInstance()->MessageBox(strEvent.c_str(), "Error", MB_OK);
		FreeLibrary(hMod);
		return false;
	}

	typedef int (*TAddProc)(const float* input_img, float* output_img, unsigned int  width,	unsigned int  height);///typedef 返り値 (*関数ポインタ名)(引数);

	//Pbパッチ位置を検出する画像の読み込みと指定サイズへの変換
#ifdef _DLL_INPUT_SIZE_352
	const int iFixedSize = 352;//検出モジュール指定の画像サイズ
	const int iTrimSize = 2816;
	const int iMOffset = -8;
	const int iSOffset = -10;
#else
	const int iFixedSize = 350;//検出モジュール指定の画像サイズ　→DLL側が352前提のため350はNG
	const int iTrimSize = 2800;
	const int iMOffset = -16;
	const int iSOffset = -18;
#endif
	const int iDiv = 8;//8分の１縮小
	const double dPixelSize = 0.15;//1pix 0.15mm
	float* pfInputImg  = new float[iFixedSize * iFixedSize];
	const int iMainPixelOrg = MainPixel;
	const int iSubPixelOrg  = SubPixel;

	TTMA ImgBin(*this);
	//画像の端部カット
	ImgBin.Format(iTrimSize, iTrimSize, iSOffset, iMOffset, 0);
	string StrCutFile = ChangeFileExt(ExtractFilePath(StrFile) + "F" + ExtractFileName(StrFile), ".tma");;
#ifdef _DEBUG_OUTPUT
	ImgBin.WriteTma(StrCutFile, false);
#endif
	//画像の縮小（整数倍）
	ImgBin.Div(iDiv, iDiv);
	string StrDivFile = ChangeFileExt(ExtractFilePath(StrFile) + "DF" + ExtractFileName(StrFile), ".tma");;
#ifdef _DEBUG_OUTPUT
	ImgBin.WriteTma(StrDivFile, false);
#endif

	TFIMG FImg;
	FImg = ImgBin;
	double dQLMax = (double)((1 << ImgBin.Bit) - 1);
	FImg /= dQLMax;
	long lCnt;
	for (lCnt = 0; lCnt < ImgBin.MainPixel * ImgBin.SubPixel; lCnt++) {
		int iM = lCnt % ImgBin.MainPixel;
		int iS = lCnt / ImgBin.MainPixel;
//		pfInputImg[lCnt] = FImg.Dt[iM][iS];//iM, iS
		pfInputImg[lCnt] = 1.0 - FImg.Dt[iM][iS];//iM, iS     //白黒反転させてからDLLに入力しないとNG。
	}
//	StrOutFile = StrFile;


	//Pbパッチ位置の検出
	float* pfOutputImg = new float[iFixedSize * iFixedSize];
	TAddProc Detection = reinterpret_cast<TAddProc>(proc);
	int iRet = Detection(pfInputImg, pfOutputImg, iFixedSize, iFixedSize);
	if (iRet) {
		string strEvent = "DLLの関数内でエラー発生しました。\n" + StrFile;
		CApplication::GetInstance()->MessageBox(strEvent.c_str(), "Error", MB_OK);
		delete[] pfInputImg;
		delete[] pfOutputImg;
		FreeLibrary(hMod);
		return false;
	}

//	FImg.NewDt(iFixedSize, iFixedSize);
	FImg.Fill(0.0);
	TSIMARS ImgDetect;
	const iBit = 14;
//	double dQLMax = (double)((1 << iBit) - 1);
	for (lCnt = 0; lCnt < iFixedSize * iFixedSize; lCnt++) {
		int iM = lCnt % iFixedSize;
		int iS = lCnt / iFixedSize;
		FImg.Dt[iM][iS] = pfOutputImg[lCnt] * dQLMax;//iM, iS
//		FImg.Dt[iM][iS] = (1.0 - pfOutputImg[lCnt]) * dQLMax;//iM, iS
	}
	ImgDetect = FImg;
	ImgDetect.SetBit(iBit);
	string StrDetect = ChangeFileExt(ExtractFilePath(StrFile) + "Detect_" + ExtractFileName(StrFile), ".tma");;
#ifdef _DEBUG_OUTPUT
	ImgDetect.WriteTma(StrDetect, false);
#endif
	//CTFチャートの縞の配置角度の検出
	dCtfStripesAngle = ImgDetect.CalcCtfStripesAngle(StrDetect);


	ImgDetect.LinearExpand(iDiv, iDiv);//主副8倍線形拡大 →CTFチャート境界部のカクカクのスケールが大きくなるだけ
	ImgDetect.Format(iSubPixelOrg, iMainPixelOrg, -iSOffset, -iMOffset, 0);
#ifdef _DEBUG_OUTPUT
	string StrDetectExp = ChangeFileExt(ExtractFilePath(StrFile) + "DetectExp(" + FloatToStrF(dCtfStripesAngle, ffFixed, 4, 1) + "deg)_" + ExtractFileName(StrFile), ".tma");//有効数字4桁、小数点以下1桁を表示
	ImgDetect.WriteTma(StrDetectExp, false);
#endif

	*this = ImgDetect;//CTFチャートの縞領域をthisにコピー

	delete[] pfInputImg;
	delete[] pfOutputImg;
	FreeLibrary(hMod);

	return true;
}
//---------------------------------------------------------------------------
//CTFチャートの縞の配置角度の検出

double TSIMARS::CalcCtfStripesAngle(string StrFile)
{
	double dCtfStripesAngle;
	int iSCnt, iMCnt;

#if 0
	//Sobel Filterを用いたエッジ方向検出 →斜めの線にならずジグザグの線なので正しく検出できない
	TTMA ImgOrient, ImgEnergy;
	ImgOrient.NewDt(SubPixel, MainPixel, Bit);
	ImgOrient.FillValue(DtMax);
	ImgEnergy.NewDt(SubPixel, MainPixel, Bit);
	ImgEnergy.FillValue(0);

	const double dThEnergy = 100.;
	DIFFERENTIALARROUNDPIXELS DiffPixels;
	for (iSCnt = 1; iSCnt < SubPixel - 1; iSCnt++) {
		for (iMCnt = 1; iMCnt < MainPixel - 1; iMCnt++) {
			DiffPixels = CalcDifferentialArroundPixel_Fast(*this,  iSCnt, iMCnt);

			if (DiffPixels.dEnergy > dThEnergy) {
				ImgOrient.SetVal(iSCnt, iMCnt, DiffPixels.dOrientation * 10);//10倍値を四捨五入（小数第1位まで計算）
				ImgEnergy.SetVal(iSCnt, iMCnt, DiffPixels.dEnergy);
			}
		}
	}
 #ifdef _DEBUG_OUTPUT
	string StrOrient = ChangeFileExt(ExtractFilePath(StrFile) + "Orient_" + ExtractFileName(StrFile), ".tma");;
	ImgOrient.WriteTma(StrOrient, false);
	string StrEnergy = ChangeFileExt(ExtractFilePath(StrFile) + "Energy_" + ExtractFileName(StrFile), ".tma");;
	ImgEnergy.WriteTma(StrEnergy, false);
 #endif

#else //Hough変換を用いたエッジ方向検出
	TSIMARS ImgLaplacian = *this;
	const int iQLOffset = 8000;
	ImgLaplacian.CalcLaplacianFilter(iQLOffset);//ラプラシアンフィルタ
 #ifdef _DEBUG_OUTPUT
	string StrLaplacian = ChangeFileExt(ExtractFilePath(StrFile) + "Laplacian_" + ExtractFileName(StrFile), ".tma");;
	ImgLaplacian.WriteTma(StrLaplacian, false);
 #endif

	const bool bDetectOnlyPlusEdge = true;//プラス側の微分値のみでHough変換
	const int iThDiffQL = 1000;//Hough変換する画素の微分閾値
	const int iTHETA_RESOLUTION = 1024;//thetaの範囲は、0～π[rad]を0から1023までで分割する
	const int iRHO_RESOLUTION = 500 * 2;//rhoの範囲は、-500からまで499まで（HoughImg.MainPixel、HoughImg.SubPixelが352なので、Rhoの絶対値は500あればOK）
	const int iLNUMBER_MAX = 10;//検出する直線の数の最大
//	const int iLNUMBER_MAX = 2;//検出する直線の数の最大（CTFチャート検出領域の矩形における2本の長辺のみを検出）
//	const int iCounterTh = 60;//カウンターの閾値
	const int iCounterTh = 20;//カウンターの閾値
//	const int iThetaNeighbor = 20;//同一直線とみなすΘ近傍（±3.5deg相当）
	const int iThetaNeighbor = 60;//同一直線とみなすΘ近傍（±10deg相当）
	const int iRhoNeighbor = 10;//同一直線とみなすρ近傍（CTFチャート検出領域の矩形における2本の長辺のρの間隔が20程度）
//	const int iRhoNeighbor = 15;//同一直線とみなすρ近傍（CTFチャート検出領域の矩形における2本の長辺のρの間隔が20程度）
//	const int iRhoNeighbor = 20;//同一直線とみなすρ近傍（CTFチャート検出領域の矩形における2本の長辺のρの間隔が20程度）
	double* dThetaDetect = new double[iLNUMBER_MAX];
	int* iRhoDetect = new int[iLNUMBER_MAX];
	if (!ImgLaplacian.HoughTrans(StrLaplacian, bDetectOnlyPlusEdge, iThDiffQL, iQLOffset, iTHETA_RESOLUTION, iRHO_RESOLUTION, iThetaNeighbor, iRhoNeighbor, iLNUMBER_MAX, iCounterTh, dThetaDetect, iRhoDetect)) {
		string strEvent = "Hough変換でエラー発生しました。\n" + StrFile;
		CApplication::GetInstance()->MessageBox(strEvent.c_str(), "Error", MB_OK);
		delete[] dThetaDetect;
		delete[] iRhoDetect;
		return -10000;//tmp
	}


	//LNUMBER_MAX個の候補から長辺の角度を抽出
	int iIndexLongSide[2] = {0, 1};//矩形の長辺のindexの初期値
	double dThTheta = 5.;//2つの長辺の検出角度の差分閾値
	int iMinThRho = 15;//２つの長辺のρ閾値(閾値以上であれば同一直線でない)
	//0番目は長辺と仮定し、1番目以降をチェック
	int iIndex = 1;
	while (!(abs(dThetaDetect[iIndexLongSide[0]] - dThetaDetect[iIndex]) < dThTheta && abs(iRhoDetect[iIndexLongSide[0]] - iRhoDetect[iIndex]) > iMinThRho)
		   && iIndex < iLNUMBER_MAX - 1) {
		iIndex++;
	}

    //長辺の２辺が見つかった場合は、それらの平均値を検出角度とする
	if (iIndex < iLNUMBER_MAX - 1) {
		iIndexLongSide[1] = iIndex;
		dCtfStripesAngle = (dThetaDetect[iIndexLongSide[0]] + dThetaDetect[iIndexLongSide[iIndex]]) / 2.0;//Average
	}
	else{//長辺のもう片側が見つからない場合は0番目のThetaを検出角度とする
		dCtfStripesAngle = dThetaDetect[iIndexLongSide[0]];
	}
	delete[] dThetaDetect;
	delete[] iRhoDetect;
#endif

	return dCtfStripesAngle;
}
//---------------------------------------------------------------------------
/*
Hough変換

引数リスト
string str,//微分画像ファイルのパス（中間データのファイル名作成に使用）
const bool bDetectOnlyPlusEdge,//微分のプラス側のみのHough変換か、両側かのフラグ
const int iThDiffQL,//Hough変換対象画素の微分閾値（閾値より大きい画素のみHough変換する）
const int iBaseQL, //入力画像（微分画像）の微分0相当のベースQL
const int iTHETA_RESOLUTION,//0～πの範囲のθの分解能
const int iRHO_RESOLUTION,  //画像原点から対象画素までの距離ρの範囲
const int iThetaNeighbor,//同一直線とみなすθの範囲
const int iRhoNeighbor,  //同一直線とみなすρの範囲
const int iLNUMBER_MAX,　//検出する直線数の最大値
const int iCounterTh　//直線を形成するピクセル数の閾値（閾値以下の直線候補は直線とみなさない）
double* dThetaDetect,//検出したiLNUMBER_MAX個のθ配列（出力）
int* iRhoDetect //検出したiLNUMBER_MAX個のρ配列（出力）

*/

#define _PI 3.141592653589793238462643383279//30桁

bool TSIMARS::HoughTrans(string str, const bool bDetectOnlyPlusEdge, const int iThDiffQL, const int iBaseQL, const int iTHETA_RESOLUTION, const int iRHO_RESOLUTION, const int iThetaNeighbor, const int iRhoNeighbor, const int iLNUMBER_MAX, const int iCounterTh, double* dThetaDetect, int* iRhoDetect)
{
	int counter_max;
	int iTheta, iRho;
	int iThetaMax, iRhoMax, count;
	int iThetaCut, iRhoCut;

	//直線検出のためのカウンタを用意する
	TIIMG counter;
	counter.NewDt(iTHETA_RESOLUTION, iRHO_RESOLUTION);//NewDt(M,S)の順
	counter.Fill(0);

	//ハフ逆変換（直線）の結果を表示する画像
	TTMA HoughImg;
	HoughImg = *this;


	//sinとcosのテーブルを用意する
	double* dSin = new double[iTHETA_RESOLUTION];
	double* dCos = new double[iTHETA_RESOLUTION];
	double dPai = _PI / iTHETA_RESOLUTION;
	for(iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
		double dRad = iTheta * dPai;
		dSin[iTheta] = Sin(dRad);
		dCos[iTheta] = Cos(dRad);
	}

//	string strAddPrefix = AddPrefix(str, "HF(" + IntToStr(iThDiffQL) + "," + IntToStr(iLNUMBER_MAX) + ")" + str);
	string strAddPrefix = ExtractFilePath(str) + "Hough(" + IntToStr(iThDiffQL) + "," + IntToStr(iLNUMBER_MAX) + ")_" + ExtractFileName(str);
	ofstream TextFile( ChangeFileExt(strAddPrefix, ".txt").c_str() );


	//ハフ変換（直線）の実行
	for (int iM = 1; iM < MainPixel - 1; iM++){//微分画像の画像の端1行1列は無効のため
		for (int iS = 1; iS < SubPixel - 1; iS++){
			int iQL;
			switch (bDetectOnlyPlusEdge) {
			case true:
				iQL = Dt[iS][iM] - iBaseQL;
				break;
			case false:
				iQL = abs(Dt[iS][iM] - iBaseQL);//ベースQLからの差分の絶対値
				break;
			}

			if (iQL > iThDiffQL) {
				for(iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
					iRho = round(iM * dCos[iTheta] + iS * dSin[iTheta]);//負の数の四捨五入も考慮
 #if 1
					if (iRho == 0) {

					}
 #endif
					if (0 > iTheta || iTHETA_RESOLUTION <= iTheta) {
						CApplication::GetInstance()->MessageBox("Thetaのレンジオーバー", "Error", MB_OK);
						delete[] dSin;
						delete[] dCos;
						delete[] dThetaDetect;
						delete[] iRhoDetect;
						return false;
					}
					if (0 > iRho + iRHO_RESOLUTION/2 || iRHO_RESOLUTION <= iRho + iRHO_RESOLUTION/2) {
						CApplication::GetInstance()->MessageBox("Rhoのレンジオーバー", "Error", MB_OK);
						delete[] dSin;
						delete[] dCos;
						delete[] dThetaDetect;
						delete[] iRhoDetect;
						return false;
					}
					counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2]++;//画像data（Dt[iM][iS]の順）
				}
			}
		}
	}

 #ifdef _DEBUG_OUTPUT
	//Hough空間の画像化
	counter.SaveTmaTdt(ExtractFilePath(str) + "Counter(" + IntToStr(iThDiffQL) + "," + IntToStr(iLNUMBER_MAX) + ")_" + ExtractFileName(str));
 #endif

	TextFile << "ThDiffQL\t"<< (IntToStr(iThDiffQL)).c_str() << endl;
	TextFile << "id\tTheta\tRho\tiCounter\t"<< endl;

	//Hough逆変換（直線）の実行
	for (int n = 0; n < iLNUMBER_MAX; n++){
		//counterが最大になるtheta_maxとrho_maxを求める
		counter_max = 0;
		for (iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
			for(iRho = -iRHO_RESOLUTION/2; iRho < iRHO_RESOLUTION/2; iRho++){
				if (counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2] > counter_max){
					counter_max = counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2];
					iThetaMax = iTheta;
					iRhoMax = iRho;
				}
			}
		}
		//直線を形成するピクセルがiCounterTh未満であれば候補として採用しない
		if (counter_max < iCounterTh){
			continue;
		}

		dThetaDetect[n] = Degrees(iThetaMax * dPai);
		iRhoDetect[n] = iRhoMax;
		TextFile << n << "\t" << dThetaDetect[n] << "\t" << iRhoDetect[n] << "\t" << counter_max << endl;

		//逆ハフ変換（直線）した結果を表示する
		int iM, iS;
		if (iThetaMax != 0){//垂直の線を除く
			for (iM = 0; iM < MainPixel; iM++){
				iS = (int)((iRhoMax - iM * dCos[iThetaMax]) / dSin[iThetaMax] + 0.5);

				if (iS >= SubPixel || iS < 0){
					continue;
				}
				HoughImg.SetVal(iS, iM, HoughImg.DtMax);
			}
		}
		if (iThetaMax != iTHETA_RESOLUTION/2){//水平の線を除く
			for (iS = 0; iS < SubPixel; iS++){
				iM = (int)((iRhoMax - iS * dSin[iThetaMax]) / dCos[iThetaMax] + 0.5);
				if (iM >= MainPixel || iM < 0){
					continue;
				}
				HoughImg.SetVal(iS, iM, HoughImg.DtMax);
			}
		}

		//counter.Dt[iThetaMax][iRhoMax]の近傍を0にする
		for (int i = -iThetaNeighbor; i <= iThetaNeighbor; i++){
			for (int j = -iRhoNeighbor; j <= iRhoNeighbor; j++){
				iThetaCut = iThetaMax + i;
				iRhoCut = iRhoMax + j;
				if (iThetaCut < 0){
					iThetaCut += iTHETA_RESOLUTION;
					iRhoCut = -iRhoCut;
				}
				else if (iThetaCut > iTHETA_RESOLUTION - 1){
					iThetaCut -= iTHETA_RESOLUTION;
					iRhoCut = -iRhoCut;
				}
				counter.Dt[iThetaCut][iRhoCut + iRHO_RESOLUTION/2] = 0;//削除する
			}
		}
	} //end for n
#ifdef _DEBUG_OUTPUT
	HoughImg.WriteTma(ExtractFilePath(str) + "HF(" + IntToStr(iThDiffQL) + "," + IntToStr(iLNUMBER_MAX) + ")_" + ExtractFileName(str));
#endif

	delete[] dSin;
	delete[] dCos;
	TextFile.close();

	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TIMG_ATTRIBUTE_INF::TIMG_ATTRIBUTE_INF()
{
	InitializeImgAttributeInf();
}
//---------------------------------------------------------------------------

TIMG_ATTRIBUTE_INF::~TIMG_ATTRIBUTE_INF()
{
	;
}
//---------------------------------------------------------------------------

void TIMG_ATTRIBUTE_INF::InitializeImgAttributeInf()
{
	iShotGoID = 0;
	iMode = 0;
	iFr = 0;
	iStatus = 0;
	iElapsedTimeSinceSWOFF = 0;
	iXrayOnTime = 0;
	StrFileName = "";
	bCor = true;
	bFirstFrOfMode = false;
	iElapsedTimeOfLagAveFinalFr = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TIMG_ATTRIBUTE_INFS::TIMG_ATTRIBUTE_INFS()
{
	InitializeImgAttributeInfs();
}
//---------------------------------------------------------------------------

TIMG_ATTRIBUTE_INFS::~TIMG_ATTRIBUTE_INFS()
{
	DeleteImgAttributeInfs();
}
//---------------------------------------------------------------------------

void TIMG_ATTRIBUTE_INFS::InitializeImgAttributeInfs()
{
	for (int iMode = 0; iMode < 4; iMode++) {
		if (ImgAttInf[iMode]) {
			delete[] ImgAttInf[iMode];
			ImgAttInf[iMode] = NULL;
		}
//		ImgAttInf[iMode] = new IMG_ATTRIBUTE_INF[iNumOfArrayN];
//		iNumOfArray[iMode] = iNumOfArrayN;
		iNumOfArray[iMode] = 0;
		iModeSorted[iMode] = 0;
		iNumOfLagT2Array[iMode] = 0;
		iNumOfModeCycle[iMode] = 0;
		iElapsedTimeOfFinalFr[iMode] = 0;
		bReachFinalFr[iMode] = false;
		iFixedLagT2ID_CorO2[iMode] = -1;//負のID値で初期化
	}
	iNumOfMode = 0;
	iModeCorrected[0] = iModeCorrected[1] = iModeCorrected[2] = 0;
	iNumOfModeCorrected = 0;
	iCurrentShotGoID = 0;
}
//---------------------------------------------------------------------------

void TIMG_ATTRIBUTE_INFS::DeleteImgAttributeInfs()
{
	for (int iMode = 0; iMode < 4; iMode++) {
		if (ImgAttInf[iMode]) {
			delete[] ImgAttInf[iMode];
			ImgAttInf[iMode] = NULL;
		}
		iNumOfArray[iMode] = 0;
		iModeSorted[iMode] = 0;
		iNumOfLagT2Array[iMode] = 0;
		iNumOfModeCycle[iMode] = 0;
		iElapsedTimeOfFinalFr[iMode] = 0;
		bReachFinalFr[iMode] = false;
		iFixedLagT2ID_CorO2[iMode] = -1;
	}
	iNumOfMode = 0;
	iCurrentShotGoID = 0;
	iModeCorrected[0] = iModeCorrected[1] = iModeCorrected[2] = 0;
	iNumOfModeCorrected = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TROI_Lag::TROI_Lag()
{
	m_iX0 = m_iX1 = m_iY0 = m_iY1 = 0;
	m_dQLave = NULL;
}
//---------------------------------------------------------------------------
TROI_Lag::~TROI_Lag()
{
	//デバッグ中　一時的にコメントアウトする
//	DeleteROILag();
}
//---------------------------------------------------------------------------

void TROI_Lag::DeleteROILag()
{
	m_iX0 = m_iX1 = m_iY0 = m_iY1 = 0;
	if (m_dQLave){
		delete[] m_dQLave;
		m_dQLave = NULL;
	}
}
//---------------------------------------------------------------------------

void TROI_Lag::Set(int iX0, int iY0, int iX1, int iY1)
{
	m_iX0 = iX0;
	m_iX1 = iX1;
	m_iY0 = iY0;
	m_iY1 = iY1;
}
//---------------------------------------------------------------------------

void TROI_Lag::AllocateQLave(int iNumOfFrame)
{
	if (iNumOfFrame < 0){
		CApplication::GetInstance()->MessageBox(("Frame数が不正です。\niNumOfFrame=" + IntToStr(iNumOfFrame)).c_str(),"Error", MB_OK);
		return;
	}

	if (m_dQLave) {
		delete[] m_dQLave;
        m_dQLave = NULL;
	}
	m_dQLave = new double[iNumOfFrame];

//	memset(m_dQLave, 0, sizeof(m_dQLave));
	for (int iFr = 0; iFr < iNumOfFrame; iFr++) {
		m_dQLave[iFr] = 0.0;
	}
}
//---------------------------------------------------------------------------

void TROI_Lag::SetQLave(int iNumOfFrame, double* dQLave)
{
	for (int iFr = 0; iFr < iNumOfFrame; iFr++) {
		m_dQLave[iFr] = dQLave[iFr];
	}
}
//---------------------------------------------------------------------------

void TROI_Lag::SetQLave(int iFrame, double dQLave)
{
	m_dQLave[iFrame] = dQLave;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TLagCalib_D3::TLagCalib_D3()
{
	m_iNumOfROI = m_iNumOfFrame = m_iModel = m_iNumOfVerTable = m_iNumOfHorTable = 0;
	m_dAave = 0.0;
	m_dTime = m_dA = m_dB = m_dCoef2 = m_d2DLUT = NULL;
	m_ROI_X = NULL;
	m_iHorTable = m_i2DLUT = NULL;
#ifdef _INT_VER_TABLE
	m_iVerTable = NULL;
#else
	m_dVerTable = NULL;
#endif
}
//---------------------------------------------------------------------------

TLagCalib_D3::~TLagCalib_D3()
{
	if (m_ROI_X) {
		for  (int iROI = 0; iROI < m_iNumOfROI; iROI++) {
			m_ROI_X[iROI].DeleteROILag();
		}
		m_ROI_X = NULL;
	}


	m_iNumOfROI = m_iNumOfFrame = m_iModel = m_iNumOfVerTable = m_iNumOfHorTable = 0;
	m_dAave = 0.0;
	if (m_dTime) {
		delete[] m_dTime;
		m_dTime = NULL;
	}
	if (m_dA) {
		delete[] m_dA;
		m_dA = NULL;
	}
	if (m_dB) {
		delete[] m_dB;
		m_dB = NULL;
	}
	if (m_dCoef2) {
		delete[] m_dCoef2;
		m_dCoef2 = NULL;
	}
#ifdef _INT_VER_TABLE
	if (m_iVerTable) {
		delete[] m_iVerTable;
		m_iVerTable = NULL;
	}
#else
	if (m_dVerTable) {
		delete[] m_dVerTable;
		m_dVerTable = NULL;
	}
#endif
	if (m_iHorTable) {
		delete[] m_iHorTable;
		m_iHorTable = NULL;
	}
	if (m_d2DLUT) {
		delete[] m_d2DLUT;
		m_d2DLUT = NULL;
	}
	if (m_i2DLUT) {
		delete[] m_i2DLUT;
		m_i2DLUT = NULL;
	}
}
//---------------------------------------------------------------------------

void TLagCalib_D3::AllocateROILag(int iNumOfRoi)
{
	if (iNumOfRoi < 0){
		CApplication::GetInstance()->MessageBox(("ROI数が不正です。\niNumOfRoi=" + IntToStr(iNumOfRoi)).c_str(),"Error", MB_OK);
		return;
	}

	if (m_iNumOfROI) {
		for (int iCntRoi = 0; iCntRoi < m_iNumOfROI; iCntRoi++) {
			m_ROI_X[iCntRoi].DeleteROILag();
		}
		m_ROI_X = NULL;
		m_iNumOfROI = 0;
	}
	m_iNumOfROI = iNumOfRoi;
	m_ROI_X = new TROI_Lag[m_iNumOfROI];

	if (m_dA) {
		delete[] m_dA;
		m_dA = NULL;
	}
	m_dA = new double[m_iNumOfROI];

	if (m_dB) {
		delete[] m_dB;
		m_dB = NULL;
	}
	m_dB = new double[m_iNumOfROI];

	if (m_dCoef2) {
		delete[] m_dCoef2;
		m_dCoef2 = NULL;
	}
	m_dCoef2 = new double[m_iNumOfROI];

    //初期化
	for (int iCntRoi = 0; iCntRoi < m_iNumOfROI; iCntRoi++) {
		m_ROI_X[iCntRoi].Set(0, 0, 0, 0);
		m_dA[iCntRoi] = 0.0;
		m_dB[iCntRoi] = 0.0;
		m_dCoef2[iCntRoi] = 0.0;
	}
   	m_dAave = 0.0;

	return;
}
//---------------------------------------------------------------------------

void TLagCalib_D3::AllocateNumOfFrame(int iNumOfFrame)
{
	if (iNumOfFrame < 0){
		CApplication::GetInstance()->MessageBox(("Frame数が不正です。\niNumOfFrame=" + IntToStr(iNumOfFrame)).c_str(),"Error", MB_OK);
		return;
	}

	if (m_dTime) {
		delete[] m_dTime;
		m_dTime = NULL;
		m_iNumOfFrame = 0;
	}
	m_iNumOfFrame = iNumOfFrame;
	m_dTime = new double[iNumOfFrame];

	for (int iFr = 0; iFr < iNumOfFrame; iFr++) {
		m_dTime[iFr] = 0.0;
	}

	for (int iROI = 0; iROI < m_iNumOfROI; iROI++) {
		if (m_ROI_X[iROI].GetQLave()){
			m_ROI_X[iROI].DeleteROILag();
		}
		m_ROI_X[iROI].AllocateQLave(iNumOfFrame);
	}

	return;
}
//---------------------------------------------------------------------------
/*
#define _EXPONENTIAL	(0) //指数fitting
#define _POWER			(1) //冪乗fitting
#define _LINEAR			(2) //直線fitting
#define _TWO_DIM		(3) //放物線fitting
#define _THREE_DIM		(4) //3次関数fitting
*/

bool TLagCalib_D3::CalcLagAttenuation()
{
	int iROI, iFrame;
	for (iROI = 0; iROI < m_iNumOfROI; iROI++) {
		CLeastSquare ls(m_iNumOfFrame, m_dTime, GetRoi(iROI).GetQLave(), m_iModel);

		if (!ls.Calc()){
			return false;
		}
		Vct v = ls.GetSolve();
		m_dA[iROI] = v[1];
		m_dB[iROI] = v[0];
		m_dCoef2[iROI] = ls.GetCorCoef2();
	}

	return true;
}
//---------------------------------------------------------------------------
//2DLUTの横方向テーブルのメモリ確保（連続照射時間に関するテーブル）

void TLagCalib_D3::AllocateNumOfVerTable(int iNumOfVerTable)
{
	m_iNumOfVerTable = iNumOfVerTable;

#ifdef _INT_VER_TABLE
	if (m_iVerTable) {
		delete[] m_iVerTable;
		m_iVerTable = NULL;
	}
	m_iVerTable = new int[m_iNumOfVerTable];

	//初期化
	for (int iCnt = 0; iCnt < m_iNumOfVerTable; iCnt++) {
		m_iVerTable[iCnt] = 0;
	}

#else
	if (m_dVerTable) {
		delete[] m_dVerTable;
		m_dVerTable = NULL;
	}
	m_dVerTable = new double[m_iNumOfVerTable];

	//初期化
	for (int iCnt = 0; iCnt < m_iNumOfVerTable; iCnt++) {
		m_dVerTable[iCnt] = 0.0;
	}
#endif

    return;
}
//---------------------------------------------------------------------------
//2DLUTの縦方向テーブルのメモリ確保（ΔT/Tに関するテーブル)

void TLagCalib_D3::AllocateNumOfHorTable(int iNumOfHorTable)
{
	m_iNumOfHorTable = iNumOfHorTable;

	if (m_iHorTable) {
		delete[] m_iHorTable;
		m_iHorTable = NULL;
	}
	m_iHorTable = new int[m_iNumOfHorTable];

	//初期化
	for (int iCnt = 0; iCnt < m_iNumOfHorTable; iCnt++) {
		m_iHorTable[iCnt] = 0;
	}

	return;
}
//---------------------------------------------------------------------------
//2DLUTのメモリ確保（横方向のループが優先で、次が縦のループ）

void TLagCalib_D3::AllocateD2DLUT(int iNumOfHorTable, int iNumOfVerTable)
{
	if (m_d2DLUT) {
		delete[] m_d2DLUT;
		m_d2DLUT = NULL;
	}
	m_d2DLUT = new double[iNumOfHorTable * iNumOfVerTable];

	//初期化
	for (int iCnt = 0; iCnt < iNumOfHorTable * iNumOfVerTable; iCnt++) {
		m_d2DLUT[iCnt] = 0.0;
	}

	return;
}
//---------------------------------------------------------------------------
//2DLUTのメモリ確保（横方向のループが優先で、次が縦のループ）

void TLagCalib_D3::AllocateI2DLUT(int iNumOfHorTable, int iNumOfVerTable)
{
	if (m_i2DLUT) {
		delete[] m_i2DLUT;
		m_i2DLUT = NULL;
	}
	m_i2DLUT = new int[iNumOfHorTable * iNumOfVerTable];

	//初期化
	for (int iCnt = 0; iCnt < iNumOfHorTable * iNumOfVerTable; iCnt++) {
		m_i2DLUT[iCnt] = 0;
	}

	return;
}
//---------------------------------------------------------------------------
//2DLUTを計算テーブル（横方向のループが優先で、次が縦のループ）

void TLagCalib_D3::CalcD2DLUT()
{
	AllocateD2DLUT(m_iNumOfHorTable, m_iNumOfVerTable);

	int iNorm2 = 1 << 8;//256
	int iCntTable;
	for (int iRow = 0; iRow < m_iNumOfVerTable; iRow++) {
		for (int iColumn = 0; iColumn < m_iNumOfHorTable; iColumn++) {
			double dPower = m_dAave + m_dMagicPara1 + m_iHorTable[iColumn] * 0.0001;
			iCntTable = iRow * m_iNumOfHorTable + iColumn;
#ifdef _INT_VER_TABLE
			m_d2DLUT[iCntTable] = pow((double)m_iVerTable[iRow] / iNorm2, dPower);
#else
			m_d2DLUT[iCntTable] = pow(m_dVerTable[iRow], dPower);
#endif
		}
	}

	return;
}
//---------------------------------------------------------------------------
//2DLUTを計算テーブル（横方向のループが優先で、次が縦のループ）

void TLagCalib_D3::CalcI2DLUT()
{
	AllocateI2DLUT(m_iNumOfHorTable, m_iNumOfVerTable);

	int iNorm  = 1 << 15;//32768
	int iNorm2 = 1 << 8;//256
	int iCntTable;
	for (int iRow = 0; iRow < m_iNumOfVerTable; iRow++) {
		for (int iColumn = 0; iColumn < m_iNumOfHorTable; iColumn++) {
			double dPower = m_dAave + m_dMagicPara1 + m_iHorTable[iColumn] * 0.0001;
			iCntTable = iRow * m_iNumOfHorTable + iColumn;
#ifdef _INT_VER_TABLE
			m_i2DLUT[iCntTable] = (int)(pow((double)m_iVerTable[iRow] / iNorm2, dPower) * iNorm);//切り捨て
#else
			m_i2DLUT[iCntTable] = (int)(pow(m_dVerTable[iRow], dPower) * iNorm);//切り捨て
#endif
		}
	}

	return;
}
//---------------------------------------------------------------------------

double TLagCalib_D3::GetD2DLUT(int iColumn, int iRow) const
{
	if (0 > iColumn || m_iNumOfHorTable <= iColumn) {
		return -1.0;
	}
	if (0 > iRow || m_iNumOfVerTable <= iRow) {
		return -1.0;
	}

	return m_d2DLUT[iRow * m_iNumOfHorTable + iColumn];
}
//---------------------------------------------------------------------------

int TLagCalib_D3::GetI2DLUT(int iColumn, int iRow) const
{
	if (0 > iColumn || m_iNumOfHorTable <= iColumn) {
		return -1.0;
	}
	if (0 > iRow || m_iNumOfVerTable <= iRow) {
		return -1.0;
	}

	return m_i2DLUT[iRow * m_iNumOfHorTable + iColumn];
}
//---------------------------------------------------------------------------
//2DLUTの縦、横の格子点は入力の引数を越えない格子点のうち、最大のものとする

int TLagCalib_D3::GetI2DLUT(int iXrayOnTime, int iT1, int iT, int& iColumn, int& iRow) const
{
//	int iColumn(0), iRow(0);
	iColumn = iRow = 0;
	//LUTの横方向の特定
	while (m_iHorTable[iColumn] < iXrayOnTime && iColumn < m_iNumOfHorTable){
		iColumn++;
	}
	if (iColumn >= m_iNumOfHorTable) {
    	iColumn = m_iNumOfHorTable - 1;//横方向の最後のテーブルとする
	}
	else{
		iColumn--;
	}

	//LUTの縦方向の特定
	double dTT = (double)iT / iT1;
#ifdef _INT_VER_TABLE
	int iNorm2 = 1 << 8;//256
	while (m_iVerTable[iRow] < dTT * iNorm2 && iRow < m_iNumOfVerTable){
#else
	while (m_dVerTable[iRow] < dTT && iRow < m_iNumOfVerTable){
#endif
		iRow++;
	}
	if (iRow >= m_iNumOfVerTable) {
		iRow = m_iNumOfVerTable - 1;//縦方向の最後のテーブルとする
	}
	else{
		iRow--;
	}

	return m_i2DLUT[iRow * m_iNumOfHorTable + iColumn];
}
//---------------------------------------------------------------------------

#endif
