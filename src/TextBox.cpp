#include "TextBox.h"

TextBox::TextBox(IPlugBase* pPlug, IRECT pR, int paramIdx, IText* pText, IGraphics* pGraphics, const char * maxText, bool showParamUnits)
	: ICaptionControl(pPlug, pR, paramIdx, pText, showParamUnits)
	, mTextRect(pR)
{
	mTextRect.GetPadded(-1);
	pGraphics->MeasureIText(pText, const_cast<char*>(maxText), &mTextRect);
#ifdef OS_OSX
	mTextRect.B -= 4;
#endif
	const int offset = (mRECT.H() - mTextRect.H()) / 2;
	mTextRect.T += offset;
	mTextRect.B += offset;
	
	IParam* param = pPlug->GetParam(paramIdx);
	const double maxVal = param->GetMax();
	switch(param->Type())
	{
		case IParam::kTypeInt:
			if ( maxVal < 10 ) SetTextEntryLength(1);
			else if ( maxVal < 100 ) SetTextEntryLength(2);
			else SetTextEntryLength(3);
			break;
			
		case IParam::kTypeDouble:
			if ( maxVal < 10 ) SetTextEntryLength(4);
			else if ( maxVal < 100 ) SetTextEntryLength(5);
			else if ( maxVal < 1000 ) SetTextEntryLength(6);
			break;
			
		default: break;
	}
}

bool TextBox::Draw(IGraphics* pGraphics)
{
	pGraphics->FillIRect(&mText.mTextEntryBGColor, &mRECT);
	pGraphics->DrawRect(&mText.mTextEntryFGColor, &mRECT);

	IRECT ourRect = mRECT;
	mRECT = mTextRect;
	bool result = ICaptionControl::Draw(pGraphics);
	mRECT = ourRect;

	return result;
}

void TextBox::OnMouseDown(int x, int y, IMouseMod* pMod)
{
	if (pMod->L)
	{
		IText ourText = mText;
		IRECT promptRect = mTextRect;
#if defined(OS_OSX)
		mText.mSize -= 2;
		promptRect.T -= 1;
#endif
		mPlug->GetGUI()->PromptUserInput(this, mPlug->GetParam(mParamIdx), &promptRect);
		mText = ourText;
		Redraw();
	}
}

void TextBox::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod)
{
	if (pMod->R)
	{
		OnMouseWheel(x, y, pMod, -dY);
	}
}

void TextBox::OnMouseWheel(int x, int y, IMouseMod* pMod, int d)
{
#ifdef PROTOOLS
	if (pMod->C)
	{
		mValue += GetParam()->GetStep() * 0.001 * d;
	}
#else
	if (pMod->C || pMod->S)
	{
		mValue += GetParam()->GetStep() * 0.001 * d;
	}
#endif
	else
	{
		mValue += GetParam()->GetStep() * 0.01 * d;
	}

	SetDirty();
}